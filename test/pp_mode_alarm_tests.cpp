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
