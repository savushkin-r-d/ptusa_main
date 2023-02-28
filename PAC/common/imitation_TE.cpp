#include "imitation_TE.h"

float imitation_TE::get_random()
    {
    max = get_max();
    min = get_min();
    std::vector<int> iv(10);
    std::vector<float> fv(10);
    std::iota(iv.begin(), iv.end(), min);
    std::iota(fv.begin(), fv.end(), 1);
    std::random_shuffle(iv.begin(), iv.end());
    std::random_shuffle(fv.begin(), fv.end());
    return static_cast<float>(fv.front() * 0.1f + iv.front());
    }

void imitation_TE::set_max(int max_in_range)
    {
    this->max = max_in_range;
    }

int imitation_TE::get_max() const
    {
    return max;
    }

void imitation_TE::set_min(int min_in_range)
    {
    this->min = min_in_range;
    }

int imitation_TE::get_min() const
    {
    return min;
    }

bool imitation_TE::is_p() const
    {
    // f(x)=1/(σ√2π)∙e^(-1/2∙((x-μ)/σ)^2) - функция Гаусса
    // σ - стандартное отклонение
    // x - случайная величина
    // μ - математическое ожидание
	return pow((st_deviation * sqrt(2 * M_PI)), -1) * exp(-(pow(x - m_expec, 2) / (2 * pow(st_deviation, 2)))) > 0.5; // 0.5 вероятность того, что случайная величина находится на [25,30].
    }

float imitation_TE::get_st_deviation() const
    {
    return float(sqrt(D));
    }

float imitation_TE::get_TE()
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
