#include "tcp_cmctr_tests.h"
#include "dtime.h"
#include "fmt/chrono.h"
#include "mock_tcp_communicator.h"

using namespace ::testing;

TEST(tcp_communicator, get_millisec_test)
{
    unsigned long milliseconds = get_millisec();

    ASSERT_GE(milliseconds, 0);
}

TEST(tcp_communicator, evaluate_test)
{
    mock_tcp_communicator communicator;

    EXPECT_CALL(communicator, evaluate()).Times(::testing::AtLeast(1));

    communicator.evaluate();
}

TEST(tcp_communicator, reg_service)
{
    mock_tcp_communicator communicator;

    tcp_communicator::srv_ptr mockService = [](long int, u_char*, u_char*) -> long int { return 0; };

    EXPECT_CALL(communicator, reg_service(42, mockService)).Times(1);

    tcp_communicator::srv_ptr result = communicator.reg_service(42, mockService);

    ASSERT_EQ(result, nullptr);
}