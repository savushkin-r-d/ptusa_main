#ifndef l_tcp_client_h__
#define l_tcp_client_h__
#include "tcp_client.h"
#include <sys/socket.h>

class linux_tcp_client: public tcp_client
    {
    public:
        int InitLib();
        void DeinitLib();
        int Connect();
        void Disconnect();
        linux_tcp_client(const char* client_ip, unsigned int client_port, unsigned int client_id, unsigned char alarm_subclass, unsigned int exchange_buf_size = 256, unsigned long send_receive_timeout = 100);
        virtual int Communicate( unsigned int bytestosend ) override;
        ~linux_tcp_client();

    };


#endif // l_tcp_client_h__
