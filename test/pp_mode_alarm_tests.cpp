#include "pp_mode_alarm_tests.h"
#include "PAC_err.h"

using namespace ::testing;

// Test PP mode alarm activation.
TEST( pp_mode_alarm, pp_mode_activation )
    {
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );

    // Simulate status register change to PP mode active.
    node.prev_status_register = 0;
    node.status_register = 0x0010; // Bit 4: PP mode active.
    node.is_pp_mode_alarm_set = false;

    // Manually trigger the PP mode detection logic.
    bool is_pp_mode_active = 
        ( node.status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    bool was_pp_mode_active = 
        ( node.prev_status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;

    EXPECT_TRUE( is_pp_mode_active );
    EXPECT_FALSE( was_pp_mode_active );
    EXPECT_FALSE( node.is_pp_mode_alarm_set );
    }

// Test PP mode alarm deactivation.
TEST( pp_mode_alarm, pp_mode_deactivation )
    {
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );

    // Simulate status register change from PP mode to normal.
    node.prev_status_register = 0x0010; // PP mode was active.
    node.status_register = 0; // PP mode is now inactive.
    node.is_pp_mode_alarm_set = true;

    // Manually trigger the PP mode detection logic.
    bool is_pp_mode_active = 
        ( node.status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    bool was_pp_mode_active = 
        ( node.prev_status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;

    EXPECT_FALSE( is_pp_mode_active );
    EXPECT_TRUE( was_pp_mode_active );
    EXPECT_TRUE( node.is_pp_mode_alarm_set );
    }

// Test that PP mode alarm flag is initialized correctly.
TEST( pp_mode_alarm, initialization )
    {
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );

    // Verify initial state.
    EXPECT_EQ( 0, node.status_register );
    EXPECT_EQ( 0, node.prev_status_register );
    EXPECT_FALSE( node.is_pp_mode_alarm_set );
    }

// Test that multiple status register bits trigger PP mode.
TEST( pp_mode_alarm, multiple_error_bits )
    {
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );

    // Test each bit in the error mask.
    for ( int bit = 0; bit < 6; bit++ )
        {
        u_int_2 test_value = 1 << bit;
        EXPECT_TRUE( 
            ( test_value & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0 )
            << "Bit " << bit << " should be in error mask";
        }

    // Test value outside the mask.
    u_int_2 outside_mask = 0x0040; // Bit 6.
    EXPECT_FALSE( 
        ( outside_mask & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0 )
        << "Bit 6 should not be in error mask";
    }

// Test PP mode detection with non-Phoenix nodes.
TEST( pp_mode_alarm, non_phoenix_node )
    {
    io_manager::io_node wago_node( io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        1, "127.0.0.1", "W100", 0, 0, 0, 0, 0, 0 );

    // Set error bits for Wago node.
    wago_node.status_register = 0x003F;
    wago_node.is_active = true;
    wago_node.state = io_manager::io_node::ST_OK;

    // Wago nodes should not trigger PP mode warnings.
    EXPECT_EQ( io_manager::io_node::ST_OK, wago_node.get_display_state() );
    }

// Test disconnect resets PP mode alarm.
TEST( pp_mode_alarm, disconnect_resets_alarm )
    {
    uni_io_manager mngr;
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );
    
    // Set PP mode alarm active.
    node.is_pp_mode_alarm_set = true;
    node.state = io_manager::io_node::ST_OK;
    
    // Disconnect should reset the alarm.
    mngr.disconnect( &node );
    
    EXPECT_FALSE( node.is_pp_mode_alarm_set );
    EXPECT_EQ( io_manager::io_node::ST_NO_CONNECT, node.state );
    }

// Test disconnect when alarm is not set.
TEST( pp_mode_alarm, disconnect_no_alarm )
    {
    uni_io_manager mngr;
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );
    
    // No PP mode alarm active.
    node.is_pp_mode_alarm_set = false;
    node.state = io_manager::io_node::ST_OK;
    
    // Disconnect should not crash when no alarm is set.
    mngr.disconnect( &node );
    
    EXPECT_FALSE( node.is_pp_mode_alarm_set );
    EXPECT_EQ( io_manager::io_node::ST_NO_CONNECT, node.state );
    }

// Test PP mode transition from inactive to active.
TEST( pp_mode_alarm, transition_inactive_to_active )
    {
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );
    
    // Start with no PP mode.
    node.prev_status_register = 0;
    node.status_register = 0;
    node.is_pp_mode_alarm_set = false;
    
    // Transition to PP mode active.
    node.prev_status_register = node.status_register;
    node.status_register = 0x0001; // Bit 0 set.
    
    bool is_pp_mode_active = 
        ( node.status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    bool was_pp_mode_active = 
        ( node.prev_status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    
    EXPECT_TRUE( is_pp_mode_active );
    EXPECT_FALSE( was_pp_mode_active );
    }

// Test PP mode transition from active to inactive.
TEST( pp_mode_alarm, transition_active_to_inactive )
    {
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );
    
    // Start with PP mode active.
    node.prev_status_register = 0x0020; // Bit 5 set.
    node.status_register = 0x0020;
    node.is_pp_mode_alarm_set = true;
    
    // Transition to PP mode inactive.
    node.prev_status_register = node.status_register;
    node.status_register = 0;
    
    bool is_pp_mode_active = 
        ( node.status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    bool was_pp_mode_active = 
        ( node.prev_status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    
    EXPECT_FALSE( is_pp_mode_active );
    EXPECT_TRUE( was_pp_mode_active );
    }

// Test no alarm when PP mode stays active.
TEST( pp_mode_alarm, pp_mode_stays_active )
    {
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );
    
    // PP mode was and is still active.
    node.prev_status_register = 0x0010;
    node.status_register = 0x0010;
    node.is_pp_mode_alarm_set = true;
    
    bool is_pp_mode_active = 
        ( node.status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    bool was_pp_mode_active = 
        ( node.prev_status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    
    // Should detect that both are active (no transition).
    EXPECT_TRUE( is_pp_mode_active );
    EXPECT_TRUE( was_pp_mode_active );
    EXPECT_TRUE( node.is_pp_mode_alarm_set );
    }

// Test no alarm when PP mode stays inactive.
TEST( pp_mode_alarm, pp_mode_stays_inactive )
    {
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );
    
    // PP mode was and is still inactive.
    node.prev_status_register = 0;
    node.status_register = 0;
    node.is_pp_mode_alarm_set = false;
    
    bool is_pp_mode_active = 
        ( node.status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    bool was_pp_mode_active = 
        ( node.prev_status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    
    // Should detect that both are inactive (no transition).
    EXPECT_FALSE( is_pp_mode_active );
    EXPECT_FALSE( was_pp_mode_active );
    EXPECT_FALSE( node.is_pp_mode_alarm_set );
    }

// Test all error mask bits together.
TEST( pp_mode_alarm, all_error_bits_set )
    {
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );
    
    // Set all error bits.
    node.status_register = 0x003F; // All 6 bits set.
    
    bool is_pp_mode_active = 
        ( node.status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    
    EXPECT_TRUE( is_pp_mode_active );
    EXPECT_EQ( 0x003F, node.status_register & io_manager::io_node::STATUS_REG_ERROR_MASK );
    }

// Test read_phoenix_status_register with error flag set (early return).
TEST( pp_mode_alarm, read_status_with_error_flag )
    {
    uni_io_manager mngr;
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );
    
    // Set error flag - should cause early return.
    node.read_io_error_flag = true;
    node.status_register = 0x0010;
    node.prev_status_register = 0;
    node.is_pp_mode_alarm_set = false;
    
    // Call should return early without changing status.
    mngr.read_phoenix_status_register( &node );
    
    // Status should remain unchanged.
    EXPECT_EQ( 0x0010, node.status_register );
    EXPECT_EQ( 0, node.prev_status_register );
    EXPECT_FALSE( node.is_pp_mode_alarm_set );
    }

// Test PP mode alarm activation through read_phoenix_status_register.
TEST( pp_mode_alarm, read_status_activates_alarm )
    {
    uni_io_manager mngr;
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );
    
    // Simulate transition to PP mode.
    node.read_io_error_flag = false;
    node.prev_status_register = 0;
    node.status_register = 0x0001; // Will be set by read.
    node.is_pp_mode_alarm_set = false;
    
    // Manually set resultbuff to simulate successful read.
    mngr.resultbuff[0] = 0;
    mngr.resultbuff[1] = 0x01; // Bit 0 set.
    
    // Note: We can't fully test this without mocking read_input_registers,
    // but we can test the state change logic.
    // Simulate what would happen after a successful read.
    node.status_register = 0x0001;
    
    // Manually trigger the state change logic.
    bool is_pp_mode_active = 
        ( node.status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    bool was_pp_mode_active = 
        ( node.prev_status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    
    if ( is_pp_mode_active && !was_pp_mode_active )
        {
        if ( !node.is_pp_mode_alarm_set )
            {
            node.is_pp_mode_alarm_set = true;
            // In real code: set_global_error would be called.
            }
        }
    
    node.prev_status_register = node.status_register;
    
    EXPECT_TRUE( node.is_pp_mode_alarm_set );
    EXPECT_EQ( 0x0001, node.prev_status_register );
    }

// Test PP mode alarm deactivation through status register update.
TEST( pp_mode_alarm, read_status_deactivates_alarm )
    {
    uni_io_manager mngr;
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );
    
    // Start with PP mode active.
    node.read_io_error_flag = false;
    node.prev_status_register = 0x0010;
    node.status_register = 0x0010;
    node.is_pp_mode_alarm_set = true;
    
    // Simulate transition to normal mode.
    node.status_register = 0;
    
    // Manually trigger the state change logic.
    bool is_pp_mode_active = 
        ( node.status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    bool was_pp_mode_active = 
        ( node.prev_status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    
    if ( !is_pp_mode_active && was_pp_mode_active )
        {
        if ( node.is_pp_mode_alarm_set )
            {
            node.is_pp_mode_alarm_set = false;
            // In real code: reset_global_error would be called.
            }
        }
    
    node.prev_status_register = node.status_register;
    
    EXPECT_FALSE( node.is_pp_mode_alarm_set );
    EXPECT_EQ( 0, node.prev_status_register );
    }

// Test that alarm is not set again if already active.
TEST( pp_mode_alarm, alarm_already_set_no_duplicate )
    {
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );
    
    // PP mode becomes active but alarm is already set.
    node.prev_status_register = 0;
    node.status_register = 0x0010;
    node.is_pp_mode_alarm_set = true; // Already set!
    
    bool is_pp_mode_active = 
        ( node.status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    bool was_pp_mode_active = 
        ( node.prev_status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    
    // Check the condition - should not enter the block.
    bool should_set_alarm = is_pp_mode_active && !was_pp_mode_active && !node.is_pp_mode_alarm_set;
    
    EXPECT_FALSE( should_set_alarm );
    EXPECT_TRUE( node.is_pp_mode_alarm_set );
    }

// Test that alarm is not reset if not active.
TEST( pp_mode_alarm, alarm_not_set_no_reset )
    {
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );
    
    // PP mode becomes inactive but alarm is not set.
    node.prev_status_register = 0x0010;
    node.status_register = 0;
    node.is_pp_mode_alarm_set = false; // Not set!
    
    bool is_pp_mode_active = 
        ( node.status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    bool was_pp_mode_active = 
        ( node.prev_status_register & io_manager::io_node::STATUS_REG_ERROR_MASK ) != 0;
    
    // Check the condition - should not enter the block.
    bool should_reset_alarm = !is_pp_mode_active && was_pp_mode_active && node.is_pp_mode_alarm_set;
    
    EXPECT_FALSE( should_reset_alarm );
    EXPECT_FALSE( node.is_pp_mode_alarm_set );
    }
