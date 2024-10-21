#include "g_device_tests.h"
#include "tcp_cmctr.h"
#include "lua_manager.h"

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

    data[ 0 ] = device_communicator::CMD_GET_INFO_ON_CONNECT;
    G_DEVICE_CMMCTR->write_devices_states_service( cmd_size, data, out_data );
    EXPECT_EQ( 'x', out_data[ 0 ] );

    data[ 0 ] = device_communicator::CMD_GET_DEVICES;
    G_DEVICE_CMMCTR->write_devices_states_service( cmd_size, data, out_data );
    EXPECT_EQ( 'x', out_data[ 0 ] );

    data[ 0 ] = device_communicator::CMD_GET_DEVICES_STATES;
    G_DEVICE_CMMCTR->write_devices_states_service( cmd_size, data, out_data );
    EXPECT_EQ( 'x', out_data[ 0 ] );

    data[ 0 ] = device_communicator::CMD_GET_PAC_ERRORS;
    G_DEVICE_CMMCTR->write_devices_states_service( cmd_size, data, out_data );
    EXPECT_EQ( 'x', out_data[ 0 ] );

    data[ 0 ] = device_communicator::CMD_EXEC_DEVICE_COMMAND;
    G_DEVICE_CMMCTR->write_devices_states_service( cmd_size, data, out_data );
    EXPECT_EQ( 'x', out_data[ 0 ] );

    G_LUA_MANAGER->free_Lua();
    }
