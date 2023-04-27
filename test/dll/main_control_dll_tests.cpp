#include "../includes.h"
#include "lua.h"

using namespace ::testing;

extern int G_DEBUG;
extern int no_print_stack_traceback( lua_State* L );
extern int use_print_stack_traceback( lua_State* L );
extern int switch_on_verbose( lua_State* L );
extern int switch_off_verbose( lua_State* L );

TEST( dll, no_print_stack_traceback )
    {
    EXPECT_EQ( 0, no_print_stack_traceback( nullptr ) );
    }

TEST( dll, use_print_stack_traceback )
    {
    EXPECT_EQ( 0, use_print_stack_traceback( nullptr ) );
    }

TEST( dll, switch_on_verbose )
    {
    switch_on_verbose( nullptr );
    EXPECT_EQ( 1, G_DEBUG );
    }

TEST( dll, switch_off_verbose )
    {
    switch_off_verbose( nullptr );
    EXPECT_EQ( 0, G_DEBUG );
    }
