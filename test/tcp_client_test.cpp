#include "tcp_client_test.h"

using namespace ::testing;

TEST( tcp_client, checkConnection )
    {
#ifdef WIN_OS
    win_tcp_client cl( "127.0.0.1", 10000, 1, 1 );
#else
    linux_tcp_client cl( "127.0.0.1", 10000, 1, 1 );
#endif // WIN_OS	

    cl.AsyncSend( 2 );
    cl.AsyncRecive();

    EXPECT_EQ( 0, cl.AsyncConnect() );

    EXPECT_EQ( 0, cl.checkConnection() );

    cl.reconnectTimeout = 1000000;

    EXPECT_EQ( 0, cl.checkConnection() );
    }
