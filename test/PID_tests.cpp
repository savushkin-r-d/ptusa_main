#include "PID_tests.h"

using namespace ::testing;

TEST( PID, get_actuator )
    {
    G_DEVICE_MANAGER()->clear_io_devices();

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
    test_PID.on();
    auto res = test_PID.eval( 100 );

    // По умолчанию получить должны 0-ое значение выхода.
    EXPECT_EQ( res, 0 );
    }
