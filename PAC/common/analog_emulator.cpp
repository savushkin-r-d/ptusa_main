#include "analog_emulator.h"

analog_emulator::analog_emulator( float dispersion, float m_expec,
    float min_TE, float max_TE , float x) : dispersion( dispersion ),
    m_expec( m_expec ), min_TE( min_TE ), max_TE( max_TE ), x( x )
    {
    initial_arrays( min_TE, max_TE );
    st_deviation = get_st_deviation();
    }


float analog_emulator::get_random() const
    {
    unsigned int index_flptr = get_index();
    unsigned int index_iptr = get_index();
    // Вспомогательная величина для генерации случайных вещественных чисел.
    const float real = 0.2f;
    return iptr[ index_flptr ] * real + flptr[ index_iptr ];
    }

void analog_emulator::set_max( float max_in_range )
    {
    this->max_TE = max_in_range;
    }

float analog_emulator::get_max() const
    {
    return max_TE;
    }

void analog_emulator::set_min( float min_in_range )
    {
    this->min_TE = min_in_range;
    }

float analog_emulator::get_min() const
    {
    return min_TE;
    }

bool analog_emulator::is_p() const
    {
    // f(x)=1/(σ√2π)∙e^(-1/2∙((x-μ)/σ)^2) - функция Гаусса
    // σ - стандартное отклонение
    // x - случайная величина
    // μ - математическое ожидание
    constexpr double two_pi = 2 * M_PI;
    return ( 1 / ( st_deviation * sqrt( two_pi ) ) ) *
        exp( -( pow( x - m_expec, 2 ) / ( 2 * st_deviation * st_deviation ) ) ) > 0.01;
    }   // 0.01 вероятность того, что случайная величина находится на [25,30]

float analog_emulator::get_st_deviation() const
    {
    return float( sqrt( dispersion ) );
    }

unsigned analog_emulator::get_index() const
    {
    auto value = unsigned( std::time( nullptr ) );
    unsigned int rd = value % 10;
    return rd;
    }

void analog_emulator::initial_arrays( float min, float max )
    {
    const auto ptr_size = static_cast<std::size_t>( max - min );
    // Массив для генерации случайных вещественных чисел.
    iptr = std::unique_ptr<float[]>( new float[ ptr_size ] );
    // Массив с заданным диапазоном значений температуры.
    flptr = std::unique_ptr<float[]>( new float[ ptr_size ] );
    std::iota( iptr.get(), iptr.get() + ptr_size, 1 );
    std::iota( flptr.get(), flptr.get() + ptr_size, min_TE );
    st_deviation = get_st_deviation();
    }

float analog_emulator::get_value()
    {
    x = get_random();
    static float old_value = x;
    if ( is_p() )
        {
        old_value = x;
        return x;
        }
    else
        {
        return old_value;
        }
    }
