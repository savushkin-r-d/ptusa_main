#pragma once
#define _USE_MATH_DEFINES
#include <random>
#include <cmath>


class imitation_TE {

    const float D = 0.234;
    const float m_expec = 27;
    const float st_deviation = 0.48;
    std::default_random_engine gen = random_gen();
    std::uniform_real_distribution<float> x_dis = std::uniform_real_distribution<float>(20, 30);
    float x = x_dis(gen);
    float old_value = 25.7;

    bool is_p();

    std::default_random_engine random_gen();

public:
    float get_TE();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
};

