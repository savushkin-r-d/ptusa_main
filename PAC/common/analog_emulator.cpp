#include "analog_emulator.h"

analog_emulator::analog_emulator( float dispersion, float math_expec ) : dispersion( dispersion ),
    m_expec( math_expec ), d( m_expec, dispersion )
    {
    st_deviation = get_st_deviation();
    }

float analog_emulator::get_st_deviation() const
    {
    return float( sqrt( dispersion ) );
    }

float analog_emulator::get_value()
    {
    return d( rd );
    }
