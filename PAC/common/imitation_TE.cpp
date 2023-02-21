#include "imitation_TE.h"

 bool imitation_TE::is_p() const
    {
    // f(x)=1/(σ√2π)∙e^(-1/2∙((x-μ)/σ)^2) - функция Гаусса 
    // σ - стандартное отклонение
    // x - случайная величина
    // μ - математическое ожидание
	return pow((st_deviation * sqrt(2 * M_PI)), -1) * exp(-(pow(x - m_expec, 2) / (2 * pow(st_deviation, 2)))) > 0.5; // 0.5 вероятность того, что случайная велична находится на [25,30].
    }

float imitation_TE::get_st_deviation() const
    {
    return float(sqrt(D));
    }

float imitation_TE::get_TE()
    {
    x = x_dis(gen);
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
