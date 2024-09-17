#include "PAC_dev_tests.h"
#include "uni_bus_coupler_io.h"

using namespace ::testing;

#include <cstring>


TEST( signal_column, turn_off_blue )
    {
    class test_signal_column_iolink : public signal_column_iolink
        {
        public:
            test_signal_column_iolink( const char* dev_name ) :
                signal_column_iolink( dev_name ) {};

            bool is_blue_switched_on()
                {
                return blue.step == STEP::on || blue.step == STEP::blink_on;
                }
        };

    test_signal_column_iolink test_dev( "test_HL1" );

    test_dev.turn_on_blue();
    EXPECT_EQ( true, test_dev.is_blue_switched_on() );
    test_dev.turn_off_blue();
    EXPECT_EQ( false, test_dev.is_blue_switched_on() );
    }

TEST( signal_column, get_type_name )
    {
    signal_column_iolink test_dev( "test_HL1" );
    EXPECT_STREQ( "Сигнальная колонна", test_dev.get_type_name() );
    }


TEST( valve_iol_terminal, get_state_data )
    {
    valve_iol_terminal_DO1_DI1_on v1( "V1" );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, v1.get_valve_state() );
    }


TEST( temperature_e_analog, get_value )
    {
    temperature_e_analog t1( "T1" );
    auto v = t1.get_value();
    EXPECT_EQ( .0, v );
    }

TEST( temperature_e_analog, get_type_name )
    {
    temperature_e_analog test_dev( "test_TE1" );
    EXPECT_STREQ( "Температура", test_dev.get_type_name() );
    }


TEST( device_manager, add_io_device )
    {
    G_DEVICE_MANAGER()->clear_io_devices();
    auto dev = G_DEVICE_MANAGER()->get_device( "NO_DEVICE" );
    EXPECT_EQ( G_DEVICE_MANAGER()->get_stub_device(), dev );

    //device::DT_TE, device::DST_TE_ANALOG
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_ANALOG, "T1", "Test sensor", "T" );
    EXPECT_NE( nullptr, res );

    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    G_DEVICE_MANAGER()->save_device( buff );
    EXPECT_STREQ( 
        "t=\n"
            "\t{\n"
            "\tT1={M=0, ST=1, V=0, E=0, M_EXP=20.0, S_DEV=2.0, P_CZ=0, P_ERR_T=0, P_MIN_V=0, P_MAX_V=0},\n"
            "\t}\n",
        buff );


    //device::DST_GS, device::DST_GS_INVERSE
    auto name = std::string( "GS1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_GS, device::DST_GS, name.c_str(), "Test GS", "Firma 1" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto GS1 = GS( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( GS1 ) );

    //device::DST_GS, device::DST_GS_VIRT
    name = std::string( "GS2" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_GS, device::DST_GS_VIRT, name.c_str(), "Test GS",
        "Firma 1" );
    EXPECT_EQ( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto GS2 = GS( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( GS2 ) );

    //device::DST_GS, device::DST_GS_INVERSE
    name = std::string( "GS3" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_GS, device::DST_GS_INVERSE, name.c_str(), "Test GS",
        "Firma 1");
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto GS3 = GS( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( GS3 ) );


    //device::DT_V, device::V_IOLINK_DO1_DI2, Definox
    name = std::string( "V1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_V, device::V_IOLINK_DO1_DI2, name.c_str(), "Test valve",
        valve_iolink_shut_off_sorio::SORIO_ARTICLE.c_str() );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto Vx = V( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( Vx ) );

    //device::DT_V, device::V_IOLINK_DO1_DI2, AlfaLaval
    name = std::string( "V2" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_V, device::V_IOLINK_DO1_DI2, name.c_str(), "Test valve",
        "AlfaLaval" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    Vx = V( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( Vx ) );

    //device::DT_V, device::DST_V_MINI_FLUSHING, Test
    name = std::string( "V3" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_V, device::DST_V_MINI_FLUSHING, name.c_str(), "Test valve",
        "Test" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    Vx = V( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( Vx ) );

    //device::DT_V, device::V_IOLINK_VTUG_DO1
    name = std::string( "V4" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_V, device::V_IOLINK_VTUG_DO1, name.c_str(), "Test valve",
        "Test" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    Vx = V( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( Vx ) );

    //device::DT_V, device::V_IOLINK_VTUG_DO1_FB_OFF
    name = std::string( "V5" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_V, device::V_IOLINK_VTUG_DO1_FB_OFF, name.c_str(), "Test valve",
        "Test" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    Vx = V( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( Vx ) );

    //device::DT_V, device::V_IOLINK_VTUG_DO1_FB_ON
    name = std::string( "V6" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_V, device::V_IOLINK_VTUG_DO1_FB_ON, name.c_str(), "Test valve",
        "Test" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    Vx = V( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( Vx ) );

    //device::DT_V, device::V_IOLINK_VTUG_DO1_DI2
    name = std::string( "V7" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_V, device::V_IOLINK_VTUG_DO1_DI2, name.c_str(), "Test valve",
        "Test" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    Vx = V( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( Vx ) );

    //device::DT_V, device::V_IOL_TERMINAL_MIXPROOF_DO3
    name = std::string( "V8" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_V, device::V_IOL_TERMINAL_MIXPROOF_DO3, name.c_str(), "Test valve",
        "Test" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    Vx = V( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( Vx ) );

    //device::DT_V, device::V_IOL_TERMINAL_MIXPROOF_DO3
    name = std::string( "V9" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_V, device::V_IOL_TERMINAL_MIXPROOF_DO3_DI2, name.c_str(), "Test valve",
        "Test" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    Vx = V( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( Vx ) );

    //device::DT_V, device::DST_V_AS_MIXPROOF, AlfaLaval new V70 mixproof
    name = std::string( "V91" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_V, device::DST_V_AS_MIXPROOF, name.c_str(), "Test valve",
        "AL.9615-4002-12" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    Vx = V( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( Vx ) );
    EXPECT_EQ( dynamic_cast<valve_AS_mix_proof*>( Vx )->reverse_seat_connection, true);

    //device::DT_V, device::DST_V_AS_MIXPROOF, AlfaLaval old mixproof
    name = std::string( "V92" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_V, device::DST_V_AS_MIXPROOF, name.c_str(), "Test valve",
        "Test" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    Vx = V( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( Vx ) );
    EXPECT_EQ( dynamic_cast<valve_AS_mix_proof*>( Vx )->reverse_seat_connection, false);

    //device::DT_FQT, device::DST_FQT_IOLINK
    name = std::string( "FQT1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT_IOLINK, name.c_str(), "Test counter", "IFM");
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto FQT1 = FQT( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( FQT1 ) );

    //device::DT_FQT, device::DST_FQT
    name = std::string( "FQT2" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT, name.c_str(), "Test counter", "IFM" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto FQT2 = FQT( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( FQT2 ) );

    //device::DT_FQT, device::DST_FQT_F
    name = std::string( "FQT3" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT_F, name.c_str(), "Test counter", "IFM" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto FQT3 = FQT( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( FQT3 ) );

    //device::DT_PDS, device::DST_PDS
    name = std::string( "PDS1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_PDS, device::DST_PDS, name.c_str(), "Test sensor", "CR" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto PDS1 = PDS( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( PDS1 ) );

    //device::DT_PDS, device::DST_PDS_virt
    name = std::string( "PDS2" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_PDS, device::DST_PDS_VIRT, name.c_str(), "Test sensor", "CR" );
    EXPECT_EQ( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto PDS2 = PDS( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( PDS2 ) );

    //device::DT_PDS, --
    name = std::string( "PDS3" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_PDS, device::DST_PDS_VIRT + 1, name.c_str(), "Test sensor", "CR" );
    EXPECT_EQ( nullptr, res );

    //device::DT_TS, device::DST_TS
    name = std::string( "TS1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TS, device::DST_TS, name.c_str(), "Test sensor", "CR" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto TS1 = TS( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( TS1 ) );

    //device::DT_TS, device::DST_TS_virt
    name = std::string( "TS2" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TS, device::DST_TS_VIRT, name.c_str(), "Test sensor", "CR" );
    EXPECT_EQ( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto TS2 = TS( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( TS2 ) );

    //device::DT_TS, --
    name = std::string( "TS3" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TS, device::DST_TS_VIRT + 1, name.c_str(), "Test sensor", "CR" );
    EXPECT_EQ( nullptr, res );

    //device::DT_WT, DST_WT_RS232
    name = std::string( "WT1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_WT, device::DST_WT_RS232, name.c_str(), "Test scales", "W" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto WT1 = WT( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( WT1 ) );

    //device::DT_REGULATOR, DST_REGULATOR_PID
    name = std::string( "C1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_REGULATOR, device::DST_REGULATOR_PID, name.c_str(),
        "Test PID", "C" );
    EXPECT_EQ( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto C1 = C( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( C1 ) );

    //device::DT_REGULATOR, DST_REGULATOR_THLD
    name = std::string( "C2" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_REGULATOR, device::DST_REGULATOR_THLD, name.c_str(),
        "Test regulator", "C" );
    EXPECT_EQ( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto C2 = C( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( C2 ) );

    //device::DT_REGULATOR, --
    name = std::string( "C3" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_REGULATOR, device::DST_REGULATOR_THLD + 1, name.c_str(),
        "Test regulator", "C" );
    EXPECT_EQ( nullptr, res );
    auto C3 = C( name.c_str() );
    EXPECT_EQ( STUB(), dynamic_cast<dev_stub*>( C3 ) );

    //device::DT_WT, DST_WT_ETH
    name = std::string( "W1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_WT, device::DST_WT_ETH, name.c_str(), "Test wages", "W" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto W1 = WT( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( W1 ) );

    //device::DT_WT, DST_WT_PXC_AXL
    name = std::string( "W2" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_WT, device::DST_WT_PXC_AXL, name.c_str(), "Test wages", "W" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto W2 = WT( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( W2 ) );

    //device::DT_G, DST_G_IOL_4
    name = std::string( "G1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_G, device::DST_G_IOL_4, name.c_str(), "Test power unit", "G" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( name.c_str() );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto G1 = get_G( name.c_str() );
    EXPECT_NE( STUB(), dynamic_cast<dev_stub*>( G1 ) );
    //Добавляем устройство с несуществующим подтипом.
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_G, device::DST_G_IOL_8 + 1, name.c_str(), "Test power unit", "G" );
    EXPECT_EQ( nullptr, res );
    }

TEST( device_manager, clear_io_devices )
    {
    G_DEVICE_MANAGER()->clear_io_devices();

    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_VIRT, "T1", "Test sensor", "T" );
    ASSERT_EQ( nullptr, res );    
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(),
        G_DEVICE_MANAGER()->get_TE( "T1" ) );   //Search should find device.

    G_DEVICE_MANAGER()->clear_io_devices();
    EXPECT_EQ( G_DEVICE_MANAGER()->get_stub_device(),
        G_DEVICE_MANAGER()->get_TE( "T1" ) );   //Search shouldn't find device.
    }

TEST( device_manager, get_device )
    {
    G_DEVICE_MANAGER()->clear_io_devices();

    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_VIRT, "T1", "Test sensor", "T" );
    ASSERT_EQ( nullptr, res );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(),
        G_DEVICE_MANAGER()->get_device( 0u ) );    //Search should find device.
    EXPECT_EQ( G_DEVICE_MANAGER()->get_stub_device(),
        G_DEVICE_MANAGER()->get_device( 1 ) );    //Search shouldn't find device.
    }

TEST( device_manager, get_name_in_Lua )
    {
    ASSERT_STREQ( G_DEVICE_MANAGER()->get_name_in_Lua(), "Device manager" );
    }

TEST( device_manager, evaluate_io )
    {
    G_DEVICE_MANAGER()->clear_io_devices();

    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_VIRT, "T1", "Test sensor", "T" );
    ASSERT_EQ( nullptr, res );

    G_DEVICE_MANAGER()->evaluate_io();
    }


TEST( dev_stub, get_pump_dt )
    {
    EXPECT_EQ( .0f, STUB()->get_pump_dt() );
    }

TEST( dev_stub, get_min_flow )
    {
    EXPECT_EQ( .0f, STUB()->get_min_flow() );
    }

TEST( device, device )
    {
    device dev( nullptr, device::DEVICE_TYPE::DT_NONE,
        device::DEVICE_SUB_TYPE::DST_NONE, 0 );
    EXPECT_STREQ( dev.get_name(), "?" );
    }

TEST( device, get_type_str )
    {
    device dev1( "DEV1", device::DEVICE_TYPE::DT_NONE,
        device::DEVICE_SUB_TYPE::DST_NONE, 0 );
    EXPECT_STREQ( dev1.get_type_str(), "NONE" );

    device dev2( "DEV1", device::DEVICE_TYPE::DT_V,
        device::DEVICE_SUB_TYPE::DST_V_VIRT, 0 );
    EXPECT_STREQ( dev2.get_type_str(), "V" );
    }

TEST( device, save_device )
    {
    temperature_e_analog t1( "T1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    t1.save_device( buff, "" );
    EXPECT_STREQ( 
        "T1={M=0, ST=1, V=0, E=0, M_EXP=20.0, S_DEV=2.0, P_CZ=0, "
        "P_ERR_T=0, P_MIN_V=0, P_MAX_V=0},\n", buff );
    }

TEST( device, set_article )
    {
    temperature_e_analog T1( "T1" );
    auto IFM_TE = "IFM.TE11";
    T1.set_article( IFM_TE );
    EXPECT_STREQ( IFM_TE, T1.get_article() );
    }

TEST( device, get_type_name )
    {
    device dev1( "DEV1", device::DEVICE_TYPE::DT_NONE,
        device::DEVICE_SUB_TYPE::DST_NONE, 0 );
    EXPECT_STREQ( "???", dev1.get_type_name() );
    }

TEST( device, evaluate_io )
    {
    temperature_e_analog T1( "T1" );
    T1.evaluate_io();
    }

TEST( device, set_property )
    {
    temperature_e_analog T1( "T1" );
    T1.set_property( "site", nullptr );
    }

TEST( device, set_cmd )
    {
    device dev1( "DEV1", device::DEVICE_TYPE::DT_NONE,
        device::DEVICE_SUB_TYPE::DST_NONE, 0 );
    auto res = dev1.set_cmd( "PROPERTY", 1, "value" );
    EXPECT_EQ( res, 0 );
    }


TEST( analog_io_device, set_cmd )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    analog_io_device obj( "OBJ1", device::DEVICE_TYPE::DT_TE,
        device::DEVICE_SUB_TYPE::DST_TS, 0 );
    
    obj.set_cmd( "M_EXP", 0, 10 );
    obj.set_cmd( "S_DEV", 0, 20 );
    obj.set_cmd( "E", 0, 0 );
    obj.set_cmd( "NOT_EXIST", 0, 1 );       //Несуществующее поле.

    obj.save_device( buff, "" );
    EXPECT_STREQ( "OBJ1={M=0, ST=0, V=0, E=0, M_EXP=10.0, S_DEV=20.0},\n", buff );

    // Проверка включения ручного режима.
    obj.set_cmd( "M", 0, 1 );
    obj.save_device( buff, "" );
    EXPECT_STREQ( "OBJ1={M=1, ST=0, V=0, E=0, M_EXP=10.0, S_DEV=20.0},\n", buff );
    // Проверка отключения ручного режима.
    obj.set_cmd( "M", 0, 0 );
    obj.save_device( buff, "" );
    EXPECT_STREQ( "OBJ1={M=0, ST=0, V=0, E=0, M_EXP=10.0, S_DEV=20.0},\n", buff );
    // Проверка включения ручного режима - только на 1 должен включиться.
    obj.set_cmd( "M", 0, 100 );
    obj.save_device( buff, "" );
    EXPECT_STREQ( "OBJ1={M=0, ST=0, V=0, E=0, M_EXP=10.0, S_DEV=20.0},\n", buff );
    }


TEST( DO1, get_state )
    {
    DO1 do1( "DO1", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO );
    do1.init( 1, 0, 0, 0 );
    do1.DO_channels.char_write_values[ 0 ] = new u_char{ 0 };
    do1.DO_channels.char_read_values[ 0 ] = new u_char{ 0 };

    G_PAC_INFO()->emulation_off();
    // Когда отключена эмуляция, пишем в буфер обмена с модулями ввода\вывода.
    EXPECT_EQ( do1.get_state(), 0 );
    EXPECT_EQ( do1.DO_channels.char_write_values[ 0 ][ 0 ], 0 );
    do1.on();
    EXPECT_EQ( do1.get_state(), 1 );
    EXPECT_EQ( do1.DO_channels.char_write_values[ 0 ][ 0 ], 1 );

    G_PAC_INFO()->emulation_on();
    // Когда включена эмуляция, пишем в поле состояния объекта. Здесь буфер
    // обмена с модулями ввода\вывода не должен изменяться [ 1 ].
    EXPECT_EQ( do1.get_state(), 0 );
    do1.on();
    EXPECT_EQ( do1.get_state(), 1 );
    do1.off();
    EXPECT_EQ( do1.get_state(), 0 );
    EXPECT_EQ( do1.DO_channels.char_write_values[ 0 ][ 0 ], 1 );           //1
    }

TEST( DO1, get_type_name )
    {
    DO1 do1( "DO1", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO );
    EXPECT_STREQ( do1.get_type_name(), "Дискретный выходной сигнал" );
    }


TEST( AI1, get_max_val )
    {
    AI1 sensor( "AI1", device::DEVICE_TYPE::DT_AI,
        device::DEVICE_SUB_TYPE::DST_AI, 0 );
    EXPECT_EQ( sensor.get_max_val(), 0 );
    }

TEST( AI1, get_min_val )
    {
    AI1 sensor( "AI1", device::DEVICE_TYPE::DT_AI,
        device::DEVICE_SUB_TYPE::DST_AI, 0 );
    EXPECT_EQ( sensor.get_min_val(), 0 );
    }

TEST( AI1, get_type_name )
    {
    AI1 test_dev( "test_AI1", device::DEVICE_TYPE::DT_AI,
        device::DEVICE_SUB_TYPE::DST_AI_VIRT, 0 );
    EXPECT_STREQ( "Аналоговый входной сигнал", test_dev.get_type_name() );
    }


TEST( level, get_params_count )
    {
    level L1( "L1", device::DEVICE_SUB_TYPE::DST_LT_CONE, 1 );
    EXPECT_EQ( L1.get_params_count(), 2 );
    }

TEST( level, get_type_name )
    {
    level test_dev( "test_LT1", device::DEVICE_SUB_TYPE::DST_LT_VIRT, 0 );
    EXPECT_STREQ( "Текущий уровень", test_dev.get_type_name() );
    }


TEST( pressure_e, pressure_e )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    pressure_e P1( "P1" );

    P1.save_device( buff, "" );
    EXPECT_STREQ( "P1={M=0, ST=1, V=0, E=0, M_EXP=1.0, S_DEV=0.2, P_CZ=0, "
        "P_MIN_V=0, P_MAX_V=0},\n", buff );
    }

TEST( pressure_e, get_type_name )
    {
    pressure_e test_dev( "test_PT1" );
    EXPECT_STREQ( "Давление", test_dev.get_type_name() );
    }


TEST( analog_output, get_max_value )
    {
    analog_output A1( "AO1" );
    EXPECT_EQ( A1.get_max_value(), 0 );
    }

TEST( analog_output, get_min_value )
    {
    analog_output A1( "AO1" );
    EXPECT_EQ( A1.get_min_value(), 0 );
    }

TEST( analog_output, get_type_name )
    {
    analog_output test_dev( "test_AO1" );
    EXPECT_STREQ( "Аналоговый выходной сигнал", test_dev.get_type_name() );
    }

TEST( analog_output, get_value )
    {
    analog_output AO01( "AO1" );

    G_PAC_INFO()->emulation_on();
    EXPECT_EQ( AO01.get_value(), 0.f );

    G_PAC_INFO()->emulation_off();
    AO01.init( 0, 0, 1, 0 );
    AO01.AO_channels.int_read_values[ 0 ] = new int_2[ 1 ]{ 0 };
    AO01.AO_channels.int_write_values[ 0 ] = new int_2[ 1 ]{ 0 };  

    AO01.init_channel( io_device::IO_channels::CT_AO, 0, 0, 0 );
    uni_io_manager mngr;
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );
    mngr.init( 1 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    mngr.init_node_AO( 0, 0, 555, 0 );


    AO01.set_value( 10.f );
    EXPECT_EQ( AO01.get_value(), 10.f );

    G_PAC_INFO()->emulation_on();
    io_manager::replace_instance( prev_mngr );
    }


TEST( flow_s, flow_s )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    flow_s FS1( "FS1" );

    FS1.save_device( buff, "" );
    EXPECT_STREQ( "FS1={M=0, ST=0, P_DT=0},\n", buff );
    }

TEST( flow_s, get_type_name )
    {
    flow_s test_dev( "test_FS1" );
    EXPECT_STREQ( "Расход", test_dev.get_type_name() );
    }


TEST( DI_signal, DI_signal )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    DI_signal DI1( "DI1" );

    DI1.save_device( buff, "" );
    EXPECT_STREQ( "DI1={M=0, ST=0, P_DT=0},\n", buff );
    }

TEST( DI_signal, get_type_name )
    {
    DI_signal test_dev( "test_DI1" );
    EXPECT_STREQ( "Дискретный входной сигнал", test_dev.get_type_name() );
    }


TEST( button, button )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    button BT1( "BT1" );

    BT1.save_device( buff, "" );
    EXPECT_STREQ( "BT1={M=0, ST=0, P_DT=0},\n", buff );
    }

TEST( button, get_type_name )
    {
    button test_dev( "test_BT1" );
    EXPECT_STREQ( "Кнопка", test_dev.get_type_name() );
    }


TEST( DO_signal, DO_signal )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    DO_signal DO1( "DO1" );

    DO1.save_device( buff, "" );
    EXPECT_STREQ( "DO1={M=0, ST=0},\n", buff );
    }

TEST( DO_signal, get_type_name )
    {
    DO_signal test_dev( "test_DO1" );
    EXPECT_STREQ( "Дискретный выходной сигнал", test_dev.get_type_name() );
    }


TEST( siren, siren )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    siren S1( "S1" );

    S1.save_device( buff, "" );
    EXPECT_STREQ( "S1={M=0, ST=0},\n", buff );
    }

TEST( siren, get_type_name )
    {
    siren test_dev( "test_HA1" );
    EXPECT_STREQ( "Аварийная звуковая сигнализация", test_dev.get_type_name() );
    }


TEST( lamp, lamp )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    lamp L1( "L1" );

    L1.save_device( buff, "" );
    EXPECT_STREQ( "L1={M=0, ST=0},\n", buff );
    }

TEST( lamp, get_type_name )
    {
    lamp test_dev( "test_HA1" );
    EXPECT_STREQ( "Аварийная световая сигнализация", test_dev.get_type_name() );
    }


TEST( concentration_e, get_type_name )
    {
    concentration_e test_dev( "test_QT1", device::DEVICE_SUB_TYPE::DST_QT_VIRT );
    EXPECT_STREQ( "Концентрация", test_dev.get_type_name() );
    }


TEST( concentration_e_ok, get_state )
    {
    concentration_e_ok Q1( "Q1" );
    EXPECT_EQ( Q1.get_state(), 1 );
    }


TEST( state_s, is_active )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    state_s GS1( "GS1" );

    EXPECT_FALSE( GS1.is_active() );

    GS1.save_device( buff, "" );
    EXPECT_STREQ( "GS1={M=0, ST=0, P_DT=0},\n", buff );
    }

TEST( state_s, get_type_name )
    {
    state_s test_dev( "test_GS1" );
    EXPECT_STREQ( "Датчик положения", test_dev.get_type_name() );
    }

TEST( state_s_inverse, is_active )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    state_s_inverse GS1( "GS1" );

    EXPECT_TRUE( GS1.is_active() );
        
    GS1.save_device( buff, "" );
    EXPECT_STREQ( "GS1={M=0, ST=0, P_DT=0},\n", buff );
    }


TEST( valve, evaluate )
    {
    valve::evaluate();

    valve_mini_flushing V1( "V1" );
    const auto DELAY_TIME = 1;
    G_PAC_INFO()->par[ PAC_info::P_V_OFF_DELAY_TIME ] = DELAY_TIME;

    EXPECT_EQ( valve::V_OFF, V1.get_state() );  //Should be closed.
    V1.on();
    V1.off();
    EXPECT_EQ( valve::V_ON, V1.get_state() );   //Should be opened.
    valve::evaluate();
    sleep_ms( DELAY_TIME + DELAY_TIME );
    valve::evaluate();
    EXPECT_EQ( valve::V_OFF, V1.get_state() );  //Should be closed.
    }

TEST( valve, is_closed )
    {
    valve V1( true, true, "V1", device::DEVICE_TYPE::DT_V,
        device::DEVICE_SUB_TYPE::DST_V_DO1 );

    EXPECT_TRUE( V1.is_closed() );
    }

TEST( valve, get_fb_state )
    {
    valve V1( true, true, "V1", device::DEVICE_TYPE::DT_V,
        device::DEVICE_SUB_TYPE::DST_V_DO1 );

    EXPECT_TRUE( V1.get_fb_state() );
    }


TEST( analog_valve, get_min_value )
    {
    const analog_valve VC1( "VC1" );
    EXPECT_EQ( VC1.get_min_value(), 0 );
    }

TEST( analog_valve, get_max_value )
    {
    const analog_valve VC1( "VC1" );
    EXPECT_EQ( VC1.get_max_value(), 100 );
    }

TEST( analog_valve, get_type_name )
    {
    analog_valve test_dev( "test_VC1" );
    EXPECT_STREQ( "Управляемый клапан", test_dev.get_type_name() );
    }


TEST( valve_bottom_mix_proof, is_switching_off_finished )
    {
    valve_bottom_mix_proof V1( "V1" );
    const auto DELAY_TIME = 1;
    G_PAC_INFO()->par[ PAC_info::P_V_BOTTOM_OFF_DELAY_TIME ] = DELAY_TIME;    

    EXPECT_EQ( true, V1.is_switching_off_finished() );

    V1.on();
    EXPECT_EQ( true, V1.is_switching_off_finished() );

    V1.off();
    V1.set_mini_closing_state( true );
    EXPECT_EQ( false, V1.is_switching_off_finished() );
    sleep_ms( DELAY_TIME + DELAY_TIME );
    EXPECT_EQ( true, V1.is_switching_off_finished() );
    }


TEST( valve_mini_flushing, get_state )
    {
    valve_mini_flushing V1( "V1" );
    const auto DELAY_TIME = 1;
    G_PAC_INFO()->par[ PAC_info::P_V_OFF_DELAY_TIME ] = DELAY_TIME;

    V1.open_upper_seat();
    EXPECT_EQ( valve::V_OFF, V1.get_state() );  //No upper seat.

    V1.open_lower_seat();
    EXPECT_EQ( valve::V_LOWER_SEAT, V1.get_state() );

    V1.off();
    EXPECT_EQ( valve::V_OFF, V1.get_state() );
    }


TEST( valve_iolink_shut_off_sorio, save_device )
    {
    valve_iolink_shut_off_sorio V1( "V1" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff, "" );
    EXPECT_STREQ(
        "V1={M=0, ST=0, BLINK=0, CS=0, ERR=0, V=0.0, P_ON_TIME=0, P_FB=0},\n", buff );    
    }

TEST( valve_iolink_shut_off_sorio, evaluate_io )
    {
    valve_iolink_shut_off_sorio V1( "V1" );
    V1.init( 0, 0, 1, 1 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ] { 0 };
    V1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };
    auto buff = reinterpret_cast<char*>( V1.AI_channels.int_read_values[ 0 ] );

    
    EXPECT_EQ( 0, V1.get_value() ); //Default value.


    const int POS = 341;
    *reinterpret_cast<int*>( &V1.AI_channels.int_read_values[ 0 ][ 1 ] ) = POS;
    std::swap( buff[ 0 ], buff[ 3 ] );  //Reverse byte order to get correct int.
    std::swap( buff[ 2 ], buff[ 1 ] );
    V1.evaluate_io();    
    const int BUFF_SIZE = 100;
    char str_buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "V1={M=0, ST=0, BLINK=0, CS=0, ERR=0, V=34.1, P_ON_TIME=0, P_FB=0},\n",
        str_buff );

    V1.direct_set_value( 12.1f );
    V1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "V1={M=0, ST=0, BLINK=0, CS=0, ERR=0, V=12.1, P_ON_TIME=0, P_FB=0},\n",
        str_buff );
    }

TEST( valve_iol_terminal, direct_set_state )
    {
    valve_iol_terminal V1( "V1", device::DEVICE_SUB_TYPE::V_IOLINK_VTUG_DO1 );
    V1.init( 0, 0, 1, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.set_rt_par( 1, 1 );

    V1.direct_set_state( valve::V_ON );
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );

    V1.direct_set_state( valve::V_OFF );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.direct_set_state( valve::V_LOWER_SEAT + 10 );  //Неправильное состояние.
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    }

TEST( valve_iol_terminal_DO1, on )
    {
    valve_iol_terminal_DO1 V1( "V1" );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.init( 0, 0, 1, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.set_rt_par( 5, 1 );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.set_rt_par( 1, 1 );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    }


TEST( valve_iol_terminal_DO1_DI1_off, get_fb_state )
    {
    valve_iol_terminal_DO1_DI1_off V1( "V1" );

    V1.init( 0, 1, 1, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.DI_channels.char_read_values[ 0 ] = new u_char{ 0 };
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::ON ), 2 );

    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff, "" );
    EXPECT_STREQ( "V1={M=0, ST=0, FB_OFF_ST=1, P_ON_TIME=0, P_FB=0},\n", buff );

    EXPECT_EQ( false, V1.get_fb_state() );

    const auto WAIT_TIME = 10;
    V1.set_par( 1 /*valve::CONSTANTS::P_ON_TIME*/, 0, WAIT_TIME );
    EXPECT_EQ( true, V1.get_fb_state() );

    sleep_ms( 2 * WAIT_TIME );
    EXPECT_EQ( false, V1.get_fb_state() );

    *( V1.DI_channels.char_read_values[ 0 ] ) = 1;    
    EXPECT_EQ( true, V1.get_fb_state() );
    }

TEST( valve_iol_terminal_DO1_DI1_off, get_on_fb_value )
    {
    valve_iol_terminal_DO1_DI1_off V1( "V1" );
    EXPECT_EQ( false, V1.get_on_fb_value() );
    }


TEST( valve_iol_terminal_DO1_DI1_on, get_fb_state )
    {
    valve_iol_terminal_DO1_DI1_on V1( "V1" );

    V1.init( 0, 1, 1, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.DI_channels.char_read_values[ 0 ] = new u_char{ 0 };
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::ON ), 2 );

    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff, "" );
    EXPECT_STREQ( "V1={M=0, ST=0, FB_ON_ST=1, P_ON_TIME=0, P_FB=0},\n", buff );

    EXPECT_EQ( true, V1.get_fb_state() );

    *( V1.DI_channels.char_read_values[ 0 ] ) = 1;
    const auto WAIT_TIME = 10;
    V1.set_par( 1 /*valve::CONSTANTS::P_ON_TIME*/, 0, WAIT_TIME );
    EXPECT_EQ( true, V1.get_fb_state() );
    sleep_ms( 2 * WAIT_TIME );
    EXPECT_EQ( false, V1.get_fb_state() );

    V1.on();
    EXPECT_EQ( true, V1.get_fb_state() );
    }

TEST( valve_iol_terminal_DO1_DI1_on, get_off_fb_value )
    {
    valve_iol_terminal_DO1_DI1_on V1( "V1" );
    EXPECT_EQ( false, V1.get_off_fb_value() );
    }


TEST( valve_iol_terminal_mixproof_DO3, on )
    {
    valve_iol_terminal_mixproof_DO3 V1( "V1" );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.init( 0, 0, 3, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };    
    V1.AO_channels.int_write_values[ 1 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 2 ] = new int_2[ 2 ]{ 0 };
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.set_rt_par( 5, 1 );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.set_rt_par( 1, 1 );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.set_rt_par( 2, 2 );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.set_rt_par( 3, 3 );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    }

TEST( valve_iol_terminal_mixproof_DO3, off )
    {
    valve_iol_terminal_mixproof_DO3 V1( "V1" );
    V1.init( 0, 0, 3, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 1 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 2 ] = new int_2[ 2 ]{ 0 };
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::ON ), 1 );
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::UPPER_SEAT ), 2 );
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::LOWER_SEAT ), 3 );

    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff, "" );
    EXPECT_STREQ( "V1={M=0, ST=0},\n", buff );

    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    V1.instant_off();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );
    V1.open_upper_seat();
    EXPECT_EQ( valve::VALVE_STATE::V_UPPER_SEAT, V1.get_valve_state() );
    V1.instant_off();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );
    V1.open_lower_seat();
    EXPECT_EQ( valve::VALVE_STATE::V_LOWER_SEAT, V1.get_valve_state() );
    V1.instant_off();
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.open_upper_seat();
    EXPECT_EQ( valve::VALVE_STATE::V_UPPER_SEAT, V1.get_valve_state() );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    V1.open_lower_seat();
    EXPECT_EQ( valve::VALVE_STATE::V_LOWER_SEAT, V1.get_valve_state() );
    V1.on();
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    }

TEST( valve_iol_terminal_mixproof_DO3, direct_set_state )
    {
    valve_iol_terminal_mixproof_DO3 V1( "V1" );
    V1.init( 0, 0, 3, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 1 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 2 ] = new int_2[ 2 ]{ 0 };
    V1.set_rt_par( 1, 1 );
    V1.set_rt_par( 2, 2 );
    V1.set_rt_par( 3, 3 );

    V1.direct_set_state( valve::V_ON );
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );

    V1.direct_set_state( valve::V_UPPER_SEAT );
    EXPECT_EQ( valve::VALVE_STATE::V_UPPER_SEAT, V1.get_valve_state() );

    V1.direct_set_state( valve::V_LOWER_SEAT );
    EXPECT_EQ( valve::VALVE_STATE::V_LOWER_SEAT, V1.get_valve_state() );

    V1.direct_set_state( valve::V_OFF );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, V1.get_valve_state() );

    V1.direct_set_state( valve::V_LOWER_SEAT + 10 );  //Неправильное состояние.
    EXPECT_EQ( valve::VALVE_STATE::V_ON, V1.get_valve_state() );
    }

TEST( valve_iol_terminal_mixproof_DO3_DI2, get_fb_state )
    {
    valve_iol_terminal_mixproof_DO3_DI2 V1( "V1" );

    V1.init( 0, 2, 3, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 1 ] = new int_2[ 2 ]{ 0 };
    V1.AO_channels.int_write_values[ 2 ] = new int_2[ 2 ]{ 0 };
    V1.DI_channels.char_read_values[ 0 ] = new u_char{ 0 };
    V1.DI_channels.char_read_values[ 1 ] = new u_char{ 0 };

    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::ON ), 1 );
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::UPPER_SEAT ), 1 );
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::LOWER_SEAT ), 1 );

    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff, "" );
    EXPECT_STREQ( "V1={M=0, ST=0, FB_ON_ST=1, FB_OFF_ST=1, P_ON_TIME=0, P_FB=0},\n", buff );

    const auto WAIT_TIME = 10;
    V1.set_par( 1 /*valve::CONSTANTS::P_ON_TIME*/, 0, WAIT_TIME );
    //Не прошёл заданый интервал - должно вернуться "истина".
    EXPECT_EQ( true, V1.get_fb_state() );

    sleep_ms( 2 * WAIT_TIME );
    //Прошёл заданый интервал - должно вернуться "ложь".
    EXPECT_EQ( false, V1.get_fb_state() );

    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 0 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 1 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 2 ][ 0 ] );//Все биты 0.
    V1.direct_on();
    EXPECT_EQ( 0b1, V1.AO_channels.int_write_values[ 0 ][ 0 ] );//Бит включения 1.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 1 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 2 ][ 0 ] );//Все биты 0.
    *( V1.DI_channels.char_read_values[ 0 ] ) = 1;
    //Есть обратная связь - должно вернуться "истина".
    EXPECT_EQ( true, V1.get_fb_state() );

    V1.direct_off();
    *( V1.DI_channels.char_read_values[ 0 ] ) = 0;
    *( V1.DI_channels.char_read_values[ 1 ] ) = 1;
    //Есть обратная связь - должно вернуться "истина".
    EXPECT_EQ( true, V1.get_fb_state() );

    *( V1.DI_channels.char_read_values[ 0 ] ) = 0;
    *( V1.DI_channels.char_read_values[ 1 ] ) = 0;
    V1.open_upper_seat();
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 0 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b1,
        V1.AO_channels.int_write_values[ 1 ][ 0 ] );//Бит включения верхнего седла 1.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 2 ][ 0 ] );//Все биты 0.
    
    EXPECT_EQ( true,
        V1.get_fb_state() ); //Открыто верхнее седло - должно вернуться "истина".
    V1.open_lower_seat();
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 0 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 1 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b1,
        V1.AO_channels.int_write_values[ 2 ][ 0 ] );//Бит включения нижнего седла 1.    
    EXPECT_EQ( true,
        V1.get_fb_state() ); //Открыто нижнее седло - должно вернуться "истина".

    V1.direct_off();
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 0 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 1 ][ 0 ] );//Все биты 0.
    EXPECT_EQ( 0b0, V1.AO_channels.int_write_values[ 2 ][ 0 ] );//Все биты 0.
    }


TEST( valve_iol_terminal_DO1_DI2, get_fb_state )
    {
    valve_iol_terminal_DO1_DI2 V1( "V1" );

    V1.init( 0, 2, 1, 0 );
    V1.AO_channels.int_write_values[ 0 ] = new int_2[ 2 ]{ 0 };
    V1.DI_channels.char_read_values[ 0 ] = new u_char{ 0 };
    V1.DI_channels.char_read_values[ 1 ] = new u_char{ 0 };
    V1.set_rt_par(
        static_cast<u_int>( valve_iol_terminal::TERMINAL_OUTPUT::ON ), 2 );

    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    V1.save_device( buff, "" );
    EXPECT_STREQ( "V1={M=0, ST=0, FB_ON_ST=1, FB_OFF_ST=1, P_ON_TIME=0, P_FB=0},\n", buff );

    EXPECT_EQ( false, V1.get_fb_state() );

    *( V1.DI_channels.char_read_values[ 1 ] ) = 1;//ОС для отключенного состояния.
    EXPECT_EQ( true, V1.get_fb_state() );

    *( V1.DI_channels.char_read_values[ 0 ] ) = 1;//ОС для включенного состояния.
    EXPECT_EQ( false, V1.get_fb_state() );

    *( V1.DI_channels.char_read_values[ 1 ] ) = 0;//ОС для отключенного состояния.
    V1.direct_on();
    EXPECT_EQ( true, V1.get_fb_state() );
    }

TEST( valve_DO2, valve_DO2 )
    {
    valve_DO2 V2( "V2" );
    EXPECT_STREQ( "Клапан", V2.get_type_name() );
    EXPECT_STREQ( "V2", V2.get_name());
    }

TEST( valve_DO2, get_valve_state )
    {
    valve_DO2 V2( "V2" );
    V2.direct_on();
    EXPECT_EQ( 1, V2.get_valve_state() );
    }

TEST( valve_DO2, get_fb_state )
    {
    valve_DO2 V2( "V2" );
    EXPECT_EQ( true, V2.get_fb_state() );
    }


TEST( valve_AS_DO1_DI2, direct_set_state )
    {
    valve_AS_DO1_DI2 v1( "V1" );
    EXPECT_EQ( valve::VALVE_STATE::V_OFF, v1.get_state() );
    v1.direct_set_state( 1 );
    EXPECT_EQ( valve::VALVE_STATE::V_ON,v1.get_state() );
    }


TEST( level_s, is_active )
    {
    level_s LS1( "LS1", device::DST_LS_MAX );    
    EXPECT_EQ( true, LS1.is_active() );

    level_s LS2( "LS2", device::DST_LS_MIN );
    EXPECT_EQ( false, LS2.is_active() );

    level_s LS3( "LS3", device::DST_NONE );     //Unknown sub type.
    EXPECT_EQ( false, LS3.is_active() );
    }

TEST( level_s, get_type_name )
    {
    level_s test_dev( "test_LS1", device::DEVICE_SUB_TYPE::DST_LS_VIRT );
    EXPECT_STREQ( "Уровень", test_dev.get_type_name() );
    }


TEST( level_s_iolink, set_article )
    {
    level_s_iolink LS1( "LS1", device::LS_IOLINK_MAX );
    LS1.set_article( "IFM.LMT100" );
    EXPECT_EQ( false, LS1.is_active() );
    }


TEST( i_motor, i_motor )
    {
    i_motor M1( "M1", device::DST_M_FREQ, 0 );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    M1.save_device( buff, "" );
    EXPECT_STREQ( "M1={M=0, ST=0, V=0},\n", buff );
    }

TEST( i_motor, reverse )
    {
    i_motor M1( "M1", device::DST_M_FREQ, 0 );
    M1.reverse();
    EXPECT_EQ( M1.get_state(), 2 );
    }

TEST( i_motor, get_linear_speed )
    {
    i_motor M1( "M1", device::DST_M_FREQ, 0 );
    EXPECT_EQ( M1.get_linear_speed(), 0.f );
    }

TEST( i_motor, get_amperage )
    {
    i_motor M1( "M1", device::DST_M_FREQ, 0 );
    EXPECT_EQ( M1.get_amperage(), 0.f );
    }


TEST( motor, direct_set_state )
    {
    motor M1( "M1", device::DST_M_FREQ );
    M1.set_state( 1 );
    EXPECT_EQ( M1.get_state(), 1 );
    }

TEST( motor, save_device )
    {
    motor M1( "M1", device::DST_M_FREQ );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    M1.save_device( buff, "" );
    EXPECT_STREQ( "M1={M=0, ST=0, V=0, R=0, ERRT=0, P_ON_TIME=0},\n", buff );
    }

TEST( motor, get_type_name )
    {
    motor test_dev( "test_M1", device::DST_M_VIRT );
    EXPECT_STREQ( "Двигатель", test_dev.get_type_name() );
    }


TEST( motor_altivar, set_cmd )
    {
    motor_altivar m1( "M1", device::M_ATV );    
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    m1.save_device( buff, "" );
    EXPECT_STREQ( 
        "M1={M=0, ST=0, V=0, R=0, FRQ=0.0, RPM=0, EST=0, AMP=0.0, "
        "MAX_FRQ=0.0, P_ON_TIME=0},\n", buff );

    m1.set_cmd( "R", 0, 1 );
    m1.set_cmd( "FRQ", 0, 1.1 );
    m1.set_cmd( "RPM", 0, 12 );
    m1.set_cmd( "EST", 0, 2 );
    m1.set_cmd( "AMP", 0, 23.3 );
    m1.save_device( buff, "" );
    EXPECT_STREQ( 
        "M1={M=0, ST=0, V=1.10, R=1, FRQ=1.1, RPM=12, EST=2, AMP=23.3, "
        "MAX_FRQ=0.0, P_ON_TIME=0},\n", buff );
    }

TEST( motor_altivar, get_amperage )
    {
    motor_altivar_linear m1( "M1" );
    EXPECT_EQ( .0f, m1.get_amperage( ) );

    m1.set_cmd( "AMP", 0, 25.7 );
    EXPECT_EQ( 25.7f, m1.get_amperage( ) );
    }

TEST( motor_altivar, set_string_property )
    {
    motor_altivar_linear M1( "M1" );
    EXPECT_EQ( nullptr, M1.get_atv() );
    M1.set_string_property( "IP", "127.0.0.1" );
    EXPECT_NE( nullptr, M1.get_atv() );
    }

TEST( motor_altivar_linear, get_linear_speed )
    {
    motor_altivar_linear m1( "M1" );
    EXPECT_EQ( .0f, m1.get_linear_speed() );

    m1.set_cmd( "RPM", 0, 100 );
    m1.set_cmd( "P_SHAFT_DIAMETER", 0, 2 );
    m1.set_cmd( "P_TRANSFER_RATIO", 0, 5 );    
    EXPECT_EQ( 2.09439516f, m1.get_linear_speed() );
    }


TEST( camera, is_ready )
    {
    camera c1( "C1", device::DST_CAM_DO1_DI1 );
    auto v = c1.is_ready();
    EXPECT_TRUE( v );

    camera_DI2 c2( "C2", device::DST_CAM_DO1_DI2 );
    v = c2.is_ready();
    EXPECT_FALSE( v );

    camera_DI3 c3( "C3" );
    v = c3.is_ready();
    EXPECT_FALSE( v );
    }

TEST( camera, get_type_name )
    {
    camera test_dev( "test_CAM1", device::DST_CAM_DO1_DI1 );
    EXPECT_STREQ( "Камера", test_dev.get_type_name() );
    }


TEST( counter_f, get_state )
    {
    counter_f fqt1( "FQT1" );
    EXPECT_EQ( (int) i_counter::STATES::S_WORK, fqt1.get_state() );

    //Малый расход - но счетчик меняет показания - нет ошибки.
    fqt1.set_cmd( "F", 0, 1 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );
    fqt1.set_cmd( "ABS_V", 0, 100 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    //Малый расход - ошибка должна появиться, даже при отсутствии мотора.
    //Не прошло заданное время.
    fqt1.set_cmd( "P_DT", 0, 1000 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );
    //Прошло заданное время.
    fqt1.set_cmd( "P_DT", 0, 0 );
    EXPECT_EQ( (int)i_counter::STATES::S_ERROR, fqt1.get_state() );

    //В состоянии паузы ошибки не должно быть.
    fqt1.pause();
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );
    fqt1.start();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );


    //Далее проверяем на ошибки при наличии привязанного мотора.
    motor m1( "M1", device::DST_M_FREQ );
    fqt1.set_property( "M", &m1 );
    EXPECT_EQ( (int) i_counter::STATES::S_WORK, fqt1.get_state() );

    //Расход ниже минимального - ошибка не должна появиться.
    fqt1.set_cmd( "F", 0, 0 );
    m1.on();    
    fqt1.get_state();
    EXPECT_EQ( (int) i_counter::STATES::S_WORK, fqt1.get_state() );

    //Устанавливаем расход - ошибка должна появиться.
    fqt1.set_cmd( "F", 0, 1 );
    fqt1.get_state();
    EXPECT_EQ( (int)i_counter::STATES::S_ERROR, fqt1.get_state() );

    fqt1.start();
    //Расход стал ниже минимального - ошибка не должна появиться.
    fqt1.set_cmd( "P_ERR_MIN_FLOW", 0, 2 );
    fqt1.get_state();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.set_cmd( "P_ERR_MIN_FLOW", 0, 0 );
    fqt1.get_state();
    fqt1.set_cmd( "ABS_V", 0, 200 );
    EXPECT_EQ( (int) i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.pause();
    EXPECT_EQ( (int) i_counter::STATES::S_PAUSE, fqt1.get_state() );

    fqt1.start();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );
    }

TEST( counter_f, set_cmd )
    {
    counter_f fqt1( "FQT1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    EXPECT_EQ( 0, fqt1.get_quantity() );
    EXPECT_EQ( 0, fqt1.get_abs_quantity() );
    EXPECT_EQ( 0, fqt1.get_value() );
    EXPECT_EQ( 0, fqt1.get_flow() );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.set_cmd( "V", 0, 50 );
    EXPECT_EQ( 50, fqt1.get_quantity() );
    EXPECT_EQ( 50.f, fqt1.get_value() );

    fqt1.set_cmd( "ABS_V", 0, 100 );
    EXPECT_EQ( 100, fqt1.get_abs_quantity() );

    fqt1.set_cmd( "F", 0, 9.9 );
    EXPECT_EQ( 9.9f, fqt1.get_flow() );

    fqt1.save_device( buff, "" );
    EXPECT_STREQ( 
        "FQT1={M=0, ST=1, V=50, ABS_V=100, F=9.90, P_MIN_FLOW=0,"
        " P_MAX_FLOW=0, P_CZ=0, P_DT=0, P_ERR_MIN_FLOW=0},\n", buff );
    }

TEST( counter_f, get_min_flow )
    {
    counter_f fqt1( "FQT1" );
    auto res = fqt1.get_min_flow();
    EXPECT_EQ( .0f, res );

    fqt1.set_cmd( "P_ERR_MIN_FLOW", 0, 1.1 );
    res = fqt1.get_min_flow();
    EXPECT_EQ( 1.1f, res );
    }

TEST( counter_f, get_error_description )
    {
    counter fqt1( "FQT1" );
    auto res = fqt1.get_error_description();        //Нет ошибок.
    EXPECT_STREQ( "нет ошибок", res );

    fqt1.set_cmd( "ST", 0, -1 );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "обратная связь", res );

    fqt1.set_cmd( "ST", 0, static_cast<int>( i_counter::STATES::S_ERROR ) );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "счет импульсов", res );
    fqt1.set_cmd( "ST", 0, static_cast<int>( i_counter::STATES::S_WORK ) );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "счет импульсов (rtn)", res );

    fqt1.set_cmd( "ST", 0, static_cast<int>( i_counter::STATES::S_LOW_ERR ) );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "канал потока (нижний предел)", res );
    fqt1.set_cmd( "ST", 0, static_cast<int>( i_counter::STATES::S_WORK ) );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "канал потока (нижний предел, rtn)", res );

    fqt1.set_cmd( "ST", 0, static_cast<int>( i_counter::STATES::S_HI_ERR ) );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "канал потока (верхний предел)", res );
    fqt1.set_cmd( "ST", 0, static_cast<int>( i_counter::STATES::S_WORK ) );
    res = fqt1.get_error_description();
    EXPECT_STREQ( "канал потока (верхний предел, rtn)", res );
    }


TEST( counter, set_cmd )
    {
    counter fqt1( "FQT1" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };

    EXPECT_EQ( 0, fqt1.get_quantity() );
    EXPECT_EQ( 0, fqt1.get_abs_quantity() );
    EXPECT_EQ( 0, fqt1.get_value() );
    EXPECT_EQ( 0, fqt1.get_flow() );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.set_cmd( "V", 0, 50 );
    EXPECT_EQ( 50, fqt1.get_quantity() );
    EXPECT_EQ( 50.f, fqt1.get_value() );

    fqt1.set_cmd( "ABS_V", 0, 100 );
    EXPECT_EQ( 100, fqt1.get_abs_quantity() );

    fqt1.save_device( buff, "" );
    EXPECT_STREQ( "FQT1={M=0, ST=1, V=50, ABS_V=100},\n", buff );

    fqt1.set_cmd( "ST", 0, 2 );
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, 1 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, -10 );
    EXPECT_EQ( (int)i_counter::STATES::S_ERROR, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, 1 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, -10 );
    EXPECT_EQ( (int)i_counter::STATES::S_ERROR, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, 2 );
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );
    }

TEST( counter, get_pump_dt )
    {
    counter fqt1( "FQT1" );
    auto res = fqt1.get_pump_dt();
    EXPECT_EQ( 0, res );
    }

TEST( counter, get_min_flow )
    {
    counter fqt1( "FQT1" );
    auto res = fqt1.get_min_flow();
    EXPECT_EQ( .0f, res );
    }

TEST( counter, get_max_raw_value )
    {
    counter fqt1( "FQT1" );
    auto res = fqt1.get_max_raw_value();
    EXPECT_EQ( USHRT_MAX, res );
    }

TEST( counter, get_type_name )
    {
    counter test_dev( "test_FQT1" );
    EXPECT_STREQ( "Счетчик", test_dev.get_type_name() );
    }


TEST( virtual_counter, get_pump_dt )
    {
    virtual_counter fqt1( "FQT1" );
    auto res = fqt1.get_pump_dt();
    EXPECT_EQ( 0, res );
    }

TEST( virtual_counter, get_min_flow )
    {
    virtual_counter fqt1( "FQT1" );
    auto res = fqt1.get_min_flow();
    EXPECT_EQ( .0f, res );
    }

TEST( virtual_counter, set_cmd )
    {
    virtual_counter fqt1( "FQT1" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };

    EXPECT_EQ( 0, fqt1.get_quantity() );
    EXPECT_EQ( 0, fqt1.get_abs_quantity() );
    EXPECT_EQ( 0, fqt1.get_value() );
    EXPECT_EQ( 0, fqt1.get_flow() );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, 2 );
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, 1 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );
    }


TEST( counter_iolink, set_cmd )
    {
    counter_iolink fqt1( "FQT1" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };

    EXPECT_EQ( 0, fqt1.get_quantity() );
    EXPECT_EQ( 0, fqt1.get_abs_quantity() );
    EXPECT_EQ( 0, fqt1.get_value() );
    EXPECT_EQ( 0, fqt1.get_temperature() );
    EXPECT_EQ( 0, fqt1.get_flow() );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );
    
    fqt1.set_cmd( "V", 0, 50 );
    EXPECT_EQ( counter_iolink::mL_in_L * 50, fqt1.get_quantity() );
    EXPECT_EQ( counter_iolink::mL_in_L * 50.f, fqt1.get_value() );
    
    fqt1.set_cmd( "ABS_V", 0, 100 );
    EXPECT_EQ( counter_iolink::mL_in_L * 100, fqt1.get_abs_quantity() );
    
    fqt1.set_cmd( "F", 0, 9.9 );
    EXPECT_EQ( 9.9f, fqt1.get_flow() );
    
    fqt1.set_cmd( "T", 0, 1.1 );
    EXPECT_EQ( 1.1f, fqt1.get_temperature() );
    
    fqt1.save_device( buff, "" );
    EXPECT_STREQ( 
        "FQT1={M=0, ST=1, V=50000, ABS_V=100000, F=9.90, T=1.1, "
        "P_CZ=0, P_DT=0, P_ERR_MIN_FLOW=0},\n", buff );

    fqt1.set_cmd( "ST", 0, 2 );
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, 1 );
    EXPECT_EQ( (int)i_counter::STATES::S_ERROR, fqt1.get_state() );
    }

TEST( counter_iolink, evaluate_io )
    {
    counter_iolink fqt1( "FQT1" );
    fqt1.init( 0, 0, 0, 1 );
    fqt1.AI_channels.int_read_values[ 0 ] = new int_2[8]{ 0 };
    auto buff = reinterpret_cast<char*>( fqt1.AI_channels.int_read_values[ 0 ] );

    *reinterpret_cast<float*>( fqt1.AI_channels.int_read_values[ 0 ] ) = 11.11f;
    std::swap( buff[ 0 ], buff[ 3 ] );  //Reverse byte order to get correct float.
    std::swap( buff[ 2 ], buff[ 1 ] );
    fqt1.evaluate_io();
    EXPECT_EQ( 0, fqt1.get_quantity() );//First read.

    *reinterpret_cast<float*>( fqt1.AI_channels.int_read_values[ 0 ] ) = 22.22f;
    std::swap( buff[ 0 ], buff[ 3 ] );  //Reverse byte order to get correct float.
    std::swap( buff[ 2 ], buff[ 1 ] );
    *reinterpret_cast<int*>( fqt1.AI_channels.int_read_values[ 0 ] + 2 ) = 22;
    *reinterpret_cast<int*>( fqt1.AI_channels.int_read_values[ 0 ] + 3 ) = 33 << 2;
    std::swap( buff[ 5 ], buff[ 4 ] );  //Reverse byte order to get correct int16.
    std::swap( buff[ 7 ], buff[ 6 ] );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 11.11, fqt1.get_quantity() );
    EXPECT_EQ( 22 * 0.01f, fqt1.get_flow() );
    EXPECT_EQ( 33 * 0.1f, fqt1.get_temperature() );
    }

TEST( counter_iolink, get_quantity )
    {
    class counter_iolink_test:public counter_iolink
        {
        float totalizer = .0f;

        public:
            counter_iolink_test( const char* dev_name ) :
                counter_iolink( dev_name ) {};

            float get_raw_value() const
                {
                return totalizer;
                };

            void set_raw_value( float totalizer )
                {
                this->totalizer = totalizer;
                };
        };

    counter_iolink_test fqt1( "FQT1" );
    auto res = fqt1.get_quantity();
    EXPECT_EQ( 0, res );
    EXPECT_EQ( 0, fqt1.get_abs_quantity() );

    fqt1.set_raw_value( 10 );
    fqt1.evaluate_io();
    res = fqt1.get_quantity();
    EXPECT_EQ( 0, res );
    EXPECT_EQ( 0, fqt1.get_abs_quantity() );

    fqt1.set_raw_value( 20 );
    fqt1.evaluate_io();
    res = fqt1.get_quantity();
    EXPECT_EQ( counter_iolink::mL_in_L * 10, res );
    EXPECT_EQ( counter_iolink::mL_in_L * 10, fqt1.get_abs_quantity() );


    fqt1.off();
    EXPECT_EQ( counter_iolink::mL_in_L * 10, res );
    EXPECT_EQ( counter_iolink::mL_in_L * 10, fqt1.get_abs_quantity() );

    fqt1.pause();
    fqt1.set_raw_value( 30 );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 10, fqt1.get_quantity() );
    EXPECT_EQ( counter_iolink::mL_in_L * 20, fqt1.get_abs_quantity() );

    fqt1.on();
    fqt1.set_raw_value( 40 );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 20, fqt1.get_quantity() );
    EXPECT_EQ( counter_iolink::mL_in_L * 30, fqt1.get_abs_quantity() );


    fqt1.set_state( 0 );        //Off.
    EXPECT_EQ( counter_iolink::mL_in_L * 20, fqt1.get_quantity() );
    EXPECT_EQ( counter_iolink::mL_in_L * 30, fqt1.get_abs_quantity() );

    fqt1.set_state( 2 );        //Pause.
    fqt1.set_raw_value( 50 );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 20, fqt1.get_quantity() );
    EXPECT_EQ( counter_iolink::mL_in_L * 40, fqt1.get_abs_quantity() );

    fqt1.set_state( 1 );        //Start
    fqt1.set_raw_value( 60 );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 40, fqt1.get_quantity() );
    EXPECT_EQ( counter_iolink::mL_in_L * 50, fqt1.get_abs_quantity() );


    //Test reset to 0 physical counter after its power reboot.
    fqt1.set_raw_value( 10 );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 50, fqt1.get_quantity() );

    //Test physical counter overflow.
    fqt1.set_raw_value( fqt1.get_max_raw_value() - 10 );
    fqt1.evaluate_io();
    fqt1.get_quantity();
    fqt1.reset();
    fqt1.set_raw_value( 10 );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 10, fqt1.get_quantity() );

    fqt1.pause();
    fqt1.restart();
    EXPECT_EQ( 0, fqt1.get_quantity() );


    fqt1.abs_reset();
    EXPECT_EQ( 0, fqt1.get_abs_quantity() );
    }

TEST( counter_iolink, get_pump_dt )
    {
    counter_iolink fqt1( "FQT1" );
    auto res = fqt1.get_pump_dt();
    EXPECT_EQ( 0, res );

    fqt1.set_cmd( "P_DT", 0, 11 );
    res = fqt1.get_pump_dt();
    EXPECT_EQ( 11, res );
    }

TEST( counter_iolink, get_min_flow )
    {
    counter_iolink fqt1( "FQT1" );
    auto res = fqt1.get_min_flow();
    EXPECT_EQ( .0f, res );

    fqt1.set_cmd( "P_ERR_MIN_FLOW", 0, 1.1 );
    res = fqt1.get_min_flow();
    EXPECT_EQ( 1.1f, res );
    }


TEST( wages_RS232, get_value_from_wages )
    {
    wages_RS232 w1( "W1" );
    w1.init( 0, 0, 1, 1 );

    //1 - Тест на пустой указатель
    //2, 3 - Данные корректные
    //4, 5 - Некорректные данные, возвращает 0
    //6 - Если в первом байте 1, буффер не пустой. Переключение в режим
    // чтения данных. Возвращает старое значение (в данном случае 0)
    //7 - Если в первом байте 0, буффер пустой. Вернуть старое значение 
    //(в данном случае 0).
    w1.AI_channels.int_read_values[ 0 ] = nullptr;                         //1
    EXPECT_EQ( 0.0f, w1.get_value_from_wages() );
    EXPECT_EQ( -1, w1.get_state() );

    char tmp_str[] = "   +0000.00k";                                       //2
    std::swap( tmp_str[ 2 ], tmp_str[ 3 ] );
    std::swap( tmp_str[ 5 ], tmp_str[ 4 ] );
    std::swap( tmp_str[ 6 ], tmp_str[ 7 ] );
    std::swap( tmp_str[ 8 ], tmp_str[ 9 ] );
    std::swap( tmp_str[ 10 ], tmp_str[ 11 ] );
    w1.AI_channels.int_read_values[ 0 ] = reinterpret_cast<int_2*>( tmp_str );
    EXPECT_EQ( 0.0f, w1.get_value_from_wages() );
    EXPECT_EQ( 1, w1.get_state() );

    strcpy( tmp_str, "   +0012.34k" );                                     //3   
    std::swap( tmp_str[ 2 ], tmp_str[ 3 ] );
    std::swap( tmp_str[ 5 ], tmp_str[ 4 ] );
    std::swap( tmp_str[ 6 ], tmp_str[ 7 ] );
    std::swap( tmp_str[ 8 ], tmp_str[ 9 ] );
    std::swap( tmp_str[ 10 ], tmp_str[ 11 ] );
    w1.AI_channels.int_read_values[ 0 ] = reinterpret_cast<int_2*>( tmp_str );
    EXPECT_EQ( 12.34f, w1.get_value_from_wages() );
    EXPECT_EQ( 1, w1.get_state() );

    strcpy( tmp_str, "  +12.34k   " );                                     //4
    std::swap( tmp_str[ 2 ], tmp_str[ 3 ] );
    std::swap( tmp_str[ 5 ], tmp_str[ 4 ] );
    std::swap( tmp_str[ 6 ], tmp_str[ 7 ] );
    std::swap( tmp_str[ 8 ], tmp_str[ 9 ] );
    std::swap( tmp_str[ 10 ], tmp_str[ 11 ] );
    w1.AI_channels.int_read_values[ 0 ] = reinterpret_cast<int_2*>( tmp_str );
    EXPECT_EQ( 0.0f, w1.get_value_from_wages() );
    EXPECT_EQ( -1, w1.get_state() );

    strcpy( tmp_str, "   -0001.34k" );                                     //5
    std::swap( tmp_str[ 2 ], tmp_str[ 3 ] );
    std::swap( tmp_str[ 5 ], tmp_str[ 4 ] );
    std::swap( tmp_str[ 6 ], tmp_str[ 7 ] );
    std::swap( tmp_str[ 8 ], tmp_str[ 9 ] );
    std::swap( tmp_str[ 10 ], tmp_str[ 11 ] );
    w1.AI_channels.int_read_values[ 0 ] = reinterpret_cast<int_2*>( tmp_str );
    EXPECT_EQ( 0.0f, w1.get_value_from_wages() );
    EXPECT_EQ( -1, w1.get_state() );

    w1.AI_channels.int_read_values[ 0 ] =                                  //6
        new int_2[ 7 ]{ 1, 0, 0, 0, 0, 0, 0 };
    EXPECT_EQ( 0.0f, w1.get_value_from_wages() );

    w1.AI_channels.int_read_values[ 0 ] =                                  //7
        new int_2[ 7 ]{ 0, 0, 0, 0, 0, 0, 0 };
    EXPECT_EQ( 0.0f, w1.get_value_from_wages() );
    }

TEST( wages_RS232, get_value )
    {
    wages_RS232 w1( "W1" );
    w1.init( 0, 0, 1, 1 );

    char tmp_str[] = "   +0000.00k";
    std::swap( tmp_str[ 2 ], tmp_str[ 3 ] );
    std::swap( tmp_str[ 5 ], tmp_str[ 4 ] );
    std::swap( tmp_str[ 6 ], tmp_str[ 7 ] );
    std::swap( tmp_str[ 8 ], tmp_str[ 9 ] );
    std::swap( tmp_str[ 10 ], tmp_str[ 11 ] );
    w1.AI_channels.int_read_values[ 0 ] = reinterpret_cast<int_2*>( tmp_str );
    w1.evaluate_io();

    EXPECT_EQ( 0.0f, w1.get_value() );

    w1.set_par( 1, 0, 10 );                    //Установка параметра P_CZ = 10
    EXPECT_EQ( 10.0f, w1.get_value() );
    }

TEST( wages_RS232, get_state )
    {
    wages_RS232 w1( "W1" );
    w1.init( 0, 0, 1, 1 );

    w1.AI_channels.int_read_values[ 0 ] = nullptr;
    w1.get_value_from_wages();
    EXPECT_EQ( -1, w1.get_state() );

    char tmp_str[] = "   +0000.00k";
    std::swap( tmp_str[ 2 ], tmp_str[ 3 ] );
    std::swap( tmp_str[ 5 ], tmp_str[ 4 ] );
    std::swap( tmp_str[ 6 ], tmp_str[ 7 ] );
    std::swap( tmp_str[ 8 ], tmp_str[ 9 ] );
    std::swap( tmp_str[ 10 ], tmp_str[ 11 ] );
    w1.AI_channels.int_read_values[ 0 ] = reinterpret_cast<int_2*>( tmp_str );
    w1.get_value_from_wages();
    EXPECT_EQ( 1, w1.get_state() );
    }

TEST( wages_RS232, tare )
    {
    wages_RS232 w1( "W1" );
    w1.tare();
    }

TEST( wages_RS232, evaluate_io )
    {
    wages_RS232 w1( "W1" );
    w1.evaluate_io();
    }


TEST( wages, save_device )
    {
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    wages w1( "W1" );
    w1.save_device( buff, "" );
    EXPECT_STREQ(
        "W1={M=0, ST=0, V=0, W=0.000, P_NOMINAL_W=0, P_RKP=0, P_CZ=0, P_DT=0},\n",
        buff );
    }

TEST( wages, get_type_name )
    {
    wages test_dev( "test_W1" );
    EXPECT_STREQ( "Тензорезистор", test_dev.get_type_name() );
    }


TEST( wages_eth, evaluate_io )
    {
    wages_eth w1( "W1" );

    auto ip = "0.0.0.0";
    auto field = "IP";
    w1.set_string_property( field, ip );

    w1.evaluate_io();
    EXPECT_EQ( 0, w1.get_value() );

    iot_wages_eth w2( 1, "127.0.0.1", 10000 );

    w2.evaluate();
    EXPECT_EQ( 0, w2.get_wages_value() );
    }

TEST( wages_eth, get_value )
    {
    wages_eth w1( "W1" );

    auto ip = "0.0.0.0";
    auto field = "IP";
    w1.set_string_property( field, ip );

    w1.direct_set_value( 10.0f );
    EXPECT_EQ( 10, w1.get_value() );
    w1.set_par( 1, 0, 10 );
    EXPECT_EQ( 20, w1.get_value() );
    }

TEST( wages_eth, tare )
    {
    wages_eth w1( "W1" );
    w1.tare();
    }

TEST( wages_eth, get_state )
    {
    wages_eth w1( "W1" );

    auto ip = "0.0.0.0";
    auto field = "IP";
    w1.set_string_property( field, ip );

    EXPECT_EQ( 0, w1.get_state() );
    }

TEST( wages_eth, direct_set_value )
    {
    wages_eth w1( "W1" );

    auto ip = "0.0.0.0";
    auto field = "IP";
    w1.set_string_property( field, ip );
    w1.direct_set_value( 10.0f );
    EXPECT_EQ( 10, w1.get_value() );
    w1.direct_set_value( -10.0f );
    EXPECT_EQ( 10, w1.get_value() );
    }

TEST( wages_eth, direct_set_state )
    {
    wages_eth w1( "W1" );

    auto ip = "0.0.0.0";
    auto field = "IP";
    w1.set_string_property( field, ip );
    w1.direct_set_state( 1 );
    EXPECT_EQ( 1, w1.get_state() );
    w1.direct_set_state( 0 );
    EXPECT_EQ( 0, w1.get_state() );
    }

TEST( wages_eth, direct_set_tcp_buff )
    {
    wages_eth w1( "W1" );

    auto ip = "0.0.0.0";
    auto field = "IP";
    w1.set_string_property( field, ip );

    //Корректные данные.
    char new_value[] = "+91234.5kg";
    const auto SIZE = sizeof( new_value );
    w1.direct_set_tcp_buff( new_value, SIZE, 1 );
    const auto CORRECT_VALUE = 91234.5f;
    EXPECT_EQ( CORRECT_VALUE, w1.get_value() );

    //Некорректные данные. Сохраняется предыдущее значение веса.
    const auto NOT_VALID_DATA = {
        "          ", "+00000.5kt", "+00000.5zg", "+00000.5zt" };
    for ( auto str : NOT_VALID_DATA )
        {
        strcpy( new_value, str );
        w1.direct_set_tcp_buff( new_value, SIZE, 1 );
        EXPECT_EQ( 0, w1.get_state() );
        EXPECT_EQ( CORRECT_VALUE, w1.get_value() );
        }
    }

TEST( wages_eth, set_string_property )
    {
    wages_eth w1( "W1" );

    auto ip = "0.0.0.0";
    auto field = "NOT_IP";
    w1.set_string_property( field, ip );
    }


TEST( wages_pxc_axl, evaluate_io )
    {
    wages_pxc_axl w1( "W1" );
    w1.init( 0, 0, 1, 1 );
    w1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };
    auto buff = reinterpret_cast<char*>( w1.AI_channels.int_read_values[ 0 ] );

    auto par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_CZ );
    w1.set_par( par_idx, 0, 0 );
    par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_K );
    w1.set_par( par_idx, 0, 1 );

    const int VALUE = 65900;
    *reinterpret_cast<int_4*>( buff ) = VALUE;
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( 0.001f * VALUE, w1.get_value() );

    *reinterpret_cast<int_4*>( buff ) =
        static_cast<int_4>( wages_pxc_axl::ERR_VALUES::ERR_OVERRANGE );
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( -1, w1.get_state() );

    *reinterpret_cast<int_4*>( buff ) =
        static_cast<int_4>( wages_pxc_axl::ERR_VALUES::ERR_WIRE_BREAK );
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( -2, w1.get_state() );

    *reinterpret_cast<int_4*>( buff ) =
        static_cast<int_4>( wages_pxc_axl::ERR_VALUES::ERR_SHORT_CIRCUIT );
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( -3, w1.get_state() );

    *reinterpret_cast<int_4*>( buff ) =
        static_cast<int_4>( wages_pxc_axl::ERR_VALUES::ERR_INVALID_VALUE );
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( -4, w1.get_state() );

    *reinterpret_cast<int_4*>( buff ) =
        static_cast<int_4>( wages_pxc_axl::ERR_VALUES::ERR_FAULTY_SUPPLY_VOLTAGE );
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( -5, w1.get_state() );

    *reinterpret_cast<int_4*>( buff ) =
        static_cast<int_4>( wages_pxc_axl::ERR_VALUES::ERR_FAULTY_DEVICE );
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( -6, w1.get_state() );

    *reinterpret_cast<int_4*>( buff ) =
        static_cast<int_4>( wages_pxc_axl::ERR_VALUES::ERR_UNDERRANGE );
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( -7, w1.get_state() );
    }

TEST( wages_pxc_axl, tare )
    {
    wages_pxc_axl w1( "W1" );
    w1.init( 0, 0, 1, 1 );
    w1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };
    auto buff = reinterpret_cast<char*>( w1.AI_channels.int_read_values[ 0 ] );
    auto par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_CZ );
    w1.set_par( par_idx, 0, 0 );
    par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_K );
    w1.set_par( par_idx, 0, 1 );

    const int VALUE = 65900;
    *reinterpret_cast<int_4*>( buff ) = VALUE;
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( 0.001f * VALUE, w1.get_value() );

    w1.tare();
    EXPECT_EQ( .0f, w1.get_value() );
    }

TEST( wages_pxc_axl, reset_tare )
    {
    wages_pxc_axl w1( "W1" );
    w1.init( 0, 0, 1, 1 );
    w1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };
    auto buff = reinterpret_cast<char*>( w1.AI_channels.int_read_values[ 0 ] );
    auto par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_CZ );
    w1.set_par( par_idx, 0, 0 );
    par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_K );
    w1.set_par( par_idx, 0, 1 );

    const int VALUE = 65900;
    *reinterpret_cast<int_4*>( buff ) = VALUE;
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( 0.001f * VALUE, w1.get_value() );

    w1.tare();
    EXPECT_EQ( .0f, w1.get_value() );

    w1.reset_tare();
    EXPECT_EQ( 0.001f * VALUE, w1.get_value() );
    }

TEST( wages_pxc_axl, direct_set_state )
    {
    wages_pxc_axl w1( "W1" );
    w1.init( 0, 0, 1, 1 );
    w1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };
    auto buff = reinterpret_cast<char*>( w1.AI_channels.int_read_values[ 0 ] );
    auto par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_CZ );
    w1.set_par( par_idx, 0, 0 );
    par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_K );
    w1.set_par( par_idx, 0, 1 );

    const int VALUE = 65900;
    *reinterpret_cast<int_4*>( buff ) = VALUE;
    //Reverse byte order to get correct int32.
    std::swap( buff[ 0 ], buff[ 2 ] );
    std::swap( buff[ 1 ], buff[ 3 ] );
    w1.evaluate_io();
    EXPECT_EQ( 0.001f * VALUE, w1.get_value() );

    w1.direct_set_state( 1 );   // CMDS::TARE
    EXPECT_EQ( .0f, w1.get_value() );

    w1.direct_set_state( 2 );   // CMDS::RESET_TARE
    EXPECT_EQ( 0.001f * VALUE, w1.get_value() );

    w1.direct_set_state( 0 );   // No such command.
    EXPECT_EQ( 0.001f * VALUE, w1.get_value() );
    }

TEST( wages_pxc_axl, direct_set_value )
    {
    wages_pxc_axl w1( "W1" );
    w1.init( 0, 0, 1, 1 );
    w1.AI_channels.int_read_values[ 0 ] = new int_2[ 2 ]{ 0 };
    auto par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_CZ );
    w1.set_par( par_idx, 0, 0 );
    par_idx = static_cast<unsigned int>( wages_pxc_axl::CONSTANTS::P_K );
    w1.set_par( par_idx, 0, 1 );

    const int VALUE = 65900;
    w1.direct_set_value( VALUE );   // Do nothing.
    EXPECT_EQ( 0, w1.get_value() );
    }


TEST( temperature_e, save_device )
    {
    temperature_e T1( "T1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    T1.save_device( buff, "" );
    EXPECT_STREQ(
        "T1={M=0, ST=1, V=0, E=0, M_EXP=20.0, S_DEV=2.0, P_CZ=0, P_ERR_T=0},\n", buff );

    T1.set_cmd( "E", 0, 1 );
    T1.save_device( buff, "" );
    EXPECT_STRNE(
        "T1={M=0, ST=1, V=0, E=1, P_CZ=0, P_ERR_T=0},\n", buff );
    }

TEST( temperature_e, get_type_name )
    {
    temperature_e test_dev( "test_TE1" );
    EXPECT_STREQ( "Температура", test_dev.get_type_name() );
    }


TEST( threshold_regulator, set_value )
    {
    threshold_regulator TRC1( "TRC1" );
    device* dev = static_cast<device*>( &TRC1 );

    G_DEVICE_MANAGER()->clear_io_devices();
    auto TE_name = std::string( "TE1" );
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_VIRT, TE_name.c_str(), "Test sensor", "T" );
    ASSERT_EQ( nullptr, res );
    const auto TE1 = TE( TE_name.c_str() );
    ASSERT_NE( STUB(), dynamic_cast<dev_stub*>( TE1 ) );
    auto M_name = std::string( "M1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_M, device::DST_M_VIRT, M_name.c_str(), "Test motor", "M" );
    ASSERT_EQ( nullptr, res );
    const auto M1 = G_DEVICE_MANAGER()->get_M( M_name.c_str() );
    ASSERT_NE( nullptr, M1 );
    ASSERT_NE( STUB(), dynamic_cast<dev_stub*>( M1 ) );

    const auto SET_VALUE = 10;
    const auto DELTA_VALUE = 1;

    //Correct processing when we have no set sensor and actuator.
    dev->set_value( SET_VALUE );

    dev->set_string_property( "IN_VALUE", TE_name.c_str() );
    dev->set_string_property( "OUT_VALUE", M_name.c_str() );
    dev->set_cmd( "P_DELTA", 0, DELTA_VALUE );
    dev->off();

    //Regulator switched off - it should not switch on actuator regardless
    //temperature.
    dev->set_value( SET_VALUE );
    EXPECT_EQ( 0, M1->get_state() );

    dev->on();

    //Regulator switched on, temperature is below range - it should switch
    //on actuator.
    TE1->set_cmd( "V", 0, SET_VALUE / 10 );
    dev->set_value( SET_VALUE );
    EXPECT_EQ( 1, M1->get_state() );

    //Regulator switched on, temperature is above range - it should switch off
    //actuator.
    TE1->set_cmd( "V", 0, SET_VALUE * 2 );
    dev->set_value( SET_VALUE );
    EXPECT_EQ( 0, M1->get_state() );

    //Regulator switched on, temperature is inside range - it should not change
    //actuators state.
    TE1->set_cmd( "V", 0, SET_VALUE );
    dev->set_value( SET_VALUE );
    EXPECT_EQ( 0, M1->get_state() );

    dev->set_cmd( "P_is_reverse", 0, 1 );

    //Regulator switched on, temperature is above range, but reverse is on -
    //it should switch on actuators.
    TE1->set_cmd( "V", 0, SET_VALUE * 2 );
    dev->set_value( SET_VALUE );
    EXPECT_EQ( 1, M1->get_state() );

    //Regulator switched on, temperature is below range, but reverse is on -
    //it should switch on actuators.
    TE1->set_cmd( "V", 0, SET_VALUE / 10 );
    dev->set_value( SET_VALUE * 2 );
    EXPECT_EQ( 0, M1->get_state() );

    dev->set_cmd( "P_is_reverse", 0, .0 );
    dev->off();

    //Regulator switched off.    
    dev->set_value( SET_VALUE );
    EXPECT_EQ( 0, M1->get_state() );

    dev->on();

    //Use counter as a sensor.    
    auto name = std::string( "FQT1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT_VIRT, name.c_str(), "Test counter", "F" );
    ASSERT_EQ( nullptr, res );
    const auto FQT1 = virtual_FQT( name.c_str() );
    ASSERT_NE( STUB(), dynamic_cast<dev_stub*>( FQT1 ) );
    dev->set_string_property( "IN_VALUE", name.c_str() );
    
    //Regulator switched on, flow is below range - it should switch
    //on actuator.
    FQT1->set_cmd( "F", 0, SET_VALUE );
    dev->set_value( SET_VALUE * 2 );
    EXPECT_EQ( 1, M1->get_state() );
    }

TEST( threshold_regulator, set_cmd )
    {
    threshold_regulator p1( "C1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    p1.save_device( buff );
    EXPECT_STREQ(
        "\tC1={M=0, ST=0, V=0, P_is_reverse=0, P_delta=0},\n", buff );

    //Set a property that does not exist.
    auto res = p1.set_cmd( "NO_SUCH_PROPERTY", 0, 1 );
    EXPECT_EQ( 1, res );
    //Set a parameter that does not exist.
    res = p1.set_cmd( "P_NO_SUCH_PARAMETER", 0, 1 );
    EXPECT_EQ( 1, res );

    res = p1.set_cmd( "P_delta", 0, 10 );
    EXPECT_EQ( 0, res );
    res = p1.set_cmd( "P_is_reverse", 0, 1 );
    EXPECT_EQ( 0, res );
    p1.on();
    p1.save_device( buff );
    EXPECT_STREQ(
        "\tC1={M=0, ST=1, V=0, P_is_reverse=1, P_delta=10},\n", buff );
    }

TEST( threshold_regulator, set_state )
    {
    threshold_regulator p1( "C1" );

    //Switch on regulator.
    p1.set_state( 1 );
    EXPECT_EQ( 1, p1.get_state() );

    //Switch on regulator (already on).
    p1.set_state( 1 );
    EXPECT_EQ( 1, p1.get_state() );

    //Non correct new state - should be no changes.
    p1.set_state( 2 );
    EXPECT_EQ( 1, p1.get_state() );

    //Switch off regulator.
    p1.set_state( 0 );
    EXPECT_EQ( 0, p1.get_state() );
    }

TEST( threshold_regulator, get_name_in_Lua )
    {
    auto name = "C1";
    threshold_regulator p1( name );

    EXPECT_STREQ( name, p1.get_name_in_Lua() );
    }

TEST( threshold_regulator, set_string_property )
    {
    auto name = "C1";
    threshold_regulator p1( name );

    //TODO - refactor set_string_property() to return result.
    p1.set_string_property( nullptr, nullptr );
    p1.set_string_property( "IN_VALUE", "FQT1" );
    p1.set_string_property( "OUT_VALUE", "M1" );
    p1.set_string_property( "NO_SUCH_PROPERTY", "AA1" );
    }

TEST( threshold_regulator, get_type_name )
    {
    threshold_regulator test_dev( "test_C1" );
    EXPECT_STREQ( "ПИД-регулятор", test_dev.get_type_name() );
    }


TEST( par_device, get_par )
    {
    const auto IDX = 0;
    const auto OFFSET = 0;
    par_device dev1( 2 );
    dev1.set_par( IDX, OFFSET, 10.f );

    EXPECT_EQ( dev1.get_par( IDX, OFFSET ), 10.f );

    par_device dev2( 0 );
    // Корректный вызов, когда нет параметров.
    EXPECT_EQ( dev2.get_par( IDX, OFFSET ), 0.f );
    }

TEST( par_device, set_par_name )
    {
    const auto IDX = 1;
    const auto OFFSET = 0;

    par_device dev( 1 );
    dev.set_par_name( 1, OFFSET, "TEST_VERY_VERY_VERY_LONG_NAME" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    dev.save_device( buff );
    EXPECT_STREQ( "", buff );

    dev.set_par_name( IDX, OFFSET, "TEST_NAME" );
    dev.save_device( buff );
    EXPECT_STREQ( "TEST_NAME=0, ", buff );

    // Попытка повторной установки имени.
    dev.set_par_name( IDX, OFFSET, "TEST_NAME" );
    // Попытка установки имени для несуществующего индекса параметра.
    dev.set_par_name( IDX, OFFSET + 1, "TEST_NAME" );
    } 

TEST ( valve_AS, get_lower_seat_offset)
    {
    valve_AS valve( "V1", device::DST_V_AS_MIXPROOF );
    EXPECT_EQ( valve.C_OPEN_S2, valve.get_lower_seat_offset() );
    valve.reverse_seat_connection = true;
    EXPECT_EQ( valve.C_OPEN_S3, valve.get_lower_seat_offset() );
    }

TEST ( valve_AS, get_upper_seat_offset)
    {
    valve_AS valve( "V1", device::DST_V_AS_MIXPROOF );
    EXPECT_EQ( valve.C_OPEN_S3, valve.get_upper_seat_offset() );
    valve.reverse_seat_connection = true;
    EXPECT_EQ( valve.C_OPEN_S2, valve.get_upper_seat_offset() );
    }


TEST( circuit_breaker, get_type_name )
    {
    circuit_breaker F1( "F1" );
    auto res = F1.get_type_name();
    EXPECT_STREQ( "Автоматический выключатель", res );
    }


TEST( power_unit, evaluate_io )
    {
    power_unit G1( "G1" );
    G1.init( 0, 0, 1, 1 );
    G1.AO_channels.int_write_values[ 0 ] = new int_2[ 7 ]{ 0 };
    G1.AI_channels.int_read_values[ 0 ] = new int_2[ 18 ]{ 0 };

    EXPECT_EQ( 0, G1.get_value() ); //Default value.

    const int BUFF_SIZE = 1000;
    char str_buff[ BUFF_SIZE ] = { 0 };
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={0,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );

    // ST = DC_not_OK, OUT_POWER_90, VOLTAGE = 25.7
    G1.AI_channels.int_read_values[ 0 ][ 0 ] = 0b100011001;
    // SUM_CURRENTS = 0.1
    G1.AI_channels.int_read_values[ 0 ][ 1 ] = 0b100000000;
    // Status, channel 1
    G1.AI_channels.int_read_values[ 0 ][ 2 ] = 0b01000000;
    // Nominal current, channel 1
    G1.AI_channels.int_read_values[ 0 ][ 3 ] = 0b00001000;
    // G1.AI_channels.int_read_values[ 0 ][ 4 ];
    // Load current, channel 1
    G1.AI_channels.int_read_values[ 0 ][ 5 ] = 0b00001000;
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0.10, NOMINAL_CURRENT_CH={1,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.8,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={1,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.1, VOLTAGE=25.7, OUT_POWER_90=1, ERR=1},\n",
        str_buff );

    std::memset( G1.AI_channels.int_read_values[ 0 ], 0, 18 );
    std::memset( G1.AO_channels.int_write_values[ 0 ], 0, 7 );

    G1.on();
    G1.evaluate_io();
    // Управляющий бит еще не должен быть включен (все каналы устанавливаются
    // в 1).
    EXPECT_EQ( 255, G1.AO_channels.int_write_values[ 0 ][ 3 ] );

    G1.set_cmd_time( get_millisec() - G1.WAIT_DATA_TIME - 10 );
    G1.evaluate_io();
    // Управляющий бит уже должен быть включен (устанавливается управляющий
    // бит в 1).
    EXPECT_EQ( 255, G1.AO_channels.int_write_values[ 0 ][ 3 ] );
    EXPECT_EQ( 128, G1.AO_channels.int_write_values[ 0 ][ 0 ] );

    G1.set_cmd_time( get_millisec() - G1.WAIT_CMD_TIME - 10 );
    G1.evaluate_io();
    // Управляющий бит уже должен быть отключен (сбрасывается управляющий бит
    // в 0, а также, так как нет записи в реальное устройство, сбрасываются
    // все каналы в 0).
    EXPECT_EQ( 0, G1.AO_channels.int_write_values[ 0 ][ 3 ] );
    EXPECT_EQ( 0, G1.AO_channels.int_write_values[ 0 ][ 0 ] );
    }

TEST( power_unit, get_type_name )
    {
    power_unit G1( "G1" );
    auto res = G1.get_type_name();
    EXPECT_STREQ( "Блок питания", res );
    }

TEST( power_unit, set_value )
    {
    power_unit G1( "G1" );
    // Функция set_value() ничего не делает - состояние не должно измениться.
    G1.set_value( 0.f );
    EXPECT_EQ( 0, G1.get_state() );
    }

TEST( power_unit, on )
    {    
    const int BUFF_SIZE = 1000;
    char str_buff[ BUFF_SIZE ] = { 0 };
    power_unit G1( "G1" );
    G1.off();
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={0,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.on();
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={0,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={1,1,1,1,1,1,1,1}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    }

TEST( power_unit, set_cmd )
    {
    const int BUFF_SIZE = 1000;
    char str_buff[ BUFF_SIZE ] = { 0 };
    power_unit G1( "G1" );
    G1.off();

    G1.set_cmd( "ST", 0, 1 );       // On().
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={0,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={1,1,1,1,1,1,1,1}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );

    G1.set_cmd( "ST", 0, 0 );       // Off().
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={0,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );

    G1.set_cmd( "ST_CH", 1, 1 );    // Channel 1.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={0,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={1,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.off();
    G1.set_cmd( "ST_CH", 2, 1 );    // Channel 2.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={0,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,1,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.off();
    G1.set_cmd( "ST_CH", 3, 1 );    // Channel 3.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={0,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,1,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.off();
    G1.set_cmd( "ST_CH", 4, 1 );    // Channel 4.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={0,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,1,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.off();
    G1.set_cmd( "ST_CH", 5, 1 );    // Channel 5.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={0,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,1,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.off();
    G1.set_cmd( "ST_CH", 6, 1 );    // Channel 6.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={0,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,1,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.off();
    G1.set_cmd( "ST_CH", 7, 1 );    // Channel 7.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={0,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,1,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.off();
    G1.set_cmd( "ST_CH", 8, 1 );    // Channel 8.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={0,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,1}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );

    G1.set_cmd( "ST_CH", 9, 1 );    // Channel 9 - incorrect.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=1, V=0, NOMINAL_CURRENT_CH={0,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,1}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );

    G1.off();
    G1.set_cmd( "NOMINAL_CURRENT_CH", 1, 1 );    // Nominal current 1.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={1,0,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.set_cmd( "NOMINAL_CURRENT_CH", 2, 1 );    // Nominal current 2.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={1,1,0,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.set_cmd( "NOMINAL_CURRENT_CH", 3, 1 );    // Nominal current 3.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={1,1,1,0,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.set_cmd( "NOMINAL_CURRENT_CH", 4, 1 );    // Nominal current 4.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={1,1,1,1,0,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.set_cmd( "NOMINAL_CURRENT_CH", 5, 1 );    // Nominal current 5.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={1,1,1,1,1,0,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.set_cmd( "NOMINAL_CURRENT_CH", 6, 1 );    // Nominal current 6.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={1,1,1,1,1,1,0,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.set_cmd( "NOMINAL_CURRENT_CH", 7, 1 );    // Nominal current 7.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={1,1,1,1,1,1,1,0}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    G1.set_cmd( "NOMINAL_CURRENT_CH", 8, 1 );    // Nominal current 8.
    G1.evaluate_io();
    G1.save_device( str_buff, "" );
    EXPECT_STREQ(
        "G1={M=0, ST=0, V=0, NOMINAL_CURRENT_CH={1,1,1,1,1,1,1,1}, "
        "LOAD_CURRENT_CH={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}, "
        "ST_CH={0,0,0,0,0,0,0,0}, "
        "SUM_CURRENTS=0.0, VOLTAGE=0.0, OUT_POWER_90=0, ERR=0},\n",
        str_buff );
    }


TEST( timer_manager, get_count )
    {
    const auto TIMERS_COUNT = 10;
    const timer_manager TM1( TIMERS_COUNT );
    EXPECT_EQ( TM1.get_count(), TIMERS_COUNT );
    }
