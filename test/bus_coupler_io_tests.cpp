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
	EXPECT_EQ( -1, node->get_display_state() );
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
	node->status_register = io_manager::io_node::STATUS_REG_PP_MODE_BIT;  // PP mode active.
	EXPECT_EQ( io_manager::io_node::ST_PP_MODE, node->get_display_state() );
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
	node->status_register = io_manager::io_node::STATUS_REG_PP_MODE_BIT;  // PP mode bit set but not Phoenix.
	// Non-Phoenix nodes should return ST_OK even if PP mode bit is set.
	EXPECT_EQ( io_manager::io_node::ST_OK, node->get_display_state() );
	}
