#include "pp_mode_alarm_tests.h"
#include "PAC_err.h"

using namespace ::testing;

class test_uni_io_manager1 : public uni_io_manager
    {
    public:
        ~test_uni_io_manager1() override = default;

        void activate_error()
            {
            buff[ 1 ] = 0x10; // PP mode is active.
            }

        void deactivate_error()
            {
            buff[ 1 ] = 0x0; // PP mode is inactive.
            }

        int read_input_registers( io_node* node, unsigned int address,
            unsigned int quantity, unsigned char station /*= 0*/ ) override
            {
            resultbuff = buff;
            return 1;
            }
    };

// Test PP mode alarm activation.
TEST( pp_mode_alarm, pp_mode_activation_deactivation )
    {
    tcp_communicator::init_instance( "Тест", "Test" );

    G_IO_MANAGER()->init( 1 );
    G_IO_MANAGER()->add_node( 0,
        io_manager::io_node::TYPES::PHOENIX_BK_ETH, 1, "127.0.0.1", "A100",
        0, 0, 0, 0, 0, 0 );
    auto node = G_IO_MANAGER()->get_node( 0 );

    test_uni_io_manager1 mngr;
    mngr.activate_error();
    mngr.read_phoenix_status_register( node );

    u_int_2 err_id = 0;
    std::array<char, 300> lua_buff{};
    PAC_critical_errors_manager::get_instance()->save_as_Lua_str( lua_buff.data(),
        err_id );
    auto REF_STR = R"s(	{
	description = "6-1-1 : Узел I/O 'A100' ('127.0.0.1', 'Тест') - активен PP mode (каналы управления заблокированы)",
	type = AT_SPECIAL,
	group = 'Авария',
	priority = 100,
	state = AS_ALARM,
	id_n = 1,
	},
)s";
    EXPECT_STREQ( lua_buff.data(), REF_STR );    // Есть ошибка.

    mngr.deactivate_error();
    mngr.read_phoenix_status_register( node );
    PAC_critical_errors_manager::get_instance()->save_as_Lua_str( lua_buff.data(),
        err_id );
    EXPECT_STREQ( lua_buff.data(), "" );        // Нет ошибки.

    // После потери связи ошибка должна быть только одна - "нет связи",
    // а не PP mode.
    mngr.activate_error();
    mngr.read_phoenix_status_register( node );
    node->last_poll_time = get_millisec() - 
        io_manager::io_node::C_MAX_WAIT_TIME - 1;
    auto res = mngr.e_communicate( node, 1, 1 );
    EXPECT_NE( res, 0 );
    auto REF_STR_NO_CONNECTION = R"s(	{
	description = "1-1-1 : Нет связи с узлом I/O 'A100' ('127.0.0.1', 'Тест').",
	type = AT_SPECIAL,
	group = 'Авария',
	priority = 100,
	state = AS_ALARM,
	id_n = 1,
	},
)s";
    PAC_critical_errors_manager::get_instance()->save_as_Lua_str( lua_buff.data(),
        err_id );
    // Есть только ошибка связи.
    EXPECT_STREQ( lua_buff.data(), REF_STR_NO_CONNECTION );


    tcp_communicator::clear_instance();
    PAC_critical_errors_manager::get_instance()->reset_all_error();
    }

// Test that PP mode alarm flag is initialized correctly.
TEST( pp_mode_alarm, initialization )
    {
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );

    // Verify initial state.
    EXPECT_EQ( 0, node.status_register );
    EXPECT_EQ( 0, node.prev_status_register );
    EXPECT_FALSE( node.is_err_mode_alarm_set );
    }

// Test PP mode detection with non-Phoenix nodes.
TEST( pp_mode_alarm, non_phoenix_node )
    {
    io_manager::io_node wago_node( 
        io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        1, "127.0.0.1", "W100", 0, 0, 0, 0, 0, 0 );

    // Set error bits for Wago node.
    wago_node.status_register = 0x003F;
    wago_node.is_active = true;
    wago_node.state = io_manager::io_node::ST_OK;

    G_PAC_INFO()->emulation_off();
    // Wago nodes should not trigger PP mode warnings.
    EXPECT_EQ( io_manager::io_node::DISPLAY_STATES::DST_OK,
        wago_node.get_display_state() );
    G_PAC_INFO()->emulation_on();
    }

// Test disconnect resets PP mode alarm.
TEST( pp_mode_alarm, disconnect_resets_alarm )
    {
    uni_io_manager mngr;
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 0, 0, 0, 0, 0, 0 );
    
    // Set PP mode alarm active.
    node.is_err_mode_alarm_set = true;
    node.state = io_manager::io_node::ST_OK;
    
    // Disconnect should reset the alarm.
    mngr.disconnect( &node );
    
    EXPECT_FALSE( node.is_err_mode_alarm_set );
    EXPECT_EQ( io_manager::io_node::ST_NO_CONNECT, node.state );
    }

// Test disconnect when alarm is not set.
TEST( pp_mode_alarm, disconnect_no_alarm )
    {
    EXPECT_FALSE( PAC_critical_errors_manager::get_instance()->is_any_error() );

    tcp_communicator::init_instance( "Тест", "Test" );

    G_IO_MANAGER()->init( 1 );
    G_IO_MANAGER()->add_node( 0,
        io_manager::io_node::TYPES::PHOENIX_BK_ETH, 1, "127.0.0.1", "A100",
        0, 0, 0, 0, 0, 0 );
    auto node = G_IO_MANAGER()->get_node( 0 );
   
    // No PP mode alarm active.
    node->is_err_mode_alarm_set = false;
    node->state = io_manager::io_node::ST_OK;
    test_uni_io_manager1 mngr;
    mngr.activate_error();
    mngr.read_phoenix_status_register( node );
    
    EXPECT_TRUE( PAC_critical_errors_manager::get_instance()->is_any_error() );

    // Disconnect should not crash when no alarm is set.
    mngr.disconnect( node );
    
    EXPECT_FALSE( node->is_err_mode_alarm_set );
    EXPECT_EQ( io_manager::io_node::ST_NO_CONNECT, node->state );

    EXPECT_FALSE( PAC_critical_errors_manager::get_instance()->is_any_error() );

    tcp_communicator::clear_instance();
    }
