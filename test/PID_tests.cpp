#include "PID_tests.h"
#include "g_errors.h"

using namespace ::testing;

/// @brief Helper to init PID parameters for output scaling tests.
///        P_out_min and P_out_max are now in engineering units.
void init_PID_scaling_parameters( PID& test_PID, float out_min,
    float out_max )
    {
    test_PID.init_param( PID::P_k, 1 );
    test_PID.init_param( PID::P_Ti, 15 );
    test_PID.init_param( PID::P_Td, 0.01f );
    test_PID.init_param( PID::P_dt, 1000 );
    test_PID.init_param( PID::P_max, 100 );
    test_PID.init_param( PID::P_min, 0 );
    test_PID.init_param( PID::P_acceleration_time, 0 );
    test_PID.init_param( PID::P_is_manual_mode, 0 );
    test_PID.init_param( PID::P_U_manual, 50 );
    test_PID.init_param( PID::P_k2, 0 );
    test_PID.init_param( PID::P_Ti2, 0 );
    test_PID.init_param( PID::P_Td2, 0 );
    test_PID.init_param( PID::P_out_max, out_max );
    test_PID.init_param( PID::P_out_min, out_min );
    test_PID.init_param( PID::P_is_zero_start, 1 );
    test_PID.init_param( PID::P_is_reverse, 0 );
    test_PID.save_param();

    test_PID.set( 100.f );
    }

/// @brief Test that PID output is scaled from internal 0-100% to
///        engineering units defined by P_out_min and P_out_max.
TEST( PID, output_scaling )
    {
    // P_out_min = 50 units, P_out_max = 150 units.
    // Internal 0% -> 50 units, 100% -> 150 units.
    PID test_PID( "PID_scaling" );
    init_PID_scaling_parameters( test_PID, 50.f, 150.f );
    test_PID.reset();
    test_PID.on();

    // With millitime = 0, no update occurs (delta < dt = 1000).
    // Uk = 0 (from reset with is_zero_start=1).
    // Expected: out = 50 + 0/100 * 100 = 50 units.
    auto res = test_PID.eval( 0 );
    EXPECT_FLOAT_EQ( res, 50.f );

    // Advance time to trigger a PID update (delta > dt).
    // Input = 0, setpoint = 100 -> error = 100 -> Uk increases.
    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    res = test_PID.eval( 0 );
    DeltaMilliSecSubHooker::set_default_time();

    // After update, Uk should be 100 (clamped) -> output = 50 + 100/100*100 = 150.
    EXPECT_FLOAT_EQ( res, 150.f );

    // Test with P_out_min = 0, P_out_max = 200 units.
    // Internal 50% -> 100 units.
    PID test_PID2( "PID_scaling2" );
    init_PID_scaling_parameters( test_PID2, 0.f, 200.f );
    test_PID2.reset();
    test_PID2.on();

    // Force Uk to 50% by having setpoint = input (zero error after some cycles).
    // Easier: check scale at Uk = 0 and Uk = 100.
    // Uk = 0 -> out = 0.
    auto res2 = test_PID2.eval( 0 );
    EXPECT_FLOAT_EQ( res2, 0.f );

    // After one update cycle with full error, Uk clamps to 100% -> out = 200 units.
    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    res2 = test_PID2.eval( 0 );
    DeltaMilliSecSubHooker::set_default_time();
    EXPECT_FLOAT_EQ( res2, 200.f );
    }

/// @brief Test that manual mode P_U_manual (in %) is correctly scaled
///        to engineering units when output scaling is active.
TEST( PID, output_scaling_manual_mode )
    {
    // P_out_min = 50 units, P_out_max = 150 units.
    PID test_PID( "PID_manual_scaling" );
    init_PID_scaling_parameters( test_PID, 50.f, 150.f );
    test_PID.reset();
    test_PID.on();

    // Enable manual mode - on first eval this stores out_value as % into
    // P_U_manual (bumpless transfer).
    test_PID.init_param( PID::P_is_manual_mode, 1 );
    test_PID.eval( 0 );

    // Now P_U_manual has been captured from out_value.
    // Set P_U_manual = 50% explicitly for the next eval.
    // P_U_manual = 50% of range [50, 150] -> output = 50 + 50/100*100 = 100 units.
    test_PID.init_param( PID::P_U_manual, 50.f );
    auto res = test_PID.eval( 0 );
    EXPECT_FLOAT_EQ( res, 100.f );

    // P_U_manual = 0% -> output = 50 units.
    test_PID.init_param( PID::P_U_manual, 0.f );
    res = test_PID.eval( 0 );
    EXPECT_FLOAT_EQ( res, 50.f );

    // P_U_manual = 100% -> output = 150 units.
    test_PID.init_param( PID::P_U_manual, 100.f );
    res = test_PID.eval( 0 );
    EXPECT_FLOAT_EQ( res, 150.f );
    }

TEST( PID, get_actuator )
    {
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_REGULATOR, device::DST_REGULATOR_PID, "TC1",
        "Test controller", "T" );
    ASSERT_EQ( nullptr, res );
    auto p1_dev = G_DEVICE_MANAGER()->get_C( "TC1" );
    ASSERT_NE( nullptr, p1_dev );
    auto p1 = dynamic_cast<PID*>( p1_dev );
    ASSERT_NE( nullptr, p1 );
    
    ASSERT_EQ( nullptr, p1->get_actuator() );

    p1->set_string_property( "OUT_VALUE", "TC1" );
    auto m = p1->get_actuator();
    ASSERT_EQ( nullptr, m );

    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_M, device::DST_M_VIRT, "M1", "Test motor", "M" );
    ASSERT_EQ( nullptr, res );
    auto m1 = G_DEVICE_MANAGER()->get_M( "M1" );
    ASSERT_NE( nullptr, m1 );
    p1->set_string_property( "OUT_VALUE", "M1" );
    m = p1->get_actuator();
    ASSERT_NE( nullptr, m );
    ASSERT_EQ( m1, m );

    G_DEVICE_MANAGER()->clear_io_devices();
    }

void init_PID_parameters( PID& test_PID, float min_value = 0.f )
    {
    test_PID.init_param( PID::P_k, 1 );
    test_PID.init_param( PID::P_Ti, 15 );
    test_PID.init_param( PID::P_Td, 0.01f );
    test_PID.init_param( PID::P_dt, 1000 );
    test_PID.init_param( PID::P_max, 100 );
    test_PID.init_param( PID::P_min, 0 );
    test_PID.init_param( PID::P_acceleration_time, 30 );
    test_PID.init_param( PID::P_is_manual_mode, 0 );
    test_PID.init_param( PID::P_U_manual, 65 );

    test_PID.init_param( PID::P_out_max, 100 );
    test_PID.init_param( PID::P_out_min, min_value );
    test_PID.init_param( PID::P_is_zero_start, 1 );
    test_PID.save_param();

    test_PID.set( 100.f );
    }

TEST( PID, direct_set_value )
    {
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_REGULATOR, device::DST_REGULATOR_PID, "TC1",
        "Test controller", "T" );
    ASSERT_EQ( nullptr, res );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_VIRT, "TE1", "Test sensor", "T" );
    ASSERT_EQ( nullptr, res );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_M, device::DST_M_VIRT, "M1", "Test motor", "M" );
    ASSERT_EQ( nullptr, res );
    G_DEVICE_MANAGER()->print();

    auto m1 = G_DEVICE_MANAGER()->get_M( "M1" );
    ASSERT_NE( nullptr, m1 );
    auto p1_dev = G_DEVICE_MANAGER()->get_C( "TC1" );
    ASSERT_NE( nullptr, p1_dev );
    PID* p1 = dynamic_cast<PID*>( p1_dev );
    ASSERT_NE( nullptr, p1 );

    p1->set_string_property( "IN_VALUE", "TE1" );
    p1->set_string_property( "OUT_VALUE", "M1" );

    init_PID_parameters( *p1 );

    //PID switched off.
    p1_dev->set_value( 10 );
    EXPECT_EQ( .0f, m1->get_value() );
    EXPECT_EQ( 0, m1->get_state() );
    EXPECT_EQ( static_cast<int>( PID::STATE::OFF ), p1_dev->get_state() );

    //PID switched on.
    p1_dev->on();
    p1_dev->set_value( 10 );
    EXPECT_EQ( .0f, m1->get_value() );
    EXPECT_EQ( 1, m1->get_state() );
    EXPECT_EQ( static_cast<int>( PID::STATE::ON ), p1_dev->get_state() );

    //PID switched off.
    p1_dev->off();
    EXPECT_EQ( .0f, m1->get_value() );
    EXPECT_EQ( 1, m1->get_state() );
    EXPECT_EQ( static_cast<int>( PID::STATE::STOPPING ), p1_dev->get_state() );
    p1_dev->off();
    EXPECT_EQ( 0, m1->get_state() );
    EXPECT_EQ( static_cast<int>( PID::STATE::OFF ), p1_dev->get_state() );

    //Нельзя установить состояние "Выключаюсь" - в него ПИД-регулятор переходит
    //при выключении из работы.
    p1_dev->set_state( static_cast<int>( PID::STATE::STOPPING ) );
    EXPECT_EQ( static_cast<int>( PID::STATE::OFF ), p1_dev->get_state() );

    p1_dev->set_state( static_cast<int>( PID::STATE::ON ) );
    EXPECT_EQ( static_cast<int>( PID::STATE::ON ), p1_dev->get_state() );

    //ПИД-регулятор переходит в состояние "Выключаюсь" при выключении из
    //состояния "Работа".
    p1_dev->set_state( static_cast<int>( PID::STATE::OFF ) );
    EXPECT_EQ( static_cast<int>( PID::STATE::STOPPING ), p1_dev->get_state() );
    //ПИД-регулятор переходит в состояние "Выключено" при выключении из 
    //состояни "Выключаюсь".
    p1_dev->set_state( static_cast<int>( PID::STATE::OFF ) );
    EXPECT_EQ( static_cast<int>( PID::STATE::OFF ), p1_dev->get_state() );

    const int BUFF_SIZE = 512;
    char buff[ BUFF_SIZE ] = { 0 };

    p1->save_device( buff );
    EXPECT_STREQ(
        "TC1={M=0, ST=0, V=0, Z=10.00, P_k=1, P_Ti=15, P_Td=0.01, "
        "P_dt=1000, P_max=100, P_min=0, P_acceleration_time=30, "
        "P_is_manual_mode=0, P_U_manual=65, P_k2=0, P_Ti2=0, P_Td2=0, "
        "P_out_max=100, P_out_min=0, P_is_reverse=0, P_is_zero_start=1},\n",
        buff );        

    G_DEVICE_MANAGER()->clear_io_devices();
    }

TEST( PID, save_device )
    {
    const int BUFF_SIZE = 512;
    char buff[ BUFF_SIZE ] = { 0 };
     
    auto REF_STR =
        "t.PID1=\n"
        "\t{\n"
        "\tST=0,\n"
        "\tS_PAR_F=\n"
        "\t\t{\n"
        "\t\t0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \n"
        "\t\t},\n"
        "\tRT_PAR_F={ 0.00, 0.00 }\n"
        "\t}\n";

    PID pid1( 1 );
    pid1.save_device( buff );
    EXPECT_STREQ( buff, REF_STR );

    PID pid2( "PID2" );
    pid2.save_device( buff );
    EXPECT_STREQ(
        "PID2={M=0, ST=0, V=0, Z=0.00, P_k=0, P_Ti=0, P_Td=0, P_dt=0, "
        "P_max=0, P_min=0, P_acceleration_time=0, P_is_manual_mode=0, "
        "P_U_manual=0, P_k2=0, P_Ti2=0, P_Td2=0, P_out_max=0, P_out_min=0, "
        "P_is_reverse=0, P_is_zero_start=0},\n", buff );
    }


TEST( PID, eval )
    {
    PID test_PID( "PID1" );

    // Корректная работа регулятора, когда параметры не инициализированы.
    test_PID.on();
    auto res = test_PID.eval( 0 );
    EXPECT_EQ( res, .0f );

    auto MIN_VALUE = 35.f;
    init_PID_parameters( test_PID, MIN_VALUE );
    test_PID.reset();
    test_PID.on();

    // Должны получить значение выхода, которое больше минимального.
    // Так должен работать ПИД во время разгона.
    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    res = test_PID.eval( 0 );
    DeltaMilliSecSubHooker::set_default_time();
    EXPECT_GT( res, MIN_VALUE );

    DeltaMilliSecSubHooker::set_millisec( 2UL * 1001UL );
    res = test_PID.eval( 1 );
    EXPECT_GT( res, MIN_VALUE );
    DeltaMilliSecSubHooker::set_default_time();

    DeltaMilliSecSubHooker::set_millisec( 3UL * 1001UL );
    res = test_PID.eval( 2 );
    EXPECT_GT( res, MIN_VALUE );
    DeltaMilliSecSubHooker::set_default_time();
    }
