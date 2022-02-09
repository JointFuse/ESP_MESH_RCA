/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  04.10.2021
 */
#include "ClientGUI.h"
#include <FL/FL_Multiline_Output.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <sstream>
#include <fstream>
#include <windows.h>
#include <iomanip>
#include <bitset>

using namespace ClientGUI_lib;
using namespace Sizes;
using namespace std;

//------------------------------------------------------------------------------

struct ClientGUI_lib::asynch_send_dat {
    asynch_send_dat(ConnectHub* obj, Message_t message, char* report)
        : own(obj), mesg(message), rep(report) { }
    asynch_send_dat(asynch_send_dat&& asd)
        : own(asd.own), mesg(asd.mesg), rep(asd.rep) { }

    ConnectHub* own;
    Message_t mesg;
    char* rep;
};

//------------------------------------------------------------------------------

const enum IP_def {
    NOTIP = -1, ISIP = -2, 
    IPDOTCNT = 3, IPNUMCNT = 4,
    MAXIPLENGTH = 15, MINIPLENGTH = 7
};

//------------------------------------------------------------------------------

const enum SEND_report {
    SENDGOOD = int('g'),
    SENDFAIL = int('f')
};

//------------------------------------------------------------------------------

const char TURNON{ 'o' };
const char SETLEVEL{ 's' };
const char GETDEVICES{ 'g' };
const float SYMBLEN{ 7.3 };

//------------------------------------------------------------------------------
//---Functions-assistants-------------------------------------------------------
//------------------------------------------------------------------------------
/*
 *  Edits message
 *  to fit window
 */
string message_loger(string message, Widget& win)
{
    auto ptr = message.begin();
    const int length{ int(win.get_width() / SYMBLEN) };
    int i{ 0 };

    while (ptr != message.end()) 
    {
        if (*ptr == '\n')
            i = 0;
        if (i == length)
        {
            i = 0; 
            ptr = message.insert(ptr, '\n');
        }
        ++ptr;
        ++i;
    }
    return message;
}

//------------------------------------------------------------------------------
/*
 *  Check if the entered
 *  data is ip address
 */
int is_ipaddr(string addr)
{
    if (MAXIPLENGTH < addr.length()||
        addr.length() < MINIPLENGTH)
        return NOTIP;

    int dot_cnt{ 0 };
    try {
        int i{ 0 };

        for_each(addr.begin(), addr.end(),
            [&i, &dot_cnt](char& ch) 
            {
                if (ch == '.') 
                { 
                    ch = ' '; 
                    ++i; 
                    ++dot_cnt;
                    return; 
                }

                if (ch < -1 || !isdigit(ch))
                    throw i;

                i += 2;
            });
    }
    catch (int position) {
        return position;
    }

    if (dot_cnt != IPDOTCNT)
        return NOTIP;

    istringstream nums{ addr };
    int position{ 0 };
    int num_cnt{ 0 };

    for (int num{ 0 }; nums >> num; ) 
    {
        if (MAXBYTE < num)
            return position;

        position += nums.gcount();
        ++num_cnt;
    }

    if (num_cnt != IPNUMCNT)
        return NOTIP;

    return ISIP;
}

//------------------------------------------------------------------------------
/*
 *  Generates exceptions and error text
 *  when ip address entered incorrectly
 */
void ip_check(string addr)
{
    int input_err = is_ipaddr(addr);
    switch (input_err) 
    {
    case ISIP:
        return;
    case NOTIP:
        throw runtime_error("введенные данные не являются IP адресом");
    default:
        string mesg{ "ошибка при вводе IP адреса:\n\n\t" };
        mesg += addr + "\n\t" + string(input_err, ' ') + '^';
        throw runtime_error(mesg);
    }
}

//------------------------------------------------------------------------------

ostream& ClientGUI_lib::operator<< (ostream& os, MacAddr addr)
{
    ostringstream info;

    for (uint8_t& i : addr) 
    {
        info << hex << long(i)
             << (&i == &addr.back() ? "" : ".");
    }

    return os << info.str();
}

//------------------------------------------------------------------------------
//---Asynchronous-functions-----------------------------------------------------
//------------------------------------------------------------------------------
/*
 *  Keystroke handler 
 */
void ClientGUI_lib::keyboard_check(ConnectHub* own)
{
    stringstream info;

    while (own->started) 
    {
        for (auto ptr{ own->devices.begin() }; 
             ptr != own->devices.end(); ++ptr) 
        {
            int state = Fl::event_key(ptr->type);

            if (state != ptr->state)
                ptr->state = state;
            if (state < 0)
                own->send_mesg({ ptr->addr, TURNON, ptr->value });
        }
        Sleep(90);
    }
}

//------------------------------------------------------------------------------
/*
 *  Asio socket sending interface 
 */
void ClientGUI_lib::asynch_send(asynch_send_dat dat)
{
    Network::NetErr res;
    res = dat.own->esp32.send(&dat.mesg, sizeof(Message_t));

    if (dat.rep == nullptr)
        return;

    if (!res) 
    {
        *(dat.rep) = SENDGOOD;
        return;
    }

    *(dat.rep) = SENDFAIL;
}

//------------------------------------------------------------------------------
/*
 *  Control key assigment handler 
 */
void ClientGUI_lib::asynch_keyset(ConnectHub* own)
try{
    stringstream info;
    int pressed{ 0 };

    while (Fl::event_key(pressed) == 0) 
    {
        pressed = Fl::event_key();
    }

    int value = own->vl_range.value() * 100;
    int dev_num = own->_get_device_num();
    char setted{ char(0) };

    own->send_mesg(
        { own->devices[dev_num].addr, SETLEVEL, value }, 
        &setted
    );

    while (setted == char(0)) 
        /* wait for result */;

    switch (setted) 
    {
    case SENDGOOD:
        info << "Клавиша \'" << char(pressed) << "\' управляет устройством [MAC: " 
             << own->devices[dev_num].addr
             << " ] со скоростью = " << value;

        own->devices[dev_num].type = char(pressed);
        own->devices[dev_num].value = value;
        break;

    case SENDFAIL:
        info << "Не удалось связаться с устройством [MAC: "
             << own->devices[dev_num].addr
             << " ] для подтверждения установки управляющей клавиши \'"
             << char(pressed) << "\'.\nПопробуйте повторить настройку";
        break;

    default:
        info << "Неизвестная ошибка при настройке управляющей клавиши,"
             << " повторите попытку и при необходимости свяжитесь с разработчиком";
        break;
    }
   
    own->log_out(info.str(), true);
}
catch (runtime_error& err) {
    stringstream info;
    info << "ERROR: " << err.what();
    own->log_out(info.str(), true);
}

//------------------------------------------------------------------------------
/*
 *  Inclusion value assigment handler
 */
void ClientGUI_lib::asynch_setspeed(ConnectHub* own)
try {
    stringstream info;
    info.str("");

    int value = own->vl_range.value() * 100;
    int dev_num = own->_get_device_num();
    char setted{ char(0) };

    own->send_mesg(
        { own->devices[dev_num].addr, SETLEVEL, value }, 
        &setted
    );

    while (setted == char(0))
        /* wait for result */;

    switch (setted) 
    {
    case SENDGOOD:
        info << "Скорость вращения устройства [MAC: "
             << own->devices[dev_num].addr
             << " ] изменена, новое значение = "
             << value;

        own->devices[dev_num].value = value;
        break;

    case SENDFAIL:
        info << "Не удалось связаться с устройством [MAC: " 
             << own->devices[dev_num].addr
             << " ] для подтверждения настройки скорости вращения."
             << "\nПопробуйте повторить настройку";
        break;

    default:
        info << "Неизвестная ошибка при попытке изменить скорость вращения устройства,"
             << " повторите попытку и при необходимости свяжитесь с разработчиком";
        break;
    }
   
    own->log_out(info.str(), true);
}
catch (runtime_error& err) {
    stringstream info;
    info << "ERROR: " << err.what();
    own->log_out(info.str(), true);
}

//------------------------------------------------------------------------------
/*
 *  Address table receipt and display handler 
 */
void ClientGUI_lib::asynch_write_message(ConnectHub* own)
try{
    ostringstream info;
    vector<MacAddr> received_data;

    for (MacAddr buf; own->esp32.read(&buf, sizeof(MacAddr)); ) 
    {
        if (buf == MacAddr())
            break;

        received_data.push_back(buf);
    }

    for_each(
        received_data.begin(),
        received_data.end(),
        [own](MacAddr addr) 
        {
            for (Device_t& i : own->devices)
                if (addr == i.addr) 
                    return;

            own->devices.push_back({ addr });
        }
    );

    {
        auto ptr = own->devices.begin();
        while (ptr != own->devices.end()) 
        {
            bool disconnect{ true };
            for (MacAddr i : received_data) 
            {
                if (ptr->addr == i) 
                {
                    disconnect = false;
                    break;
                }
            }

            if (disconnect)
                ptr = own->devices.erase(ptr);
            else
                ++ptr;
        }
    }

    int num_column{ 5 }, addr_column{ 19  }; 
    info << "Получена таблица адресов [ доступных устройств: "
         << received_data.size() << " ] {\n";

    for (int i{ 0 }; i < own->devices.size(); ++i) 
    {
        info << setw(num_column) << "№" << i + 1 << ":"
             << setw(addr_column) << own->devices[i].addr 
             << " клавиша: "
             << ( own->devices[i].type == -1 ? '-' : own->devices[i].type )
             << " ; уровень: " << setw(3);

        if (own->devices[i].value == -1)
            info << "-\n";
        else
            info << own->devices[i].value << "%" << '\n';
    }

    info << "};";
    own->log_out(info.str(), true);
}
catch (Network::NetException& e) {
    ostringstream info;
    info << "ERROR: " << e.what() << endl;
    own->log_out(info.str(), true);
}

//------------------------------------------------------------------------------
//---Vidgets--------------------------------------------------------------------
//------------------------------------------------------------------------------

void Multiline_out_box::put(const string& s)
{
    reference_to<Fl_Multiline_Output>(pw).value(s.c_str());
}

//------------------------------------------------------------------------------

void Multiline_out_box::set_textsize(int s)
{
    reference_to<Fl_Multiline_Output>(pw).textsize(s);
}

//------------------------------------------------------------------------------

void Multiline_out_box::attach(Window& win)
{
    pw = new Fl_Multiline_Output(loc.x, loc.y, width, height, label.c_str());
    own = &win;
}

//------------------------------------------------------------------------------

void Binary_button::attach(Window& win)
{
    pw = new Fl_Round_Button(loc.x, loc.y, width, height, label.c_str());
    own = &win;
}

//------------------------------------------------------------------------------

bool Binary_button::state()
{
    return static_cast<Fl_Round_Button*>(pw)->value();
}

//------------------------------------------------------------------------------

void Value_slider::attach(Window& win)
{
    pw = new Fl_Value_Slider(loc.x, loc.y, width, height, label.c_str());
    pw->type(FL_HOR_NICE_SLIDER);
    own = &win;
}

//------------------------------------------------------------------------------

double Value_slider::value()
{
    return static_cast<Fl_Slider*>(pw)->value();
}

//------------------------------------------------------------------------------

int Value_slider::set(int windowtop, int windowsize, int first, int totalsize)
{
    return static_cast<Fl_Slider*>(pw)->scrollvalue(
        windowtop, windowsize, first, totalsize
    );
}

//------------------------------------------------------------------------------
//---Main-window-class----------------------------------------------------------
//------------------------------------------------------------------------------

ConnectHub::ConnectHub(Point xy)
    : Window(xy, width, height, "Drill Symphony v1.0"),
    out_main({ xofs, yofs + boxheight * 2 }, outboxwid, outboxlen, ""),
    in_main({ xofs + texofs, yofs }, 
            boxlen3 - boxlen1, boxheight, "IP:"),
    b_connect({ xofs + texofs + boxlen3 - boxlen1, yofs },
              boxlen1 * 2, boxheight, "connect", cb_connect),
    b_disconnect({ xofs, yofs + boxheight * 2 + outboxlen },
                 boxlen2, boxheight, "disconnect", cb_disconnect),
    b_info({ xofs, yofs + boxheight },
           texofs, boxheight, "info", cb_info),
    b_setkey({ xofs + texofs + boxlen3 - boxlen1, yofs },
             boxlen1, boxheight, "set key", cb_setkey),
    b_setspeed({ xofs + texofs + boxlen3, yofs + boxheight },
               boxlen1, boxheight, "set speed", cb_setspeed),
    b_start({ xofs + texofs + boxlen3, yofs },
            boxlen1, boxheight, "start", cb_start),
    b_stop({ xofs + texofs + boxlen3 - boxlen1, yofs },
           boxlen1 * 2, boxheight, "stop", cb_stop),
    b_quit({ xofs + boxlen2, yofs + boxheight * 2 + outboxlen },
           boxlen2, boxheight, "quit", cb_quit),
    vl_range({ xofs + texofs, yofs + boxheight },
             boxlen3, boxheight, "")
{
    attach(out_main);
    attach(in_main);
    attach(b_connect);
    attach(b_disconnect);
    attach(b_info);
    attach(b_setkey);
    attach(b_setspeed);
    attach(b_start);
    attach(b_stop);
    attach(b_quit);
    attach(vl_range);
    show_disconnected();
}

//------------------------------------------------------------------------------
/*
 *  Hides widgets needed to interact
 *  with connected devices and shows
 *  widgets needed to connect
 */
void ConnectHub::show_disconnected()
{
    b_disconnect.hide();
    b_setkey.hide();
    b_setspeed.hide();
    vl_range.hide();
    b_start.hide();
    b_stop.hide();
    b_connect.show();
    log_out("Устройства отключены");
    connected = false;
}

//------------------------------------------------------------------------------
/*
 *  Shows widgets needed to interact
 *  with connected devices and hides
 *  widgets needed to connect
 */
void ConnectHub::show_connected()
{
    b_connect.hide();
    b_disconnect.show();
    b_setkey.show();
    b_setspeed.show();
    vl_range.show();
    b_start.show();
    b_stop.hide();
    connected = true;
}

//------------------------------------------------------------------------------
/*
 *  Sends a request to recieve a table of available
 *  device's mac addresses and starts an asynchronous
 *  task of decrypting and displaying recieved message
 */
void ConnectHub::device_check()
{
    send_mesg(GETDEVICES);
    thread(&asynch_write_message, this).detach();
}

//------------------------------------------------------------------------------
/*
 *  Reads and checks enterd mac address,
 *  tries to establish connection if
 *  data is correct
 */
void ConnectHub::f_connect()
try{
    string addr{ in_main.get_string() };
    ip_check(addr);

    Network::NetErr sock_state = esp32.connect(addr);

    if (sock_state) 
    {
        esp32.disconnect();
        log_out(sock_state.message());
        return;
    }

    device_check();
    show_connected();
}
catch (runtime_error& err) {
    stringstream info;
    info << "ERROR: " << err.what();
    log_out(info.str());
}

//------------------------------------------------------------------------------

void ConnectHub::f_disconnect()
{
    if (started) 
        started = false;
    
    Network::NetErr sock_state = esp32.disconnect();

    if (sock_state) 
    {
        log_out(sock_state.message());
        return;
    }

    show_disconnected();
}

//------------------------------------------------------------------------------
/*
 *  Displays useful infomation
 *  depending on application state
 */
void ConnectHub::f_info()
{
    if (started) 
    {
        stringstream info;
        for (auto ptr = devices.begin(); ptr != devices.end(); ++ptr) 
        {
            info << "MAC: [ " << ptr->addr
                 << " ]; key: [ " << ptr->type
                 << " ] level = " << ptr->value << '\n';
        }

        if (!devices.size()) 
        { 
            info << "Вы еще не настроили управление ни одним устройством,"
                 << " попробуйте нажать [info] для просмотра доступных устройств"; 
        }

        log_out(info.str());
    }
    else if (connected)
        device_check();
    
    else 
    {
        stringstream info;
        info << "Инструкция по подключению:\n"
             << "1)Скачать на смартфон приложение Esptouch\n"
             << "2)Подключить смартфон к WiFi роутеру и открыть Esptouch\n"
             << "3)Выбрать верхний пункт меню, в открывшемся окне установить Device count = 1 и Broadcast\n"
             << "4)При наличии пароля - ввести его в поле password, включить ОДНО! устройство, "
             << "дождаться мигающего красного сигнала, нажать confirm\n"
             << "5)Дождаться подтверждения подключения в Еsptouch, ввести в компьютерном приложении IP устройства\n"
             << "6)Зеленый мигающий сигнал устройства - подтверждение готовности к работе, на компьютере нажать connect\n"
             << "7)Включить остальные устройства и нажать confirm в Esptouch, зеленый мигающий сигнал - устройство "
             << "подключено и готово к работе\n";

        log_out(info.str());
    }
}

//------------------------------------------------------------------------------
/*
 *  Edits and displays string in the window,
 *  updates the graphics
 */
void ConnectHub::log_out(string message, bool redraw) 
{ 
    out_main.put(message_loger(message, out_main));

    if (redraw) 
    {
        this->draw();
        Fl::redraw();
    }
}

//------------------------------------------------------------------------------
/*
 *  Starts an asynchronous task of assigning
 *  control key to connected device
 */
void ConnectHub::f_setkey()
{
    stringstream info;
    info << "Установите скорость вращения слайдером и нажмите клавишу,"
         << " которая будет управлять устройством под введенным номером с заданной интесивностью\n";
    log_out(info.str());

    thread(&asynch_keyset, this).detach();
}

//------------------------------------------------------------------------------
/*
 *  Starts an asynchronous task of monitoring
 *  keystrokes and sending control instructions
 */
void ConnectHub::f_start()
{ 
    started = true; 
    b_setkey.hide(); 
    b_setspeed.hide();
    b_start.hide(); 
    b_stop.show(); 
    device_check();
    thread(&keyboard_check, this).detach(); 
}

//------------------------------------------------------------------------------

void ConnectHub::f_stop() 
{ 
    started = false; 
    b_stop.hide(); 
    b_setkey.show();
    b_setspeed.show();
    b_start.show(); 
}

//------------------------------------------------------------------------------
/*
 *  Transfers data to be sent to an
 *  asynchronous message forwarding task
 */
void ConnectHub::send_mesg(Message_t mesg, char* report)
{
    thread(&asynch_send, asynch_send_dat{ this, mesg, report }).detach();
}

//------------------------------------------------------------------------------
/*
 *  Starts an asynchronous task of setting
 *  inclusion value to connected device
 */
void ConnectHub::f_setspeed()
{
    std::thread(&asynch_setspeed, this).detach();
}

//------------------------------------------------------------------------------
/*
 *  Checks whether entered data matches
 *  relevant number of connected device
 *  This function shouldn't be called
 *  outside exception handler
 */
int ConnectHub::_get_device_num()
{
    if (devices.size() == 0)
        throw runtime_error(
            "устройства не авторизованы, попробуйте нажать [info] для авторизации"
        );

    string check{ in_main.get_string() };
    int num{ -1 };

    try {
        int i{ 0 };

        for_each(check.begin(), check.end(),
            [&i](char& ch) 
            {
                if (ch < -1 || !isdigit(ch))
                    throw i;

                i += 2;
            });
    }
    catch (int position) {
        num = position;
    }

    if (num != -1) 
    {
        string info{ "введенные данные не являются номером\n\n\t" };
        info += check + "\n\t" + string(num, ' ') + "^";
        throw runtime_error(info);
    }

    num = in_main.get_int();

    if (devices.size() < num || num < 1)
        throw runtime_error("введен недействительный номер устройства");

    return num - 1;
}