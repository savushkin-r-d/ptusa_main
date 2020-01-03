#include <stdlib.h>

#include "PtusaMainPrg.hpp"
#include "Arp/System/Commons/Logging.h"
#include "Arp/System/Core/ByteConverter.hpp"

#include "dtime.h"
#include "log.h"

#include "prj_mngr.h"
#include "lua_manager.h"

#include "iot_common.h"
#include "PAC_dev.h"
#include "PAC_err.h"
#include "error.h"
#include "tech_def.h"

int G_DEBUG   = 1; //Вывод дополнительной отладочной информации.
int G_USE_LOG = 1; //Вывод в системный лог (syslog).

namespace PtusaPLCnextEngineer
    {

    void PtusaMainPrg::Execute()
        {
        static long int sleep_time_ms = 2;

        if (ptusaMainCmpnt.init_flag)
            {
            sprintf(G_LOG->msg, "Program started.");
            G_LOG->write_log(i_log::P_INFO);

            NV_memory_manager::get_instance()->init_ex(NVRAM);

            G_PROJECT_MANAGER->init_path( "/opt/main/" );
            G_PROJECT_MANAGER->init_sys_path( "/opt/main/" );


            int res = G_LUA_MANAGER->init(0, "/opt/main/main.plua",
                G_PROJECT_MANAGER->path.c_str(),
                G_PROJECT_MANAGER->sys_path.c_str());   //-Инициализация Lua.

            if (res) //-Ошибка инициализации.
                {
                sprintf( G_LOG->msg, "Lua init returned error code %d!", res);
                G_LOG->write_log(i_log::P_ALERT);
                return;
                }

#ifdef USE_PROFIBUS
           if ( G_PROFIBUS_SLAVE()->is_active() )
               {
               G_PROFIBUS_SLAVE()->init();
               }
#endif // USE_PROFIBUS

#ifdef OPCUA
           OPCUAServer::getInstance().UserInit();

           UA_StatusCode retval = OPCUAServer::getInstance().Start();
           if(retval != UA_STATUSCODE_GOOD)
               {
               sprintf( G_LOG->msg, "OPC UA server start failed. Returned error code %d!", retval );
               G_LOG->write_log( i_log::P_CRIT );
               debug_break;
               return;
               }
#endif

            sprintf( G_LOG->msg, "Starting main loop! Sleep time is %li ms.",
                    sleep_time_ms);
            G_LOG->write_log(i_log::P_INFO);

            ptusaMainCmpnt.init_flag = false;
            }

        while (ptusaMainCmpnt.running)
            {
#ifdef TEST_SPEED
               static u_long st_time;
               static u_long all_time   = 0;
               static u_long cycles_cnt = 0;

               st_time = get_millisec();
               cycles_cnt++;
       #endif // TEST_SPEED

            lua_gc(G_LUA_MANAGER->get_Lua(), LUA_GCSTEP, 200);
            sleep_ms(sleep_time_ms);

#ifndef DEBUG_NO_WAGO_MODULES
            G_IO_MANAGER()->read_inputs();
            sleep_ms(sleep_time_ms);
#endif // DEBUG_NO_WAGO_MODULES

            valve::evaluate();
            valve_bottom_mix_proof::evaluate();
            concentration_e_iolink::evaluate();
            valve_iolink_mix_proof::evaluate();

            G_TECH_OBJECT_MNGR()->evaluate();
            sleep_ms(sleep_time_ms);

#ifndef DEBUG_NO_WAGO_MODULES
            G_IO_MANAGER()->write_outputs();
            sleep_ms(sleep_time_ms);
#endif // ifndef

            G_CMMCTR->evaluate();
#ifdef OPCUA
               OPCUAServer::getInstance().Evaluate();
       #endif
            //Основной цикл работы с дополнительными устройствами
            IOT_EVALUATE();

            sleep_ms(sleep_time_ms);

            PAC_info::get_instance()->eval();
            PAC_critical_errors_manager::get_instance()->show_errors();
            G_ERRORS_MANAGER->evaluate();
            G_SIREN_LIGHTS_MANAGER()->eval();
            sleep_ms(sleep_time_ms);

#ifdef TEST_DEVICE_N
            auto dev_idx = TEST_DEVICE_N;
            auto dev = DEVICE(dev_idx);
            auto val = dev->get_value();
            static auto prev_val = -0.1f;
            if (val != prev_val)
                {
                std::stringstream ss;
                ss << std::bitset < 8 * sizeof(int) > ((int) val);

                sprintf( G_LOG->msg, "%s st = %d, val = %.4f %s", dev->get_name(),
                        dev->get_state(), val, ss.str().c_str());
                G_LOG->write_log(i_log::P_WARNING);

                prev_val = val;
                }
#endif // TEST_DEVICE_N

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
                   cycles_per_period        = 0;
                   print_cycle_last_h       = timeInfo_->tm_hour;
                   }
               //-Информация о времени выполнения цикла программы.!->
       #endif // TEST_SPEED
            }

#ifdef OPCUA
           OPCUAServer::getInstance().Shutdown();
#endif
        }

    } // end of namespace PtusaPLCnextEngineer
