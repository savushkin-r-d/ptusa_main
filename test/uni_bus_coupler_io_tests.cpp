#include "uni_bus_coupler_io_tests.h"

using namespace ::testing;

TEST( uni_io_manager, net_init )
    {
    uni_io_manager mngr;

    //Should fail - node is null.
    auto res = mngr.net_init( nullptr );
    EXPECT_LT( res, 0 );

    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1000, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    //Should fail - can't connect - timeout.
    res = mngr.net_init( &node );
    EXPECT_LT( res, 0 );

    mngr.disconnect( &node );
    }
