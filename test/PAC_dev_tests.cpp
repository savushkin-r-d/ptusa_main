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
    res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_FQT, device::DST_FQT_IOLINK, "FQT1", "Test counter", "IFM" );
    EXPECT_NE( nullptr, res );
    dev = G_DEVICE_MANAGER()->get_device( "FQT1" );
    EXPECT_NE( G_DEVICE_MANAGER()->get_stub_device(), dev );
    auto FQT1 = FQT( "FQT1" );
    EXPECT_NE( dynamic_cast<i_counter*>( STUB() ), FQT1 );
    }

TEST( dev_stub, get_pump_dt )
    {
    EXPECT_EQ( .0f, STUB()->get_pump_dt() );
    }

TEST( device, save_device )
    {
    temperature_e_analog t1( "T1" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    t1.save_device( buff, "" );
    EXPECT_STREQ( "T1={M=0, ST=1, V=0, P_CZ=0, P_ERR_T=0, P_MIN_V=0, P_MAX_V=0},\n", buff );
    }

TEST( motor_altivar, set_cmd )
    {
    motor_altivar m1( "M1", device::M_ATV );    
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };
    m1.save_device( buff, "" );
    EXPECT_STREQ( "M1={M=0, ST=0, V=0, R=0, FRQ=0.00, RPM=0.00, EST=0, P_ON_TIME=0},\n", buff );

    m1.set_cmd( "R", 0, 1 );
    m1.set_cmd( "FRQ", 0, 1.1 );
    m1.set_cmd( "RPM", 0, 12.2 );
    m1.set_cmd( "EST", 0, 2 );
    m1.save_device( buff, "" );
    EXPECT_STREQ( "M1={M=0, ST=0, V=1.10, R=1, FRQ=1.10, RPM=12.20, EST=2, P_ON_TIME=0},\n", buff );
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

    m1.on();
    fqt1.get_state();
    sleep_ms( 10 );
    EXPECT_EQ( (int) i_counter::STATES::S_ERROR, fqt1.get_state() );

    fqt1.set_cmd( "ABS_V", 0, 100 );
    EXPECT_EQ( (int) i_counter::STATES::S_WORK, fqt1.get_state() );

    fqt1.pause();
    EXPECT_EQ( (int) i_counter::STATES::S_PAUSE, fqt1.get_state() );

    fqt1.start();
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );
    }

TEST( counter_iolink, set_cmd )
    {
    counter_iolink fqt1( "FQT1" );
    const int BUFF_SIZE = 100;
    char buff[ BUFF_SIZE ] = { 0 };

    EXPECT_EQ( 0, fqt1.get_abs_quantity() );
    
    fqt1.set_cmd( "V", 0, 50 );
    EXPECT_EQ( 50, fqt1.get_quantity() );
    EXPECT_EQ( 50.f, fqt1.get_value() );
    
    fqt1.set_cmd( "ABS_V", 0, 100 );
    EXPECT_EQ( 100, fqt1.get_abs_quantity() );
    
    fqt1.set_cmd( "F", 0, 9.9 );
    EXPECT_EQ( 9.9f, fqt1.get_flow() );
    
    fqt1.set_cmd( "T", 0, 1.1 );
    EXPECT_EQ( 1.1f, fqt1.get_temperature() );
    
    fqt1.save_device( buff, "" );
    EXPECT_STREQ( "FQT1={M=0, ST=1, V=50.00, ABS_V=100, F=9.90, T=1.1, P_CZ=0, P_DT=0},\n", buff );

    fqt1.set_cmd( "ST", 0, 2 );
    EXPECT_EQ( (int)i_counter::STATES::S_PAUSE, fqt1.get_state() );

    fqt1.set_cmd( "ST", 0, 1 );
    EXPECT_EQ( (int)i_counter::STATES::S_WORK, fqt1.get_state() );
    }

TEST( counter_iolink, evaluate_io )
    {
    counter_iolink fqt1( "FQT1" );
    fqt1.evaluate_io();
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

    fqt1.set_raw_value( 10 );
    res = fqt1.get_quantity();
    EXPECT_EQ( 0, res );

    fqt1.set_raw_value( 20 );
    res = fqt1.get_quantity();
    EXPECT_EQ( 10, res );
    }
