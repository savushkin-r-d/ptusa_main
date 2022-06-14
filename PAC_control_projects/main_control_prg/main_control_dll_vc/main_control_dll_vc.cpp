// main_control_dll_vc.cpp: определяет экспортированные функции для приложения DLL.
//
#include <stdlib.h>
#include "fcntl.h"

#include "dtime.h"

#include "prj_mngr.h"
#include "PAC_dev.h"
#include "PAC_info.h"
#include "tech_def.h"
#include "lua_manager.h"
#include "PAC_err.h"
#include "version_info.h"

#include "log.h"
#ifdef PAC_WAGO_750_860
#include "l_log.h"
#endif

#include "profibus_slave.h"

int G_DEBUG = 0;    //Вывод дополнительной отладочной информации.
int G_USE_LOG = 0;  //Вывод в системный лог (syslog).

int lua_init( lua_State* L )
    {
    int top = lua_gettop( L );

    int argc = 0;
    const char **argv = ( top == 0 ? new const char*[ 1 ] : new const char*[ top ] );
    const char *empty_par = "";

    argv[ 0 ] = empty_par;

    for ( int i = 1; i <= top; i++ )
        {
        int t = lua_type( L, 1 );
        switch ( t )
            {
            case LUA_TSTRING:   //Strings
                {
                const char* str = lua_tostring( L, 1 );
                char* tmp_str = new char[ strlen( str ) + 1 ];
                strcpy( tmp_str, str );
                argv[ argc ] = tmp_str;
                argc++;
                break;
                }

            default:            //Other values.
                break;
            }
        lua_remove( L, 1 );
        }

    G_LOG->info( "Program started (version %s).", PRODUCT_VERSION_FULL_STR );
    G_PROJECT_MANAGER->proc_main_params( argc, argv );

    //-Инициализация Lua.
    int res = G_LUA_MANAGER->init( L, argv[ 0 ],
        G_PROJECT_MANAGER->path.c_str(), G_PROJECT_MANAGER->sys_path.c_str(),
        G_PROJECT_MANAGER->extra_paths.c_str() );

    if ( res ) //-Ошибка инициализации.
        {
        sprintf( G_LOG->msg, "Lua init returned error code %d!", 1 );
        G_LOG->write_log( i_log::P_CRIT );

#ifdef _DEBUG
        debug_break;
#endif
        lua_pushnumber( L, EXIT_FAILURE );
        return 1;
        }

#ifdef USE_PROFIBUS
    if ( G_PROFIBUS_SLAVE()->is_active() )
        {
        G_PROFIBUS_SLAVE()->init();
        }
#endif // USE_PROFIBUS

    if ( top )
        {
        for ( int i = 0; i < top; i++ )
            {
            delete[] argv[ i ];
            argv[ i ] = 0;
            }
        }
    delete [] argv;
    argv = 0;

    lua_pushnumber( L, 0 );
    return 1;
    }

int eval( lua_State* L )
    {
    if ( G_DEBUG )
        {
        fflush( stdout );
        }

#ifdef TEST_SPEED
    static u_long st_time;
    static u_long all_time = 0;
    static u_long cycles_cnt = 0;

    st_time = get_millisec();
    cycles_cnt++;
#endif // TEST_SPEED

    lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCSTEP, 200 );

#ifndef DEBUG_NO_IO_MODULES
    //G_IO_MANAGER()->read_inputs();
#endif // DEBUG_NO_IO_MODULES

    valve::evaluate();
    valve_bottom_mix_proof::evaluate();
    int res = G_TECH_OBJECT_MNGR()->evaluate();

#ifndef DEBUG_NO_IO_MODULES
    //G_IO_MANAGER()->write_outputs();
#endif // ifndef

#ifdef OPCUA
    OPCUAServer::getInstance().Evaluate();
#endif

    PAC_info::get_instance()->eval();
    PAC_critical_errors_manager::get_instance()->show_errors();
    G_ERRORS_MANAGER->evaluate();
    G_SIREN_LIGHTS_MANAGER()->eval();

#ifdef USE_PROFIBUS
    if ( G_PROFIBUS_SLAVE()->is_active() )
        {
        G_PROFIBUS_SLAVE()->eval();
        }
#endif // USE_PROFIBUS

    lua_pushnumber( L, res );
    return 1;
    }

int no_print_stack_traceback( lua_State* L )
    {
    G_LUA_MANAGER->no_print_stack_traceback();
    return 0;
    }

//Регистрация реализованных в dll функций, что бы те стали доступны из lua.
struct luaL_reg ls_lib[] =
    {
    { "init", lua_init },
    { "eval", eval },
    { "no_print_stack_traceback", no_print_stack_traceback },
    { NULL, NULL },
    };

extern "C" int
#ifdef WIN32
__declspec( dllexport )
#endif
    luaopen_ptusa_main( lua_State* L )
    {
    luaL_openlib( L, "ptusa_main", ls_lib, 0 );
    return 0;
    }
