#include "analog_emulator_tests.h"

using namespace ::testing;

TEST( analog_emulator, get_value )
    {
    analog_emulator obj( 26.f, 2.f );

    // Заполняем вектор значениями в количестве MAX_ITER.
    std::vector<float> res_value;
    const size_t MAX_ITER = 100;
    for ( size_t i = 0; i < MAX_ITER; i++ )
        {
        res_value.push_back( obj.get_value() );
        }

    std::unordered_set<float> s( res_value.begin(), res_value.end() );
    res_value.assign( s.begin(), s.end() );
    // Значения должны быть с разбросом - как минимум MAX_ITER/2 уникальных.
    EXPECT_TRUE( res_value.size() >= MAX_ITER / 2 );
    }

TEST( analog_emulator, analog_emulator )
    {
    analog_emulator obj1( 26.f, 1.f );
    float st_d = obj1.get_st_deviation();
    float mean = obj1.get_m_expec();
    EXPECT_EQ( mean, 26.f );
    EXPECT_EQ( st_d, 1.f );
    }
