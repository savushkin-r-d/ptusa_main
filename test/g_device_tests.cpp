#include "g_device_tests.h"
#include "tcp_cmctr.h"
#include "lua_manager.h"
#include "device/manager.h"

using namespace ::testing;


TEST( device_communicator, write_devices_states_service )
    {
    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );

    const int IN_BUFF_SIZE = 100;
    const int OUT_BUFF_SIZE = 1000;
    unsigned char data[ IN_BUFF_SIZE ] = { '\0' };
    auto cmd_size = 1;
    unsigned char out_data[ OUT_BUFF_SIZE ] = { '\0' };

    tcp_communicator::init_instance( "Тест", "Test" );

    device_communicator::switch_on_compression();

    data[0] = device_communicator::CMD_GET_INFO_ON_CONNECT;
    device_communicator::write_devices_states_service(0, data, out_data);
    EXPECT_EQ('\0', out_data[0]);

    data[ 0 ] = device_communicator::CMD_GET_INFO_ON_CONNECT;
    device_communicator::write_devices_states_service( cmd_size, data, out_data );
    EXPECT_EQ( 'x', out_data[ 0 ] );

    data[ 0 ] = device_communicator::CMD_GET_DEVICES;
    device_communicator::write_devices_states_service( cmd_size, data, out_data );
    EXPECT_EQ( 'x', out_data[ 0 ] );

    data[ 0 ] = device_communicator::CMD_GET_DEVICES_STATES;
    device_communicator::write_devices_states_service( cmd_size, data, out_data );
    EXPECT_EQ( 'x', out_data[ 0 ] );

    data[ 0 ] = device_communicator::CMD_GET_PAC_ERRORS;
    device_communicator::write_devices_states_service( cmd_size, data, out_data );
    EXPECT_EQ( 'x', out_data[ 0 ] );

    data[ 0 ] = device_communicator::CMD_EXEC_DEVICE_COMMAND;
    device_communicator::write_devices_states_service( cmd_size, data, out_data );
    EXPECT_EQ( 'x', out_data[ 0 ] );

    unsigned char recman_data[IN_BUFF_SIZE] = " __RECMAN[1]:set_cmd( \"hello\", 1, 2.5 )";
    recman_data[0] = device_communicator::CMD_EXEC_DEVICE_COMMAND;
    device_communicator::write_devices_states_service(cmd_size, recman_data, out_data);
    EXPECT_EQ('x', out_data[0]);

    G_LUA_MANAGER->free_Lua();
    }

TEST( device_communicator, print )
    {
    G_DEVICE_MANAGER()->clear_io_devices();

    std::string STR_check = R"(Device communicator. Dev count = 1.
[   0 ] Device manager
)";
    testing::internal::CaptureStdout();
    G_DEVICE_CMMCTR->print();
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, STR_check );
    EXPECT_TRUE( true );
    }
