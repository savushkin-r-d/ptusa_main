#if !defined WIN_OS && !defined LINUX_OS
#error You must define OS!
#endif

#include "tcp_cmctr.h"

#include "tcp_client.h"

#ifdef WIN_OS
#include "w_tcp_cmctr.h"
#endif  // WIN_OS

#ifdef LINUX_OS
#include "l_tcp_cmctr.h"
#endif

auto_smart_ptr<tcp_communicator> tcp_communicator::instance = 0;
int tcp_communicator::port = 10000;
int tcp_communicator::port_modbus = 10502;
#ifdef PTUSA_TEST
bool tcp_communicator::is_init = false;
#endif  // PTUSA_TEST
//------------------------------------------------------------------------------
tcp_communicator::tcp_communicator() : in_buffer_count(0), pidx(0), net_id(0) {
  max_cycles = 10;
  glob_cmctr_ok = 1;
  for (int i = 0; i < TC_MAX_SERVICE_NUMBER; i++) services[i] = nullptr;

  clients = new std::map<int, tcp_client*>();
}
//------------------------------------------------------------------------------
tcp_communicator::srv_ptr tcp_communicator::reg_service(u_char srv_id,
                                                        srv_ptr fk) {
  if (services[srv_id] == nullptr) {
    services[srv_id] = fk;
    return nullptr;
  } else {
    return services[srv_id];
  }
}
//------------------------------------------------------------------------------
void tcp_communicator::_ErrorAkn(u_char error) {
  buf[0] = net_id;
  buf[1] = AKN_ERR;
  buf[2] = pidx;
  buf[3] = 0;
  buf[4] = 1;
  buf[5] = error;
  in_buffer_count = 6;
}
//------------------------------------------------------------------------------
void tcp_communicator::_AknData(u_long len) {
  buf[0] = net_id;
  buf[1] = AKN_OK;
  buf[2] = pidx;
  buf[3] = (len >> 8) & 0xFF;
  buf[4] = len & 0xFF;
  in_buffer_count = len + 5;
}
//------------------------------------------------------------------------------
void tcp_communicator::_AknOK() {
  buf[0] = net_id;
  buf[1] = AKN_OK;
  buf[2] = pidx;
  buf[3] = 0;
  buf[4] = 0;
  in_buffer_count = 5;
}
//------------------------------------------------------------------------------
tcp_communicator* tcp_communicator::get_instance() { return instance; }
//------------------------------------------------------------------------------
char* tcp_communicator::get_host_name_rus() { return host_name_rus; }
//------------------------------------------------------------------------------
char* tcp_communicator::get_host_name_eng() { return host_name_eng; }
//------------------------------------------------------------------------------
tcp_communicator::~tcp_communicator() { delete clients; }
//------------------------------------------------------------------------------
bool tcp_communicator::checkBuff( int s )
    {
    // Настраиваем  file descriptor set.
    fd_set fds;
    FD_ZERO( &fds );
    FD_SET( s, &fds );

    // Настраиваем время на таймаут.
    timeval rec_tv;
    rec_tv.tv_sec = 0;
    rec_tv.tv_usec = 0;

    // Ждем таймаута или полученных данных.
    int n = select( s + 1, &fds, nullptr, nullptr, &rec_tv );

    return n >= 1;
    }
//------------------------------------------------------------------------------
void tcp_communicator::init_instance( const char *name_rus, const char *name_eng )
    {
#ifdef PTUSA_TEST
  if (!is_init) {
    is_init = true;
#endif  // PTUSA_TEST
#ifdef WIN_OS
    instance = new tcp_communicator_win(name_rus, name_eng);
#endif  // WIN_OS

#ifdef LINUX_OS
    instance = new tcp_communicator_linux(name_rus, name_eng);
#endif
#ifdef PTUSA_TEST
  }
#endif  // PTUSA_TEST
}

int tcp_communicator::add_async_client(tcp_client* client) {
  clients[0][client->get_socket()] = client;
  client->async_queued = get_millisec();
  return 0;
}

int tcp_communicator::remove_async_client(tcp_client* client) {
  std::map<int, tcp_client*>::iterator it = clients->find(client->get_socket());
  if (it != clients->end()) {
    clients->erase(it);
  }
  return 0;
}

//------------------------------------------------------------------------------
