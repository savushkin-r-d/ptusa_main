#include "analog_emulator.h"

analog_emulator::analog_emulator( float math_expec, float st_deviation ) :
    d( math_expec, st_deviation )
    {
    }

float analog_emulator::get_st_deviation() const
    {
    return d.stddev();
    }

float analog_emulator::get_m_expec() const
    {
    return d.mean();
    }

float analog_emulator::get_value()
    {
    return d( rd );
    }
