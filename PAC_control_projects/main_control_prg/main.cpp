/// @par Описание директив препроцессора:
/// @c LINUX_OS         - компиляция для ОС Linux.
/// @par Тип PAC:
/// @c PAC_PC           - PAC на PC с ОС Linux.
/// @c PAC_WAGO_750_860 - PAC Wago 750-860.
///
/// @c WIN_OS           - компиляция для ОС Windows.
///

#include <stdlib.h>
#include <signal.h>
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
#ifdef PAC_WAGO_750_860r
#include "l_log.h"
#endif

#include "profibus_slave.h"
#include "iot_common.h"

#ifdef OPCUA
#include "OPCUAServer.h"
#endif

#ifdef RFID
#include "rfid_reader.h"
#endif

int G_DEBUG = 0;    //Вывод дополнительной отладочной информации.
int G_USE_LOG = 0;  //Вывод в системный лог (syslog).

int running = 1;
static void stopHandler(int sig)
    {
    running = 0;
    }


int main( int argc, const char *argv[] )
    {
#if defined WIN_OS
    setlocale(LC_ALL, "ru_RU.UTF-8");
#endif

    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    if ( argc < 2 )
        {
        printf( "Usage: main script.plua\n" );
        return EXIT_SUCCESS;
        }
#ifdef PAC_WAGO_750_860
    log_mngr::lg = new l_log();
#endif

    sprintf( G_LOG->msg, "Program started." );
    G_LOG->write_log( i_log::P_INFO );
#ifdef OPCUA
    OPCUAServer::getInstance().Init(4840);
#endif

    G_PROJECT_MANAGER->proc_main_params( argc, argv );

    //-Инициализация Lua.
    int res = G_LUA_MANAGER->init( 0, argv[ 1 ], 
        G_PROJECT_MANAGER->path.c_str(), G_PROJECT_MANAGER->sys_path.c_str() );

    if ( res ) //-Ошибка инициализации.
        {
        sprintf( G_LOG->msg, "Lua init returned error code %d!", res );
        G_LOG->write_log( i_log::P_ALERT );

        debug_break;
        return EXIT_FAILURE;
        }

#ifdef USE_PROFIBUS
    if ( G_PROFIBUS_SLAVE()->is_active() )
        {
        G_PROFIBUS_SLAVE()->init();
        }
#endif // USE_PROFIBUS

    long int sleep_time_ms = 2;
    if ( argc >= 3 )
        {
        char *stopstring;
        sleep_time_ms = strtol( argv[ 2 ], &stopstring, 10 );
        }

#ifdef OPCUA
    OPCUAServer::getInstance().UserInit();
    //OPCUAServer::getInstance().BaseConfig();

    UA_StatusCode retval = OPCUAServer::getInstance().Start();
    if(retval != UA_STATUSCODE_GOOD)
        {
        sprintf( G_LOG->msg, "OPC UA server start failed. Returned error code %d!", retval );
        G_LOG->write_log( i_log::P_CRIT );
        debug_break;
        return EXIT_FAILURE;
        }
#endif

    //Инициализация дополнительных устройств
    IOT_INIT();

    sprintf( G_LOG->msg, "Starting main loop! Sleep time is %li ms.",
        sleep_time_ms);
    G_LOG->write_log( i_log::P_INFO );

    while ( running )
        {
        if ( G_DEBUG )
            {
            fflush( stdout );
            }

#ifdef TEST_SPEED
        static u_long st_time;
        static u_long all_time   = 0;
        static u_long cycles_cnt = 0;

        st_time = get_millisec();
        cycles_cnt++;
#endif // TEST_SPEED

        lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCSTEP, 200 );
        sleep_ms( sleep_time_ms );

#ifndef DEBUG_NO_IO_MODULES
        G_IO_MANAGER()->read_inputs();
        sleep_ms( sleep_time_ms );
#endif // DEBUG_NO_IO_MODULES

        G_DEVICE_MANAGER()->evaluate_io();

        valve::evaluate();
        valve_bottom_mix_proof::evaluate();

        G_TECH_OBJECT_MNGR()->evaluate();
        sleep_ms( sleep_time_ms );

#ifndef DEBUG_NO_IO_MODULES
        G_IO_MANAGER()->write_outputs();
        sleep_ms( sleep_time_ms );
#endif // ifndef

        G_CMMCTR->evaluate();
#ifdef OPCUA
        OPCUAServer::getInstance().Evaluate();
#endif
        //Основной цикл работы с дополнительными устройствами
        IOT_EVALUATE();

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

#ifdef USE_PROFIBUS
        if ( G_PROFIBUS_SLAVE()->is_active() )
            {
            G_PROFIBUS_SLAVE()->eval();
            }
#endif // USE_PROFIBUS

#ifdef TEST_SPEED
        u_int TRESH_AVG =
            G_PAC_INFO()->par[ PAC_info::P_MAIN_CYCLE_WARN_ANSWER_AVG_TIME ];

        //-Информация о времени выполнения цикла программы.!->
        all_time += get_delta_millisec( st_time );

        static u_int cycle_time = 0;
        cycle_time = get_delta_millisec( st_time );

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

            if ( TRESH_AVG < avg_time )
                {
                sprintf( G_LOG->msg,
                    "Main control cycle avg time above threshold : "
                    "%4lu > %4u ms (Lua mem = %d b).",
                    avg_time, TRESH_AVG,
                    lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNT, 0 ) * 1024 +
                    lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNTB, 0 ) );
                G_LOG->write_log( i_log::P_ALERT );
                }

            sprintf( G_LOG->msg,
                "Main control cycle performance : "
                "avg = %lu, max = %4u, tresh = %4u ms (%4u cycles, Lua mem = %d b).",
                avg_time, max_iteration_cycle_time, TRESH_AVG,
                cycles_per_period,
                lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNT, 0 ) * 1024 +
                lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNTB, 0 ) );
            G_LOG->write_log( i_log::P_INFO );

            all_time   = 0;
            cycles_cnt = 0;
            max_iteration_cycle_time = 0;
            cycles_per_period 	     = 0;
            print_cycle_last_h       = timeInfo_->tm_hour;
            }
        //-Информация о времени выполнения цикла программы.!->
#endif // TEST_SPEED
        }
#ifdef OPCUA
    OPCUAServer::getInstance().Shutdown();
#endif
    //Деинициализация дополнительных устройств.
    IOT_FINAL();

    return( EXIT_SUCCESS );
    }

