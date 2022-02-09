/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  04.10.2021
 */
#ifndef TextGraph_h
#define TextGraph_h

#include "GUI/Window.h"
#include "GUI/my_graph.h"
#include "GUI/Point.h"
#include <thread>
#include <array>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "asio_client.h"

namespace TextGraph_lib {
    using namespace std;
	using namespace Graph_lib;
    using MacAddr = array<uint8_t, 6>;

    //------------------------------------------------------------------------------
    //---General-display-settings---------------------------------------------------
    //------------------------------------------------------------------------------

    namespace Sizes {
        // Pixel dimensions for window and widgets
        constexpr int x_frame{ 8 };
        constexpr int y_frame{ 8 };
        constexpr int xofs{ 20 };
        constexpr int yofs{ 20 };
        constexpr int texofs{ 40 };
        constexpr int boxheight{ 25 };
        constexpr int boxlen1{ 80 };
        constexpr int outboxwid{ 450 };
        constexpr int outboxlen{ 250 };
        constexpr int boxlen2{ outboxwid / 2 };
        constexpr int boxlen3{ outboxwid - texofs - boxlen1 };
        constexpr int width{ xofs * 2 + outboxwid + x_frame };
        constexpr int height{ yofs * 2 + boxheight * 3 + outboxlen+ y_frame };
    }

    //------------------------------------------------------------------------------
    
    namespace ErrText {
        const string folder_err{ "file may not be in the programm folder" };
        const string name_err{ "file name may be entered incorrectly" };
        const string opend_err{ "file may not have been uploaded" };
    }

    //------------------------------------------------------------------------------
    //---Data-organization----------------------------------------------------------
    //------------------------------------------------------------------------------

    extern class ConnectHub;
    extern struct asynch_send_dat;

    struct Message_t {
        // Working type to simplify
        // data transfer to methods
        // and storage of connected
        // device data
        Message_t(char command)
            :type(command),
            addr{ 0, 0, 0, 0, 0, 0 },
            value(0)
        { }
        Message_t(MacAddr device, char command = -1, int level = -1)
            :type(command), addr(device), value(level) { }

        char type;      // type of instruction transmitted
        MacAddr addr;   // recipient's mac address
        int value;      // optional field, using with SETLEVEL instruction
    };

    struct Device_t : Message_t {
        // Data about connected
        // device and its state
        Device_t(Message_t device)
            :Message_t(device), state(0) { }

        int state;
    };
    
    //------------------------------------------------------------------------------
    //---Asynchronous-functions-----------------------------------------------------
    //------------------------------------------------------------------------------

    void keyboard_check(ConnectHub*);
    void asynch_keyset(ConnectHub*);
    void asynch_setspeed(ConnectHub*);
    void asynch_send(asynch_send_dat dat);
    void asynch_write_message(ConnectHub* own);

    //------------------------------------------------------------------------------
    //---Vidgets--------------------------------------------------------------------
    //------------------------------------------------------------------------------

    struct Multiline_out_box : Widget {
        Multiline_out_box(Point xy, int w, int h, const string& s)
            :Widget(xy, w, h, s, 0) { }
        void put(const string&);

        void set_textsize(int s);

        void attach(Window& win);
    };

    //------------------------------------------------------------------------------

    struct Binary_button : Widget {
        Binary_button(Point xy, int w, int h, const string& label)
            : Widget(xy, w, h, label) { }

        void attach(Window&);
        bool state();
    };
    
    //------------------------------------------------------------------------------

    struct Value_slider : Widget {
        Value_slider(Point xy, int w, int h, const string& label)
            : Widget(xy, w, h, label) { }

        void attach(Window&);
        double value();
        int set(int windowtop, int windowsize, int first, int totalsize);
    };

    //------------------------------------------------------------------------------
    //---Main-window-class----------------------------------------------------------
    //------------------------------------------------------------------------------

    class ConnectHub : Window {
        bool started{ false };
        bool connected{ false };
        Network::Web_client esp32;
        vector<Device_t> devices;

        // Vidges
        Multiline_out_box out_main;
        In_box in_main;

        Button b_connect;
        Button b_disconnect;
        Button b_info;
        Button b_setkey;
        Button b_setspeed;
        Button b_start;
        Button b_stop;
        Button b_quit;

        Value_slider vl_range;

        // process functions
        void show_disconnected();
        void show_connected();
        void log_out(string message, bool redraw = false);
        void device_check();
        int _get_device_num();

        void send_mesg(Message_t, char* report = nullptr);

        friend void keyboard_check(ConnectHub*);
        friend void asynch_keyset(ConnectHub*);
        friend void asynch_setspeed(ConnectHub*);
        friend void asynch_send(asynch_send_dat dat);
        friend void asynch_write_message(ConnectHub* own);

        // button functions
        void f_connect();
        void f_disconnect();
        void f_info();
        void f_setkey();
        void f_setspeed();
        void f_start();
        void f_stop();
        void f_quit() { f_disconnect(); hide(); throw runtime_error("program stoped"); }

        // Call back funstions
        static void cb_connect(Address, Address pw)     { reference_to<ConnectHub>(pw).f_connect(); }
        static void cb_disconnect(Address, Address pw)  { reference_to<ConnectHub>(pw).f_disconnect(); }
        static void cb_info(Address, Address pw)        { reference_to<ConnectHub>(pw).f_info(); }
        static void cb_setkey(Address, Address pw)      { reference_to<ConnectHub>(pw).f_setkey(); }
        static void cb_setspeed(Address, Address pw)    { reference_to<ConnectHub>(pw).f_setspeed(); }
        static void cb_start(Address, Address pw)       { reference_to<ConnectHub>(pw).f_start(); }
        static void cb_stop(Address, Address pw)        { reference_to<ConnectHub>(pw).f_stop(); }
        static void cb_quit(Address, Address pw)        { reference_to<ConnectHub>(pw).f_quit(); }
    public:
        ConnectHub(Point xy);
    };

    ostream& operator<< (ostream&, MacAddr);
}
#endif	// !TextGraph_h