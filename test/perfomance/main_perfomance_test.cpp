#include <benchmark/benchmark.h>
#include <clocale>

#include "g_device.h"
#include "lua_manager.h"
#include "log.h"

int G_DEBUG = 1;    //Вывод дополнительной отладочной информации.
int G_USE_LOG = 0;  //Вывод в системный лог (syslog).

lua_State* L = nullptr;
u_char in_data_devices[] = { device_communicator::CMD_GET_DEVICES };
u_char out_data[ 5048 ] = { 0 };

static void DoSetup( const benchmark::State& state )
    {
    static bool is_init = false;
    if ( !is_init )
        {
#ifdef WIN_OS
        setlocale( LC_ALL, "ru_RU.UTF-8" );
        setlocale( LC_NUMERIC, "C" );
#endif
        L = lua_open();             // Create Lua context.
        lua_gc( L, LUA_GCSTOP, 0 );
        luaL_openlibs( L );         // Open standard libraries.

        G_LUA_MANAGER->init( L,
            "../../../../demo_projects/T1-PLCnext-Demo/main.plua",
            "../../../../demo_projects/T1-PLCnext-Demo/",
            "../../../../demo_projects/T1-PLCnext-Demo/sys/" );

        device_communicator::switch_off_compression();
        auto res = G_DEVICE_CMMCTR->write_devices_states_service(
            1, in_data_devices, out_data );
        printf( "\n");
        printf( "Saved devices uncompressed buffer size:\t%ld\n", res );
        device_communicator::switch_on_compression();
        res = G_DEVICE_CMMCTR->write_devices_states_service(
            1, in_data_devices, out_data );
        printf( "Saved devices compressed buffer size:\t%ld\n", res );

        is_init = true;
        }
    }

static void write_devices_service_with_compression( benchmark::State& state )
    {
    device_communicator::switch_on_compression();
    for ( auto _ : state )
        G_DEVICE_CMMCTR->write_devices_states_service( 1,
            in_data_devices, out_data );
    }

static void write_devices_service_no_compression( benchmark::State& state )
    {
    device_communicator::switch_off_compression();
    for ( auto _ : state )
        G_DEVICE_CMMCTR->write_devices_states_service( 1, 
            in_data_devices, out_data );
    }

// Register the function as a benchmark
BENCHMARK( write_devices_service_no_compression )->Setup( DoSetup )->Unit( benchmark::kMicrosecond );
BENCHMARK( write_devices_service_with_compression )->Setup( DoSetup )->Unit( benchmark::kMicrosecond );

BENCHMARK_MAIN();
