#ifndef _PID_H
#define _PID_H

#include "param_ex.h"
#include "util.h"
#include "sys.h"

class PID 
    { 
    enum STATES 
        {
        STATE_OFF,
        STATE_ON,  
        };

    float uk_1;
    float ek_1;
    float ek_2;
    float q0;
    float q1;
    float q2;
    float Uk;
    float dUk;
    unsigned long start_time;
    unsigned long last_time;

    char prev_manual_mode;

    //Надо ли при старте регулятора уменьшать, а не увеличивать выходную величину.
    char is_down_to_inaccel_mode;  

    saved_params_float    *par;
    run_time_params_float *w_par;

    u_int_4 state;

    int   used_par_n;

    smart_ptr< complex_device > com_dev; ///< Связь с сервером.

    void reset();

    public:
        enum PARAM 
            {        
            P_k = 0,               ///< Параметр k.
            P_Ti,                  ///< Параметр Ti.
            P_Td,                  ///< Параметр Td.
            P_dt,                  ///< Интервал расчёта
            P_max,                 ///< Мax значение входной величины.
            P_min,                 ///< Мin значение входной величины.
            P_acceleration_time,   ///< Время выхода на режим регулирования.
            P_is_manual_mode,      ///< Ручной режим.
            P_U_manual,            ///< Заданное ручное значение выходного сигнала.        

            P_k2,                  ///< Параметр k2.
            P_Ti2,                 ///< Параметр Ti2.
            P_Td2,                 ///< Параметр Td2. 
            }; 

        enum WORK_PARAMS 
            {
            WP_Z,  ///< Требуемое значение.
            WP_Uk, ///< Выход ПИД.
            };

        PID( int n );

        ~PID(); 
                
        void  on( char is_down_to_inaccel_mode = 0 );
        void  off();

        float eval( float current_value, int delta_sign = 1 );

        /// @brief Установка нового задания ПИД.
        void set( float new_z );    

        /// @brief Получение задания ПИД.
        float get_assignment();

        void init_param( int par_n, float val );
        void init_work_param( int par_n, float val );

        void save_param();

        /// @brief Использование kN, TiN, TdN.     
        void set_used_par ( int par_n ); 
    };
#endif
