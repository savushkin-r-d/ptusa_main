#pragma once
#include <algorithm>
#include <random>

class analog_emulator
    {
    public:
        analog_emulator( float dispersion = 0.238f, float m_expec = 27.f,
            float min = 20.f, float max = 30.f );

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

    };
