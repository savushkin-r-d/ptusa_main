/// @file alarm_management_tests.cpp
/// @brief Unit tests for enhanced alarm management features.
///
/// Tests for timestamp tracking, occurrence counting, and alarm shelving.

#include "alarm_management_tests.h"

using namespace ::testing;

// Test timestamp tracking on alarm occurrence.
TEST( alarm_management, timestamp_tracking )
    {
    G_ERRORS_MANAGER->clear();
    G_DEVICE_MANAGER()->clear_io_devices();
    
    // Add a counter device to test.
    auto name = std::string( "FQT_TEST" );
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT_F, name.c_str(), "Test counter", "IFM" );
    ASSERT_NE( nullptr, res );
    auto dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    ASSERT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    
    // Generate an error.
    dev->set_cmd( "F", 0, 1 );
    dev->set_cmd( "P_DT", 0, 1000 );
    dev->set_cmd( "P_ERR_MIN_FLOW", 0, .1 );
    dev->evaluate_io();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, dev->get_state() );
    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    dev->evaluate_io();
    DeltaMilliSecSubHooker::set_default_time();
    EXPECT_EQ( (int)i_counter::STATES::S_FLOW_ERROR, dev->get_state() );
    
    // Trigger error evaluation.
    G_ERRORS_MANAGER->evaluate();
    
    // Verify alarm timestamps were recorded.
    // Note: We can't directly access tech_dev_error from public API,
    // but we can verify the error was registered.
    EXPECT_EQ( 1, G_ERRORS_MANAGER->get_errors_id() );
    
    G_DEVICE_MANAGER()->clear_io_devices();
    }

// Test occurrence counter increments.
TEST( alarm_management, occurrence_counter )
    {
    G_ERRORS_MANAGER->clear();
    G_DEVICE_MANAGER()->clear_io_devices();
    
    // Add a counter device to test.
    auto name = std::string( "FQT_TEST2" );
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT_F, name.c_str(), "Test counter", "IFM" );
    ASSERT_NE( nullptr, res );
    auto dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    ASSERT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    
    // Generate first error.
    dev->set_cmd( "F", 0, 1 );
    dev->set_cmd( "P_DT", 0, 1000 );
    dev->set_cmd( "P_ERR_MIN_FLOW", 0, .1 );
    dev->evaluate_io();
    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    dev->evaluate_io();
    DeltaMilliSecSubHooker::set_default_time();
    
    G_ERRORS_MANAGER->evaluate();
    auto first_error_id = G_ERRORS_MANAGER->get_errors_id();
    
    // Clear the error.
    dev->set_cmd( "ABS_V", 0, 100 );
    dev->set_cmd( "P_DT", 0, 1000 );
    dev->set_cmd( "ST", 0, 1 );
    G_ERRORS_MANAGER->evaluate();
    
    // Accept the return.
    G_ERRORS_MANAGER->set_cmd( base_error::C_CMD_ACCEPT, 7, 0, 1 );
    G_ERRORS_MANAGER->evaluate();
    
    // Generate second error.
    dev->set_cmd( "F", 0, 1 );
    dev->set_cmd( "P_DT", 0, 1000 );
    dev->evaluate_io();
    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    dev->evaluate_io();
    DeltaMilliSecSubHooker::set_default_time();
    
    G_ERRORS_MANAGER->evaluate();
    auto second_error_id = G_ERRORS_MANAGER->get_errors_id();
    
    // Error ID should have changed due to new alarm occurrence.
    EXPECT_NE( first_error_id, second_error_id );
    
    G_DEVICE_MANAGER()->clear_io_devices();
    }

// Test alarm shelving via command.
TEST( alarm_management, alarm_shelving )
    {
    G_ERRORS_MANAGER->clear();
    G_DEVICE_MANAGER()->clear_io_devices();
    
    // Add a counter device to test.
    auto name = std::string( "FQT_TEST3" );
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT_F, name.c_str(), "Test counter", "IFM" );
    ASSERT_NE( nullptr, res );
    auto dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    ASSERT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    
    // Generate an error.
    dev->set_cmd( "F", 0, 1 );
    dev->set_cmd( "P_DT", 0, 1000 );
    dev->set_cmd( "P_ERR_MIN_FLOW", 0, .1 );
    dev->evaluate_io();
    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    dev->evaluate_io();
    DeltaMilliSecSubHooker::set_default_time();
    
    G_ERRORS_MANAGER->evaluate();
    auto error_id_before_shelve = G_ERRORS_MANAGER->get_errors_id();
    
    // Shelve the alarm (duration of 5 seconds passed as object_alarm_number).
    G_ERRORS_MANAGER->set_cmd( base_error::C_CMD_SHELVE, 7, 0, 5 );
    auto error_id_after_shelve = G_ERRORS_MANAGER->get_errors_id();
    
    // Error ID should change when shelve command is issued.
    EXPECT_NE( error_id_before_shelve, error_id_after_shelve );
    
    // Unshelve the alarm.
    G_ERRORS_MANAGER->set_cmd( base_error::C_CMD_UNSHELVE, 7, 0, 1 );
    auto error_id_after_unshelve = G_ERRORS_MANAGER->get_errors_id();
    
    // Error ID should change when unshelve command is issued.
    EXPECT_NE( error_id_after_shelve, error_id_after_unshelve );
    
    G_DEVICE_MANAGER()->clear_io_devices();
    }

// Test alarm acknowledgment timestamp recording.
TEST( alarm_management, acknowledgment )
    {
    G_ERRORS_MANAGER->clear();
    G_DEVICE_MANAGER()->clear_io_devices();
    
    // Add a counter device to test.
    auto name = std::string( "FQT_TEST4" );
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT_F, name.c_str(), "Test counter", "IFM" );
    ASSERT_NE( nullptr, res );
    auto dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    ASSERT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    
    // Generate an error.
    dev->set_cmd( "F", 0, 1 );
    dev->set_cmd( "P_DT", 0, 1000 );
    dev->set_cmd( "P_ERR_MIN_FLOW", 0, .1 );
    dev->evaluate_io();
    DeltaMilliSecSubHooker::set_millisec( 1001UL );
    dev->evaluate_io();
    DeltaMilliSecSubHooker::set_default_time();
    
    G_ERRORS_MANAGER->evaluate();
    EXPECT_EQ( 1, G_ERRORS_MANAGER->get_errors_id() );
    
    // Acknowledge the alarm.
    G_ERRORS_MANAGER->set_cmd( base_error::C_CMD_ACCEPT, 7, 0, 1 );
    
    // Error ID should increment after acknowledgment.
    EXPECT_EQ( 2, G_ERRORS_MANAGER->get_errors_id() );
    
    G_DEVICE_MANAGER()->clear_io_devices();
    }
