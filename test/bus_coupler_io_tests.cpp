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