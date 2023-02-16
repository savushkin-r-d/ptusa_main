#include "imitation_TE.h"

bool imitation_TE::is_p()
{
	return pow((st_deviation * sqrt(2 * M_PI)), -1) * exp(-(pow(x - m_expec, 2) / (2 * pow(st_deviation, 2)))) > 0.5;
}

std::default_random_engine imitation_TE::random_gen()
{
	  std::default_random_engine gen;
	return gen;
}

float imitation_TE::get_TE()
{
    
       x = x_dis(gen);
       if (is_p()) {                                                                           
           old_value = x;
           return x;                             
       }
       else return old_value;
        
        
}
