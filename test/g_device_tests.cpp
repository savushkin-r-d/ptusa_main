#include "g_device_tests.h"
#include "tcp_cmctr.h"
#include "lua_manager.h"
#include "device/manager.h"
#include "g_errors.h"

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

    G_DEVICE_CMMCTR->clear_devices();
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
    std::string STR_check = R"(Device communicator. Dev count = 0.
)";
    testing::internal::CaptureStdout();
    G_DEVICE_CMMCTR->clear_devices();
    G_DEVICE_CMMCTR->print();
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, STR_check );
    EXPECT_TRUE( true );
    }

TEST( tech_dev_error, save_as_Lua_str )
    {
    std::array<char, 300> buff{};
    u_int_2 err_id = 0;
    
    G_ERRORS_MANAGER->clear();
    G_ERRORS_MANAGER->save_as_Lua_str( buff.data(), err_id );
    EXPECT_STREQ( buff.data(), "" );    // Нет устройств - нет ошибок.

    G_DEVICE_MANAGER()->add_io_device(
        device::DT_V, device::DST_V_DO1_DI1_FB_OFF, "V1", "Test valve", "Gea" );
    auto v1 = G_DEVICE_MANAGER()->get_device( "V1" );
    v1->set_cmd( "ST", 0, -1 );
    G_ERRORS_MANAGER->evaluate();
    G_ERRORS_MANAGER->save_as_Lua_str( buff.data(), err_id );
    auto res_str =
        R"s({
description="V1 - обратная связь",
priority=250,
state=1,
type=4,
group="тревога",
id_n=0,
id_object_alarm_number=1,
id_type=0,
suppress=false
},
)s";
    EXPECT_STREQ( buff.data(), res_str );
    
    v1->set_cmd( "ST", 0, .0 );
    G_ERRORS_MANAGER->set_cmd( base_error::COMMANDS::C_CMD_ACCEPT, 0, 0, 0 );
    G_ERRORS_MANAGER->evaluate();
    G_ERRORS_MANAGER->save_as_Lua_str( buff.data(), err_id );   
    EXPECT_STREQ( buff.data(), "" );    // Не должно быть ошибок.

    G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT_IOLINK, "FQT1", "Test counter", "IFM" );
    auto fqt1 = G_DEVICE_MANAGER()->get_device( "FQT1" );
    fqt1->set_cmd( "ST", 0, -100 );

    G_ERRORS_MANAGER->evaluate();
    G_ERRORS_MANAGER->save_as_Lua_str( buff.data(), err_id );
    res_str =
        R"s({
description="FQT1 - IOL-устройство не подключено",
priority=250,
state=1,
type=4,
group="тревога",
id_n=1,
id_object_alarm_number=100,
id_type=7,
suppress=false
},
)s";
    EXPECT_STREQ( buff.data(), res_str );

    fqt1->set_cmd( "ST", 0, 1 );
    G_ERRORS_MANAGER->evaluate();
    G_ERRORS_MANAGER->save_as_Lua_str( buff.data(), err_id );
    res_str =
        R"s({
description="FQT1 - IOL-устройство не подключено",
priority=250,
state=2,
type=4,
group="тревога",
id_n=1,
id_object_alarm_number=100,
id_type=7,
suppress=false
},
)s";
    EXPECT_STREQ( buff.data(), res_str );


    G_DEVICE_MANAGER()->clear_io_devices();
    }
