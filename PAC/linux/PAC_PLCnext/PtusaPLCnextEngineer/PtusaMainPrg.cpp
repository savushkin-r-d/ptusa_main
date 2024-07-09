#include "PtusaMainPrg.hpp"

#include <stdlib.h>

#include "Arp/System/Commons/Logging.h"
#include "Arp/System/Core/ByteConverter.hpp"
#include "PAC_dev.h"
#include "PAC_err.h"
#include "dtime.h"
#include "error.h"
#include "iot_common.h"
#include "log.h"
#include "lua_manager.h"
#include "prj_mngr.h"
#include "tech_def.h"

#ifdef OPCUA
#include "OPCUAServer.h"
#endif

int G_DEBUG = 1;  // Вывод дополнительной отладочной информации.
int G_USE_LOG = 1;  // Вывод в системный лог (syslog).

bool G_NO_IO_MODULES = false; // По умолчанию обмен с модулями включен.

namespace PtusaPLCnextEngineer
    {

void PtusaMainPrg::Execute() {
  static long int sleep_time_ms = 2;

  if (ptusaMainCmpnt.init_flag && !ptusaMainCmpnt.error_flag) {
    G_LOG->info("Program started (version %s).", PRODUCT_VERSION_FULL_STR);

    NV_memory_manager::get_instance()->init_ex(NVRAM);

    G_PROJECT_MANAGER->init_path("/opt/main/");
    G_PROJECT_MANAGER->init_sys_path("/opt/main/sys/");

    chdir("/opt/main/");

    int res = G_LUA_MANAGER->init(
        0, "/opt/main/main.plua", G_PROJECT_MANAGER->path.c_str(),
        G_PROJECT_MANAGER->sys_path.c_str());  //-Инициализация Lua.

    if (res)  //-Ошибка инициализации.
    {
      sprintf(G_LOG->msg, "Lua init returned error code %d!", res);
      G_LOG->write_log(i_log::P_ALERT);
      ptusaMainCmpnt.error_flag = true;
      return;
    }

#ifdef OPCUA
    if (G_PAC_INFO()->par[PAC_info::P_IS_OPC_UA_SERVER_ACTIVE] == 1) {
      UA_StatusCode retval = G_OPCUA_SERVER.init_all_and_start();
      if (retval != UA_STATUSCODE_GOOD) {
        G_LOG->critical("OPC UA server start failed. Returned error code %d!",
                        retval);
        return;
      }
    }
#endif

    sprintf(G_LOG->msg, "Starting main loop! Sleep time is %li ms.",
            sleep_time_ms);
    G_LOG->write_log(i_log::P_INFO);

    ptusaMainCmpnt.init_flag = false;
    ptusaMainCmpnt.running = 1;
  }

  while (ptusaMainCmpnt.running) {
#ifdef TEST_SPEED
    static u_long st_time;
    static u_long all_time = 0;
    static u_long cycles_cnt = 0;

    st_time = get_millisec();
    cycles_cnt++;
#endif  // TEST_SPEED

    const int LUA_GC_SIZE = 200;
    lua_gc(G_LUA_MANAGER->get_Lua(), LUA_GCSTEP, LUA_GC_SIZE);
    sleep_ms(sleep_time_ms);

            if ( !G_NO_IO_MODULES )
                {
                G_IO_MANAGER()->read_inputs();
                sleep_ms( sleep_time_ms );
                }


            G_DEVICE_MANAGER()->evaluate_io();
            valve::evaluate();
            valve_bottom_mix_proof::evaluate();

    G_TECH_OBJECT_MNGR()->evaluate();

    sleep_ms(sleep_time_ms);

        if ( !G_NO_IO_MODULES )
            {
            G_IO_MANAGER()->write_outputs();
            sleep_ms( sleep_time_ms );
            }


            G_CMMCTR->evaluate();
#ifdef OPCUA
    if (G_PAC_INFO()->par[PAC_info::P_IS_OPC_UA_SERVER_ACTIVE] == 1) {
      G_OPCUA_SERVER.evaluate();
    }
#endif
    // Основной цикл работы с дополнительными устройствами
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
    if (val != prev_val) {
      std::stringstream ss;
      ss << std::bitset<8 * sizeof(int)>((int)val);

      sprintf(G_LOG->msg, "%s st = %d, val = %.4f %s", dev->get_name(),
              dev->get_state(), val, ss.str().c_str());
      G_LOG->write_log(i_log::P_WARNING);

      prev_val = val;
    }
#endif  // TEST_DEVICE_N

#ifdef TEST_SPEED
    u_int TRESH_AVG =
        G_PAC_INFO()->par[PAC_info::P_MAIN_CYCLE_WARN_ANSWER_AVG_TIME];

    //-Информация о времени выполнения цикла программы.!->
    all_time += get_delta_millisec(st_time);

    static u_int cycle_time = 0;
    cycle_time = get_delta_millisec(st_time);

    static u_int max_iteration_cycle_time = 0;
    static u_int cycles_per_period = 0;
    cycles_per_period++;

    static time_t t_;
    struct tm* timeInfo_;
    t_ = time(0);
    timeInfo_ = localtime(&t_);
    static int print_cycle_last_h = timeInfo_->tm_hour;

    if (max_iteration_cycle_time < cycle_time) {
      max_iteration_cycle_time = cycle_time;
    }

    // Once per hour writing performance info.
    if (print_cycle_last_h != timeInfo_->tm_hour) {
      u_long avg_time = all_time / cycles_cnt;

      const unsigned int KILOBYTE = 1024;
      if (TRESH_AVG < avg_time) {
        sprintf(G_LOG->msg,
                "Main control cycle avg time above threshold : "
                "%4lu > %4u ms (Lua mem = %d b).",
                avg_time, TRESH_AVG,
                lua_gc(G_LUA_MANAGER->get_Lua(), LUA_GCCOUNT, 0) * KILOBYTE +
                    lua_gc(G_LUA_MANAGER->get_Lua(), LUA_GCCOUNTB, 0));
        G_LOG->write_log(i_log::P_ALERT);
      }

      sprintf(
          G_LOG->msg,
          "Main control cycle performance : "
          "avg = %lu, max = %4u, tresh = %4u ms (%4u cycles, Lua mem = %d b).",
          avg_time, max_iteration_cycle_time, TRESH_AVG, cycles_per_period,
          lua_gc(G_LUA_MANAGER->get_Lua(), LUA_GCCOUNT, 0) * KILOBYTE +
              lua_gc(G_LUA_MANAGER->get_Lua(), LUA_GCCOUNTB, 0));
      G_LOG->write_log(i_log::P_INFO);

      all_time = 0;
      cycles_cnt = 0;
      max_iteration_cycle_time = 0;
      cycles_per_period = 0;
      print_cycle_last_h = timeInfo_->tm_hour;
    }
    //-Информация о времени выполнения цикла программы.!->
#endif  // TEST_SPEED
  }

#ifdef OPCUA
  G_OPCUA_SERVER.shutdown();
#endif
}

}  // end of namespace PtusaPLCnextEngineer
