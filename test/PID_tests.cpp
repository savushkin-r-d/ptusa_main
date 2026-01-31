#include "PID_tests.h"
#include "g_errors.h"

using namespace ::testing;

TEST( PID, output_scaling )
    {
    PID test_PID( "PID_SCALE" );
    
    // Инициализация параметров ПИД.
    test_PID.init_param( PID::P_k, 1 );
    test_PID.init_param( PID::P_Ti, 15 );
    test_PID.init_param( PID::P_Td, 0.01f );
    test_PID.init_param( PID::P_dt, 1000 );
    test_PID.init_param( PID::P_max, 100 );
    test_PID.init_param( PID::P_min, 0 );
    test_PID.init_param( PID::P_acceleration_time, 0 );
    test_PID.init_param( PID::P_is_manual_mode, 0 );
    test_PID.init_param( PID::P_U_manual, 65 );
    
    // Тест 1: Масштабирование 0-100% на 0-130 единиц.
    test_PID.init_param( PID::P_out_max, 130 );
    test_PID.init_param( PID::P_out_min, 0 );
    test_PID.init_param( PID::P_is_zero_start, 1 );
    test_PID.save_param();
    
    test_PID.reset();
    test_PID.on();
    test_PID.set( 100.f );
    
    // При is_zero_start=1 и отсутствии разгона, ПИД должен начать с 0%.
    // Это должно масштабироваться в 0 единиц.
    float value = test_PID.get_value();
    EXPECT_FLOAT_EQ( value, 0.0f );
    
    // Тест 2: Масштабирование с ненулевым минимумом (0-40 единиц).
    test_PID.init_param( PID::P_out_max, 40 );
    test_PID.init_param( PID::P_out_min, 0 );
    test_PID.reset();
    value = test_PID.get_value();
    EXPECT_FLOAT_EQ( value, 0.0f );
    
    // Тест 3: Масштабирование со смещением (10-50 единиц).
    test_PID.init_param( PID::P_out_max, 50 );
    test_PID.init_param( PID::P_out_min, 10 );
    test_PID.reset();
    value = test_PID.get_value();
    EXPECT_FLOAT_EQ( value, 10.0f ); // 0% -> 10 единиц.
    
    // Тест 4: При выключенном ПИД и is_zero_start=1 должны получить out_min.
    test_PID.off();
    test_PID.off(); // Переход из STOPPING в OFF.
    float res = test_PID.eval( 50 );
    EXPECT_FLOAT_EQ( res, 10.0f );
    
    // Тест 5: При выключенном ПИД и is_zero_start=0 должны получить out_max.
    test_PID.init_param( PID::P_is_zero_start, 0 );
    res = test_PID.eval( 50 );
    EXPECT_FLOAT_EQ( res, 50.0f );
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
    G_DEVICE_MANAGER()->clear_io_devices();

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
