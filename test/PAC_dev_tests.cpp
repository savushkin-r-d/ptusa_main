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
