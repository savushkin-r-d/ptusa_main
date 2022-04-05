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
    auto res = G_DEVICE_MANAGER()->add_io_device(
        device::DT_TE, device::DST_TE_ANALOG, "T1", "Test sensor", "T" );
    EXPECT_NE( nullptr, res );
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
