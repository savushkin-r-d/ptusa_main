#include <stdlib.h>

#include "PtusaMainPrg.hpp"
#include "Arp/System/Commons/Logging.h"
#include "Arp/System/Core/ByteConverter.hpp"

#include "dtime.h"
#include "log.h"

#include "prj_mngr.h"
#include "lua_manager.h"

#include "iot_common.h"
#include "device/device.h"
#include "PAC_err.h"
#include "error.h"
#include "tech_def.h"

#ifdef OPCUA
#include "OPCUAServer.h"
#endif

#include "main_cycle.h"

int G_DEBUG = 1;    //Вывод дополнительной отладочной информации.
int G_USE_LOG = 1;  //Вывод в системный лог (syslog).

// В Linux по умолчанию обмен с модулями ввода/вывода включен.
bool G_NO_IO_NODES = false;     
bool G_READ_ONLY_IO_NODES = false;

namespace PtusaPLCnextEngineer
    {

    void PtusaMainPrg::Execute()
        {
        static long int sleep_time_ms = 2;

        if ( ptusaMainCmpnt.init_flag && !ptusaMainCmpnt.error_flag )
            {
            G_LOG->info( "Program started (version %s).",
                PRODUCT_VERSION_FULL_STR );

            NV_memory_manager::get_instance()->init_ex( NVRAM );

            G_PROJECT_MANAGER->init_path( "/opt/main/" );
            G_PROJECT_MANAGER->init_sys_path( "/opt/main/sys/" );

            chdir( "/opt/main/" );

            int res = G_LUA_MANAGER->init( 0, "/opt/main/main.plua",
                G_PROJECT_MANAGER->path.c_str(),
                G_PROJECT_MANAGER->sys_path.c_str() );   //-Инициализация Lua.

            if ( res ) //-Ошибка инициализации.
                {
                sprintf( G_LOG->msg, "Lua init returned error code %d!", res );
                G_LOG->write_log( i_log::P_ALERT );
                ptusaMainCmpnt.error_flag = true;
                return;
                }

#ifdef OPCUA
            if ( G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] == 1 )
                {
                UA_StatusCode retval = G_OPCUA_SERVER.init_all_and_start();
                if ( retval != UA_STATUSCODE_GOOD )
                    {
                    G_LOG->critical( "OPC UA server start failed. Returned error code %d!",
                        retval );
                    return;
                    }
                }
#endif

            sprintf( G_LOG->msg, "Starting main loop! Sleep time is %li ms.",
                sleep_time_ms );
            G_LOG->write_log( i_log::P_INFO );

            ptusaMainCmpnt.init_flag = false;
            ptusaMainCmpnt.running = 1;
            }

        while ( ptusaMainCmpnt.running )
            {
            main_cycle();
            }

#ifdef OPCUA
            G_OPCUA_SERVER.shutdown();
#endif
        }

    } // end of namespace PtusaPLCnextEngineer
