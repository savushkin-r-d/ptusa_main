#include "analog_emulator.h"
#include "log.h"

analog_emulator::analog_emulator( float dispersion, float m_expec,
    float min, float max, float p, float x ) : dispersion( dispersion ),
    m_expec( m_expec ), min_value( min ), max_value( max ), x( x ), p( p )
    {
    if ( max < min )
        {
        std::swap( min, max );
        max_value = max;
        min_value = min;
        G_LOG->alert( "analog_emulator: max > min (%f > %f)", max, min );
        }
    init_vector( min, max );
    prev_x = get_random();
    st_deviation = get_st_deviation();
    }

void analog_emulator::init_vector( float min_t, float max_t )
    {
    if ( max_t == min_t )
        {
        fv.resize( 1 );
        G_LOG->alert( "analog_emulator: max = min (%f = %f)", max_t, min_t );
        }
    else
        {
        const auto v_size = static_cast<std::size_t>( max_t - min_t );
        fv.resize( v_size );
        }
    std::iota( fv.begin(), fv.end(), min_t );
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

bool analog_emulator::is_p() const
    {
    // f(x)=1/(σ√2π)∙e^(-1/2∙((x-μ)/σ)^2) - функция Гаусса.
    // σ - стандартное отклонение.
    // x - случайная величина.
    // μ - математическое ожидание.
    // 0.01 вероятность того, что случайная величина находится 
    // в заданном диапазоне температур.
    constexpr double two_pi = 2 * M_PI;
    return ( 1 / ( st_deviation * sqrt( two_pi ) ) ) *
        exp( -( pow( x - m_expec, 2 ) / ( 2 * st_deviation * st_deviation ) ) ) > p;
    }

float analog_emulator::get_value()
    {
    x = get_random();
    if ( is_p() )
        {
        prev_x = x;
        return x;
        }
    else
        {
        return prev_x;
        }
    }

float analog_emulator::get_random() const
    {
    auto value = unsigned( std::time( nullptr ) );
    std::size_t fv_size = fv.size();
    unsigned int fv_index = value % fv_size;
    // Вспомогательная величина для генерации случайных вещественных чисел.
    float real = (float)fv_index / max_value;
    float round_real = ( (float)( (int)( real * 10 ) ) ) / 10;
    return fv.at( fv_index ) + round_real;
    }
