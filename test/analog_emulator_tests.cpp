#include "analog_emulator_tests.h"

using namespace ::testing;

TEST( analog_emulator, get_value )
    {
    const auto MIN_VALUE = 20.f;
    const auto MAX_VALUE = 35.f;
    analog_emulator obj = analog_emulator( 0.444f, 26.f, MIN_VALUE, MAX_VALUE );
    EXPECT_GE( MIN_VALUE, obj.get_min() );
    EXPECT_LE( MAX_VALUE, obj.get_max() );

    // Заполняем вектор значениями в количестве MAX_ITER.
    std::vector<float> res_value;
    const size_t MAX_ITER = 100;
    for ( size_t i = 0; i < MAX_ITER; i++ )
        {
        res_value.push_back( obj.get_value() );
        }

    // Значения должны быть в заданном диапазоне.
    ASSERT_THAT( res_value, Each( AllOf( Ge( MIN_VALUE ), Le( MAX_VALUE ) ) ) );
 
    std::unordered_set<float> s(res_value.begin(), res_value.end());
    res_value.assign(s.begin(), s.end());
    // Значения должны быть с разбросом - как минимум MAX_ITER/2 уникальных.
    EXPECT_TRUE( res_value.size() >= MAX_ITER / 2 );
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

