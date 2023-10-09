#include "tcp_cmctr_tests.h"
#include "dtime.h"


using namespace ::testing;

TEST(tcp_communicator, get_millisec_test)
{
    unsigned long milliseconds = get_millisec();

    ASSERT_GE(milliseconds, 0);
}