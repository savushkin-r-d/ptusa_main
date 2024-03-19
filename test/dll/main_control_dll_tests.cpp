#include "../includes.h"

#ifdef  __cplusplus
extern "C" {
#endif

#include    "lua.h"
#include    "lauxlib.h"
#include    "lualib.h"

#ifdef  __cplusplus
    };
#endif

using namespace ::testing;

extern int G_DEBUG;
extern int no_print_stack_traceback( lua_State* L );
extern int use_print_stack_traceback( lua_State* L );
extern int switch_on_verbose( lua_State* L );
extern int switch_off_verbose( lua_State* L );
extern int lua_init( lua_State* L );


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

TEST( dll, lua_init )
    {
    auto L = lua_open();

    char argv0[] = "ptusa_main.exe";
    char argv1[] = "main.plua";
    char argv2[] = "--sys_path";
    char argv3[] = "./sys/";
    char argv4[] = "--path";
    char argv5[] = "./";

    lua_pushlstring( L, argv0, sizeof( argv0 ) );
    lua_pushlstring( L, argv1, sizeof( argv1 ) );
    lua_pushlstring( L, argv2, sizeof( argv2 ) );
    lua_pushlstring( L, argv3, sizeof( argv3 ) );
    lua_pushlstring( L, argv4, sizeof( argv4 ) );
    lua_pushlstring( L, argv5, sizeof( argv5 ) );    

    auto res = lua_init( L );
    EXPECT_EQ( 1, res );
    }
