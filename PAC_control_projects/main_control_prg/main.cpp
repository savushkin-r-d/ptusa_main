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
#include <codecvt>

#include <fmt/core.h> 

#include "dtime.h"

#include "prj_mngr.h"
#include "device/device.h"
#include "PAC_info.h"
#include "tech_def.h"
#include "lua_manager.h"
#include "PAC_err.h"
#include "version_info.h"

#ifdef WIN_OS
#include <shellapi.h>
#endif

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

int G_DEBUG = 0;                //Вывод дополнительной отладочной информации.
int G_USE_LOG = 0;              //Вывод в системный лог (syslog).

// Обмен с модулями ввода/вывода.
#if defined WIN_OS
bool G_NO_IO_NODES = true;  // В Windows по умолчанию обмен отключен.
#else
bool G_NO_IO_NODES = false; // В Linux по умолчанию обмен включен.
#endif

bool G_READ_ONLY_IO_NODES = false;

int running = 1;
static void stopHandler(int sig)
    {
    running = 0;
    }

int main( int argc, const char *argv[] )
    {
#if defined WIN_OS
    setlocale( LC_ALL, "ru_RU.UTF-8" );
    setlocale( LC_NUMERIC, "C" );

    const char** argv_utf8 = new const char* [ argc ];
    int w_argc;
    wchar_t** w_argv = CommandLineToArgvW( GetCommandLineW(), &w_argc );
    for ( int i = 0; i < argc; i++ )
        {
        wchar_t* w_path = w_argv[ i ];
        int utf16len = wcslen( w_path );
        int utf8len = WideCharToMultiByte( CP_UTF8, 0, w_path, utf16len,
            NULL, 0, NULL, NULL );

        char* path = new char[ utf8len + 1 ];
        memset( path, 0, utf8len + 1 );

        WideCharToMultiByte( CP_UTF8, 0, w_path, utf16len, path, utf8len, 0, 0 );
        argv_utf8[ i ] = path;
        }
#else
    const char** argv_utf8 = argv;
#endif

    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);
    signal(SIGABRT, stopHandler);
    signal(SIGFPE, stopHandler);
    signal(SIGILL, stopHandler);
    signal(SIGSEGV, stopHandler);

#ifdef PAC_WAGO_750_860
    log_mngr::lg = new l_log();
#endif

    G_LOG->info( "Program started (version %s).", PRODUCT_VERSION_FULL_STR );

    //-Работа с параметрами командной строки.
    int res = G_PROJECT_MANAGER->proc_main_params( argc, argv_utf8 );
    if ( res )
        {
        exit( EXIT_SUCCESS );
        }

    //-Инициализация Lua.
    res = G_LUA_MANAGER->init( nullptr, G_PROJECT_MANAGER->main_script.c_str(),
        G_PROJECT_MANAGER->path.c_str(), G_PROJECT_MANAGER->sys_path.c_str(),
        G_PROJECT_MANAGER->extra_paths.c_str() );

    if ( res ) //-Ошибка инициализации.
        {
        G_LOG->alert( "Lua init returned error code %d!", res );

        debug_break;
        return EXIT_FAILURE;
        }

#ifdef USE_PROFIBUS
    if ( G_PROFIBUS_SLAVE()->is_active() )
        {
        G_PROFIBUS_SLAVE()->init();
        }
#endif // USE_PROFIBUS

#if defined WIN_OS
    for ( int i = 0; i < argc; i++ )
        {
        delete[] argv_utf8[ i ];
        argv_utf8[ i ] = 0;
        }
    delete[] argv_utf8;
    argv_utf8 = 0;
#endif

#ifdef OPCUA
    if ( G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] == 1 )
        {
        UA_StatusCode retval = G_OPCUA_SERVER.init_all_and_start();
        if ( retval != UA_STATUSCODE_GOOD )
            {
            G_LOG->critical( "OPC UA server start failed. Returned error code %d!",
                retval );
            return EXIT_FAILURE;
            }
        }
#endif

    //Инициализация дополнительных устройств
    IOT_INIT();

    G_LOG->info( "Starting main loop! Sleep time is %u ms.",
        G_PROJECT_MANAGER->sleep_time_ms );

    while ( running )
        {
#ifdef TEST_SPEED
        static u_long st_time;
        st_time = get_millisec();

        static u_long all_time = 0;

        static u_long cycles_cnt = 0;
        cycles_cnt++;

#endif // TEST_SPEED

        if ( G_DEBUG )
            {
            fflush( stdout );
            }

        lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCSTEP, 200 );
        sleep_ms( G_PROJECT_MANAGER->sleep_time_ms );

        if ( !G_NO_IO_NODES ) G_IO_MANAGER()->read_inputs();
        sleep_ms( G_PROJECT_MANAGER->sleep_time_ms );

        G_DEVICE_MANAGER()->evaluate_io();

        valve::evaluate();

        G_TECH_OBJECT_MNGR()->evaluate();
        sleep_ms( G_PROJECT_MANAGER->sleep_time_ms );

        if ( !G_NO_IO_NODES &&
            !G_READ_ONLY_IO_NODES ) G_IO_MANAGER()->write_outputs();
        sleep_ms( G_PROJECT_MANAGER->sleep_time_ms );

        G_CMMCTR->evaluate();
#ifdef OPCUA
        if ( G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] == 1 )
            {
            G_OPCUA_SERVER.evaluate();
            }
#endif
        //Основной цикл работы с дополнительными устройствами
        if (!G_NO_IO_NODES && !G_READ_ONLY_IO_NODES)
            {
            IOT_EVALUATE( );
            }

        sleep_ms( G_PROJECT_MANAGER->sleep_time_ms );

        PAC_info::get_instance()->eval();
        PAC_critical_errors_manager::get_instance()->show_errors();
        G_ERRORS_MANAGER->evaluate();
        G_SIREN_LIGHTS_MANAGER()->eval();
        sleep_ms( G_PROJECT_MANAGER->sleep_time_ms );

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

        static time_t t_now;
        struct tm *timeInfo_;
        t_now = time( 0 );
        timeInfo_ = localtime( &t_now );
        static int print_cycle_last_h = timeInfo_->tm_hour;

        static u_int max_iteration_cycle_time = 0;
        static u_int cycles_per_period = 0;
        cycles_per_period++;

        static u_int cycle_time = 0;
        cycle_time = get_delta_millisec(st_time);
        G_PAC_INFO()->par[PAC_info::P_CYCLE_TIME] = cycle_time;

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
                G_LOG->alert( "Main control cycle avg time above threshold : "
                    "%4lu > %4u ms (Lua mem = %d b).",
                    avg_time, TRESH_AVG,
                    lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNT, 0 ) * 1024 +
                    lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNTB, 0 ) );
                }

            G_LOG->info( "Main control cycle performance : "
                "avg = %lu, max = %4u, tresh = %4u ms (%4u cycles, Lua mem = %d b).",
                avg_time, max_iteration_cycle_time, TRESH_AVG,
                cycles_per_period,
                lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNT, 0 ) * 1024 +
                lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNTB, 0 ) );

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
    G_OPCUA_SERVER.shutdown();
#endif
    //Деинициализация дополнительных устройств.
    IOT_FINAL();

    return( EXIT_SUCCESS );
    }
