#include "analog_emulator_tests.h"

#include <unordered_set>

using namespace ::testing;

TEST(analog_emulator, get_value) {
  analog_emulator obj(26.f, 2.f);

  // Заполняем вектор значениями в количестве MAX_ITER.
  std::vector<float> res_value;
  const size_t MAX_ITER = 100;
  for (size_t i = 0; i < MAX_ITER; i++) {
    res_value.push_back(obj.get_value());
  }

  std::unordered_set<float> s(res_value.begin(), res_value.end());
  res_value.assign(s.begin(), s.end());
  // Значения должны быть с разбросом - как минимум MAX_ITER/2 уникальных.
  EXPECT_TRUE(res_value.size() >= MAX_ITER / 2);
}

TEST(analog_emulator, get_st_deviation) {
  const auto MATH_EXPEC = 27.f;
  const auto ST_DEVIATION = 2.f;
  analog_emulator obj(MATH_EXPEC, ST_DEVIATION);
  EXPECT_EQ(ST_DEVIATION, obj.get_st_deviation());
}

TEST(analog_emulator, get_m_expec) {
  const auto MATH_EXPEC = 27.f;
  const auto ST_DEVIATION = 2.f;
  analog_emulator obj(MATH_EXPEC, ST_DEVIATION);
  EXPECT_EQ(MATH_EXPEC, obj.get_m_expec());
}

TEST(analog_emulator, param) {
  analog_emulator obj;
  obj.param(21.f, 5.f);
  EXPECT_EQ(21.f, obj.get_m_expec());
  EXPECT_EQ(5.f, obj.get_st_deviation());
}

TEST(analog_emulator, analog_emulator) {
  analog_emulator obj(12.f, 3.f);
  EXPECT_EQ(12.f, obj.get_m_expec());
  EXPECT_EQ(3.f, obj.get_st_deviation());
}
