#include "PAC_info_tests.h"

using namespace ::testing;

/*
	TEST METHOD DEFENITION:
	void evaluate()
*/

TEST( PAC_info, set_cmd )
	{
    auto res = G_PAC_INFO()->set_cmd( "CMD", 0, PAC_info::RELOAD_RESTRICTIONS );
	EXPECT_EQ( 0, res );
	}
