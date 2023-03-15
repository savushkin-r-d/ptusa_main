#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <ctime>
#include <numeric>
#include <vector>
#include <iostream>

class analog_emulator
    {
    public:
        analog_emulator( float dispersion = 0.238f, float m_expec = 27.f,
            float min_TE = 20.f, float max_TE = 30.f, float x = 1.f );

        float get_value();  // возвращает значения температуры
        void set_max( float max_in_range );
        float get_max() const;
        void set_min( float min_in_range );
        float get_min() const;
        float get_st_deviation() const;

    private:
        float dispersion;   // дисперсия
        float m_expec;      // мат. ожидание
        float st_deviation; // стандартное отклонение
        float min_TE;
        float max_TE;
        float x;            // случайная величина
        std::vector<float> iv; // вектор для генерации случайных вещественных чисел.
        std::vector<float> fv; // вектор с заданным диапазоном значений температуры.

        float get_random() const;
        bool is_p() const;  // функция расчета вероятности
        unsigned get_index() const;
        void init_vector( float min, float max );
        float prev_x;
    };
