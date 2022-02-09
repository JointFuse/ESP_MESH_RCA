/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  17.10.2021
 */
#include "asio.hpp"
#include <string>
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <cassert>
#include "project_defs.h"

static_assert(MESSAGE_SIZE <= ASIO_MESSAGE_SIZE);

using asio::ip::tcp;

extern "C" void asio_sess_write(void* data, int size);
extern "C" esp_err_t receive_queue_add(InMessage_t * message);
extern "C" esp_err_t send_queue_get(OutMessage_t * buf);

extern int store_new_message(void* data, int size);
extern bool take_new_message(void* buf);

class session
  : public std::enable_shared_from_this<session>
{
    tcp::socket socket_;
    enum { max_length = ASIO_MESSAGE_SIZE };
    uint8_t in_data_[max_length];
    //uint8_t out_data_[max_length];
public:
  session(tcp::socket socket)
    : socket_(std::move(socket))
  { 
  }

  void start()
  {
      do_read();
  }
private:
  void do_read()
  {
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(in_data_, max_length),
        [this, self](std::error_code ec, std::size_t length)
        {   
            if (!ec) {
                in_data_[length] = 0;
                int i;
                for (i = 0; i <= (length - sizeof(InMessage_t)); i += sizeof(InMessage_t))
                    store_new_message(&in_data_[i], sizeof(InMessage_t));

                if (length && in_data_[0] == GETDEVICES) {
                    while (!take_new_message(in_data_))
                        /* wait */;
                    do_write();
                }
                else
                    do_read();
            }
        });
  }

  void do_write()
  {
    int length = sizeof(OutMessage_t);

    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(in_data_, length),
        [this, self](std::error_code ec, std::size_t length)
        {
            if (!ec) {
                if (take_new_message(in_data_))
                    do_write();
                else
                    do_read();
            }
        });
  }
};

class server
{
public:
  server(asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(
        [this](std::error_code ec, tcp::socket socket)
        {
          if (!ec) {
            std::make_shared<session>(std::move(socket))->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
};

extern "C" void asioserver_start()
{
    asio::io_context io_context;
    server s(io_context, std::atoi(CONFIG_ASIO_SERVER_PORT));
    
    std::cout << "ASIO engine is up and running" << std::endl;

    io_context.run();
    std::cerr << "ASIO engine is down" << std::endl;
}

int store_new_message(void* data, int size)
{
    if (sizeof(InMessage_t) < size)
        return 0;

    InMessage_t* mesg = static_cast<InMessage_t*>(data);
    if (receive_queue_add(mesg) == ESP_OK)
        return 1;
    return 0;
}

bool take_new_message(void* buf)
{
    if (send_queue_get(static_cast<OutMessage_t*>(buf)) != ESP_OK)
        return false;
    return true;
}