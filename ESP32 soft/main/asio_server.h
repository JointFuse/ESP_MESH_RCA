/*
 *	This header provides using c++ library
 *	boost::asio in c source files
 */
#ifndef asio_server_h
#define asio_server_h

/* 
 *	create socket and session to
 *	connect with remote app
 */
extern void asioserver_start();

/* asynch send mesg throw opend session */
extern void asio_sess_write(void* data, int size);

#endif