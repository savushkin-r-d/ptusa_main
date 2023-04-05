#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <ctime>
#include <numeric>
#include <vector>
#include <iostream>
#include <algorithm>

class analog_emulator
    {
    public:
        analog_emulator( float dispersion = 0.238f, float m_expec = 27.f,
            float min = 20.f, float max = 30.f, float p = 0.01f, float x = 1.f );

        /// @brief Получение значения аналоговой величины.
        ///
        /// @return Случайное значение аналоговой величины.
        float get_value();

        void set_max( float max_in_range );
        float get_max() const;
        void set_min( float min_in_range );
        float get_min() const;
        float get_st_deviation() const;

    private:
        float dispersion;   ///< Дисперсия.
        float m_expec;      ///< Математическое ожидание.
        float st_deviation; ///< Стандартное отклонение.
        float min_value;
        float max_value;
        float x;                ///< Случайная величина.
        float p;                ///< Плотность вероятности.
        std::vector<float> fv;  ///< Вектор с заданным диапазоном значений температуры.

        float get_random() const;

        /// @brief Расчет вероятности.
        bool is_p() const;

        void init_vector( float min, float max );
        float prev_x;
    };
