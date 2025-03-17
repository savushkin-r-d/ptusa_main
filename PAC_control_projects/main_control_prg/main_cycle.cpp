#include <cstdio>

#include "dtime.h"
#include "lua_manager.h"
#include "prj_mngr.h"
#include "tech_def.h"
#include "device/manager.h"
#include "device/valve.h"
#include "PAC_info.h"
#include "PAC_err.h"
#include "iot_common.h"
#ifdef OPCUA
#include "OPCUAServer.h"
#endif

extern bool G_NO_IO_NODES;
extern bool G_READ_ONLY_IO_NODES;

int main_cycle()
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
    if ( !G_NO_IO_NODES && !G_READ_ONLY_IO_NODES )
        {
        IOT_EVALUATE();
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

    static tm timeInfo_;
    timeInfo_ = get_time();
    static int print_cycle_last_h = timeInfo_.tm_hour;

    static u_long max_iteration_cycle_time = 0;
    static u_int cycles_per_period = 0;
    cycles_per_period++;

    static u_long cycle_time = 0;
    cycle_time = get_delta_millisec( st_time );
    G_PAC_INFO()->set_cycle_time( cycle_time );

    if ( max_iteration_cycle_time < cycle_time )
        {
        max_iteration_cycle_time = cycle_time;
        }

    //Once per hour writing performance info.
    if ( print_cycle_last_h != timeInfo_.tm_hour )
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
            "avg = %lu, max = %4lu, tresh = %4u ms (%4u cycles, Lua mem = %d b).",
            avg_time, max_iteration_cycle_time, TRESH_AVG,
            cycles_per_period,
            lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNT, 0 ) * 1024 +
            lua_gc( G_LUA_MANAGER->get_Lua(), LUA_GCCOUNTB, 0 ) );

        all_time = 0;
        cycles_cnt = 0;
        max_iteration_cycle_time = 0;
        cycles_per_period = 0;
        print_cycle_last_h = timeInfo_.tm_hour;
        }
    //-Информация о времени выполнения цикла программы.!->
#endif // TEST_SPEED

    return 0;
    }
