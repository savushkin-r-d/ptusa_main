///// @par Описание директив препроцессора:
///// @c LINUX_OS         - компиляция для ОС Linux.
/// @par Тип PAC:
/// @c PAC_PC           - PAC на PC с ОС Linux.
/// @c PAC_WAGO_750_860 - PAC Wago 750-860.
///
/// @c WIN_OS           - компиляция для ОС Windows.
///
/// @c DEBUG - компиляция c выводом отладочной информации в консоль.@n@n

#include <stdlib.h>
#include "fcntl.h"

#include "dtime.h"

#include "prj_mngr.h"
#include "PAC_dev.h"
#include "PAC_info.h"
#include "tech_def.h"
#include "lua_manager.h"
#include "PAC_err.h"

#include "rm_manager.h"
#include "log.h"
#ifdef PAC_WAGO_750_860
#include "l_log.h"
#endif

#include "profibus_slave.h"

int main( int argc, char *argv[] )
    {
#if defined WIN_OS
    setlocale( LC_ALL, "" );
#endif

    if ( argc < 2 )
        {
        fprintf( stderr, "Usage: main script.plua\n" );
        return EXIT_SUCCESS;
        }
#ifdef PAC_WAGO_750_860
    log_mngr::lg = new l_log();
#endif

    sprintf( G_LOG->msg, "Program started.\n" );
    G_LOG->write_log( i_log::P_INFO );

    G_PROJECT_MANAGER->proc_main_params( argc, argv );

    int res = G_LUA_MANAGER->init( 0, argv[ 1 ] ); //-Инициализация Lua.

    if ( res ) //-Ошибка инициализации.
        {
        sprintf( G_LOG->msg, "Lua init error - %d!\n", res );
        G_LOG->write_log( i_log::P_CRIT );

        debug_break;
        return EXIT_FAILURE;
        }

#ifdef DEBUG
    fflush( stdout );
#endif // DEBUG

    if ( G_PROFIBUS_SLAVE()->is_active() )
        {
        G_PROFIBUS_SLAVE()->init();
        }

    long int sleep_time_ms = 2;
    if ( argc >= 3 )
        {
        char *stopstring;
        sleep_time_ms = strtol( argv[ 2 ], &stopstring, 10 );
        }

    sprintf( G_LOG->msg, "Starting main loop! Sleep time is %li ms.\n",
	    sleep_time_ms);
    G_LOG->write_log( i_log::P_INFO );

#ifdef DEBUG
    while ( !kb_hit() )
#else
    while ( 1 )
#endif // DEBUG
        {
#ifdef TEST_SPEED
        static u_long st_time;
        static u_long all_time   = 0;
        static u_long cycles_cnt = 0;

        st_time = get_millisec();
        cycles_cnt++;
#endif // TEST_SPEED

#ifdef DEBUG
        fflush( stdout );
#endif // DEBUG

        lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCSTEP, 200 );
        sleep_ms( sleep_time_ms );

#ifndef DEBUG_NO_WAGO_MODULES
        G_WAGO_MANAGER()->read_inputs();
        sleep_ms( sleep_time_ms );
#endif // DEBUG_NO_WAGO_MODULES

        valve::evaluate();
        valve_bottom_mix_proof::evaluate();
        G_TECH_OBJECT_MNGR()->evaluate();
        sleep_ms( sleep_time_ms );

#ifndef DEBUG_NO_WAGO_MODULES
        G_WAGO_MANAGER()->write_outputs();
        sleep_ms( sleep_time_ms );
#endif // ifndef

        G_CMMCTR->evaluate();
        sleep_ms( sleep_time_ms );

        PAC_info::get_instance()->eval();
        PAC_critical_errors_manager::get_instance()->show_errors();
        G_ERRORS_MANAGER->evaluate();
        G_SIREN_LIGHTS_MANAGER()->eval();
        sleep_ms( sleep_time_ms );

#ifdef RM_PAC
        // Связь с удаленными PAC.
        G_RM_MANAGER()->evaluate();
#endif // RM_PAC

        if ( G_PROFIBUS_SLAVE()->is_active() )
            {
            G_PROFIBUS_SLAVE()->eval();
            }


#ifdef TEST_SPEED
        //-Информация о времени выполнения цикла программы.!->
        all_time += get_delta_millisec( st_time );

        static u_int max_cycle_time = 500;
        static u_int cycle_time = 0;
        cycle_time = get_delta_millisec( st_time );

        if ( max_cycle_time < cycle_time )
            {
            max_cycle_time = cycle_time;

            sprintf( G_LOG->msg,
                "Main cycle avg = %lu ms, max = %4u ms, Lua mem = %d b.\n",
                all_time / cycles_cnt, max_cycle_time,
                lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNT, 0 ) * 1024 +
                lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNTB, 0 ) );
            G_LOG->write_log( i_log::P_INFO );
            }

        static u_int max_iteration_cycle_time = 0;
        static u_int cycles_per_period        = 0;
        cycles_per_period++;

        static time_t t_;
        struct tm *timeInfo_;
        t_ = time( 0 );
        timeInfo_ = localtime( &t_ );
        static int print_cycle_last_h = timeInfo_->tm_hour;

        if ( max_iteration_cycle_time < cycle_time )
            {
            max_iteration_cycle_time = cycle_time;
            }

        //Once per hour writing performance info.
        if ( print_cycle_last_h != timeInfo_->tm_hour )
            {
            u_long avg_time = all_time / cycles_cnt;
            sprintf( G_LOG->msg,
                "%4u cycles, avg = %lu, max = %4u (ms), Lua mem = %d b.\n",
                cycles_per_period,
                avg_time, max_iteration_cycle_time,
                lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNT, 0 ) * 1024 +
                lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNTB, 0 ) );
            G_LOG->write_log( i_log::P_INFO );

            //At 0:0:0 set max time to 2*average.
            if ( timeInfo_->tm_hour == 0 )
                {
                max_cycle_time = avg_time + avg_time;
                }

            all_time   = 0;
            cycles_cnt = 0;
            max_iteration_cycle_time = 0;
            cycles_per_period 	     = 0;
            print_cycle_last_h       = timeInfo_->tm_hour;
            }
        //-Информация о времени выполнения цикла программы.!->
#endif // TEST_SPEED
        }

    return( EXIT_SUCCESS );
    }

