#ifndef l_tcp_client_h__
#define l_tcp_client_h__
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include "l_tcp_cmctr.h"
#include "tcp_client.h"

class linux_tcp_client : public tcp_client {
 private:
  unsigned long async_startconnnect;
  timeval async_tv;
  sockaddr_in async_sock_address;
  unsigned int async_bytes_to_send;
  u_long asyncReciveTime;
  bool newDataIsAvailable;

 public:
  int Connect();
  int AsyncConnect() override;
  void Disconnect();
  virtual int Communicate(unsigned int bytestosend);
  virtual int AsyncSend(unsigned int bytestosend);
  int AsyncRecive() override;
  virtual int get_async_result() override;
  linux_tcp_client(const char* client_ip, unsigned int client_port,
                   unsigned int client_id, unsigned char alarm_subclass,
                   unsigned int exchange_buf_size = 256,
                   unsigned long send_receive_timeout = 100);
  ~linux_tcp_client();
};

#endif  // l_tcp_client_h__
