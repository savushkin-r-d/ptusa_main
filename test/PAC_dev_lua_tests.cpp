#include "PAC_dev_lua_tests.h"

using namespace ::testing;

TEST( toLuapp, tolua_PAC_dev_open )
    {
    lua_State* L = lua_open();

    EXPECT_EQ( 1, tolua_PAC_dev_open(L));
    
    lua_close( L );
    }
