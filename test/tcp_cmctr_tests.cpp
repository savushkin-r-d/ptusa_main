#include "tcp_cmctr_tests.h"
#include "dtime.h"
#include "fmt/chrono.h"
#include "w_tcp_cmctr.h"

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

    tcp_communicator_win* communicator = new tcp_communicator_win( name_rus, name_eng );

    char* host_name = communicator->get_host_name_eng();

    ASSERT_NE(host_name, nullptr);
}