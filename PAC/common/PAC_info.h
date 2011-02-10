#ifndef PAC_INFO_H
#define PAC_INFO_H

#include "g_device.h"
#include "PAC_dev.h"
#include "param_ex.h"

class PAC_info
    {
    public:
        PAC_info();

        ~PAC_info();

        void eval();

        complex_device  *com_dev;

        enum PARAMETERS
            {
            P_PARAMS_COUNT  = 1,  // Количество параметров.

            P_CTR_ERROR_TIME = 0, // Время ожидания ошибки счетчика, мсек.
            };

        saved_params_u_int_4 *par;

        void reset_params();

    private:    
        static const u_int_4 MSEC_IN_DAY; 
        enum CONSTANTS
            {
            C_MAX_STR_LENGTH = 50,
            };

        u_int_4 up_days;
        u_int_4 up_hours;
        u_int_4 up_mins;
        u_int_4 up_secs;

        u_int_4 up_msec;
        u_int_4 last_check_time;

        u_int_4 reset_type;

        string_device str_datatime;
        char up_time_str[ C_MAX_STR_LENGTH ];
    };
#endif // PAC_INFO_H
