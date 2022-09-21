#include "PAC_dev_tests.h"

using namespace ::testing;

/*
    TEST METHOD DEFENITION:
    void turn_off_blue()
*/

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

/*
    TEST METHOD DEFENITION:
    char get_state_data( char* data )
*/

TEST( valve_iolink_vtug, get_state_data )
    {
    class valve_iolink_vtug_test : public valve_iolink_vtug
        {
        public:
            valve_iolink_vtug_test( bool is_on_fb, bool is_off_fb,
                const char* dev_name, device::DEVICE_SUB_TYPE sub_type ) :
                valve_iolink_vtug( is_on_fb, is_off_fb, dev_name, sub_type ) {};

            valve_iolink_vtug_test( const char* dev_name,
                device::DEVICE_SUB_TYPE sub_type ) :valve_iolink_vtug(
                dev_name, sub_type ) {};

            /// @brief Получение данных состояния устройства.
            char get_state_data( char* data )
                {
                return valve_iolink_vtug::get_state_data( data );
                }

        };

    valve_iolink_vtug_test v1( "V1", device::DEVICE_SUB_TYPE::V_IOLINK_VTUG_DO1 );

    EXPECT_EQ( 0, v1.get_state_data( nullptr ) );
    char state = 0b1;
    EXPECT_EQ( 0, v1.get_state_data( &state ) );
    const int VTUG_NUMBER_IDX = 1;
    v1.set_rt_par( VTUG_NUMBER_IDX, 1 );            //Set vtug number to 1.
    EXPECT_EQ( 1, v1.get_state_data( &state ) );

    valve_iolink_vtug_test v2( false, false, "V2",
        device::DEVICE_SUB_TYPE::V_IOLINK_VTUG_DO1 );

    v2.set_rt_par( VTUG_NUMBER_IDX, 2 );			//Set vtug number to 2.
    EXPECT_EQ( 0, v2.get_state_data( &state ) );
    state = 0b10;
    EXPECT_EQ( 1, v2.get_state_data( &state ) );
    }


/*
    TEST METHOD DEFENITION:
    float temperature_e_analog::get_value()
*/

TEST( temperature_e_analog, get_value )
    {
    temperature_e_analog t1( "T1" );
    auto v = t1.get_value();
    EXPECT_EQ( .0, v );
    }

/*
    TEST METHOD DEFENITION:
    io_device* add_io_device( int dev_type, int dev_sub_type,
                        const char* dev_name, char * descr, char* article )
*/

TEST( device_manager, add_io_device )
    {
    auto dev = G_DEVICE_MANAGER()->get_device( "NO_DEVICE" );
    EXPECT_EQ( G_DEVICE_MANAGER()->get_stub_device(), dev );

    //device::DT_TE, device::DST_TE_ANALOG
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_ANALOG, "T1", "Test sensor", "T" );
    EXPECT_NE( nullptr, res );

    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    G_DEVICE_MANAGER()->save_device( buff );
    EXPECT_STREQ( 
        "t=\n"
            "\t{\n"
            "\tT1={M=0, ST=1, V=0, P_CZ=0, P_ERR_T=0, P_MIN_V=0, P_MAX_V=0},\n"
            "\t}\n",
        buff );

    //device::DT_FQT, device::DST_FQT_IOLINK
    auto name = std::string( "FQT1" );
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

TEST( dev_stub, get_pump_dt )
    {
    EXPECT_EQ( .0f, STUB()->get_pump_dt() );
    }

TEST( dev_stub, get_min_flow )
    {
    EXPECT_EQ( .0f, STUB()->get_min_flow() );
    }

TEST( device, save_device )
    {
    temperature_e_analog t1( "T1" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    t1.save_device( buff, "" );
    EXPECT_STREQ( 
        "T1={M=0, ST=1, V=0, P_CZ=0, P_ERR_T=0, P_MIN_V=0, P_MAX_V=0},\n", buff );
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

TEST( counter_f, get_state )
    {
    counter_f fqt1( "FQT1" );
    EXPECT_EQ( (int) i_counter::STATES::S_WORK, fqt1.get_state() );

    motor m1( "M1", device::DST_M_FREQ );
    fqt1.set_property( "M", &m1 );
    EXPECT_EQ( (int) i_counter::STATES::S_WORK, fqt1.get_state() );

    //Расход ниже минимального - ошибка не должна появиться.
    m1.on();    
    fqt1.get_state();
    sleep_ms( 1 );
    EXPECT_EQ( (int) i_counter::STATES::S_WORK, fqt1.get_state() );

    //Устанавливаем расход - ошибка должна появиться.
    fqt1.set_cmd( "F", 0, 1 );
    fqt1.get_state();
    sleep_ms( 1 );
    EXPECT_EQ( (int)i_counter::STATES::S_ERROR, fqt1.get_state() );

    fqt1.start();
    //Расход стал ниже минимального - ошибка не должна появиться.
    fqt1.set_cmd( "P_ERR_MIN_FLOW", 0, 2 );
    fqt1.get_state();
    sleep_ms( 1 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.set_cmd( "P_ERR_MIN_FLOW", 0, 0 );
    fqt1.get_state();
    fqt1.set_cmd( "ABS_V", 0, 100 );
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
        "FQT1={M=0, ST=1, V=50.00, ABS_V=100, F=9.90, P_MIN_FLOW=0,"
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
    EXPECT_STREQ( "FQT1={M=0, ST=1, V=50.00, ABS_V=100},\n", buff );

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
        "FQT1={M=0, ST=1, V=50000.00, ABS_V=100000, F=9.90, T=1.1, "
        "P_CZ=0, P_DT=0, P_ERR_MIN_FLOW=0},\n", buff );

    fqt1.set_cmd( "ST", 0, 2 );
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, 1 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );
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
    EXPECT_EQ( counter_iolink::mL_in_L * 30, fqt1.get_quantity() );
    EXPECT_EQ( counter_iolink::mL_in_L * 50, fqt1.get_abs_quantity() );


    //Test reset to 0 physical counter after its power reboot.
    fqt1.set_raw_value( 10 );
    fqt1.evaluate_io();
    EXPECT_EQ( counter_iolink::mL_in_L * 40, fqt1.get_quantity() );

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

    //12336, 11824 и т.д. - десятичное представление строки 00, .0 и тд. В
    //таком формате приходят данные с весов. 
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

TEST( threshold_regulator, set_value )
    {
    threshold_regulator p1( "test" );
    device* p1_dev = static_cast<device*>( &p1 );


    G_DEVICE_MANAGER()->clear_io_devices();
    auto name = std::string( "TE1" );
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_VIRT, name.c_str(), "Test sensor", "T" );
    ASSERT_EQ( nullptr, res );
    auto TE1 = TE( name.c_str() );
    ASSERT_NE( STUB(), dynamic_cast<dev_stub*>( TE1 ) );
    name = std::string( "M1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_M, device::DST_M_VIRT, "M1", "Test motor", "M" );
    ASSERT_EQ( nullptr, res );
    auto M1 = M( name.c_str() );
    ASSERT_NE( STUB(), dynamic_cast<dev_stub*>( M1 ) );

    auto m1 = G_DEVICE_MANAGER()->get_M( "M1" );
    ASSERT_NE( nullptr, m1 );
    p1.set_string_property( "IN_VALUE", "TE1" );
    p1.set_string_property( "OUT_VALUE", "M1" );

    p1_dev->off();
    //Regulator switched off.    
    p1_dev->set_value( 10 );
    EXPECT_EQ( 0, m1->get_state() );
    EXPECT_EQ( 0, p1_dev->get_state() );

    p1_dev->on();
    //Regulator switched on, it should switch on actuator.
    TE1->set_cmd( "V", 0, 1 );
    p1_dev->set_value( 10 );
    EXPECT_EQ( 1, m1->get_state() );
    EXPECT_EQ( 1, p1_dev->get_state() );

    //Regulator switched on, it should switch off actuator.
    TE1->set_cmd( "V", 0, 20 );
    p1_dev->set_value( 10 );
    EXPECT_EQ( 0, m1->get_state() );

    //Regulator switched on, temperature is inside range - it should not change
    //actuators state.
    p1.set_cmd( "P_DELTA", 0, 10 );
    p1_dev->set_value( 10 );
    EXPECT_EQ( 0, m1->get_state() );

    //Regulator switched on, temperature is not inside range - it should change
    //actuators state.
    p1.set_cmd( "P_DELTA", 0, 1 );
    TE1->set_cmd( "V", 0, 5 );
    p1_dev->set_value( 10 );
    EXPECT_EQ( 1, m1->get_state() );

    p1_dev->off();
    //Regulator switched off.    
    p1_dev->set_value( 10 );
    EXPECT_EQ( 0, m1->get_state() );
    EXPECT_EQ( 0, p1_dev->get_state() );

    //Use counter as a sensor.
    p1_dev->on();
    name = std::string( "FQT1" );
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT_VIRT, name.c_str(), "Test counter", "F" );
    ASSERT_EQ( nullptr, res );
    auto FQT1 = virtual_FQT( name.c_str() );
    ASSERT_NE( STUB(), dynamic_cast<dev_stub*>( FQT1 ) );
    p1.set_string_property( "IN_VALUE", "FQT1" );
    FQT1->set_cmd( "F", 0, 10 );
    p1_dev->set_value( 20 );
    EXPECT_EQ( 1, m1->get_state() );
    }

TEST( threshold_regulator, set_cmd )
    {
    threshold_regulator p1( "C1" );
    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };

    p1.save_device( buff );
    EXPECT_STREQ(
        "\tC1={M=0, ST=0, V=0, P_IS_REVERSE=0, P_DELTA=0},\n", buff );

    p1.set_cmd( "P_DELTA", 0, 10 );
    p1.set_cmd( "P_IS_REVERSE", 0, 1 );
    p1.on();
    p1.save_device( buff );
    EXPECT_STREQ(
        "\tC1={M=0, ST=1, V=0, P_IS_REVERSE=1, P_DELTA=10},\n", buff );
    }

TEST( threshold_regulator, set_state )
    {
    threshold_regulator p1( "C1" );

    //Switch on regulator.
    p1.set_state( 1 );
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
