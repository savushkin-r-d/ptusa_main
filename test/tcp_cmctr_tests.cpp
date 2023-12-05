#include "tcp_cmctr_tests.h"
#include "dtime.h"
#include "fmt/chrono.h"

using namespace ::testing;

TEST(tcp_communicator, get_millisec_test)
{
    unsigned long milliseconds = get_millisec();

    ASSERT_GE(milliseconds, 0);
}

TEST(tcp_communicator, local_time_test)
{
    auto now = std::chrono::system_clock::now();

    auto now_time_t = std::chrono::system_clock::to_time_t(now);

    auto timeInfo = fmt::localtime(now_time_t);

    ASSERT_NE(timeInfo.tm_year, -1);
    ASSERT_NE(timeInfo.tm_mon, -1);
    ASSERT_NE(timeInfo.tm_mday, -1);
    ASSERT_NE(timeInfo.tm_hour, -1);
    ASSERT_NE(timeInfo.tm_min, -1);
    ASSERT_NE(timeInfo.tm_sec, -1);
}