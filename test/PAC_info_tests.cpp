#include "PAC_info_tests.h"

#include "bus_coupler_io.h"

using namespace ::testing;

TEST(PAC_info, set_cmd) {
  G_PAC_INFO()->set_cmd("CMD", 0, PAC_info::RELOAD_RESTRICTIONS);
#ifdef OPCUA
  EXPECT_EQ(0, G_PAC_INFO()->par[PAC_info::P_IS_OPC_UA_SERVER_ACTIVE]);
  G_PAC_INFO()->set_cmd("P_IS_OPC_UA_SERVER_ACTIVE", 0, 1);
  EXPECT_EQ(1, G_PAC_INFO()->par[PAC_info::P_IS_OPC_UA_SERVER_ACTIVE]);
  G_PAC_INFO()->set_cmd("P_IS_OPC_UA_SERVER_ACTIVE", 0, 0);
  EXPECT_EQ(0, G_PAC_INFO()->par[PAC_info::P_IS_OPC_UA_SERVER_ACTIVE]);
#endif
}

TEST(PAC_info, reset_params) {
  G_PAC_INFO()->par[PAC_info::P_MIX_FLIP_PERIOD] = 100;
  EXPECT_EQ(100, G_PAC_INFO()->par[PAC_info::P_MIX_FLIP_PERIOD]);
  G_PAC_INFO()->reset_params();
  EXPECT_EQ(180, G_PAC_INFO()->par[PAC_info::P_MIX_FLIP_PERIOD]);
}

TEST(PAC_info, save_device) {
  io_manager::get_instance()->init(1);
  io_manager::get_instance()->add_node(
      0, io_manager::io_node::TYPES::PHOENIX_BK_ETH, 100, "127.0.0.1", "A100",
      0, 0, 0, 0, 0, 0);

  G_PAC_INFO()->reset_params();
  G_PAC_INFO()->set_cmd("CMD", 0, PAC_info::CLEAR_RESULT_CMD);

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
      "\tCMD=0,\n"
      "\tCMD_ANSWER=\"\",\n"
      "\tVERSION=\"" PRODUCT_VERSION_FULL_STR
      "\",\n"
      "\tNODEENABLED = \n"
      "\t{\n"
      "\t1, \n"
      "\t},\n"
      "\tP_IS_OPC_UA_SERVER_ACTIVE=0,\n"
      "\tP_IS_OPC_UA_SERVER_CONTROL=0,\n"
      "\t}\n";
  char buff[MAX_SIZE] = {0};

  G_PAC_INFO()->save_device(buff);
  EXPECT_STREQ(REF_STR, buff);
}

TEST(PAC_info, get_name_in_Lua) {
  EXPECT_STREQ("SYSTEM", G_PAC_INFO()->get_name_in_Lua());
}

TEST(PAC_info, get_up_time_str) {
  EXPECT_STREQ("0 дн. 0:0:0", G_PAC_INFO()->get_up_time_str());
}
