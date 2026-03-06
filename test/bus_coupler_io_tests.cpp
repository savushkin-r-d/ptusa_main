#include "bus_coupler_io_tests.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

using namespace ::testing;

const int STD_OUT_FILENO = 1;

/*
	TEST METHODS DEFENITION:
	void print()
	void print_log()
*/

TEST( io_manager, print )
	{
	const int ZERO_SIZE = 0;
	const int ZERO_COUNT = 0;

	io_manager::get_instance()->init( 1 );
	io_manager::get_instance()->add_node( 0,
		io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
		"Axxx", ZERO_COUNT, ZERO_COUNT,
		ZERO_COUNT, ZERO_SIZE, ZERO_COUNT, ZERO_SIZE );

	fflush( stdout );
	auto old = dup( STD_OUT_FILENO );
	auto tmp_name = "out1.txt";
	auto tmp = open( tmp_name, O_CREAT | O_WRONLY | O_TRUNC, S_IWRITE );
	if ( tmp > 0 )
		{
		dup2( tmp, STD_OUT_FILENO );
		close( tmp );

		io_manager::get_instance()->print();
		io_manager::get_instance()->print_log();

		fflush( stdout );
		dup2( old, STD_OUT_FILENO );
		close( old );
		}
	struct stat st;
	stat( tmp_name, &st );
	EXPECT_GT( st.st_size, 0 );
	}

TEST( io_node, get_display_state_not_active )
	{
	io_manager::get_instance()->init( 1 );
	io_manager::get_instance()->add_node( 0,
		io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
		"Axxx", 0, 0, 0, 0, 0, 0 );

	auto node = io_manager::get_instance()->get_node( 0 );
	node->is_active = false;
	EXPECT_EQ( io_manager::io_node::ST_NO_CONNECT, node->get_display_state() );
	}

TEST( io_node, get_display_state_not_connected )
	{
	io_manager::get_instance()->init( 1 );
	io_manager::get_instance()->add_node( 0,
		io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
		"Axxx", 0, 0, 0, 0, 0, 0 );

	auto node = io_manager::get_instance()->get_node( 0 );
	node->is_active = true;
	node->state = io_manager::io_node::ST_NO_CONNECT;
	EXPECT_EQ( io_manager::io_node::ST_ERROR, node->get_display_state() );
	}

TEST( io_node, get_display_state_connected_ok )
	{
	io_manager::get_instance()->init( 1 );
	io_manager::get_instance()->add_node( 0,
		io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
		"Axxx", 0, 0, 0, 0, 0, 0 );

	auto node = io_manager::get_instance()->get_node( 0 );
	node->is_active = true;
	node->state = io_manager::io_node::ST_OK;
	node->status_register = 0;  // No PP mode.
	EXPECT_EQ( io_manager::io_node::ST_OK, node->get_display_state() );
	}

TEST( io_node, get_display_state_pp_mode )
	{
	io_manager::get_instance()->init( 1 );
	io_manager::get_instance()->add_node( 0,
		io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
		"Axxx", 0, 0, 0, 0, 0, 0 );

	auto node = io_manager::get_instance()->get_node( 0 );
	node->is_active = true;
	node->state = io_manager::io_node::ST_OK;
	node->status_register = 0x0010;  // Bit 4: PP mode active.
	EXPECT_EQ( io_manager::io_node::ST_WARNING, node->get_display_state() );
	}

TEST( io_node, get_display_state_non_phoenix_node )
	{
	io_manager::get_instance()->init( 1 );
	io_manager::get_instance()->add_node( 0,
		io_manager::io_node::WAGO_750_XXX_ETHERNET, 1, "127.0.0.1",
		"Axxx", 0, 0, 0, 0, 0, 0 );

	auto node = io_manager::get_instance()->get_node( 0 );
	node->is_active = true;
	node->state = io_manager::io_node::ST_OK;
	node->status_register = 0x003F;  // Error bits set but not Phoenix.
	// Non-Phoenix nodes should return ST_OK even if error bits are set.
	EXPECT_EQ( io_manager::io_node::ST_OK, node->get_display_state() );
	}

TEST( io_node, get_display_state_pp_mode_with_other_bits )
	{
	io_manager::get_instance()->init( 1 );
	io_manager::get_instance()->add_node( 0,
		io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
		"Axxx", 0, 0, 0, 0, 0, 0 );

	auto node = io_manager::get_instance()->get_node( 0 );
	node->is_active = true;
	node->state = io_manager::io_node::ST_OK;
	// PP mode bit set along with other bits.
	node->status_register = 0x1234 | 0x0010;
	EXPECT_EQ( io_manager::io_node::ST_WARNING, node->get_display_state() );
	}

TEST( io_node, get_display_state_phoenix_not_active )
	{
	io_manager::get_instance()->init( 1 );
	io_manager::get_instance()->add_node( 0,
		io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
		"Axxx", 0, 0, 0, 0, 0, 0 );

	auto node = io_manager::get_instance()->get_node( 0 );
	node->is_active = false;
	node->state = io_manager::io_node::ST_OK;
	node->status_register = 0x0010;
	// Not active should return ST_NO_CONNECT regardless of other
	// states.
	EXPECT_EQ( io_manager::io_node::ST_NO_CONNECT, node->get_display_state() );
	}

TEST( io_node, get_display_state_all_node_types )
	{
	io_manager::get_instance()->init( 3 );
	
	// Test PHOENIX_BK_ETH
	io_manager::get_instance()->add_node( 0,
		io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
		"A100", 0, 0, 0, 0, 0, 0 );
	
	// Test WAGO_750_XXX_ETHERNET
	io_manager::get_instance()->add_node( 1,
		io_manager::io_node::WAGO_750_XXX_ETHERNET, 2, "127.0.0.1",
		"A200", 0, 0, 0, 0, 0, 0 );
	
	// Test WAGO_750_820x
	io_manager::get_instance()->add_node( 2,
		io_manager::io_node::WAGO_750_820x, 3, "127.0.0.1",
		"A300", 0, 0, 0, 0, 0, 0 );

	// All nodes active, connected, with error bits set.
	for ( int i = 0; i < 3; i++ )
		{
		auto node = io_manager::get_instance()->get_node( i );
		node->is_active = true;
		node->state = io_manager::io_node::ST_OK;
		node->status_register = 0x0010;  // Bit 4 set
		}
	
	// Only Phoenix should report PP mode.
	EXPECT_EQ( io_manager::io_node::ST_WARNING, 
		io_manager::get_instance()->get_node( 0 )->get_display_state() );
	EXPECT_EQ( io_manager::io_node::ST_OK, 
		io_manager::get_instance()->get_node( 1 )->get_display_state() );
	EXPECT_EQ( io_manager::io_node::ST_OK, 
		io_manager::get_instance()->get_node( 2 )->get_display_state() );
	}

TEST( io_device, check_output_node_state_ok )
	{
	io_manager::get_instance()->init( 1 );
	io_manager::get_instance()->add_node( 0,
		io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
		"Axxx", 1, 0, 0, 0, 0, 0 );

	io_device dev( "TEST_DO" );
	dev.init_and_alloc( 1, 0, 0, 0 );
	dev.init_channel( io_device::IO_channels::CT_DO, 0, 0, 0 );

	auto node = io_manager::get_instance()->get_node( 0 );
	node->is_active = true;
	node->state = io_manager::io_node::ST_OK;
	node->status_register = 0;

	EXPECT_EQ( 1, dev.check_output_node_state( 0, true ) );
	}

TEST( io_device, check_output_node_state_pp_mode )
	{
	io_manager::get_instance()->init( 1 );
	io_manager::get_instance()->add_node( 0,
		io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
		"Axxx", 1, 0, 0, 0, 0, 0 );

	io_device dev( "TEST_DO" );
	dev.init_and_alloc( 1, 0, 0, 0 );
	dev.init_channel( io_device::IO_channels::CT_DO, 0, 0, 0 );

	auto node = io_manager::get_instance()->get_node( 0 );
	node->is_active = true;
	node->state = io_manager::io_node::ST_OK;
	node->status_register = 0x0010;  // PP mode.

	EXPECT_EQ( -1, dev.check_output_node_state( 0, true ) );
	}

TEST( io_device, check_output_node_state_not_connected )
	{
	io_manager::get_instance()->init( 1 );
	io_manager::get_instance()->add_node( 0,
		io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
		"Axxx", 1, 0, 0, 0, 0, 0 );

	io_device dev( "TEST_DO" );
	dev.init_and_alloc( 1, 0, 0, 0 );
	dev.init_channel( io_device::IO_channels::CT_DO, 0, 0, 0 );

	auto node = io_manager::get_instance()->get_node( 0 );
	node->is_active = true;
	node->state = io_manager::io_node::ST_NO_CONNECT;

	EXPECT_EQ( -1, dev.check_output_node_state( 0, true ) );
	}

TEST( io_device, check_output_node_state_not_active )
	{
	io_manager::get_instance()->init( 1 );
	io_manager::get_instance()->add_node( 0,
		io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
		"Axxx", 1, 0, 0, 0, 0, 0 );

	io_device dev( "TEST_DO" );
	dev.init_and_alloc( 1, 0, 0, 0 );
	dev.init_channel( io_device::IO_channels::CT_DO, 0, 0, 0 );

	auto node = io_manager::get_instance()->get_node( 0 );
	node->is_active = false;
	node->state = io_manager::io_node::ST_OK;
	node->status_register = 0;

	EXPECT_EQ( -1, dev.check_output_node_state( 0, true ) );
	}

TEST( io_device, check_output_node_state_no_channels )
	{
	io_device dev( "TEST_DO" );
	
	// No channels configured.
	EXPECT_EQ( 0, dev.check_output_node_state( 0, true ) );
	}
