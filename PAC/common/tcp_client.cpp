#include "tcp_client.h"

#ifdef WIN_OS
#include "w_tcp_client.h"
#endif //WIN_OS

tcp_client::tcp_client( const char* client_ip, unsigned int client_port, unsigned int client_id, unsigned char alarm_subclass, unsigned int exchange_buf_size /*= 256*/, unsigned long send_receive_timeout /*= 100*/ )
	{
	ip = new char[16];
	strcpy(ip, client_ip);
	port = client_port;
	id = client_id;
	subclass = alarm_subclass;
	buff = new char[exchange_buf_size];
	buff_size = exchange_buf_size;
	timeout = send_receive_timeout;
	socket_number = 0;
	connectedstate = 0;
	is_initialized = 0;
	}

tcp_client::~tcp_client()
	{
	delete ip;
	delete buff;
	}

tcp_client* tcp_client::Create( char* ip, unsigned int port, unsigned int id, unsigned char alarm_subclass, unsigned int exchange_buf_size /*= 256*/, unsigned long send_receive_timeout /*= 100*/ )
	{
#ifdef WIN_OS
	return new win_tcp_client(ip, port, id, alarm_subclass, exchange_buf_size, send_receive_timeout);
#endif // WIN_OS
	return 0;
	}

int tcp_client::Communicate( unsigned int bytestosend )
	{
	return 0;
	}

int tcp_client::get_id()
	{
	return id;
	}


