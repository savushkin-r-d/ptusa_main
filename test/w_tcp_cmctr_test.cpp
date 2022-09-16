#include "w_tcp_cmctr_test.h"

using namespace ::testing;

#ifdef WIN_OS
TEST( tcp_communicator_win, evaluate )
{
    G_CMMCTR->init_instance("Тест", "Test");
    EXPECT_EQ(0, G_CMMCTR->evaluate());

    win_tcp_client cl("127.0.0.1", 10000, 1, 1); 
    cl.Connect();

    EXPECT_EQ(0, G_CMMCTR->evaluate());

    cl.buff[0] = 33;
    cl.buff[1] = 33;
    cl.AsyncSend(2);

    //sleep(0); 
    EXPECT_EQ(0, G_CMMCTR->evaluate());

    EXPECT_EQ(0, G_CMMCTR->evaluate());
}

#endif
