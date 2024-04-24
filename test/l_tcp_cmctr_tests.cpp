#include "l_tcp_cmctr_tests.h"

#include "dtime.h"

using namespace ::testing;

#ifndef WIN_OS  // For linux to deal with __stdcall.
#define __stdcall
#endif

int __stdcall fail_recvtimeout(int, unsigned char *, int, long, long,
                               const char *, const char *, stat_time *, char) {
  return -1;
}

int __stdcall checkbuff_true(int) { return 1; }

#ifdef LINUX_OS

TEST(tcp_communicator_linux, evaluate) {
  G_CMMCTR->init_instance("Тест", "Test");
  EXPECT_EQ(0, G_CMMCTR->evaluate());

  linux_tcp_client cl("127.0.0.1", 10000, 1, 1);
  cl.checkConnection();
  cl.reconnectTimeout = 0;
  cl.checkConnection();
  cl.Connect();

  EXPECT_EQ(0, G_CMMCTR->evaluate());

  cl.buff[0] = 33;
  cl.buff[1] = 33;
  cl.AsyncSend(2);
  cl.AsyncRecive();
  cl.async_timeout = 0;
  cl.AsyncRecive();
  cl.checkConnection();

  sleep(0);
  EXPECT_EQ(0, G_CMMCTR->evaluate());

  EXPECT_EQ(0, G_CMMCTR->evaluate());

  subhook_t fail_recv = subhook_new(
      reinterpret_cast<void *>(tcp_communicator_linux::recvtimeout),
      reinterpret_cast<void *>(fail_recvtimeout), SUBHOOK_64BIT_OFFSET);
  subhook_t checkb_true = subhook_new(
      reinterpret_cast<void *>(tcp_communicator_linux::checkBuff),
      reinterpret_cast<void *>(checkbuff_true), SUBHOOK_64BIT_OFFSET);
  subhook_install(fail_recv);
  subhook_install(checkb_true);
  cl.AsyncRecive();
  subhook_remove(fail_recv);
  subhook_free(fail_recv);
  subhook_remove(checkb_true);
  subhook_free(checkb_true);
  fail_recvtimeout(0, nullptr, 0, 0, 0, nullptr, nullptr, nullptr, 0);
  checkbuff_true(0);
}

TEST(tcp_communicator_linux, checkBuff) {
  int s = socket(AF_INET, SOCK_STREAM, 0);

  auto res = tcp_communicator_linux::checkBuff(s);
  EXPECT_TRUE(res);

  res = tcp_communicator_linux::checkBuff(-10);

  EXPECT_FALSE(res);
}

#endif
