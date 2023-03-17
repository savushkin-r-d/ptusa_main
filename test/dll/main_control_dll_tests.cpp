#include "../includes.h"
#include "lua.h"

using namespace ::testing;

extern int no_print_stack_traceback( lua_State* L );

TEST( dll, no_print_stack_traceback )
    {
    EXPECT_EQ( 0, no_print_stack_traceback( 0 ) );
    }
