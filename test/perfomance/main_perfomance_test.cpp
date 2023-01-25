#include <benchmark/benchmark.h>

#include "g_device.h"

int G_DEBUG = 1;    //Вывод дополнительной отладочной информации.
int G_USE_LOG = 0;  //Вывод в системный лог (syslog).

static void write_zero_devices_states_service( benchmark::State& state ) {
    for ( auto _ : state )
        G_DEVICE_CMMCTR->write_devices_states_service( 0, nullptr, nullptr );
    }

// Register the function as a benchmark
BENCHMARK( write_zero_devices_states_service );

BENCHMARK_MAIN();
