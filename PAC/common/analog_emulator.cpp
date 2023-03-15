#include "analog_emulator.h"

analog_emulator::analog_emulator( float dispersion, float m_expec,
    float min_TE, float max_TE, float p, float x ) : dispersion( dispersion ),
    m_expec( m_expec ), min( min ), max( max ), x( x ), p( p )
    {
    if ( max < min )
        {
        std::swap( max, min );
        std::clog << "max > min\n";
        }
    init_vector( min_TE, max_TE );
    prev_x = get_random();
    st_deviation = get_st_deviation();
    }

void analog_emulator::init_vector( float min_t, float max_t )
    {
    if ( max_t == min_t )
        {
        iv.emplace_back( 0 );
        fv.emplace_back( max_t );
        std::clog << "max = min\n";
        }
    else
        {
        const auto v_size = static_cast<std::size_t>( max_t - min_t );
        iv.resize( v_size );
        fv.resize( v_size );
        std::iota( iv.begin(), iv.end(), 1 );
        std::iota( fv.begin(), fv.end(), min_t );
        }
    }

float analog_emulator::get_st_deviation() const
    {
    return float( sqrt( dispersion ) );
    }

void analog_emulator::set_max( float max_in_range )
    {
    this->max = max_in_range;
    }

float analog_emulator::get_max() const
    {
    return max;
    }

void analog_emulator::set_min( float min_in_range )
    {
    this->min = min_in_range;
    }

float analog_emulator::get_min() const
    {
    return min;
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
    if (is_p()) 
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
    unsigned int index_fv = get_index();
    unsigned int index_r = get_index();
    // Вспомогательная величина для генерации случайных вещественных чисел.
    float real = 0.1f * static_cast<float>( index_r );
    return real + fv.at( index_fv );
    }
    
unsigned analog_emulator::get_index() const
    {
    auto value = unsigned( std::time( nullptr ) );
    unsigned int rd = value % 10;
    return rd;
    }