#include "cip_tech_def_tests.h"
#include "lua_manager.h"

using namespace ::testing;

TEST( cipline_tech_object, _CheckErr )
    {
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( lua_open() );

    cip1.initline();
    EXPECT_EQ( 0, cip1._CheckErr() );
    }
