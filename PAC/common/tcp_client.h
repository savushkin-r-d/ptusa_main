#ifndef tcp_client_h__
#define tcp_client_h__
#include "PAC_err.h"


class tcp_client
	{
	protected:
		char* ip;
		unsigned int port;
		int socket_number;
		unsigned long timeout;
		char connectedstate;
		int id;
		unsigned char subclass;
		unsigned char is_initialized;
	public:
		static tcp_client* Create(char* ip, unsigned int port, unsigned int id, unsigned char alarm_subclass,
			unsigned int exchange_buf_size = 256, unsigned long send_receive_timeout = 100);
		virtual int Communicate(unsigned int bytestosend);
		unsigned int buff_size;
		int get_id();
		char* buff;
		tcp_client(const char* client_ip, unsigned int client_port, unsigned int client_id, unsigned char alarm_subclass, unsigned int exchange_buf_size = 256, unsigned long send_receive_timeout = 100);
		virtual ~tcp_client();
	};
#endif // tcp_client_h__
