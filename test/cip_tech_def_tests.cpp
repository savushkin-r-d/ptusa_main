#include "cip_tech_def_tests.h"
#include "lua_manager.h"

using namespace ::testing;

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
    



