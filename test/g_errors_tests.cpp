#include "g_errors_tests.h"

using namespace ::testing;


TEST( errors_manager, evaluate )
    {
    G_ERRORS_MANAGER->clear();
    G_ERRORS_MANAGER->evaluate();
    EXPECT_EQ( 0, G_ERRORS_MANAGER->get_errors_id() ); //No devices - no errors.

    //Add counter to produce and check errors.    
    auto name = std::string( "FQT3" );
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT_F, name.c_str(), "Test counter", "IFM" );
    EXPECT_NE( nullptr, res );
    auto dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );

    //One devices, but no errors.
    G_ERRORS_MANAGER->evaluate();
    EXPECT_EQ( 0, G_ERRORS_MANAGER->get_errors_id() );

    //Generate error.
    dev->set_cmd( "F", 0, 1 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, dev->get_state() );
    EXPECT_EQ( (int)i_counter::STATES::S_ERROR, dev->get_state() );

    //Should get an error.
    G_ERRORS_MANAGER->evaluate();
    EXPECT_EQ( 1, G_ERRORS_MANAGER->get_errors_id() );
    G_ERRORS_MANAGER->evaluate();
    EXPECT_EQ( 1, G_ERRORS_MANAGER->get_errors_id() );

    //Should get a new error id due to set_cmd() with C_CMD_ACCEPT.
    G_ERRORS_MANAGER->set_cmd( base_error::C_CMD_ACCEPT, 7, 0, 1 );
    EXPECT_EQ( 2, G_ERRORS_MANAGER->get_errors_id() );

    //Should not get errors.
    G_ERRORS_MANAGER->evaluate();
    EXPECT_EQ( 2, G_ERRORS_MANAGER->get_errors_id() );

    //Should get a new error id due to set_cmd() with C_CMD_SUPPRESS.
    G_ERRORS_MANAGER->set_cmd( base_error::C_CMD_SUPPRESS, 7, 0, 1 );
    EXPECT_EQ( 3, G_ERRORS_MANAGER->get_errors_id() );

    //Should not get errors.
    G_ERRORS_MANAGER->evaluate();
    EXPECT_EQ( 3, G_ERRORS_MANAGER->get_errors_id() );

    //Remove error.
    dev->set_cmd( "ABS_V", 0, 100 );
    dev->set_cmd( "P_DT", 0, 1000 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, dev->get_state() );
    G_ERRORS_MANAGER->evaluate(); //Process  ALARM_STATE::AS_ALARM -> AS_RETURN.
    EXPECT_EQ( 4, G_ERRORS_MANAGER->get_errors_id() );
    G_ERRORS_MANAGER->evaluate(); //Process  ALARM_STATE::AS_RETURN.
    EXPECT_EQ( 4, G_ERRORS_MANAGER->get_errors_id() );
    }
