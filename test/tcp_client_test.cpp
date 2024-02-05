#include "tcp_client_test.h"

using namespace ::testing;

extern int G_DEBUG;

TEST( tcp_client, checkConnection )
    {
#ifdef WIN_OS
    win_tcp_client cl( "127.0.0.1", 10000, 1, 1, 256, 0 );
    cl.InitLib();
#else
    linux_tcp_client cl( "127.0.0.1", 10000, 1, 1, 256, 0 );
#endif // WIN_OS	    

    EXPECT_EQ( 0, cl.AsyncConnect() );
    EXPECT_EQ( 0, cl.checkConnection() );
    }
