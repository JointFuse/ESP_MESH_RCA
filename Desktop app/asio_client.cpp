/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  17.10.2021
 */
#include "asio_client.h"

using namespace std;
using namespace Network;

Web_client::Web_client()
	: io_context(), sock(io_context) { }

Web_client::~Web_client()
{
	if (sock.is_open()) {
		sock.cancel();
		sock.close();
	}
}

NetErr Web_client::connect(std::string ip_addr)
{
	if (sock.is_open())
		return NetErr();
	ep = { NetIP::from_string(ip_addr), 80 };
	NetErr res;
	sock.connect(ep, res);
	return res;
}

NetErr Web_client::disconnect()
{
	if (!sock.is_open())
		return NetErr();
	NetErr res;
	sock.close(res);
	return res;
}

std::string Web_client::get_addres()
{
	return ep.address().to_string();
}

NetErr Web_client::send(void* data, int bytes)
try{
	sock.send(boost::asio::buffer(data, bytes));
	return NetErr();
}
catch (boost::system::system_error& err) {
	return err.code();
}

int Web_client::read(void* data, int bytes)
{
	return sock.receive(boost::asio::buffer(data, bytes));
}