#include "cip_tech_def_tests.h"
#include "lua_manager.h"

using namespace ::testing;

void InitStationParams()
    {
    printf("===================================Init station params");
    cipline_tech_object::set_station_par(1,1.5f);
    cipline_tech_object::set_station_par(2,0.3f);
    cipline_tech_object::set_station_par(3,0.1f);
    cipline_tech_object::set_station_par(4,1.0f);
    cipline_tech_object::set_station_par(5,0.2f);
    cipline_tech_object::set_station_par(6,0.1f);
    cipline_tech_object::set_station_par(7,20);
    cipline_tech_object::set_station_par(8,5);
    cipline_tech_object::set_station_par(9,0);
    cipline_tech_object::set_station_par(10,8);
    cipline_tech_object::set_station_par(11,1.51f);
    cipline_tech_object::set_station_par(12,1.8f);
    cipline_tech_object::set_station_par(13,47);
    cipline_tech_object::set_station_par(14,57);
    cipline_tech_object::set_station_par(15,1.47f);
    cipline_tech_object::set_station_par(16,1.45f);
    cipline_tech_object::set_station_par(17,1);
    cipline_tech_object::set_station_par(18,52);
    cipline_tech_object::set_station_par(19,0.5f);
    cipline_tech_object::set_station_par(20,30.8f);
    cipline_tech_object::set_station_par(21,0.4f);
    cipline_tech_object::set_station_par(22,30);
    cipline_tech_object::set_station_par(23,120);
    cipline_tech_object::set_station_par(24,50);
    cipline_tech_object::set_station_par(25,10000);
    cipline_tech_object::set_station_par(26,10000);
    cipline_tech_object::set_station_par(27,10000);
    cipline_tech_object::set_station_par(28,100);
    cipline_tech_object::set_station_par(29,100);
    cipline_tech_object::set_station_par(30,50);
    cipline_tech_object::set_station_par(31,0);
    cipline_tech_object::set_station_par(32,0);
    cipline_tech_object::set_station_par(33,12);
    cipline_tech_object::set_station_par(34,11);
    cipline_tech_object::set_station_par(35,20);
    }

void InitCipDevices( )
    {
    auto dm = device_manager::get_instance( );
    dm->clear_io_devices();
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V0", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V1", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V10", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V11", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V12", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V13", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V2", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V3", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V4", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V5", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V6", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V7", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V8", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "LINE1V9", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "V101", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_V, device::DEVICE_SUB_TYPE::DST_V_VIRT, "V102", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_VC, device::DEVICE_SUB_TYPE::DST_VC_VIRT, "LINE1VC14", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_M, device::DEVICE_SUB_TYPE::DST_M_VIRT, "LINE1M1", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LINE1LS1", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LINE1LS2", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LINE1LS3", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS4", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS5", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS6", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS7", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS8", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_LS, device::DEVICE_SUB_TYPE::DST_LS_VIRT, "LS9", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_TE, device::DEVICE_SUB_TYPE::DST_TE_VIRT, "LINE1TE1", "", "" );
    dm->add_io_device( device::DEVICE_TYPE::DT_TE, device::DEVICE_SUB_TYPE::DST_TE_VIRT, "LINE1TE2", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_FS, device::DEVICE_SUB_TYPE::DST_FS_VIRT, "LINE1FS1", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_FQT, device::DEVICE_SUB_TYPE::DST_FQT_VIRT, "LINE1FQT1", "", "" );

    dm->add_io_device( device::DEVICE_TYPE::DT_QT, device::DEVICE_SUB_TYPE::DST_QT_VIRT, "LINE1QT1", "", "" );
    
    //Watchdog device
    dm->add_io_device( device::DEVICE_TYPE::DT_WATCHDOG, device::DEVICE_SUB_TYPE::DST_WATCHDOG, "LINE1WATCHDOG1", "", "" );
    }

void ClearCipDevices( )
    {
    device_manager::get_instance( )->clear_io_devices();
    }


TEST( cipline_tech_object, lua_init_params )
    {
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( lua_open() );

    const auto PAR_VALUE = 1.5f;
    cipline_tech_object::set_station_par( P_CZAD_S, PAR_VALUE );
    EXPECT_EQ( cip1.get_station_par( P_CZAD_S ), PAR_VALUE );
    cip1.lua_init_params();
    EXPECT_EQ( cip1.get_station_par( P_CZAD_S ), 0.0f );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, lua_init_runtime_params )
    {
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( lua_open() );

    // После создания объекта параметры должны иметь 0-е значения.
    EXPECT_EQ( cip1.rt_par_float[ workParameters::P_R_NO_FLOW ], 0.0f );
    cip1.lua_init_runtime_params();
    const auto PAR_VALUE = 2.f;
    // После инициализации рабочих параметров должны быть значения по умолчанию.
    EXPECT_EQ( cip1.rt_par_float[ workParameters::P_R_NO_FLOW ], PAR_VALUE );

    G_LUA_MANAGER->free_Lua();
    }


TEST( cipline_tech_object, _CheckErr )
    {
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance( )->set_Lua( lua_open( ));

    cip1.initline( );
    virtual_device temp_sensor_supply( "LINE1TE1", device::DT_TE, device::DST_TE_VIRT );
    virtual_device temp_sensor_return( "LINE1TE2", device::DT_TE, device::DST_TE_VIRT );
    virtual_device conductivity_sensor( "LINE1QT1", device::DT_QT, device::DST_QT_VIRT );
    cip1.TP = reinterpret_cast<i_AI_device *>(&temp_sensor_supply);
    cip1.TR = reinterpret_cast<i_AI_device *>(&temp_sensor_return);
    cip1.Q = reinterpret_cast<i_AI_device *>(&conductivity_sensor);
    EXPECT_EQ( 0, cip1._CheckErr( ));
    temp_sensor_supply.set_state( -1 );
    EXPECT_EQ( ERR_SUPPLY_TEMP_SENSOR, cip1._CheckErr( ));
    temp_sensor_supply.set_state( 0 );
    temp_sensor_return.set_state( -1 );
    EXPECT_EQ( ERR_RETURN_TEMP_SENSOR, cip1._CheckErr( ));
    temp_sensor_return.set_state( 0 );
    conductivity_sensor.set_state( -1 );
    EXPECT_EQ( 0, cip1._CheckErr( ));
    for ( int step = 0; step < 100; step++ )
        {
        cip1.curstep = step;
        if ( cipline_tech_object::steps_caustic.count( step ) || cipline_tech_object::steps_acid.count( step ))
            {
            EXPECT_EQ( ERR_CONCENTRATION_SENSOR, cip1._CheckErr( ));
            }
        else
            {
            EXPECT_EQ( 0, cip1._CheckErr( ));
            }
        }
    temp_sensor_supply.set_state( 0 );
    temp_sensor_return.set_state( 0 );
    conductivity_sensor.set_state( 0 );
    cip1.curstep = 0;
    temp_sensor_supply.set_value( -1 );
    EXPECT_EQ( ERR_SUPPLY_TEMP_SENSOR, cip1._CheckErr( ));
    temp_sensor_supply.set_value( 0 );
    temp_sensor_return.set_value( -1 );
    EXPECT_EQ( ERR_RETURN_TEMP_SENSOR, cip1._CheckErr( ));

    temp_sensor_supply.set_value( 0 );
    temp_sensor_return.set_value( 0 );
    temp_sensor_return.set_state( -1 );
    temp_sensor_supply.set_state( -1 );
    conductivity_sensor.set_state( -1 );
    cip1.curstep = *(cipline_tech_object::steps_caustic.begin( ));
    cipline_tech_object::parpar[ 0 ][ P_BLOCK_ERRORS ] =
        1 << BE_ERR_SUPPLY_TEMP_SENSOR | 1 << BE_ERR_RETURN_TEMP_SENSOR;
    EXPECT_EQ( ERR_CONCENTRATION_SENSOR, cip1._CheckErr( ));
    cipline_tech_object::parpar[ 0 ][ P_BLOCK_ERRORS ] =
        1 << BE_ERR_SUPPLY_TEMP_SENSOR | 1 << BE_ERR_CONCENTRATION_SENSOR;
    EXPECT_EQ( ERR_RETURN_TEMP_SENSOR, cip1._CheckErr( ));
    cipline_tech_object::parpar[ 0 ][ P_BLOCK_ERRORS ] =
        1 << BE_ERR_RETURN_TEMP_SENSOR | 1 << BE_ERR_CONCENTRATION_SENSOR;
    EXPECT_EQ( ERR_SUPPLY_TEMP_SENSOR, cip1._CheckErr( ));
    cipline_tech_object::parpar[ 0 ][ P_BLOCK_ERRORS ] =
        1 << BE_ERR_RETURN_TEMP_SENSOR | 1 << BE_ERR_CONCENTRATION_SENSOR | 1 << BE_ERR_SUPPLY_TEMP_SENSOR;
    EXPECT_EQ( 0, cip1._CheckErr( ));

    G_LUA_MANAGER->free_Lua( );
    }

TEST( cipline_tech_object, evaluate )
    {

    lua_manager::get_instance( )->set_Lua( lua_open( ));
    auto *cip1 = new cipline_tech_object(
        "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );

    cip1->initline( );
    EXPECT_EQ( 0, cip1->evaluate( ));

    cip1->rt_par_float[ P_SELECT_REC ] = 1;
    EXPECT_EQ( 0, cip1->evaluate( ));

    cipline_tech_object::statsbase->apply( );
    cipline_tech_object::statsbase->evaluate( );

    delete cip1;

    cip1 = new cipline_tech_object(
        "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    cip1->initline( );
    EXPECT_EQ( 0, cip1->evaluate( ));

    EXPECT_EQ( nullptr, cip1->PRESSURE );
    auto current_recipe = cip1->lineRecipes->getCurrentRecipe( );
    cip1->lineRecipes->setRecipeValue( current_recipe, TRecipeManager::RV_TO_DEFAULTS, 1 );
    cip1->lineRecipes->EvalRecipe( );
    EXPECT_EQ( 0, cip1->lineRecipes->getRecipeValue( current_recipe, TRecipeManager::RV_TO_DEFAULTS ));
    EXPECT_EQ( 200, cip1->lineRecipes->getRecipeValue( current_recipe, TRecipeManager::RV_V1 ));
    EXPECT_EQ( 0, cip1->evaluate( ));
    cip1->rt_par_float[ P_SELECT_REC ] = 1;
    EXPECT_EQ( 0, cip1->evaluate( ));
    cip1->rt_par_float[ P_SELECT_PRG ] = SPROG_CAUSTIC_ACID_SANITIZER;
    cip1->SetCommand( MCMD_EVALUATE );
    EXPECT_EQ( 0, cip1->state );
    EXPECT_EQ( 0, cip1->evaluate( ));
    EXPECT_NE( 0, cip1->state );

    delete cip1;
    G_LUA_MANAGER->free_Lua( );
    }

TEST( cipline_tech_object, _LoadProgram )
    {
    lua_manager::get_instance( )->set_Lua( lua_open( ));
    auto *cip1 = new cipline_tech_object(
        "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );

    cip1->initline( );

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_RINSING_CLEAN;
    EXPECT_EQ( 83, cip1->_LoadProgram( ));
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset( );

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_SANITIZER;
    EXPECT_EQ( 71, cip1->_LoadProgram( ));
    EXPECT_EQ( true, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset( );

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_HOTWATER;
    EXPECT_EQ( 61, cip1->_LoadProgram( ));
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( true, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset( );

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_RINSING;
    EXPECT_EQ( 5, cip1->_LoadProgram( ));
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset( );

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_RINSING_HOTWATER;
    EXPECT_EQ( 5, cip1->_LoadProgram( ));
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( true, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset( );

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_ACID;
    EXPECT_EQ( 5, cip1->_LoadProgram( ));
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( true, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset( );

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_ACID_SANITIZER;
    EXPECT_EQ( 5, cip1->_LoadProgram( ));
    EXPECT_EQ( true, cip1->program_has_cold_desinfection );
    EXPECT_EQ( true, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset( );

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_ACID_HOTWATER;
    EXPECT_EQ( 5, cip1->_LoadProgram( ));
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( true, cip1->program_has_acid );
    EXPECT_EQ( false, cip1->program_has_caustic );
    EXPECT_EQ( true, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset( );

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_CAUSTIC;
    EXPECT_EQ( 5, cip1->_LoadProgram( ));
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( true, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset( );

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_CAUSTIC_SANITIZER;
    EXPECT_EQ( 5, cip1->_LoadProgram( ));
    EXPECT_EQ( true, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( true, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset( );

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_CAUSTIC_HOTWATER;
    EXPECT_EQ( 5, cip1->_LoadProgram( ));
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( false, cip1->program_has_acid );
    EXPECT_EQ( true, cip1->program_has_caustic );
    EXPECT_EQ( true, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset( );

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_CAUSTIC_ACID;
    EXPECT_EQ( 5, cip1->_LoadProgram( ));
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( true, cip1->program_has_acid );
    EXPECT_EQ( true, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset( );

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_CAUSTIC_ACID_SANITIZER;
    EXPECT_EQ( 5, cip1->_LoadProgram( ));
    EXPECT_EQ( true, cip1->program_has_cold_desinfection );
    EXPECT_EQ( true, cip1->program_has_acid );
    EXPECT_EQ( true, cip1->program_has_caustic );
    EXPECT_EQ( false, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset( );

    cip1->rt_par_float[ P_PROGRAM ] = SPROG_CAUSTIC_ACID_HOTWATER;
    EXPECT_EQ( 5, cip1->_LoadProgram( ));
    EXPECT_EQ( false, cip1->program_has_cold_desinfection );
    EXPECT_EQ( true, cip1->program_has_acid );
    EXPECT_EQ( true, cip1->program_has_caustic );
    EXPECT_EQ( true, cip1->program_has_desinfection );
    cip1->_ResetLinesDevicesBeforeReset( );

    delete cip1;
    G_LUA_MANAGER->free_Lua( );
    }

TEST( cipline_tech_object, waterTankIsEmpty )
    {
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance( )->set_Lua( lua_open( ));

    cip1.initline( );
    virtual_device waterTankLowLevel( "LWL", device::DT_LS, device::DST_LS_VIRT );
    virtual_device waterTankCurrentLevel( "LTW", device::DT_LT, device::DST_LT_VIRT );
    cip1.LWL = static_cast<i_DI_device *>(&waterTankLowLevel);
    cip1.LTW = static_cast<i_AI_device *>(&waterTankCurrentLevel);

    cip1.dont_use_water_tank = 0;

    waterTankLowLevel.direct_set_state( 0 );
    waterTankCurrentLevel.set_value( 0 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 0;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 0;
    EXPECT_EQ( true, cip1.waterTankIsEmpty( ));

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 0 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 0;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 0;
    EXPECT_EQ( false, cip1.waterTankIsEmpty( ));
    cip1.dont_use_water_tank = 1;
    EXPECT_EQ( true, cip1.waterTankIsEmpty( ));
    cip1.dont_use_water_tank = 0;

    waterTankLowLevel.direct_set_state( 0 );

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 9 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( true, cip1.waterTankIsEmpty( ));

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 11 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( true, cip1.waterTankIsEmpty( ));

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 12 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( false, cip1.waterTankIsEmpty( ));

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 11 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( false, cip1.waterTankIsEmpty( ));

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 10 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( false, cip1.waterTankIsEmpty( ));

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 9.99f );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( true, cip1.waterTankIsEmpty( ));

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 10 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( true, cip1.waterTankIsEmpty( ));

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 11.99f );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( true, cip1.waterTankIsEmpty( ));

    waterTankLowLevel.direct_set_state( 1 );
    waterTankCurrentLevel.set_value( 12 );
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_FOR_WATER ] = 10;
    cipline_tech_object::parpar[ 0 ][ P_MIN_BULK_DELTA ] = 2;
    EXPECT_EQ( false, cip1.waterTankIsEmpty( ));

    G_LUA_MANAGER->free_Lua( );
    }

TEST( cipline_tech_object, OpolRR )
    {
    InitCipDevices( );
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance( )->set_Lua( lua_open( ));

    cip1.initline( );
    InitStationParams( );

    cip1.T[ TMR_CHK_CONC ]->set_countdown_time( 0 );
    float currentConcentration;
    float cVal;
    float testVal;
    testVal = 0.12f;
    auto isCheckPoint = false;
    cipline_tech_object::set_station_par( P_CKANAL_K, testVal );
    for ( currentConcentration = 20; currentConcentration >= 0; )
        {
        reinterpret_cast<device *>(cip1.Q)->set_value( currentConcentration );
        cVal = cip1.GetConc( KISL );
        if ( cVal <= testVal )
            {
            if ( isCheckPoint )
                {
                EXPECT_EQ( 1, cip1.OpolRR( TANK_K ));
                }
            else
                {
                EXPECT_EQ( 0, cip1.OpolRR( TANK_K ));
                std::this_thread::sleep_for( std::chrono::milliseconds( 1 ));
                EXPECT_EQ( 1, cip1.OpolRR( TANK_K ));
                isCheckPoint = true;
                }
            }
        else
            {
            EXPECT_EQ( 0, cip1.OpolRR( TANK_K ));
            }
        currentConcentration -= 0.25;
        }

    testVal = 0.15f;
    isCheckPoint = false;
    cipline_tech_object::set_station_par( P_CKANAL_S, testVal );
    for ( currentConcentration = 20; currentConcentration >= 0; )
        {
        reinterpret_cast<device *>(cip1.Q)->set_value( currentConcentration );
        cVal = cip1.GetConc( SHCH );
        if ( cVal <= testVal )
            {
            if ( isCheckPoint )
                {
                EXPECT_EQ( 1, cip1.OpolRR( TANK_S ));
                }
            else
                {
                EXPECT_EQ( 0, cip1.OpolRR( TANK_S ));
                std::this_thread::sleep_for( std::chrono::milliseconds( 1 ));
                EXPECT_EQ( 1, cip1.OpolRR( TANK_S ));
                isCheckPoint = true;
                }
            }
        else
            {
            EXPECT_EQ( 0, cip1.OpolRR( TANK_K ));
            }
        currentConcentration -= 0.25;
        }

    ClearCipDevices( );
    G_LUA_MANAGER->free_Lua( );
    }

TEST( cipline_tech_object, _FromObject )
    {
    InitCipDevices( );
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance( )->set_Lua( lua_open( ));

    cip1.initline( );
    InitStationParams( );

    cip1.T[ TMR_CHK_CONC ]->set_countdown_time( 0 );
    cip1.FL->set_cmd("ST", 0, 1);
    float currentConcentration;
    float cVal;
    float testVal;
    testVal = 0.12f;
    cipline_tech_object::set_station_par( P_CKANAL_K, testVal );
    for ( currentConcentration = 20; currentConcentration >= 0; )
        {
        reinterpret_cast<device *>(cip1.Q)->set_value( currentConcentration );
        cVal = cip1.GetConc( KISL );
        cip1.concentration_ok = 0;
        cip1._FromObject( TANK_W, TANK_K );
        if ( cVal <= testVal )
            {
            EXPECT_EQ( 1, cip1.concentration_ok );
            }
        else
            {
            EXPECT_EQ( 0, cip1.concentration_ok);
            }
        cip1.concentration_ok = 0;
        cip1._FromObject( TANK_K, KANAL );
        if ( cVal >= testVal )
            {
            EXPECT_EQ( 1, cip1.concentration_ok );
            }
        else
            {
            EXPECT_EQ( 0, cip1.concentration_ok);
            }
        currentConcentration -= 0.25;
        }

    testVal = 0.15f;
    cipline_tech_object::set_station_par( P_CKANAL_S, testVal );
    for ( currentConcentration = 20; currentConcentration >= 0; )
        {
        reinterpret_cast<device *>(cip1.Q)->set_value( currentConcentration );
        cVal = cip1.GetConc( SHCH );
        cip1.concentration_ok = 0;
        cip1._FromObject( TANK_W, TANK_S );
        if ( cVal <= testVal )
            {
            EXPECT_EQ( 1, cip1.concentration_ok );
            }
        else
            {
            EXPECT_EQ( 0, cip1.concentration_ok);
            }
        cip1.concentration_ok = 0;
        cip1._FromObject( TANK_S, KANAL );
        if ( cVal >= testVal )
            {
            EXPECT_EQ( 1, cip1.concentration_ok );
            }
        else
            {
            EXPECT_EQ( 0, cip1.concentration_ok);
            }
        currentConcentration -= 0.25;
        }

    ClearCipDevices( );
    G_LUA_MANAGER->free_Lua( );
    }

TEST( cipline_tech_object, _DoStep )
    {
    InitCipDevices( );
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance( )->set_Lua( lua_open( ));

    cip1.initline( );
    InitStationParams( );

    virtual_device circ_signal( "LINE1DO101", device::DT_DO, device::DST_DO_VIRT );
    virtual_device can_continue_operation_signal( "LINE1DI101", device::DT_DI, device::DST_DI_VIRT );
    cip1.dev_upr_circulation = &circ_signal;
    cip1.dev_os_can_continue = &can_continue_operation_signal;

    //test circulation signal
    EXPECT_EQ( 0, cip1.dev_upr_circulation->get_state( ));
    cip1.curstep = 8;
    cip1._DoStep( 8);
    EXPECT_EQ( 1, cip1.dev_upr_circulation->get_state( ));
    cip1.curstep = 7;
    cip1._DoStep(7);
    EXPECT_EQ( 0, cip1.dev_upr_circulation->get_state( ));
    cip1.use_circulation_on_v2_supply = true;
    cip1.curstep = 7;
    cip1._DoStep(7);
    EXPECT_EQ( 1, cip1.dev_upr_circulation->get_state( ));
    can_continue_operation_signal.set_state( 1 );
    EXPECT_EQ( false, cip1.wasflip);
    cip1.curstep = 8;
    cip1._DoStep(8);
    EXPECT_EQ( true, cip1.wasflip);
    EXPECT_EQ( 1, cip1.dev_upr_circulation->get_state( ));
    cip1.curstep = 8;
    cip1._DoStep(8);
    EXPECT_EQ( 0, cip1.dev_upr_circulation->get_state( ));

    ClearCipDevices( );
    G_LUA_MANAGER->free_Lua( );
    }
    
TEST( cipline_tech_object, save_device ) 
    {
    // Удаляем сохраненные файлы рецептов, чтобы получить значения по умолчанию.
    const auto LINE_DEF_F_NAME_0 = "line0rec.bin";
    remove( LINE_DEF_F_NAME_0 );
    const auto MEDIUM_DEF_F_NAME_0 = "medium0rec.bin";
    remove( MEDIUM_DEF_F_NAME_0 );
    const auto LINE_DEF_F_NAME_1 = "line1rec.bin";
    remove( LINE_DEF_F_NAME_1 );
    const auto MEDIUM_DEF_F_NAME_1 = "medium1rec.bin";
    remove( MEDIUM_DEF_F_NAME_1 );

    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );

    const int BUFF_SIZE = 5000;
    char buff[ BUFF_SIZE ] = { 0 };

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( lua_open() );

    cip1.initline();
    InitStationParams();

    cip1.lineRecipes->ResetRecipeToDefaults(0);
    cip1.save_device( buff );

    std::string REF_STR0 = R"(t.CIP1 = t.CIP1 or {}
t.CIP1=
	{
	CMD=0,
	STATE=0,
	OPER=0,
	LOADED_REC='Не выбран',
	CUR_REC='',
	LASTRECNMR=0,
	LASTRECNAME='Не выбран',
	CUR_PRG='Не выбран',
	PRG_LIST='256##Наведение кислоты||512##Наведение щелочи||',
	REC_LIST='',
	NCAR='',
	NCAR1='',
	NCAR2='',
	NCAR3='',
	NCAR4='',
	SWITCH1=0,
	SWITCH2=0,
	SWITCH3=0,
	SWITCH4=0,
	IDLE_TIME = '00:00:00',
	S_PAR_F=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, )"
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, " R"(
		},
	S_PAR_UI=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, )"
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, " R"(
		},
	RT_PAR_F=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, )"
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 20, 20, "
        "20, 95, 2, 30, 0.20, 500, 130, 0, 30, 0, 30, 0, 15, 0.50, 10, 0.10, 1000, 40, 0, 2, 0, "
        "15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, " R"(
		},
	RT_PAR_UI=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, )"
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, " R"(
		},
	PAR_MAIN=
		{
		1.50, 0.30, 0.10, 1, 0.20, 0.10, 20, 5, 0, 8, 1.51, 1.80, 47, 57, 1.47, 1.45, 1, 52, )"
        "0.50, 30.80, 0.40, 30, 120, 50, 10000, 10000, 10000, 100, 100, 50, 0, 0, 12, 11, 20, "
        "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, " R"(
		},
	REC_PAR = 
	{
		0.00, 0.00, 200.00, 200.00, 1.00, 15.00, 0.00, 10.00, 30.00, 30.00, 30.00, 5.00, )"
        "80.00, 70.00, 90.00, 60.00, 200.00, 200.00, 200.00, 200.00, 40.00, 15.00, 30.00, "
        "300.00, 600.00, 600.00, 500.00, 500.00, 600.00, 600.00, 120.00, 0.00, 0.00, 0.00, "
        "0.00, 0.00, 5.00, 600.00, 30.00, 500.00, 30.00, 8.00, 15.00, 2.00, 20.00, 20.00, "
        "20.00, 95.00, 2.00, 30.00, 0.20, 500.00, 130.00, 0.00, 30.00, 0.00, 30.00, 0.00, "
        "15.00, 0.50, 10.00, 0.10, 1000.00, 40.00, 0.00, 2.00, 0.00, 15.00, 0.00, 300.00, "
        "0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, "
        "0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, "
        "0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, "
        "0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, " R"(
	},
	OBJSTATS_LASTWASH='Не известно',
	OBJSTATS_LASTPROGRAM='Не определено',
	OBJSTATS_CAUSTICCOUNT=0,
	OBJSTATS_LASTACIDWASH='Не известно',
	CAUSTIC_REC_LIST='',
	CAUSTIC_REC_NMR='0',
	CAUSTICNAME='',
	CAUSTIC_PAR_NAME='',
	CAUSTIC_PAR = 
	{
		0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, )"
        "0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, " R"(
	},
	ACID_REC_LIST='',
	ACID_REC_NMR='0',
	ACIDNAME='',
	ACID_PAR_NAME='',
	ACID_PAR = 
	{
		0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, )"
        "0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, " R"(
	}
	}
)";
    EXPECT_STREQ( REF_STR0.c_str(), buff );

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, set_cmd )
    {
    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( lua_open() );

    cip1.initline();
    InitStationParams();

    cip1.lineRecipes->ResetRecipeToDefaults( 0 );

    auto res = cip1.set_cmd( "CUR_REC", 0, "Test name" );
    EXPECT_EQ( res, 0 );
    
    res = cip1.set_cmd( "CAUSTIC_PAR_NAME", 0, "Test name" );
    EXPECT_EQ( res, 0 );

    res = cip1.set_cmd( "ACID_PAR_NAME", 0, "Test name" );
    EXPECT_EQ( res, 0 );

    res = cip1.set_cmd( "NCAR", 1, "NN 2200" );
    EXPECT_EQ( res, 0 );


    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, SCInitPumping )
    {
    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );

    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
        "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance()->set_Lua( lua_open() );

    cip1.initline();
    InitStationParams();

    auto res = cip1.SCInitPumping( -1, -1, -1, 0, 0, 1 );
    EXPECT_EQ( res, 0 );

    G_LUA_MANAGER->free_Lua();
    }

class cipline_tech_object_test : public ::testing::Test
    {
    protected:
        void SetUp() override
            {
            lua_manager::get_instance()->set_Lua( lua_open() );
            InitCipDevices();
            InitStationParams();
            }

        void TearDown() override
            {
            ClearCipDevices();
            G_LUA_MANAGER->free_Lua();
            }

        class cipline_tech_object_mock : public cipline_tech_object
            {
            public:
                cipline_tech_object_mock()
                    : cipline_tech_object( "CIP1", 1, TECH_TYPE_SELF_CLEAN,
                        "CIP1", 1, 1, 200, 200, 200, 200 )
                    {
                    }

                int DoStep( int step_to_do ) override
                    {
                    return -1; // mock implementation
                    }
            };

        cipline_tech_object_mock cip1{};
    };

TEST_F( cipline_tech_object_test, EvalCipInProgress )
    {
    cip1.initline();
    
    auto res = cip1.EvalCipInProgress();
    EXPECT_EQ( res, -1 );
    }

// =========================== WATCHDOG TESTS ===========================

TEST( cipline_tech_object, check_device_watchdog )
    {
    // Test the check_device() helper function for watchdog device validation
    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    
    device* outdev;

    // Test with zero device number (no device) - should work without initialization
    cip1.rt_par_float[P_WATCHDOG] = 0;
    int result = cip1.check_device( outdev, P_WATCHDOG, device::DT_WATCHDOG );
    EXPECT_NE( 0, result ); // No device configured
    
    // Test with negative parameter - should work without initialization
    cip1.rt_par_float[P_WATCHDOG] = -1;
    result = cip1.check_device( outdev, P_WATCHDOG, device::DT_WATCHDOG );
    EXPECT_EQ( -2, result ); // Invalid parameter
    
    // Now test with valid device - this might be the problematic part
    cip1.rt_par_float[P_WATCHDOG] = 1;
    result = cip1.check_device( outdev, P_WATCHDOG, device::DT_WATCHDOG );
    EXPECT_EQ( 0, result ); // Should find device
    
    // Test with non-existent device number  
    cip1.rt_par_float[P_WATCHDOG] = 99;
    result = cip1.check_device( outdev, P_WATCHDOG, device::DT_WATCHDOG );
    EXPECT_EQ( -3, result ); // Should not find device
    
    ClearCipDevices();
    }

TEST( cipline_tech_object, watchdog_check_err )
    {
    lua_manager::get_instance()->set_Lua( lua_open() );

    // Test ERR_WATCHDOG error detection when watchdog device is inactive
    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    
    cip1.initline();
    cip1.init_object_devices();

    auto res = cip1._CheckErr();
    EXPECT_EQ( res, 0 );

    cip1.rt_par_float[ P_WATCHDOG ] = 1;
    cip1.init_object_devices();
    res = cip1._CheckErr();
    EXPECT_EQ( res, ERR_WATCHDOG );
        
    ClearCipDevices();
    lua_manager::get_instance()->free_Lua();
    }

TEST( cipline_tech_object, watchdog_reset )
    {
    // Test proper watchdog device reset in _ResetLinesDevicesBeforeReset()
    InitCipDevices();
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    
    // Set up watchdog device manually
    auto dm = device_manager::get_instance();
    auto watchdog_dev = dm->get_device("LINE1WATCHDOG1");
    ASSERT_NE(nullptr, watchdog_dev);
    cip1.dev_watchdog = watchdog_dev;
    
    // Set watchdog to active state
    watchdog_dev->set_state(1);
    EXPECT_EQ( 1, watchdog_dev->get_state() );
    
    // Call reset function
    cip1._ResetLinesDevicesBeforeReset();
    
    // Verify dev_watchdog pointer was nullified
    EXPECT_EQ( nullptr, cip1.dev_watchdog );
    
    ClearCipDevices();
    }

TEST( cipline_tech_object, watchdog_error_message )
    {
    // Test ERR_WATCHDOG constant value and Russian error message
    EXPECT_EQ( -42, ERR_WATCHDOG );

    // Test error message exists in map
    auto it = ERR_MSG.find( ERR_WATCHDOG );
    ASSERT_NE( ERR_MSG.end(), it );

    // Test error message content (Russian)
    std::string expected_msg = "Ошибка сторожевого таймера";
    EXPECT_EQ( expected_msg, std::string( it->second ) );
    }
