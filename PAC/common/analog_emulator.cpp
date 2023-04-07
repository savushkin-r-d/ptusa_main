#include "analog_emulator.h"

analog_emulator::analog_emulator( float dispersion, float math_expec ) : dispersion( dispersion ),
    m_expec( math_expec )
    {
    st_deviation = get_st_deviation();
    }

float analog_emulator::get_st_deviation() const
    {
    return float( sqrt( dispersion ) );
    }

float analog_emulator::get_value() const
    {
    std::random_device rd{};
    std::normal_distribution<> d{ m_expec, st_deviation };
    auto value = static_cast<float>( d ( rd ) );
    return value;
    }


