#pragma once
#include <random>

class analog_emulator {
 public:
  analog_emulator(float math_expec = 1.f, float stddev = .2f);

  /// @brief Получение значения аналоговой величины.
  ///
  /// @return Случайное значение аналоговой величины.
  float get_value();

  float get_st_deviation() const;
  float get_m_expec() const;
  void param(float math_expec, float stddev);

 private:
  std::random_device rd{};
  std::normal_distribution<float> d;
};
