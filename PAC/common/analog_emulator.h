#pragma once
#include <random>

class analog_emulator
    {
    public:
  analog_emulator( float math_expec = 27.f, float stddev = 2.f );

        /// @brief Получение значения аналоговой величины.
        ///
        /// @return Случайное значение аналоговой величины.
        float get_value();

        float get_st_deviation() const;
        float get_min() const;
        float get_max() const;
        float get_m_expec() const;

    private:
        float m_expec;      ///< Математическое ожидание.
        float st_deviation; ///< Стандартное отклонение.

        std::random_device rd{};
        std::normal_distribution<float> d;
    };
