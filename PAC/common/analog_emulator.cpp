#include "analog_emulator.h"

analog_emulator::analog_emulator( float dispersion, float m_expec,
    float min_TE, float max_TE , float x) : dispersion( dispersion ),
    m_expec( m_expec ), min_TE( min_TE ), max_TE( max_TE ), x( x )
    {
    if (max_TE < min_TE)
    {
    std::swap(max_TE, min_TE);
    std::clog << "max > min\n";
    }
    init_vector(min_TE, max_TE);
    st_deviation = get_st_deviation();
    }

void analog_emulator::init_vector(float min, float max) 
    {
    std::size_t v_size;
    if ( max == min )
    {
        v_size = max;
        iv.emplace_back(0);
        fv.emplace_back(max);
        std::clog << "max = min\n";
    } 
    else
    {
        v_size = static_cast<std::size_t>( max - min );
        iv.resize(v_size);
        fv.resize(v_size);
        std::iota(iv.begin(), iv.end(), 1);
        std::iota(fv.begin(), fv.end(), min_TE);
    }
    }

float analog_emulator::get_st_deviation() const 
    {
    return float(sqrt( dispersion ));
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
        exp( -( pow( x - m_expec, 2 ) / ( 2 * st_deviation * st_deviation ) ) ) > 0.01; // 0.01 вероятность того, что случайная величина находится 
                                                                                        // в заданном диапазоне температур
    } 

float analog_emulator::get_value() 
    {
    x = get_random();
    static float old_value;
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

float analog_emulator::get_random() const 
    {
    unsigned int index_fv = get_index();
    unsigned int index_iv = get_index();
    // Вспомогательная величина для генерации случайных вещественных чисел.
    const float real = 0.2f;
    return iv.at(index_fv) * real + fv.at(index_iv);
    }

unsigned analog_emulator::get_index() const
    {
    auto value = unsigned( std::time( nullptr ) );
    unsigned int rd = value % 10;
    return rd;
    }

