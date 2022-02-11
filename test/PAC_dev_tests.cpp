#include "PAC_dev_tests.h"

using namespace ::testing;

/*
	TEST METHOD DEFENITION:
	void evaluate()
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

TEST( valve_iolink_vtug, get_state_data )
	{
	class valve_iolink_vtug_test : public valve_iolink_vtug
		{
		public:
			valve_iolink_vtug_test( bool is_on_fb, bool is_off_fb,
				const char* dev_name, device::DEVICE_SUB_TYPE sub_type ):
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
	char state = 1;
	EXPECT_EQ( 0, v1.get_state_data( &state ) );
	v1.set_rt_par( 1, 1 );							//Set "vtug_number" to 1.
	EXPECT_EQ( 1, v1.get_state_data( &state ) );

	valve_iolink_vtug_test v2( false, false, "V2",
		device::DEVICE_SUB_TYPE::V_IOLINK_VTUG_DO1 );
	v2.set_rt_par( 1, 2 );							//Set "vtug_number" to 2.
	EXPECT_EQ( 0, v2.get_state_data( &state ) );
	state = 3;
	EXPECT_EQ( 1, v2.get_state_data( &state ) );
	}
