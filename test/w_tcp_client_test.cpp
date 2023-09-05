#include "w_tcp_client_test.h"

using namespace ::testing;

#ifdef WIN_OS
TEST( tcp_client, checkConnection )
	{
	win_tcp_client cl( "127.0.0.0", 10000, 1, 1 );

	EXPECT_EQ( 0, cl.checkConnection() );

	cl.reconnectTimeout = 1000000;

	EXPECT_EQ( 0, cl.checkConnection() );

	}
#endif