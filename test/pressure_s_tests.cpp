#include "gtest/gtest.h"
#include "device/device.h"
#include "device/manager.h"

using namespace ::testing;

TEST( pressure_s, constructor )
    {
    pressure_s ps( "PS1", device::DST_PS_MIN );
    EXPECT_STREQ( "PS1", ps.get_name() );
    EXPECT_EQ( device::DT_PS, ps.get_type() );
    EXPECT_EQ( device::DST_PS_MIN, ps.get_sub_type() );
    }

TEST( pressure_s, is_active_min )
    {
    pressure_s ps( "PS1", device::DST_PS_MIN );
    
    // For DST_PS_MIN: active when state == 1, inactive when state == 0
    ps.direct_set_state( 0 );
    EXPECT_FALSE( ps.is_active() );
    
    ps.direct_set_state( 1 );
    EXPECT_TRUE( ps.is_active() );
    }

TEST( pressure_s, is_active_max )
    {
    pressure_s ps( "PS1", device::DST_PS_MAX );
    
    // For DST_PS_MAX: active when state == 0, inactive when state == 1
    ps.direct_set_state( 0 );
    EXPECT_TRUE( ps.is_active() );
    
    ps.direct_set_state( 1 );
    EXPECT_FALSE( ps.is_active() );
    }

TEST( pressure_s_iolink, constructor )
    {
    pressure_s_iolink ps( "PS1", device::PS_IOLINK_MIN );
    EXPECT_STREQ( "PS1", ps.get_name() );
    EXPECT_EQ( device::DT_PS, ps.get_type() );
    EXPECT_EQ( device::PS_IOLINK_MIN, ps.get_sub_type() );
    }

TEST( pressure_s_iolink, set_article )
    {
    pressure_s_iolink ps( "PS1", device::PS_IOLINK_MIN );
    
    ps.set_article( "IFM.PV7004" );
    #ifdef PTUSA_TEST
    EXPECT_EQ( pressure_s_iolink::ARTICLE::IFM_PV7004, ps.get_article_n() );
    #endif
    
    ps.set_article( "DEFAULT" );
    #ifdef PTUSA_TEST
    EXPECT_EQ( pressure_s_iolink::ARTICLE::DEFAULT, ps.get_article_n() );
    #endif
    }

TEST( pressure_s_iolink, is_active_min )
    {
    pressure_s_iolink ps( "PS1", device::PS_IOLINK_MIN );
    
    // For PS_IOLINK_MIN: starts with current_state == 0 (inactive)
    // This is the expected initial state for MIN sensors
    EXPECT_FALSE( ps.is_active() );
    
    // The current_state would be updated through evaluate_io() in real usage
    // For testing, we can verify the constructor sets it correctly
    EXPECT_EQ( 0, ps.get_state() );
    }

TEST( pressure_s_iolink, is_active_max )
    {
    pressure_s_iolink ps( "PS1", device::PS_IOLINK_MAX );
    
    // For PS_IOLINK_MAX: starts with current_state == 1 (active)
    // This is the expected initial state for MAX sensors
    EXPECT_TRUE( ps.is_active() );
    
    // The current_state would be updated through evaluate_io() in real usage
    // For testing, we can verify the constructor sets it correctly
    EXPECT_EQ( 1, ps.get_state() );
    }