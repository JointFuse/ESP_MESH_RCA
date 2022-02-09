/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  17.10.2021
 */
#ifndef asio_client_h
#define asio_client_h

#include <iostream>
#include <string>

#include <boost/asio.hpp>

namespace Network {
    using NetErr = boost::system::error_code;
    using NetException = boost::system::system_error;
    using NetIP = boost::asio::ip::address;

    using boost::asio::ip::tcp;

    class Web_client {
        boost::asio::io_context io_context;
        tcp::endpoint ep{};
        tcp::socket sock;
    public:
        Web_client();

        NetErr connect(std::string ip_addr);
        NetErr disconnect();
        NetErr send(void* data, int bytes);
        int read(void* data, int bytes);

        std::string get_addres();

        ~Web_client();
    };
}
#endif