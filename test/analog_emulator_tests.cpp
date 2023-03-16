#include "analog_emulator_tests.h"

using namespace ::testing;

TEST( analog_emulator, get_value )
    {
    analog_emulator obj = analog_emulator( 0.444f, 0.25f, 20.f, 30.f );
    float test_variable = obj.get_value();
    EXPECT_GE( test_variable, 20 );
    EXPECT_LE( test_variable, 35 );
    }

TEST( analog_emulator, analog_emulator )
    {
    analog_emulator obj1 = analog_emulator( 0.34f, 26.f, 30.f, 14.f );
    float st_d = obj1.get_st_deviation();
    float ex_st_d = sqrt( 0.34f );
    float min_t = obj1.get_min();
    float max_t = obj1.get_max();
    EXPECT_EQ( min_t, 14.f );
    EXPECT_EQ( max_t, 30.f );
    EXPECT_EQ( st_d, ex_st_d );
    }

TEST( analog_emulator, set_max )
    {
    analog_emulator obj2 = analog_emulator( 0.444f, 0.25f, 20.f, 30.f );
    obj2.set_max( 35 );
    float ex_max = obj2.get_max();
    EXPECT_EQ( ex_max, 35 );
    }

TEST( analog_emulator, set_min )
    {
    analog_emulator obj3 = analog_emulator( 0.444f, 0.25f, 20.f, 30.f );
    obj3.set_min( 0 );
    float ex_min = obj3.get_min();
    EXPECT_EQ( ex_min, 0 );
    }

TEST(analog_emulator, init_vector) 
    {
    analog_emulator obj4 = analog_emulator( 0.444f, 0.25f, 20.f, 20.f );
    float value = obj4.get_value();
    EXPECT_EQ( value, 20.f );
    }
