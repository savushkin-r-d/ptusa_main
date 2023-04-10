#pragma once
#include <random>

class analog_emulator
    {
    public:
        analog_emulator( float dispersion = 0.238f, float math_expec = 27.f );

        /// @brief Получение значения аналоговой величины.
        ///
        /// @return Случайное значение аналоговой величины.
        float get_value();

        float get_st_deviation() const;

    private:
        float dispersion;   ///< Дисперсия.
        float m_expec;      ///< Математическое ожидание.
        float st_deviation; ///< Стандартное отклонение.

        std::random_device rd{};
        std::normal_distribution<float> d;
    };
