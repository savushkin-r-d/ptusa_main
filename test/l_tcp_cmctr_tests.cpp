#include "l_tcp_cmctr_tests.h"

using namespace ::testing;

#ifdef LINUX_OS

TEST( tcp_communicator_linux, evaluate )
    {
    G_CMMCTR->init_instance( "Тест", "Test" );
    EXPECT_EQ( 0, G_CMMCTR->evaluate() );

    linux_tcp_client cl( "127.0.0.1", 10000, 1, 1 );
    cl.Connect();

    EXPECT_EQ( 0, G_CMMCTR->evaluate() );

    cl.buff[ 0 ] = 33;
    cl.buff[ 1 ] = 33;
    cl.AsyncSend( 2 );
    cl.AsyncRecive();

    sleep( 0 );
    EXPECT_EQ( 0, G_CMMCTR->evaluate() );

    EXPECT_EQ( 0, G_CMMCTR->evaluate() );
    }

TEST(tcp_communicator_linux, checkBuff )
    {
    SOCKET s = socket(AF_INET, SOCK_STREAM, NULL);

    auto res = linux_tcp_client::checkBuff(s);
    EXPECT_EQ(false, res);
    }

#endif
