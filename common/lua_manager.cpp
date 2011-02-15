#include <cstdlib>
#ifdef OS_WIN
#include <windows.h>
#endif // OS_WIN

#include "lua_manager.h"

#include "prj_mngr.h"
#include "PAC_dev.h"

//-----------------------------------------------------------------------------
auto_smart_ptr< lua_manager > lua_manager::instance;
//-----------------------------------------------------------------------------
lua_manager* lua_manager::get_instance()
    {
    static int is_init = 0;
     if ( 0 == is_init )
        {
        instance = new lua_manager();
        is_init = 1;
        }

     return instance;
    }
//-----------------------------------------------------------------------------
int lua_manager::init( lua_State* lua_state, char* script_name )
    {
    if ( 0 == lua_state )
        {
        //-Инициализация Lua.
        L = lua_open();   // Create Lua context.        

        if ( NULL == L )
            {
            Print( "Error creating Lua context.\n" );
            return 1;
            }
        is_free_lua = 1;

        luaL_openlibs( L );    // Open standard libraries.

        tolua_PAC_dev_open( L );

        const char *ADDITIONAL_PATH_CMD =
#ifdef PAC_PC
        "package.path = package.path..\';../../system scripts/?.lua;\'";
#else
        "package.path = package.path..\';./?.lua;\'";
#endif // PAC_PC
      
        if( luaL_dostring( L, ADDITIONAL_PATH_CMD ) != 0 )
            {
            Print( "Set additional search path error!\n" );
            Print( "\t%s\n", lua_tostring( L, -1 ) );

            lua_pop( L, 1 );
            return 1;
            }

        if( luaL_dofile( L, "main.wago.plua" ) != 0 )
            {
            Print( "Load description Wago Lua script error!\n" );
            Print( "\t%s\n", lua_tostring( L, -1 ) );

            lua_pop( L, 1 );
            return 1;
            }

        G_PROJECT_MANAGER->lua_load_configuration();       

        if( luaL_loadfile( L, script_name ) != 0 )
            {
            Print( "Load Lua main script error!\n" );
            Print( "\t%s\n", lua_tostring( L, -1 ) );

            lua_pop( L, 1 );
            return 1;
            }
        //-Инициализация Lua.--!>

        int i_line = lua_pcall( L, 0, LUA_MULTRET, 0 );
        if ( i_line != 0 )
            {
            Print( "Evaluate Lua script error!\n" );
            Print( "\t%s\n", lua_tostring( L, -1 ) );

            lua_pop( L, 1 );            
            return 1;
            }        
    	}
    else
        {
        L = lua_state;
        is_free_lua = 0;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
lua_manager::~lua_manager()
    {
    if ( 1 == is_free_lua )
    	{
        if ( L )
            {
            lua_close( L );
            L = NULL;
            }
    	}    
    }
//-----------------------------------------------------------------------------
int lua_manager::void_exec_lua_method( const char *object_name,
    const char *function_name, const char *c_function_name ) const
    {
    int res = 0;
    if ( 0 == exec_lua_method( object_name, function_name, 0, 0, 0 ) )
        {
        }
    else
        {
        res = 1;
#ifdef DEBUG
        Print( "Error during C++ call - \"%s\"\n", c_function_name );
#endif // DEBUG
        }

    return res;
    }
//-----------------------------------------------------------------------------
int lua_manager::int_exec_lua_method( const char *object_name,
    const char *function_name, int param, const char *c_function_name ) const
    {
    int res = 0;
    if ( 0 == exec_lua_method( object_name, function_name, param ) )
        {
        res = ( int ) tolua_tonumber( L, -1, 0 );
        lua_remove( L, -1 );
        }
    else
        {
#ifdef DEBUG
        Print( "Error during C++ call - \"%s\"\n", c_function_name );
#endif // DEBUG
        }

    return res;
    }
//-----------------------------------------------------------------------------
void* lua_manager::user_object_exec_lua_method( const char *object_name,
    const char *function_name, int param, const char *c_function_name ) const
    {
    void* res = NULL;
    if ( 0 == exec_lua_method( object_name, function_name, param ) )
        {
        res = tolua_tousertype( L, -1, NULL );
        lua_remove( L, -1 );
        }
    else
        {
#ifdef DEBUG
        Print( "Error during C++ call - \"%s\"\n", c_function_name );
#endif // DEBUG
        }

    return res;
    }
//-----------------------------------------------------------------------------
int lua_manager::exec_lua_method( const char *object_name,
    const char *function_name, int param, int is_use_param,
    int is_use_lua_return_value ) const
    {   
    //-Вычисление времени выполнения функций Lua.
    //LARGE_INTEGER start_time;
    //QueryPerformanceCounter( &start_time );

//    u_long start_time;
//    start_time = get_millisec();

    lua_pushcclosure( lua_manager::L, error_trace, 0 );
    instance->err_func = lua_gettop( L );

    lua_getfield( L, LUA_GLOBALSINDEX, object_name );
    lua_getfield( L, -1, function_name );
    lua_remove( L, -2 );
    lua_getfield( L, LUA_GLOBALSINDEX, object_name );
    int param_count = 1;
    if ( is_use_param )
        {
        lua_pushnumber( L, param );
        param_count++;
        }
    int results_count = is_use_lua_return_value == 1 ? 1 : 0;
    int res = lua_pcall( L, param_count, results_count, err_func );

    lua_remove( L, -2 );

    //LARGE_INTEGER finish_time;
    //QueryPerformanceCounter( &finish_time );
    //LARGE_INTEGER tick_per_sec;
    //QueryPerformanceFrequency( &tick_per_sec );
    //LONGLONG call_time = finish_time.QuadPart - start_time.QuadPart;    
    //int tiks_per_mcsec = tick_per_sec.QuadPart / ( 1000 * 1000 );
    //double dt = call_time / tiks_per_mcsec;
    //dt += 1;

//    u_long finish_time;
//    finish_time = get_millisec();
//    u_long call_time = finish_time - start_time;
//    Print( "Lua call time - %lums\n", call_time );

    return res;
    }
//-----------------------------------------------------------------------------
int lua_manager::error_trace( lua_State * L )
    {
#ifdef DEBUG
    Print( "\t%s\n", lua_tostring( L, -1 ) );
    lua_remove( L, -1 );

    Print( "\tstack traceback:\n" );
    lua_Debug ar;
    int level = 1;

    while( lua_getstack( L, level, &ar ) )
        {
        lua_getinfo( L, "Sln", &ar );
        Print( "\t\t%s:%d: in function '%s'\n",
            ar.source, ar.linedefined, ar.name );
        level++;
        }
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int lua_manager::int_no_param_exec_lua_method( const char *object_name, 
    const char *function_name, const char *c_function_name ) const
    {
    int res = 0;
    if ( 0 == exec_lua_method( object_name, function_name, 0, 0, 1 ) )
        {
        res = ( int ) tolua_tonumber( L, -1, 0 );
        lua_remove( L, -1 );
        }
    else
        {
#ifdef DEBUG
        Print( "Error during C++ call - \"%s\"\n", c_function_name );
#endif // DEBUG
        }

    return res;
    }

const char* lua_manager::char_no_param_exec_lua_method( const char *object_name, 
    const char *function_name, const char *c_function_name ) const
    {
    const char* res = 0;
    if ( 0 == exec_lua_method( object_name, function_name, 0, 0, 1 ) )
        {
        res = tolua_tostring( L, -1, 0 );
        lua_remove( L, -1 );
        }
    else
        {
#ifdef DEBUG
        Print( "Error during C++ call - \"%s\"\n", c_function_name );
#endif // DEBUG
        }

    return res;
    }
//-----------------------------------------------------------------------------
int lua_manager::exec_Lua_str( const char *Lua_str, const char *error_str,
    bool is_print_error_msg /*= true */ ) const
    {
    int res = luaL_dostring( L, Lua_str );

    if( res != 0  )
        {
        if ( is_print_error_msg )
            {
            Print( "Error during C++ call - \"%s\" - %s\n", 
                error_str, lua_tostring( L, -1 ) );
            }

        lua_pop( L, 1 );
        return 1;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
lua_State * lua_manager::get_Lua() const
    {
    return L;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
