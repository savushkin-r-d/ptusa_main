#include "uni_bus_coupler_io_tests.h"

using namespace ::testing;

#ifndef WIN_OS // For linux to deal with __stdcall.
#define __stdcall
#endif

int __stdcall fail_socket( int, int, int )
    {
    return -1;
    }

int __stdcall fail_setsockopt( int, int, int, const char*, int )
    {
    return 1;
    }

#ifdef WIN_OS
int __stdcall fail_ioctlsocket( int, long, long )
    {
    return 1;
    }
#else
int fail_fcntl( int, int )
    {
    return 1;
    }
#endif

int __stdcall fail_select_m1( int, fd_set*, fd_set*, fd_set*, struct timeval* )
    {
    return -1;
    }

int __stdcall fail_select_0( int, fd_set*, fd_set*, fd_set*, struct timeval* )
    {
    return 0;
    }

int __stdcall success_select( int, fd_set*, fd_set*, fd_set*, struct timeval* )
    {
    return 1;
    }

int __stdcall fail_getsockopt( int, int, int, char*, int* )
    {
    return -1;
    }

int __stdcall success_getsockopt( int, int, int, char*, int* )
    {
    return 0;
    }

TEST( uni_io_manager, net_init )
    {
    uni_io_manager mngr;

    // Should fail - node is null.
    auto res = mngr.net_init( nullptr );
    EXPECT_EQ( res, 1 );

    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1000, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );

    subhook_t socket_hook = subhook_new( reinterpret_cast<void*>( socket ),
        reinterpret_cast<void*>( fail_socket ), SUBHOOK_64BIT_OFFSET );
    subhook_install( socket_hook );
    // Should fail - fail with create socket.
    EXPECT_EQ( 3, mngr.net_init( &node ) );
    subhook_remove( socket_hook );

    subhook_t setsockopt_hook = subhook_new( reinterpret_cast<void*>( setsockopt ),
        reinterpret_cast<void*>( fail_setsockopt ), SUBHOOK_64BIT_OFFSET );
    subhook_install( setsockopt_hook );
    // Should fail - fail with setsockopt().
    EXPECT_EQ( 4, mngr.net_init( &node ) );
    subhook_remove( setsockopt_hook );

#ifdef WIN_OS
    subhook_t ioctlsocket_hook = subhook_new( reinterpret_cast<void*>( ioctlsocket ),
        reinterpret_cast<void*>( fail_ioctlsocket ), SUBHOOK_64BIT_OFFSET );
    subhook_install( ioctlsocket_hook );
    // Should fail - fail with fcntl().
    EXPECT_EQ( 5, mngr.net_init( &node ) );
    subhook_remove( ioctlsocket_hook );
#else
    subhook_t fcntl_hook = subhook_new( reinterpret_cast<void*>( fcntl ),
        reinterpret_cast<void*>( fail_fcntl ), SUBHOOK_64BIT_OFFSET );
    subhook_install( fcntl_hook );
    // Should fail - fail with fcntl().
    EXPECT_EQ( 5, mngr.net_init( &node ) );
    subhook_remove( fcntl_hook );
#endif

    subhook_t select_m1_hook = subhook_new( reinterpret_cast<void*>( select ),
        reinterpret_cast<void*>( fail_select_m1 ), SUBHOOK_64BIT_OFFSET );
    subhook_install( select_m1_hook );
    // Should fail - fail with select() - 1.
    EXPECT_EQ( 6, mngr.net_init( &node ) );
    subhook_remove( select_m1_hook );
    subhook_t select_0_hook = subhook_new( reinterpret_cast<void*>( select ),
        reinterpret_cast<void*>( fail_select_0 ), SUBHOOK_64BIT_OFFSET );
    subhook_install( select_0_hook );
    // Should fail - fail with select() - 0.
    EXPECT_EQ( 6, mngr.net_init( &node ) );
    subhook_remove( select_0_hook );

    subhook_t getsockopt_1_hook = subhook_new( reinterpret_cast<void*>( getsockopt ),
        reinterpret_cast<void*>( fail_getsockopt ), SUBHOOK_64BIT_OFFSET );
    subhook_install( getsockopt_1_hook );
    subhook_t select_1_hook = subhook_new( reinterpret_cast<void*>( select ),
        reinterpret_cast<void*>( success_select ), SUBHOOK_64BIT_OFFSET );
    subhook_install( select_1_hook );
    // Should fail - fail with getsockopt() - 1.
    EXPECT_EQ( 7, mngr.net_init( &node ) );
    subhook_remove( getsockopt_1_hook );
    subhook_remove( select_1_hook );

    subhook_install( select_1_hook );
    subhook_t getsockopt_0_hook = subhook_new( reinterpret_cast<void*>( getsockopt ),
        reinterpret_cast<void*>( success_getsockopt ), SUBHOOK_64BIT_OFFSET );
    subhook_install( getsockopt_0_hook );
    // Should OK.
    EXPECT_EQ( 0, mngr.net_init( &node ) );
    subhook_remove( select_1_hook );
    subhook_remove( getsockopt_0_hook );

    mngr.disconnect( &node );

    subhook_free( socket_hook );
    subhook_free( setsockopt_hook );
#ifdef WIN_OS
    subhook_free( ioctlsocket_hook );
#else
    subhook_free( fcntl_hook );
#endif    
    subhook_free( select_m1_hook );
    subhook_free( select_0_hook );
    subhook_free( getsockopt_1_hook );
    subhook_free( select_1_hook );
    subhook_free( getsockopt_0_hook );
    }

class test_uni_io_manager : public uni_io_manager
    {
    public:
        int e_communicate( io_node* node, int bytes_to_send,
            int bytes_to_receive ) override
            {
            return 0;
            }
    };

TEST( uni_io_manager, read_inputs )
    {
    test_uni_io_manager mngr;
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );

    // Should not fail - nodes count is 0.
    auto res = mngr.read_inputs();
    EXPECT_EQ( res, 0 );

    mngr.init( 4 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    mngr.add_node( 1, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        2, "127.0.0.1", "A200", 1, 1, 1, 1, 1, 1 );
    mngr.get_node( 1 )->is_active = false;
    mngr.add_node( 2, io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        3, "127.0.0.1", "A300", 1, 1, 1, 1, 1, 1 );
    mngr.add_node( 3, io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        4, "127.0.0.1", "A400", 1, 1, 1, 1, 1, 1 );
    mngr.get_node( 3 )->is_active = false;

    res = mngr.read_inputs();
    EXPECT_EQ( res, 0 );

    io_manager::replace_instance( prev_mngr );
    }

TEST( uni_io_manager, write_outputs )
    {
    test_uni_io_manager mngr;
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );

    // Should not fail - nodes count is 0.
    auto res = mngr.write_outputs();
    EXPECT_EQ( res, 0 );

    mngr.init( 4 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 6*16, 6*16, 6, 6, 1, 1 );
    // Just for testing add set of modules.
    mngr.init_node_AO( 0, 0, 1027843, 0 );
    mngr.init_node_AO( 0, 1, 2688093, 1 );
    mngr.init_node_AO( 0, 2, 2688527, 2 );
    mngr.init_node_AO( 0, 3, 2688527, 3 );
    mngr.init_node_AO( 0, 4, 2688666, 4 );
    mngr.init_node_AO( 0, 5, 10, 5 );

    mngr.add_node( 1, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        2, "127.0.0.1", "A200", 1, 1, 1, 1, 1, 1 );
    mngr.get_node( 1 )->is_active = false;
    mngr.add_node( 2, io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        3, "127.0.0.1", "A300", 1, 1, 2, 4, 1, 1 );
    mngr.init_node_AO( 2, 0, 638, 0 );
    mngr.init_node_AO( 2, 1, 10, 2 );
    mngr.add_node( 3, io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        4, "127.0.0.1", "A400", 1, 1, 1, 1, 1, 1 );
    mngr.get_node( 3 )->is_active = false;
    res = mngr.write_outputs();
    EXPECT_EQ( res, 0 );

    io_manager::replace_instance( prev_mngr );
    }

TEST( uni_io_manager, e_communicate )
    {
    uni_io_manager mngr;
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1000, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );

    auto res = mngr.e_communicate( &node, 1, 1 );
    EXPECT_NE( res, 0 );
    }
