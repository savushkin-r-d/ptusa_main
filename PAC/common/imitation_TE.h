#pragma once
#define _USE_MATH_DEFINES
#include <random>
#include <cmath>

class imitation_TE 
    {
    public:
        imitation_TE();
        imitation_TE(float D,float m_expec,float st_deviation);
        float get_TE(); // ���������� �������� ����������� 

    private:
        float D = 0.234;   // ���������
        float m_expec = 27; // ���. �������� 
        float st_deviation = 0.48; // ����������� ���������� 
        float old_value = 25.7;
        std::default_random_engine gen = random_gen();
        std::uniform_real_distribution<float> x_dis = std::uniform_real_distribution<float>(20, 30);  // ������� ���������� ��������� ������������ ����� �� ��������� ���������  
        std::default_random_engine random_gen(); // ������� ��������� ������������ ��������
        float x = x_dis(gen);  // ��������� ��������� 
        bool is_p();   // ������� ������� ����������� 

    };

