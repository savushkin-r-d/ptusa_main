#include "cip_tech_def_tests.h"

#include "lua_manager.h"

using namespace ::testing;

void InitStationParams() {
  printf("===================================Init station params");
  cipline_tech_object::set_station_par(1, 1.5f);
  cipline_tech_object::set_station_par(2, 0.3f);
  cipline_tech_object::set_station_par(3, 0.1f);
  cipline_tech_object::set_station_par(4, 1.0f);
  cipline_tech_object::set_station_par(5, 0.2f);
  cipline_tech_object::set_station_par(6, 0.1f);
  cipline_tech_object::set_station_par(7, 20);
  cipline_tech_object::set_station_par(8, 5);
  cipline_tech_object::set_station_par(9, 0);
  cipline_tech_object::set_station_par(10, 8);
  cipline_tech_object::set_station_par(11, 1.51f);
  cipline_tech_object::set_station_par(12, 1.8f);
  cipline_tech_object::set_station_par(13, 47);
  cipline_tech_object::set_station_par(14, 57);
  cipline_tech_object::set_station_par(15, 1.47f);
  cipline_tech_object::set_station_par(16, 1.45f);
  cipline_tech_object::set_station_par(17, 1);
  cipline_tech_object::set_station_par(18, 52);
  cipline_tech_object::set_station_par(19, 0.5f);
  cipline_tech_object::set_station_par(20, 30.8f);
  cipline_tech_object::set_station_par(21, 0.4f);
  cipline_tech_object::set_station_par(22, 30);
  cipline_tech_object::set_station_par(23, 120);
  cipline_tech_object::set_station_par(24, 50);
  cipline_tech_object::set_station_par(25, 10000);
  cipline_tech_object::set_station_par(26, 10000);
  cipline_tech_object::set_station_par(27, 10000);
  cipline_tech_object::set_station_par(28, 100);
  cipline_tech_object::set_station_par(29, 100);
  cipline_tech_object::set_station_par(30, 50);
  cipline_tech_object::set_station_par(31, 0);
  cipline_tech_object::set_station_par(32, 0);
  cipline_tech_object::set_station_par(33, 12);
  cipline_tech_object::set_station_par(34, 11);
  cipline_tech_object::set_station_par(35, 20);
}

void InitCipDevices() {
  auto dm = device_manager::get_instance();
  dm->clear_io_devices();
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V0", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V1", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V10", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V11", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V12", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V13", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V2", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V3", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V4", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V5", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V6", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V7", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V8", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V9", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "V101", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_V,
                    device::DEVICE_SUB_TYPE::DST_V_VIRT, "V102", "", "");

  dm->add_io_device(device::DEVICE_TYPE::DT_VC,
                    device::DEVICE_SUB_TYPE::DST_VC_VIRT, "LINE1VC14", "", "");

  dm->add_io_device(device::DEVICE_TYPE::DT_M,
                    device::DEVICE_SUB_TYPE::DST_M_VIRT, "LINE1M1", "", "");

  dm->add_io_device(device::DEVICE_TYPE::DT_LS,
                    device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LINE1LS1", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_LS,
                    device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LINE1LS2", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_LS,
                    device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LINE1LS3", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_LS,
                    device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS4", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_LS,
                    device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS5", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_LS,
                    device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS6", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_LS,
                    device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS7", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_LS,
                    device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS8", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_LS,
                    device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS9", "", "");

  dm->add_io_device(device::DEVICE_TYPE::DT_TE,
                    device::DEVICE_SUB_TYPE::DST_TE_VIRT, "LINE1TE1", "", "");
  dm->add_io_device(device::DEVICE_TYPE::DT_TE,
                    device::DEVICE_SUB_TYPE::DST_TE_VIRT, "LINE1TE2", "", "");

  dm->add_io_device(device::DEVICE_TYPE::DT_FS,
                    device::DEVICE_SUB_TYPE::DST_FS_VIRT, "LINE1FS1", "", "");

  dm->add_io_device(device::DEVICE_TYPE::DT_FQT,
                    device::DEVICE_SUB_TYPE::DST_FQT_VIRT, "LINE1FQT1", "", "");

  dm->add_io_device(device::DEVICE_TYPE::DT_QT,
                    device::DEVICE_SUB_TYPE::DST_QT_VIRT, "LINE1QT1", "", "");
}

void ClearCipDevices() { device_manager::get_instance()->clear_io_devices(); }

TEST(cipline_tech_object, _CheckErr) {
  cipline_tech_object cip1("CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200);
  lua_manager::get_instance()->set_Lua(lua_open());

  cip1.initline();
  virtual_device temp_sensor_supply("LINE1TE1", device::DT_TE,
                                    device::DST_TE_VIRT);
  virtual_device temp_sensor_return("LINE1TE2", device::DT_TE,
                                    device::DST_TE_VIRT);
  virtual_device conductivity_sensor("LINE1QT1", device::DT_QT,
                                     device::DST_QT_VIRT);
  cip1.TP = reinterpret_cast<i_AI_device *>(&temp_sensor_supply);
  cip1.TR = reinterpret_cast<i_AI_device *>(&temp_sensor_return);
  cip1.Q = reinterpret_cast<i_AI_device *>(&conductivity_sensor);
  EXPECT_EQ(0, cip1._CheckErr());
  temp_sensor_supply.set_state(-1);
  EXPECT_EQ(ERR_SUPPLY_TEMP_SENSOR, cip1._CheckErr());
  temp_sensor_supply.set_state(0);
  temp_sensor_return.set_state(-1);
  EXPECT_EQ(ERR_RETURN_TEMP_SENSOR, cip1._CheckErr());
  temp_sensor_return.set_state(0);
  conductivity_sensor.set_state(-1);
  EXPECT_EQ(0, cip1._CheckErr());
  for (int step = 0; step < 100; step++) {
    cip1.curstep = step;
    if (cipline_tech_object::steps_caustic.count(step) ||
        cipline_tech_object::steps_acid.count(step)) {
      EXPECT_EQ(ERR_CONCENTRATION_SENSOR, cip1._CheckErr());
    } else {
      EXPECT_EQ(0, cip1._CheckErr());
    }
  }
  temp_sensor_supply.set_state(0);
  temp_sensor_return.set_state(0);
  conductivity_sensor.set_state(0);
  cip1.curstep = 0;
  temp_sensor_supply.set_value(-1);
  EXPECT_EQ(ERR_SUPPLY_TEMP_SENSOR, cip1._CheckErr());
  temp_sensor_supply.set_value(0);
  temp_sensor_return.set_value(-1);
  EXPECT_EQ(ERR_RETURN_TEMP_SENSOR, cip1._CheckErr());

  temp_sensor_supply.set_value(0);
  temp_sensor_return.set_value(0);
  temp_sensor_return.set_state(-1);
  temp_sensor_supply.set_state(-1);
  conductivity_sensor.set_state(-1);
  cip1.curstep = *(cipline_tech_object::steps_caustic.begin());
  cipline_tech_object::parpar[0][P_BLOCK_ERRORS] =
      1 << BE_ERR_SUPPLY_TEMP_SENSOR | 1 << BE_ERR_RETURN_TEMP_SENSOR;
  EXPECT_EQ(ERR_CONCENTRATION_SENSOR, cip1._CheckErr());
  cipline_tech_object::parpar[0][P_BLOCK_ERRORS] =
      1 << BE_ERR_SUPPLY_TEMP_SENSOR | 1 << BE_ERR_CONCENTRATION_SENSOR;
  EXPECT_EQ(ERR_RETURN_TEMP_SENSOR, cip1._CheckErr());
  cipline_tech_object::parpar[0][P_BLOCK_ERRORS] =
      1 << BE_ERR_RETURN_TEMP_SENSOR | 1 << BE_ERR_CONCENTRATION_SENSOR;
  EXPECT_EQ(ERR_SUPPLY_TEMP_SENSOR, cip1._CheckErr());
  cipline_tech_object::parpar[0][P_BLOCK_ERRORS] =
      1 << BE_ERR_RETURN_TEMP_SENSOR | 1 << BE_ERR_CONCENTRATION_SENSOR |
      1 << BE_ERR_SUPPLY_TEMP_SENSOR;
  EXPECT_EQ(0, cip1._CheckErr());

  G_LUA_MANAGER->free_Lua();
}

TEST(cipline_tech_object, evaluate) {
  lua_manager::get_instance()->set_Lua(lua_open());
  auto *cip1 =
      new cipline_tech_object("CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200);

  cip1->initline();
  EXPECT_EQ(0, cip1->evaluate());

  cip1->rt_par_float[P_SELECT_REC] = 1;
  EXPECT_EQ(0, cip1->evaluate());

  cipline_tech_object::statsbase->apply();
  cipline_tech_object::statsbase->evaluate();

  delete cip1;

  cip1 =
      new cipline_tech_object("CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200);
  cip1->initline();
  EXPECT_EQ(0, cip1->evaluate());

  EXPECT_EQ(nullptr, cip1->PRESSURE);
  auto current_recipe = cip1->lineRecipes->getCurrentRecipe();
  cip1->lineRecipes->setRecipeValue(current_recipe,
                                    TRecipeManager::RV_TO_DEFAULTS, 1);
  cip1->lineRecipes->EvalRecipe();
  EXPECT_EQ(0, cip1->lineRecipes->getRecipeValue(
                   current_recipe, TRecipeManager::RV_TO_DEFAULTS));
  EXPECT_EQ(200, cip1->lineRecipes->getRecipeValue(current_recipe,
                                                   TRecipeManager::RV_V1));
  EXPECT_EQ(0, cip1->evaluate());
  cip1->rt_par_float[P_SELECT_REC] = 1;
  EXPECT_EQ(0, cip1->evaluate());
  cip1->rt_par_float[P_SELECT_PRG] = SPROG_CAUSTIC_ACID_SANITIZER;
  cip1->SetCommand(MCMD_EVALUATE);
  EXPECT_EQ(0, cip1->state);
  EXPECT_EQ(0, cip1->evaluate());
  EXPECT_NE(0, cip1->state);

  delete cip1;
  G_LUA_MANAGER->free_Lua();
}

TEST(cipline_tech_object, _LoadProgram) {
  lua_manager::get_instance()->set_Lua(lua_open());
  auto *cip1 =
      new cipline_tech_object("CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200);

  cip1->initline();

  cip1->rt_par_float[P_PROGRAM] = SPROG_RINSING_CLEAN;
  EXPECT_EQ(83, cip1->_LoadProgram());
  EXPECT_EQ(false, cip1->program_has_cold_desinfection);
  EXPECT_EQ(false, cip1->program_has_acid);
  EXPECT_EQ(false, cip1->program_has_caustic);
  EXPECT_EQ(false, cip1->program_has_desinfection);
  cip1->_ResetLinesDevicesBeforeReset();

  cip1->rt_par_float[P_PROGRAM] = SPROG_SANITIZER;
  EXPECT_EQ(71, cip1->_LoadProgram());
  EXPECT_EQ(true, cip1->program_has_cold_desinfection);
  EXPECT_EQ(false, cip1->program_has_acid);
  EXPECT_EQ(false, cip1->program_has_caustic);
  EXPECT_EQ(false, cip1->program_has_desinfection);
  cip1->_ResetLinesDevicesBeforeReset();

  cip1->rt_par_float[P_PROGRAM] = SPROG_HOTWATER;
  EXPECT_EQ(61, cip1->_LoadProgram());
  EXPECT_EQ(false, cip1->program_has_cold_desinfection);
  EXPECT_EQ(false, cip1->program_has_acid);
  EXPECT_EQ(false, cip1->program_has_caustic);
  EXPECT_EQ(true, cip1->program_has_desinfection);
  cip1->_ResetLinesDevicesBeforeReset();

  cip1->rt_par_float[P_PROGRAM] = SPROG_RINSING;
  EXPECT_EQ(5, cip1->_LoadProgram());
  EXPECT_EQ(false, cip1->program_has_cold_desinfection);
  EXPECT_EQ(false, cip1->program_has_acid);
  EXPECT_EQ(false, cip1->program_has_caustic);
  EXPECT_EQ(false, cip1->program_has_desinfection);
  cip1->_ResetLinesDevicesBeforeReset();

  cip1->rt_par_float[P_PROGRAM] = SPROG_RINSING_HOTWATER;
  EXPECT_EQ(5, cip1->_LoadProgram());
  EXPECT_EQ(false, cip1->program_has_cold_desinfection);
  EXPECT_EQ(false, cip1->program_has_acid);
  EXPECT_EQ(false, cip1->program_has_caustic);
  EXPECT_EQ(true, cip1->program_has_desinfection);
  cip1->_ResetLinesDevicesBeforeReset();

  cip1->rt_par_float[P_PROGRAM] = SPROG_ACID;
  EXPECT_EQ(5, cip1->_LoadProgram());
  EXPECT_EQ(false, cip1->program_has_cold_desinfection);
  EXPECT_EQ(true, cip1->program_has_acid);
  EXPECT_EQ(false, cip1->program_has_caustic);
  EXPECT_EQ(false, cip1->program_has_desinfection);
  cip1->_ResetLinesDevicesBeforeReset();

  cip1->rt_par_float[P_PROGRAM] = SPROG_ACID_SANITIZER;
  EXPECT_EQ(5, cip1->_LoadProgram());
  EXPECT_EQ(true, cip1->program_has_cold_desinfection);
  EXPECT_EQ(true, cip1->program_has_acid);
  EXPECT_EQ(false, cip1->program_has_caustic);
  EXPECT_EQ(false, cip1->program_has_desinfection);
  cip1->_ResetLinesDevicesBeforeReset();

  cip1->rt_par_float[P_PROGRAM] = SPROG_ACID_HOTWATER;
  EXPECT_EQ(5, cip1->_LoadProgram());
  EXPECT_EQ(false, cip1->program_has_cold_desinfection);
  EXPECT_EQ(true, cip1->program_has_acid);
  EXPECT_EQ(false, cip1->program_has_caustic);
  EXPECT_EQ(true, cip1->program_has_desinfection);
  cip1->_ResetLinesDevicesBeforeReset();

  cip1->rt_par_float[P_PROGRAM] = SPROG_CAUSTIC;
  EXPECT_EQ(5, cip1->_LoadProgram());
  EXPECT_EQ(false, cip1->program_has_cold_desinfection);
  EXPECT_EQ(false, cip1->program_has_acid);
  EXPECT_EQ(true, cip1->program_has_caustic);
  EXPECT_EQ(false, cip1->program_has_desinfection);
  cip1->_ResetLinesDevicesBeforeReset();

  cip1->rt_par_float[P_PROGRAM] = SPROG_CAUSTIC_SANITIZER;
  EXPECT_EQ(5, cip1->_LoadProgram());
  EXPECT_EQ(true, cip1->program_has_cold_desinfection);
  EXPECT_EQ(false, cip1->program_has_acid);
  EXPECT_EQ(true, cip1->program_has_caustic);
  EXPECT_EQ(false, cip1->program_has_desinfection);
  cip1->_ResetLinesDevicesBeforeReset();

  cip1->rt_par_float[P_PROGRAM] = SPROG_CAUSTIC_HOTWATER;
  EXPECT_EQ(5, cip1->_LoadProgram());
  EXPECT_EQ(false, cip1->program_has_cold_desinfection);
  EXPECT_EQ(false, cip1->program_has_acid);
  EXPECT_EQ(true, cip1->program_has_caustic);
  EXPECT_EQ(true, cip1->program_has_desinfection);
  cip1->_ResetLinesDevicesBeforeReset();

  cip1->rt_par_float[P_PROGRAM] = SPROG_CAUSTIC_ACID;
  EXPECT_EQ(5, cip1->_LoadProgram());
  EXPECT_EQ(false, cip1->program_has_cold_desinfection);
  EXPECT_EQ(true, cip1->program_has_acid);
  EXPECT_EQ(true, cip1->program_has_caustic);
  EXPECT_EQ(false, cip1->program_has_desinfection);
  cip1->_ResetLinesDevicesBeforeReset();

  cip1->rt_par_float[P_PROGRAM] = SPROG_CAUSTIC_ACID_SANITIZER;
  EXPECT_EQ(5, cip1->_LoadProgram());
  EXPECT_EQ(true, cip1->program_has_cold_desinfection);
  EXPECT_EQ(true, cip1->program_has_acid);
  EXPECT_EQ(true, cip1->program_has_caustic);
  EXPECT_EQ(false, cip1->program_has_desinfection);
  cip1->_ResetLinesDevicesBeforeReset();

  cip1->rt_par_float[P_PROGRAM] = SPROG_CAUSTIC_ACID_HOTWATER;
  EXPECT_EQ(5, cip1->_LoadProgram());
  EXPECT_EQ(false, cip1->program_has_cold_desinfection);
  EXPECT_EQ(true, cip1->program_has_acid);
  EXPECT_EQ(true, cip1->program_has_caustic);
  EXPECT_EQ(true, cip1->program_has_desinfection);
  cip1->_ResetLinesDevicesBeforeReset();

  delete cip1;
  G_LUA_MANAGER->free_Lua();
}

TEST(cipline_tech_object, waterTankIsEmpty) {
  cipline_tech_object cip1("CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200);
  lua_manager::get_instance()->set_Lua(lua_open());

  cip1.initline();
  virtual_device waterTankLowLevel("LWL", device::DT_LS, device::DST_LS_VIRT);
  virtual_device waterTankCurrentLevel("LTW", device::DT_LT,
                                       device::DST_LT_VIRT);
  cip1.LWL = static_cast<i_DI_device *>(&waterTankLowLevel);
  cip1.LTW = static_cast<i_AI_device *>(&waterTankCurrentLevel);

  cip1.dont_use_water_tank = 0;

  waterTankLowLevel.direct_set_state(0);
  waterTankCurrentLevel.set_value(0);
  cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 0;
  cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 0;
  EXPECT_EQ(true, cip1.waterTankIsEmpty());

  waterTankLowLevel.direct_set_state(1);
  waterTankCurrentLevel.set_value(0);
  cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 0;
  cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 0;
  EXPECT_EQ(false, cip1.waterTankIsEmpty());
  cip1.dont_use_water_tank = 1;
  EXPECT_EQ(true, cip1.waterTankIsEmpty());
  cip1.dont_use_water_tank = 0;

  waterTankLowLevel.direct_set_state(0);

  waterTankLowLevel.direct_set_state(1);
  waterTankCurrentLevel.set_value(9);
  cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 10;
  cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 2;
  EXPECT_EQ(true, cip1.waterTankIsEmpty());

  waterTankLowLevel.direct_set_state(1);
  waterTankCurrentLevel.set_value(11);
  cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 10;
  cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 2;
  EXPECT_EQ(true, cip1.waterTankIsEmpty());

  waterTankLowLevel.direct_set_state(1);
  waterTankCurrentLevel.set_value(12);
  cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 10;
  cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 2;
  EXPECT_EQ(false, cip1.waterTankIsEmpty());

  waterTankLowLevel.direct_set_state(1);
  waterTankCurrentLevel.set_value(11);
  cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 10;
  cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 2;
  EXPECT_EQ(false, cip1.waterTankIsEmpty());

  waterTankLowLevel.direct_set_state(1);
  waterTankCurrentLevel.set_value(10);
  cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 10;
  cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 2;
  EXPECT_EQ(false, cip1.waterTankIsEmpty());

  waterTankLowLevel.direct_set_state(1);
  waterTankCurrentLevel.set_value(9.99f);
  cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 10;
  cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 2;
  EXPECT_EQ(true, cip1.waterTankIsEmpty());

  waterTankLowLevel.direct_set_state(1);
  waterTankCurrentLevel.set_value(10);
  cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 10;
  cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 2;
  EXPECT_EQ(true, cip1.waterTankIsEmpty());

  waterTankLowLevel.direct_set_state(1);
  waterTankCurrentLevel.set_value(11.99f);
  cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 10;
  cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 2;
  EXPECT_EQ(true, cip1.waterTankIsEmpty());

  waterTankLowLevel.direct_set_state(1);
  waterTankCurrentLevel.set_value(12);
  cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 10;
  cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 2;
  EXPECT_EQ(false, cip1.waterTankIsEmpty());

  G_LUA_MANAGER->free_Lua();
}

TEST(cipline_tech_object, OpolRR) {
  InitCipDevices();
  cipline_tech_object cip1("CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200);
  lua_manager::get_instance()->set_Lua(lua_open());

  cip1.initline();
  InitStationParams();

  cip1.T[TMR_CHK_CONC]->set_countdown_time(0);
  float currentConcentration;
  float cVal;
  float testVal;
  testVal = 0.12f;
  auto isCheckPoint = false;
  cipline_tech_object::set_station_par(P_CKANAL_K, testVal);
  for (currentConcentration = 20; currentConcentration >= 0;) {
    reinterpret_cast<device *>(cip1.Q)->set_value(currentConcentration);
    cVal = cip1.GetConc(KISL);
    if (cVal <= testVal) {
      if (isCheckPoint) {
        EXPECT_EQ(1, cip1.OpolRR(TANK_K));
      } else {
        EXPECT_EQ(0, cip1.OpolRR(TANK_K));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        EXPECT_EQ(1, cip1.OpolRR(TANK_K));
        isCheckPoint = true;
      }
    } else {
      EXPECT_EQ(0, cip1.OpolRR(TANK_K));
    }
    currentConcentration -= 0.25;
  }

  testVal = 0.15f;
  isCheckPoint = false;
  cipline_tech_object::set_station_par(P_CKANAL_S, testVal);
  for (currentConcentration = 20; currentConcentration >= 0;) {
    reinterpret_cast<device *>(cip1.Q)->set_value(currentConcentration);
    cVal = cip1.GetConc(SHCH);
    if (cVal <= testVal) {
      if (isCheckPoint) {
        EXPECT_EQ(1, cip1.OpolRR(TANK_S));
      } else {
        EXPECT_EQ(0, cip1.OpolRR(TANK_S));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        EXPECT_EQ(1, cip1.OpolRR(TANK_S));
        isCheckPoint = true;
      }
    } else {
      EXPECT_EQ(0, cip1.OpolRR(TANK_K));
    }
    currentConcentration -= 0.25;
  }

  ClearCipDevices();
  G_LUA_MANAGER->free_Lua();
}

TEST(cipline_tech_object, _FromObject) {
  InitCipDevices();
  cipline_tech_object cip1("CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200);
  lua_manager::get_instance()->set_Lua(lua_open());

  cip1.initline();
  InitStationParams();

  cip1.T[TMR_CHK_CONC]->set_countdown_time(0);
  cip1.FL->set_cmd("ST", 0, 1);
  float currentConcentration;
  float cVal;
  float testVal;
  testVal = 0.12f;
  cipline_tech_object::set_station_par(P_CKANAL_K, testVal);
  for (currentConcentration = 20; currentConcentration >= 0;) {
    reinterpret_cast<device *>(cip1.Q)->set_value(currentConcentration);
    cVal = cip1.GetConc(KISL);
    cip1.concentration_ok = 0;
    cip1._FromObject(TANK_W, TANK_K);
    if (cVal <= testVal) {
      EXPECT_EQ(1, cip1.concentration_ok);
    } else {
      EXPECT_EQ(0, cip1.concentration_ok);
    }
    cip1.concentration_ok = 0;
    cip1._FromObject(TANK_K, KANAL);
    if (cVal >= testVal) {
      EXPECT_EQ(1, cip1.concentration_ok);
    } else {
      EXPECT_EQ(0, cip1.concentration_ok);
    }
    currentConcentration -= 0.25;
  }

  testVal = 0.15f;
  cipline_tech_object::set_station_par(P_CKANAL_S, testVal);
  for (currentConcentration = 20; currentConcentration >= 0;) {
    reinterpret_cast<device *>(cip1.Q)->set_value(currentConcentration);
    cVal = cip1.GetConc(SHCH);
    cip1.concentration_ok = 0;
    cip1._FromObject(TANK_W, TANK_S);
    if (cVal <= testVal) {
      EXPECT_EQ(1, cip1.concentration_ok);
    } else {
      EXPECT_EQ(0, cip1.concentration_ok);
    }
    cip1.concentration_ok = 0;
    cip1._FromObject(TANK_S, KANAL);
    if (cVal >= testVal) {
      EXPECT_EQ(1, cip1.concentration_ok);
    } else {
      EXPECT_EQ(0, cip1.concentration_ok);
    }
    currentConcentration -= 0.25;
  }

  ClearCipDevices();
  G_LUA_MANAGER->free_Lua();
}

TEST(cipline_tech_object, _DoStep) {
  InitCipDevices();
  cipline_tech_object cip1("CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200);
  lua_manager::get_instance()->set_Lua(lua_open());

  cip1.initline();
  InitStationParams();

  virtual_device circ_signal("LINE1DO101", device::DT_DO, device::DST_DO_VIRT);
  virtual_device can_continue_operation_signal("LINE1DI101", device::DT_DI,
                                               device::DST_DI_VIRT);
  cip1.dev_upr_circulation = &circ_signal;
  cip1.dev_os_can_continue = &can_continue_operation_signal;

  // test circulation signal
  EXPECT_EQ(0, cip1.dev_upr_circulation->get_state());
  cip1.curstep = 8;
  cip1._DoStep(8);
  EXPECT_EQ(1, cip1.dev_upr_circulation->get_state());
  cip1.curstep = 7;
  cip1._DoStep(7);
  EXPECT_EQ(0, cip1.dev_upr_circulation->get_state());
  cip1.use_circulation_on_v2_supply = true;
  cip1.curstep = 7;
  cip1._DoStep(7);
  EXPECT_EQ(1, cip1.dev_upr_circulation->get_state());
  can_continue_operation_signal.set_state(1);
  EXPECT_EQ(false, cip1.wasflip);
  cip1.curstep = 8;
  cip1._DoStep(8);
  EXPECT_EQ(true, cip1.wasflip);
  EXPECT_EQ(1, cip1.dev_upr_circulation->get_state());
  cip1.curstep = 8;
  cip1._DoStep(8);
  EXPECT_EQ(0, cip1.dev_upr_circulation->get_state());

  ClearCipDevices();
  G_LUA_MANAGER->free_Lua();
}
