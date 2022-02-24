#include "PAC_info_tests.h"

using namespace ::testing;

/*
    TEST METHOD DEFENITION:
    int set_cmd( const char *prop, u_int idx, double val )
*/

TEST( PAC_info, set_cmd )
    {
    G_PAC_INFO()->set_cmd( "CMD", 0, PAC_info::RELOAD_RESTRICTIONS );
    }
