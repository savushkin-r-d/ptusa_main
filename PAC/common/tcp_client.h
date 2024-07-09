#ifndef tcp_client_h__
#define tcp_client_h__

#include <cstdio>

#include "dtime.h"

#define RECONNECT_MIN_MULTIPLIER 5
#define RECONNECT_MAX_MULTIPLIER 30

class tcp_client {
 public:
  enum ASYNCRESULT {
    AR_SOCKETERROR = -3,
    AR_TIMEOUT = -2,
    AR_BUSY = -1,
    AR_FREE = 0,
  };

  enum ASYNCCONNECTSTATE {
    ACS_CONNECTED = 1,
    ACS_DISCONNECTED = 0,
    ACS_CONNECTING = -1,
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
  int connectedstate;
  int id;
  unsigned char subclass;
  unsigned char is_initialized;
  int async_result;

 public:
  static tcp_client* Create(const char* ip, unsigned int port, unsigned int id,
                            unsigned char alarm_subclass,
                            unsigned int exchange_buf_size = 256,
                            unsigned long send_receive_timeout = 100);

  virtual int Communicate(unsigned int bytestosend);

  int checkConnection();
  virtual int AsyncConnect();
  virtual int AsyncSend(unsigned int bytestosend);

  virtual int AsyncRecive();
  unsigned int buff_size;
  virtual int get_socket();
  virtual void Disconnect() = 0;
  int get_id();
  virtual int get_async_result();
  virtual int get_connected_state();
  int set_async_result(int ar);
  char* buff;
  tcp_client(const char* client_ip, unsigned int client_port,
             unsigned int client_id, unsigned char alarm_subclass,
             unsigned int exchange_buf_size = 256,
             unsigned long send_receive_timeout = 100);
  virtual ~tcp_client();
};
#endif  // tcp_client_h__
