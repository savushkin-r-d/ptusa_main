#include "PAC_info_tests.h"

using namespace ::testing;

TEST( PAC_info, set_cmd )
    {
    G_PAC_INFO()->set_cmd( "CMD", 0, PAC_info::RELOAD_RESTRICTIONS );
    }

TEST( PAC_info, reset_params )
    {
    G_PAC_INFO()->par[ PAC_info::P_MIX_FLIP_PERIOD ] = 100;
    EXPECT_EQ( 100, G_PAC_INFO()->par[ PAC_info::P_MIX_FLIP_PERIOD ] );
    G_PAC_INFO()->reset_params();
    EXPECT_EQ( 180, G_PAC_INFO()->par[ PAC_info::P_MIX_FLIP_PERIOD ] );
    }

TEST( PAC_info, save_device )
    {   
    const auto MAX_SIZE = 1000;
    const auto REF_STR =
        "t.SYSTEM = \n"
            "\t{\n"
            "\tRESET_BY=1,\n"
            "\tUP_DAYS=0,\n"
            "\tUP_HOURS=0,\n"
            "\tUP_MINS=0,\n"
            "\tUP_SECS=0,\n"
            "\tUP_TIME=\"0 дн. 0:0:0\",\n"
            "\tWASH_VALVE_SEAT_PERIOD=180,\n"
            "\tWASH_VALVE_UPPER_SEAT_TIME=2000,\n"
            "\tWASH_VALVE_LOWER_SEAT_TIME=1000,\n"
            "\tP_V_OFF_DELAY_TIME=1000,\n"
            "\tP_V_BOTTOM_ON_DELAY_TIME=1200,\n"
            "\tP_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME=50,\n"
            "\tP_MAIN_CYCLE_WARN_ANSWER_AVG_TIME=300,\n"
            "\tP_RESTRICTIONS_MODE=0,\n"
            "\tP_RESTRICTIONS_MANUAL_TIME=120000,\n"
            "\tP_AUTO_PAUSE_OPER_ON_DEV_ERR=0,\n"
            "\tCMD=1,\n"
            "\tCMD_ANSWER=\"\",\n"
            "\tVERSION=\"2023.8.3.0\",\n"
            "\tNODEENABLED = \n"
            "\t{\n"
            "\t\n"
            "\t},\n"
            "\tP_IS_OPC_UA_SERVER_ACTIVE=0,\n"
            "\t}\n";
    char buff[ MAX_SIZE ] = {0};

    G_PAC_INFO()->save_device( buff );
    EXPECT_STRNE( "", buff );
    }
