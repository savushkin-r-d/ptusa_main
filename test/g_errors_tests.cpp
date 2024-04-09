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

    //Generate error.
    dev->set_cmd( "F", 0, 1 );    
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, dev->get_state() );
    EXPECT_EQ( (int)i_counter::STATES::S_ERROR, dev->get_state() );

    //Should get error
    G_ERRORS_MANAGER->evaluate();
    EXPECT_EQ( 1, G_ERRORS_MANAGER->get_errors_id() );
    }
