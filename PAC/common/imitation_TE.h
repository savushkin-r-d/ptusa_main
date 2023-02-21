#pragma once
#define _USE_MATH_DEFINES
#include <random>
#include <cmath>
class imitation_TE 
    {
    public:
        imitation_TE() = default;
        imitation_TE(float D,float m_expec);
        float get_TE(); // возвращает значения температуры 

    private:
        float D = float(0.234);   // дисперсия
        float m_expec = 27; // мат. ожидание 
        float st_deviation = float(0.48); // стандартное отклонение 
        float old_value = float(25.7);
        std::random_device rd{};
        std::mt19937 gen{ rd() };
        std::uniform_real_distribution<float> x_dis = std::uniform_real_distribution<float>(20, 30);  // функция генерирует случайные вещественные числа из заданного диапазона  
        float x = x_dis(gen);  // случайная величмина 
        bool is_p() const;   // функция расчета вероятности 
        float get_st_deviation() const;
       
    };

