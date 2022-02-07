#include "bus_coupler_io_tests.h"

#include <sys/stat.h>
#include <fcntl.h>

using namespace ::testing;

/*
	TEST METHOD DEFENITION:
	void print()
*/

TEST( io_manager, print )
	{
	fflush( stdout );
	auto old = dup( 1 );
	auto tmp = open( "out.txt", O_CREAT | O_WRONLY | O_TRUNC );	
	dup2( tmp, 1 );
	close( tmp );

	io_manager::get_instance()->print();

	fflush( stdout );
	dup2( old, 1 );
	close( old );

	struct stat st;
	stat( "out.txt", &st );
	EXPECT_GT( st.st_size, 0 );
	}

/*
	TEST METHOD DEFENITION:
	void print_log()
*/

TEST( io_manager, print_log )
	{
	fflush( stdout );
	auto old = dup( 1 );
	auto tmp = open( "out.txt", O_CREAT | O_WRONLY | O_TRUNC );
	dup2( tmp, 1 );
	close( tmp );

	io_manager::get_instance()->print_log();

	fflush( stdout );
	dup2( old, 1 );
	struct stat st;
	stat( "out.txt", &st );
	EXPECT_GT( st.st_size, 0 );
	}