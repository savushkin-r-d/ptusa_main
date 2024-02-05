#include "uni_bus_coupler_io_tests.h"

using namespace ::testing;

TEST( uni_io_manager, net_init )
    {
    uni_io_manager mngr;

    // Should fail - node is null.
    auto res = mngr.net_init( nullptr );
    EXPECT_LT( res, 0 );

    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1000, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    // Should fail - can't connect - timeout.
    res = mngr.net_init( &node );
    EXPECT_LT( res, 0 );

    mngr.disconnect( &node );
    }

TEST( uni_io_manager, read_inputs )
    {
    uni_io_manager mngr;

    // Should not fail - nodes count is 0.
    auto res = mngr.read_inputs();
    EXPECT_EQ( res, 0 );

    mngr.init( 1 );
    mngr.add_node( 0,io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1000, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );

    res = mngr.read_inputs();
    EXPECT_EQ( res, 0 );
    }


TEST( uni_io_manager, write_outputs )
    {
    uni_io_manager mngr;

    // Should not fail - nodes count is 0.
    auto res = mngr.write_outputs();
    EXPECT_EQ( res, 0 );

    mngr.init( 1 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1000, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    res = mngr.write_outputs();
    EXPECT_EQ( res, 0 );
    }
