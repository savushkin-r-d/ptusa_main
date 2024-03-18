#include "tcp_cmctr_tests.h"
#include "dtime.h"
#include "fmt/chrono.h"
#ifdef WIN_OS
#include "w_tcp_cmctr.h"
#else
#include "l_tcp_cmctr.h"
#endif

using namespace ::testing;

TEST(tcp_communicator, get_millisec_test)
{
    unsigned long milliseconds = get_millisec();

    ASSERT_GE(milliseconds, 0);
}

TEST(tcp_communicator, get_host_name_eng_test)
{
    const char* name_rus = "";
    const char* name_eng = "";

#ifdef WIN_OS
    tcp_communicator_win* communicator = new tcp_communicator_win( name_rus, name_eng );
#else
    tcp_communicator_linux* communicator = new tcp_communicator_linux( name_rus, name_eng );
#endif

    char* host_name = communicator->get_host_name_eng();

    ASSERT_NE(host_name, nullptr);
}