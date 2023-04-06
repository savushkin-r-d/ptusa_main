#include "analog_emulator.h"
#include "log.h"

analog_emulator::analog_emulator( float dispersion, float math_expec,
    float min, float max ) : dispersion( dispersion ),
    m_expec( math_expec ), min_value( min ), max_value( max )
    {
    if ( max < min ) 
       {
       std::swap( min, max );
       max_value = max;
       min_value = min;
       G_LOG->alert( "analog_emulator: max > min (%f > %f)", max, min );
       }
       st_deviation = get_st_deviation();
    }

float analog_emulator::get_st_deviation() const
    {
    return float( sqrt( dispersion ) );
}

void analog_emulator::set_max( float max_in_range )
    {
    this->max_value = max_in_range;
    }

float analog_emulator::get_max() const
    {
    return max_value;
    }

void analog_emulator::set_min( float min_in_range )
    {
    this->min_value = min_in_range;
    }

float analog_emulator::get_min() const
    {
    return min_value;
    }

float analog_emulator::get_value() const
    {
    std::random_device rd{};
    std::normal_distribution<> d{ m_expec, st_deviation };
    auto value = static_cast<float>( d ( rd ) );
    return value;
    }


