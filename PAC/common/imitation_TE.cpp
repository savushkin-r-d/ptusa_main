#include "imitation_TE.h"

analog_emulator::analog_emulator(float dispersion, float m_expec, float min_TE, float max_TE)
    : dispersion(dispersion), m_expec(m_expec), min_TE(min_TE), max_TE(max_TE) 
    {
        initial_arrays(min_TE, max_TE);
        st_deviation = get_st_deviation();   
    }


float imitation_TE::get_random()
    {
    max = get_max();
    min = get_min();
    std::vector<float> iv(11);
    std::vector<float> fv(10);
    std::iota(iv.begin(), iv.end(), min);
    std::iota(fv.begin(), fv.end(), 1);
    unsigned int index_fv = get_index();
    unsigned int index_iv = get_index();
    return  fv.at(index_fv) *0.1f + iv.at(index_iv);
    }

void imitation_TE::set_max(float max_in_range)
    {
    this->max = max_in_range;
    }

float analog_emulator::get_max() const
    {
    return max;
    }

void analog_emulator::set_min(float min_in_range)
    {
    this->min = min_in_range;
    }

float analog_emulator::get_min() const
    {
    return min;
    }

bool imitation_TE::is_p() const
    {
    // f(x)=1/(σ√2π)∙e^(-1/2∙((x-μ)/σ)^2) - функция Гаусса
    // σ - стандартное отклонение
    // x - случайная величина
    // μ - математическое ожидание
	return pow((st_deviation * sqrt(2 * M_PI)), -1) * exp(-(pow(x - m_expec, 2) / (2 * pow(st_deviation, 2)))) > 0.01; // 0.01 вероятность того, что случайная величина находится на [25,30].
    }

float analog_emulator::get_st_deviation() const
    {
    return float(sqrt(D));
    }

unsigned analog_emulator::get_index() const
    {
    auto value = unsigned(std::time(nullptr));
    unsigned int rd = value % 10;
    return rd;
    }

void analog_emulator::initial_arrays(float min, float max) 
{
      const auto ptr_size = static_cast<std::size_t>(max - min);
      iptr = std::unique_ptr<float[]>(new float[ptr_size]); // массив для генерации случайных вещественных чисел
      flptr= std::unique_ptr<float[]>(new float[ptr_size]);  // массив с заданным диапазоном значений температуры
      std::iota(iptr.get(), iptr.get() + ptr_size, 1);
      std::iota(flptr.get(), flptr.get() + ptr_size, min_TE);
      st_deviation = get_st_deviation();
}

float analog_emulator::get_value()
    {
    x = get_random();
    if (is_p())
        {
        old_value = x;
        return x;
        }
    else
        {
        return old_value;
        }

    }

imitation_TE::imitation_TE(float D,float m_expec):D(D),m_expec(m_expec)
    {
    st_deviation= get_st_deviation();
    }

