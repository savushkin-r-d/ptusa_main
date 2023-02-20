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
            } else
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
    cip1.parpar[ 0 ][ P_BLOCK_ERRORS ] = 1 << BE_ERR_SUPPLY_TEMP_SENSOR | 1 << BE_ERR_RETURN_TEMP_SENSOR;
    EXPECT_EQ( ERR_CONCENTRATION_SENSOR, cip1._CheckErr( ));
    cip1.parpar[ 0 ][ P_BLOCK_ERRORS ] = 1 << BE_ERR_SUPPLY_TEMP_SENSOR | 1 << BE_ERR_CONCENTRATION_SENSOR;
    EXPECT_EQ( ERR_RETURN_TEMP_SENSOR, cip1._CheckErr( ));
    cip1.parpar[ 0 ][ P_BLOCK_ERRORS ] = 1 << BE_ERR_RETURN_TEMP_SENSOR | 1 << BE_ERR_CONCENTRATION_SENSOR;
    EXPECT_EQ( ERR_SUPPLY_TEMP_SENSOR, cip1._CheckErr( ));
    cip1.parpar[ 0 ][ P_BLOCK_ERRORS ] =
        1 << BE_ERR_RETURN_TEMP_SENSOR | 1 << BE_ERR_CONCENTRATION_SENSOR | 1 << BE_ERR_SUPPLY_TEMP_SENSOR;
    EXPECT_EQ( 0, cip1._CheckErr( ));

    G_LUA_MANAGER->free_Lua();
    }

TEST( cipline_tech_object, evaluate )
    {
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance( )->set_Lua( lua_open( ));

    cip1.initline( );
    EXPECT_EQ( nullptr, cip1.PRESSURE );
    auto current_recipe = cip1.lineRecipes->getCurrentRecipe( );
    cip1.lineRecipes->setRecipeValue( current_recipe, TRecipeManager::RV_TO_DEFAULTS, 1 );
    cip1.lineRecipes->EvalRecipe( );
    EXPECT_EQ( 0, cip1.lineRecipes->getRecipeValue( current_recipe, TRecipeManager::RV_TO_DEFAULTS ));
    EXPECT_EQ( 200, cip1.lineRecipes->getRecipeValue( current_recipe, TRecipeManager::RV_V1 ));
    EXPECT_EQ( 0, cip1.evaluate( ));
    cip1.rt_par_float[ P_SELECT_REC ] = 1;
    EXPECT_EQ( 0, cip1.evaluate( ));
    cip1.rt_par_float[ P_SELECT_PRG ] = SPROG_CAUSTIC_ACID_SANITIZER;
    cip1.SetCommand( MCMD_EVALUATE );
    EXPECT_EQ( 0, cip1.state );
    EXPECT_EQ( 0, cip1.evaluate( ));
    EXPECT_NE( 0, cip1.state );
    }

TEST( cipline_tech_object, waterTankIsEmpty )
    {
    cipline_tech_object cip1( "CIP1", 1, 1, "CIP1", 1, 1, 200, 200, 200, 200 );
    lua_manager::get_instance( )->set_Lua( lua_open( ));

    cip1.initline( );
    virtual_device waterTankLowLevel( "LWL", device::DT_LS, device::DST_LS_VIRT);
    virtual_device waterTankCurrentLevel( "LTW", device::DT_LT, device::DST_LT_VIRT);
    cip1.LWL = reinterpret_cast<i_DI_device *>(&waterTankLowLevel);
    cip1.LTW = reinterpret_cast<i_AI_device *>(&waterTankCurrentLevel);

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
    waterTankCurrentLevel.set_value(9.99);
    cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 10;
    cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 2;
    EXPECT_EQ(true, cip1.waterTankIsEmpty());

    waterTankLowLevel.direct_set_state(1);
    waterTankCurrentLevel.set_value(10);
    cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 10;
    cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 2;
    EXPECT_EQ(true, cip1.waterTankIsEmpty());

    waterTankLowLevel.direct_set_state(1);
    waterTankCurrentLevel.set_value(11.99);
    cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 10;
    cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 2;
    EXPECT_EQ(true, cip1.waterTankIsEmpty());

    waterTankLowLevel.direct_set_state(1);
    waterTankCurrentLevel.set_value(12);
    cipline_tech_object::parpar[0][P_MIN_BULK_FOR_WATER] = 10;
    cipline_tech_object::parpar[0][P_MIN_BULK_DELTA] = 2;
    EXPECT_EQ(false, cip1.waterTankIsEmpty());
    }


