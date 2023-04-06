#include "analog_emulator.h"
#include "log.h"

analog_emulator::analog_emulator( float dispersion, float m_expec,
    float min, float max ) : dispersion( dispersion ),
    m_expec( m_expec ), min_value( min ), max_value( max )
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


float analog_emulator::get_value()
    {
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<> d{27.f, 2.f};
    float value = static_cast<float>(d(gen));
    return value;
    }


