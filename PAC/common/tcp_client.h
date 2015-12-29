#ifndef tcp_client_h__
#define tcp_client_h__
#include "PAC_err.h"
#include "tcp_cmctr.h"

#define RECONNECT_MIN_MULTIPLIER 5
#define RECONNECT_MAX_MULTIPLIER 30

class tcp_client
    {
    public:
        enum ASYNCRESULT
            {
            AR_SOCKETERROR = -3,
            AR_TIMEOUT = -2,
            AR_BUSY = -1,
            AR_FREE = 0,
            };

        unsigned long async_timeout;
        unsigned long async_queued;
        unsigned long async_last_connect_try;
        unsigned long connectTimeout;
        unsigned long reconnectTimeout;
        unsigned long maxreconnectTimeout;
        char* ip;
    protected:
        unsigned int port;
        int socket_number;
        unsigned long timeout;
        char connectedstate;
        int id;
        unsigned char subclass;
        unsigned char is_initialized;
        int async_result;
    public:
        static tcp_client* Create(char* ip, unsigned int port, unsigned int id, unsigned char alarm_subclass,
            unsigned int exchange_buf_size = 256, unsigned long send_receive_timeout = 100);
        virtual int Communicate(unsigned int bytestosend);
        virtual int AsyncSend(unsigned int bytestosend);
        unsigned int buff_size;
        virtual int get_socket();
        virtual void Disconnect() = 0;
        int get_id();
        int get_async_result();
        int set_async_result(int ar);
        char* buff;
        tcp_client(const char* client_ip, unsigned int client_port, unsigned int client_id, unsigned char alarm_subclass, unsigned int exchange_buf_size = 256, unsigned long send_receive_timeout = 100);
        virtual ~tcp_client();
    };
#endif // tcp_client_h__
