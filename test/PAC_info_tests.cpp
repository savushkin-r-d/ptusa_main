#include "PAC_info_tests.h"
#include "bus_coupler_io.h"
#include "OPCUAServer.h"

// Мок для G_OPCUA_SERVER.
class MockOPCUAServer : public OPCUA_server
    {
    public:
        MOCK_METHOD( UA_StatusCode, init_all_and_start, ( ), ( override ) );
        MOCK_METHOD( void, shutdown, ( ), ( override ) );

        MockOPCUAServer()
            {
            instance = this;
            }

        ~MockOPCUAServer() final
            {
            if ( instance == this )
                {
                instance = nullptr;
                }
            }

        static OPCUA_server& get_mock_instance()
            {
            return *instance;
            }

    private:
        inline static thread_local MockOPCUAServer* instance{ nullptr };
    };

TEST( PAC_info, OPCUA_server_start_fail )
    {
    MockOPCUAServer mockServer;

    auto get_OPC_hook = subhook_new(
        reinterpret_cast<void*>( &OPCUA_server::get_instance ),
        reinterpret_cast<void*>( &MockOPCUAServer::get_mock_instance ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( get_OPC_hook );

    // Подготовка: сервер вернёт ошибку.
    EXPECT_CALL( mockServer, init_all_and_start() )
        .WillOnce( ::testing::Return( 0xBADF00D ) );
    EXPECT_CALL( mockServer, shutdown() );

    // Установим начальное значение параметра.
    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] = 0;
    // Активируем сервер (будет ошибка).
    auto ret = G_PAC_INFO()->set_cmd( "P_IS_OPC_UA_SERVER_ACTIVE", 0, 1 );
    EXPECT_EQ( ret, 1 );

    subhook_remove( get_OPC_hook );
    subhook_free( get_OPC_hook );
    }

TEST( PAC_info, set_cmd )
    {
    G_PAC_INFO()->set_cmd( "CMD", 0, PAC_info::RELOAD_RESTRICTIONS );

    G_PAC_INFO()->set_cmd( "P_IS_OPC_UA_SERVER_ACTIVE", 0, 1 );
    EXPECT_EQ( 1, G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] );
    G_PAC_INFO()->set_cmd( "P_IS_OPC_UA_SERVER_ACTIVE", 0, 0 );
    EXPECT_EQ( 0, G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] );

    // Нет 0-го узла, ничего не должно произойти.
    EXPECT_EQ( 0, G_PAC_INFO()->set_cmd( "NODEENABLED", 0, 1 ) );

    const int ZERO_SIZE = 0;
    const int ZERO_COUNT = 0;

    G_IO_MANAGER()->init( 1 );
    G_IO_MANAGER()->add_node( 0,
        io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
        "Axxx", ZERO_COUNT, ZERO_COUNT,
        ZERO_COUNT, ZERO_SIZE, ZERO_COUNT, ZERO_SIZE );

    // Есть 0-й узел.
    EXPECT_EQ( 0, G_PAC_INFO()->set_cmd( "NODEENABLED", 1, 0 ) );
    EXPECT_FALSE( G_IO_MANAGER()->get_node( 0 )->is_active );
    EXPECT_EQ( 0, G_PAC_INFO()->set_cmd( "NODEENABLED", 1, 1 ) );
    EXPECT_TRUE( G_IO_MANAGER()->get_node( 0 )->is_active );
    }

TEST( PAC_info, reset_params )
    {
    G_PAC_INFO()->par[ PAC_info::P_MIX_FLIP_PERIOD ] = 100;
    EXPECT_EQ( 100, G_PAC_INFO()->par[ PAC_info::P_MIX_FLIP_PERIOD ] );
    G_PAC_INFO()->reset_params();
    EXPECT_EQ( 180, G_PAC_INFO()->par[ PAC_info::P_MIX_FLIP_PERIOD ] );
    }

TEST( PAC_info, save_device )
    {
    io_manager::get_instance()->init( 1 );
    io_manager::get_instance()->add_node( 0,
        io_manager::io_node::TYPES::PHOENIX_BK_ETH, 100, "127.0.0.1", "A100",
        0, 0, 0, 0, 0, 0 );

    G_PAC_INFO()->reset_params();
    G_PAC_INFO()->set_cmd( "CMD", 0, PAC_info::CLEAR_RESULT_CMD );
    G_PAC_INFO()->set_cycle_time( 100 );
    G_PAC_INFO()->reset_uptime();
    DeltaMilliSecSubHooker::set_millisec( 0 );
    G_PAC_INFO()->eval();  // Update error indicators.

    const auto MAX_SIZE = 1000;
    const auto REF_STR =
        "t.SYSTEM = \n"
        "\t{\n"
        "\tRESET_BY=1,\n"
        "\tUP_DAYS=0,\n"
        "\tUP_HOURS=0,\n"
        "\tUP_MINS=0,\n"
        "\tUP_SECS=0,\n"
        "\tUP_TIME=\"0 дн. 0:0:0\",\n"
        "\tCYCLE_TIME=100,\n"
        "\tWASH_VALVE_SEAT_PERIOD=180,\n"
        "\tWASH_VALVE_UPPER_SEAT_TIME=2000,\n"
        "\tWASH_VALVE_LOWER_SEAT_TIME=1000,\n"
        "\tP_V_OFF_DELAY_TIME=1000,\n"
        "\tP_V_BOTTOM_ON_DELAY_TIME=1200,\n"
        "\tP_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME=50,\n"
        "\tP_MAIN_CYCLE_WARN_ANSWER_AVG_TIME=300,\n"
        "\tP_RESTRICTIONS_MODE=0,\n"
        "\tP_RESTRICTIONS_MANUAL_TIME=120000,\n"
        "\tP_AUTO_PAUSE_OPER_ON_DEV_ERR=0,\n"
        "\tCMD=0,\n"
        "\tCMD_ANSWER=\"\",\n"
        "\tVERSION=\"" PRODUCT_VERSION_FULL_STR "\",\n"
        "\tNODEENABLED = \n"
        "\t{\n"
        "\t1, \n"
        "\t},\n"
        "\tNODEST = \n"
        "\t{\n"
        "\t0, \n"
        "\t},\n"
        "\tP_IS_OPC_UA_SERVER_ACTIVE=1,\n"
        "\tP_IS_OPC_UA_SERVER_CONTROL=0,\n"
        "\tNODES_COMM_ERROR=0,\n"
        "\tWATCHDOG_ERROR=0,\n"
        "\tCOMMUN_ERROR=0,\n"
        "\t}\n";
    char buff[ MAX_SIZE ] = { 0 };

    G_PAC_INFO()->reset_uptime();
    G_PAC_INFO()->save_device( buff );
    EXPECT_STREQ( REF_STR, buff );

    const auto REF_STR_1s =
        "t.SYSTEM = \n"
            "\t{\n"
            "\tRESET_BY=1,\n"
            "\tUP_DAYS=0,\n"
            "\tUP_HOURS=0,\n"
            "\tUP_MINS=0,\n"
            "\tUP_SECS=1,\n"
            "\tUP_TIME=\"0 дн. 00:00:01\",\n"
            "\tCYCLE_TIME=100,\n"
            "\tWASH_VALVE_SEAT_PERIOD=180,\n"
            "\tWASH_VALVE_UPPER_SEAT_TIME=2000,\n"
            "\tWASH_VALVE_LOWER_SEAT_TIME=1000,\n"
            "\tP_V_OFF_DELAY_TIME=1000,\n"
            "\tP_V_BOTTOM_ON_DELAY_TIME=1200,\n"
            "\tP_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME=50,\n"
            "\tP_MAIN_CYCLE_WARN_ANSWER_AVG_TIME=300,\n"
            "\tP_RESTRICTIONS_MODE=0,\n"
            "\tP_RESTRICTIONS_MANUAL_TIME=120000,\n"
            "\tP_AUTO_PAUSE_OPER_ON_DEV_ERR=0,\n"
            "\tCMD=0,\n"
            "\tCMD_ANSWER=\"\",\n"
            "\tVERSION=\"" PRODUCT_VERSION_FULL_STR "\",\n"
            "\tNODEENABLED = \n"
            "\t{\n"
            "\t1, \n"
            "\t},\n"
            "\tNODEST = \n"
            "\t{\n"
            "\t0, \n"
            "\t},\n"
            "\tP_IS_OPC_UA_SERVER_ACTIVE=1,\n"
            "\tP_IS_OPC_UA_SERVER_CONTROL=0,\n"
            "\tNODES_COMM_ERROR=0,\n"
            "\tWATCHDOG_ERROR=0,\n"
            "\tCOMMUN_ERROR=0,\n"
            "\t}\n";
    DeltaMilliSecSubHooker::set_millisec( 1010 );
    G_PAC_INFO()->eval();
    G_PAC_INFO()->save_device( buff );
    DeltaMilliSecSubHooker::set_default_time();
    EXPECT_STREQ( REF_STR_1s, buff );
    }

TEST( PAC_info, get_name_in_Lua )
    {
    EXPECT_STREQ( "SYSTEM", G_PAC_INFO()->get_name_in_Lua() );
    }

TEST( PAC_info, get_up_time_str )
    {
    G_PAC_INFO()->reset_uptime();
    EXPECT_STREQ( "0 дн. 0:0:0", G_PAC_INFO()->get_up_time_str() );
    }

TEST( PAC_info, is_emulator )
    {
    EXPECT_TRUE( G_PAC_INFO()->is_emulator() );
    }

TEST_F( PAC_info_io_test, nodes_comm_error )
    {
    // Setup: create nodes in OK state.
    mngr.init( 2 );
    mngr.add_node( 0,
        io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
        "A100", 0, 0, 0, 0, 0, 0 );
    mngr.add_node( 1,
        io_manager::io_node::WAGO_750_XXX_ETHERNET, 2, "127.0.0.1",
        "A200", 0, 0, 0, 0, 0, 0 );

    auto node0 = mngr.get_node( 0 );
    node0->is_active = true;
    node0->state = io_manager::io_node::ST_OK;
    node0->status_register = 0;  // No PP mode.

    auto node1 = mngr.get_node( 1 );
    node1->is_active = true;
    node1->state = io_manager::io_node::ST_OK;
    G_PAC_INFO()->emulation_off();
    DeltaMilliSecSubHooker::set_millisec( 1010UL );

    G_PAC_INFO()->eval();
    EXPECT_EQ( 0, G_PAC_INFO()->get_nodes_comm_error() );
    EXPECT_EQ( 0, G_PAC_INFO()->get_commun_error() );

    node1->state = io_manager::io_node::ST_NO_CONNECT;  // Error!

    G_PAC_INFO()->eval();
    EXPECT_EQ( 1, G_PAC_INFO()->get_nodes_comm_error() );
    EXPECT_EQ( 1, G_PAC_INFO()->get_commun_error() );
    }

TEST_F( PAC_info_io_test, nodes_comm_error_pp_mode )
    {
    DeltaMilliSecSubHooker::set_millisec( 1010UL );

    // Setup: create Phoenix node in PP mode (warning state).
    mngr.init( 1 );
    mngr.add_node( 0,
        io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
        "A100", 0, 0, 0, 0, 0, 0 );

    auto node = mngr.get_node( 0 );
    node->is_active = true;
    node->state = io_manager::io_node::ST_OK;
    node->status_register = 0x0010;  // PP mode active (bit 4).
    G_PAC_INFO()->emulation_off();

    G_PAC_INFO()->eval();
    EXPECT_EQ( 1, G_PAC_INFO()->get_nodes_comm_error() );
    EXPECT_EQ( 1, G_PAC_INFO()->get_commun_error() );
    }

TEST_F( PAC_info_io_test, watchdog_error_all_ok )
    {
    // Setup: create watchdog devices with state >= 0.
    auto wd1 = new watchdog( "WD1" );
    G_DEVICE_MANAGER()->add_device( wd1, device::DT_WATCHDOG );
    wd1->set_state( 1 );  // OK state.

    auto wd2 = new watchdog( "WD2" );
    G_DEVICE_MANAGER()->add_device( wd2, device::DT_WATCHDOG );
    wd2->set_state( 0 );  // OK state.

    G_PAC_INFO()->eval();
    EXPECT_EQ( 0, G_PAC_INFO()->get_watchdog_error() );
    EXPECT_EQ( 0, G_PAC_INFO()->get_commun_error() );
    }

TEST_F( PAC_info_io_test, watchdog_error_one_has_error )
    {
    DeltaMilliSecSubHooker::set_millisec( 1010UL );

    // Setup: create watchdog devices, one with error.
    auto wd1 = new watchdog( "WD1" );
    G_DEVICE_MANAGER()->add_device( wd1, device::DT_WATCHDOG );
    wd1->set_state( 1 );  // OK state.

    auto wd2 = new watchdog( "WD2" );
    G_DEVICE_MANAGER()->add_device( wd2, device::DT_WATCHDOG );
    wd2->set_state( -1 );  // Error state!

    G_PAC_INFO()->eval();
    EXPECT_EQ( 1, G_PAC_INFO()->get_watchdog_error() );
    EXPECT_EQ( 1, G_PAC_INFO()->get_commun_error() );
    }

TEST_F( PAC_info_io_test, watchdog_error_no_watchdogs )
    {
    // No watchdog devices.
    G_PAC_INFO()->eval();
    EXPECT_EQ( 0, G_PAC_INFO()->get_watchdog_error() );
    EXPECT_EQ( 0, G_PAC_INFO()->get_commun_error() );
    }

TEST_F( PAC_info_io_test, combined_errors_both )
    {
    // Setup: create node error and watchdog error.
    mngr.init( 1 );
    mngr.add_node( 0,
        io_manager::io_node::PHOENIX_BK_ETH, 1, "127.0.0.1",
        "A100", 0, 0, 0, 0, 0, 0 );

    auto node = mngr.get_node( 0 );
    node->is_active = true;
    node->state = io_manager::io_node::ST_NO_CONNECT;

    G_DEVICE_MANAGER()->clear_io_devices();
    auto wd = new watchdog( "WD1" );
    G_DEVICE_MANAGER()->add_device( wd, device::DT_WATCHDOG );
    wd->set_state( -1 );
    G_PAC_INFO()->emulation_off();

    G_PAC_INFO()->eval();
    EXPECT_EQ( 0, G_PAC_INFO()->get_nodes_comm_error() );
    EXPECT_EQ( 0, G_PAC_INFO()->get_watchdog_error() );
    EXPECT_EQ( 0, G_PAC_INFO()->get_commun_error() );

    DeltaMilliSecSubHooker::set_millisec( 1010UL );
    G_PAC_INFO()->eval();
    EXPECT_EQ( 1, G_PAC_INFO()->get_nodes_comm_error() );
    EXPECT_EQ( 1, G_PAC_INFO()->get_watchdog_error() );
    EXPECT_EQ( 1, G_PAC_INFO()->get_commun_error() );
    }
