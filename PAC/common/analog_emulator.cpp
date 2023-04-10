#include "analog_emulator.h"

analog_emulator::analog_emulator( float math_expec, float stddev ) : m_expec( math_expec ),
    st_deviation( stddev ), d( m_expec, st_deviation )
    {
    }

float analog_emulator::get_st_deviation() const
    {
    return d.stddev(); 
    }

float analog_emulator::get_min() const 
    { 
    return d.min(); 
    }

float analog_emulator::get_max() const 
    { 
    return d.max();
    }

float analog_emulator::get_m_expec() const
    {
    return d.mean();
    }

float analog_emulator::get_value()
    {
    return d( rd );
    }
