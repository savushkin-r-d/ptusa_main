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
        // thread_local для изоляции при параллельном запуске тестов.
        inline static thread_local MockOPCUAServer* instance = nullptr;
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

#ifdef OPCUA
    G_PAC_INFO()->set_cmd( "P_IS_OPC_UA_SERVER_ACTIVE", 0, 1 );
    EXPECT_EQ( 1, G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] );
    G_PAC_INFO()->set_cmd( "P_IS_OPC_UA_SERVER_ACTIVE", 0, 0 );
    EXPECT_EQ( 0, G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] );
#endif
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
            "\t-1, \n"
            "\t},\n"
            "\tP_IS_OPC_UA_SERVER_ACTIVE=0,\n"
            "\tP_IS_OPC_UA_SERVER_CONTROL=0,\n"        
            "\t}\n";
    char buff[ MAX_SIZE ] = {0};

    G_PAC_INFO()->reset_uptime();
    G_PAC_INFO()->save_device( buff );
    EXPECT_STREQ( REF_STR, buff);

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
            "\t-1, \n"
            "\t},\n"
            "\tP_IS_OPC_UA_SERVER_ACTIVE=0,\n"
            "\tP_IS_OPC_UA_SERVER_CONTROL=0,\n"
            "\t}\n";
    DeltaMilliSecSubHooker::set_millisec( 1001 );
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
