#include "tcp_cmctr_tests.h"

using namespace ::testing;

TEST( tcp_communicator, evaluate )
    {
    G_CMMCTR->init_instance( "Тест", "Test" );
    EXPECT_EQ( 0, G_CMMCTR->evaluate() );

#ifdef LINUX_OS
    linux_tcp_client cl( "127.0.0.1", 10000, 1, 1 );
#else
    win_tcp_client cl( "127.0.0.1", 10000, 1, 1 );
#endif

    cl.checkConnection();
    cl.reconnectTimeout = 0;
    cl.checkConnection();
    cl.Connect();

    EXPECT_EQ( 0, G_CMMCTR->evaluate() );

    cl.buff[ 0 ] = 33;
    cl.buff[ 1 ] = 33;
    EXPECT_EQ( 0, cl.AsyncSend( 2 ) );
    sleep_ms( 0 );
    EXPECT_EQ( 0, G_CMMCTR->evaluate() );
    sleep_ms( 0 );
    EXPECT_EQ( 0, cl.AsyncRecive() );
    cl.async_timeout = 0;
    sleep_ms( 0 );
    EXPECT_EQ( 0, cl.AsyncRecive() );
    cl.checkConnection();

    sleep_ms( 0 );
    EXPECT_EQ( 0, G_CMMCTR->evaluate() );

    EXPECT_EQ( 0, G_CMMCTR->evaluate() );
    }

TEST( tcp_communicator, checkBuff )
    {
    int s = socket( AF_INET, SOCK_STREAM, 0 );

    auto res = tcp_communicator::checkBuff( s );
#ifdef LINUX_OS
    EXPECT_TRUE( res );
#else
    EXPECT_FALSE( res );
#endif

    res = tcp_communicator::checkBuff( -10 );
    EXPECT_FALSE( res );
    }
