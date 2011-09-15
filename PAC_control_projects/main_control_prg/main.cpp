/// @par Описание директив препроцессора:
/// @c LINUX_OS         - компиляция для ОС Linux.
/// @par Тип PAC:
/// @c PAC_PC           - PAC на PC с ОС Linux.
/// @c PAC_WAGO_750_860 - PAC Wago 750-860.
///
/// @c WIN_OS           - компиляция для ОС Windows.
///
/// @c DEBUG - компиляция c выводом отладочной информации в консоль.@n@n

#include <stdlib.h>

#ifdef __BORLANDC__
#include <sys\types.h>
#include <time.h>
#endif // __BORLANDC__

#include "dtime.h"

#include "prj_mngr.h"
#include "PAC_dev.h"
#include "PAC_info.h"
#include "tech_def.h"
#include "lua_manager.h"

int main( int argc, char *argv[] )
    {
#ifdef MINIOS7
    EnableHighRam();
    InitLib();
#endif

    if ( argc < 2 )
        {
        Print( "Usage: main script.plua\n" );
        return EXIT_FAILURE;
        }

    time_t t = time( 0 );
    fprintf( stderr, "Program started - %s\n", asctime( localtime( &t ) ) );

    G_PROJECT_MANAGER->proc_main_params( argc, argv );

    int res = G_LUA_MANAGER->init( 0, argv[ 1 ] ); //-Инициализация Lua.
    lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCSTOP, 0 );

    if ( res ) //-Ошибка инициализации.
        {
        fprintf( stderr, "Lua init error - %d!\n", res );
        debug_break;
        return EXIT_FAILURE;
        }  

    G_TECH_OBJECT_MNGR()->init_objects();


    const char *PAC_name =
        G_LUA_MANAGER->char_no_param_exec_lua_method( "system",
        "get_PAC_name", "lua_manager::init" );
    if ( 0 == PAC_name )
        {
        fprintf( stderr, "Lua init error - error reading PAC name!\n" );
        debug_break;
        return EXIT_FAILURE;
        }
    tcp_communicator::init_instance( PAC_name );
    G_CMMCTR->reg_service( device_communicator::C_SERVICE_N,
        device_communicator::write_devices_states_service );

    //-Добавление системных тегов контроллера.
    G_DEVICE_CMMCTR->add_device( PAC_info::get_instance() );

    params_manager::get_instance()->init( 10 );
    params_manager::get_instance()->final_init();

#ifdef DEBUG
    G_DEVICE_MANAGER()->print();
    G_DEVICE_CMMCTR->print();
    G_TECH_OBJECT_MNGR()->print();
#endif // DEBUG

    lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCRESTART, 0 );
    lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOLLECT, 0 );
    fflush( stdout );
    fprintf( stderr, "Start main loop!\n" );

#ifdef DEBUG
    while ( !kb_hit() )
#else
    while ( 1 )
#endif // DEBUG
        {
        lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCSTEP, 200 );
        sleep_ms( 1 );

#ifdef TEST_SPEED
        static u_long st_time;
        static u_long all_time   = 0;
        static u_long cycles_cnt = 0;

        st_time = get_millisec();
        cycles_cnt++;
#endif // TEST_SPEED


#ifndef DEBUG_NO_WAGO_MODULES
        G_WAGO_MANAGER()->read_inputs();
#endif // DEBUG_NO_WAGO_MODULES

        G_TECH_OBJECT_MNGR()->evaluate();

        G_CMMCTR->evaluate();

#ifndef DEBUG_NO_WAGO_MODULES
        G_WAGO_MANAGER()->write_outputs();
#endif // ifndef

        PAC_critical_errors_manager::get_instance()->show_errors();

        PAC_info::get_instance()->eval();

#ifdef TEST_SPEED
        //-Информация о времени выполнения цикла программы.!->
        all_time += get_millisec() - st_time;
#if defined LINUX_OS
#ifdef PAC_PC
        const u_int MAX_ITERATION = 1000;
#endif // PAC_PC
#ifdef PAC_WAGO_750_860
        const u_int MAX_ITERATION = 1500;
#endif // PAC_WAGO_750_860
#endif // defined LINUX_OS

#ifdef WIN_OS
        const u_int MAX_ITERATION = 1000;
#endif // WIN_OS
        const u_int END_ITERATION = 1000;        

        static u_int max_cycle_time = 0;
        u_int cycle_time = get_delta_millisec( st_time );

        if ( max_cycle_time < cycle_time ) 
            {
            max_cycle_time = cycle_time;
            print_time( "  Main cycle avg time = %lu msec, max time = %4u, Lua mem = %d b\n",
                all_time / cycles_cnt, max_cycle_time,
                lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNT, 0 ) * 1024 +
                lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNTB, 0 ) );
            fflush( stdout );
            }

        static u_int print_cycle_time_count = 0;
        if ( cycles_cnt > MAX_ITERATION )
            {
            if ( print_cycle_time_count < END_ITERATION )
                {
                print_time( "\tMain cycle avg time = %lu msec, max time = %4u, Lua mem = %d b\n",
                    all_time / cycles_cnt, max_cycle_time,
                    lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNT, 0 ) * 1024 +
                    lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNTB, 0 ) );
                print_cycle_time_count++;
                fflush( stdout );
                }

            all_time   = 0;
            cycles_cnt = 0;
            }
        //-Информация о времени выполнения цикла программы.!->
#endif // TEST_SPEED
        }

    return( EXIT_SUCCESS );
    }
