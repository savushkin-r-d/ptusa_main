#ifndef CIP_TECH_DEF
#define CIP_TECH_DEF

#include "tech_def.h"


class cipline_tech_object: public tech_object
{
    public:
    cipline_tech_object( const char* name, u_int number, u_int type, const char* name_Lua,
            u_int states_count,
            u_int timers_count,
            u_int par_float_count, u_int runtime_par_float_count,
            u_int par_uint_count, u_int runtime_par_uint_count );
    i_DO_device* V1;

};

#endif //CIP_TECH_DEF
