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

    int state;

    int start_param_index;  
    enum PARAM 
        {
        PAR_Z,            //0 Требуемое значение.
        PAR_k,            //1 Параметр k.
        PAR_Ti,           //2 Параметр Ti.
        PAR_Td,           //3 Параметр Td.
        PAR_dt,           //4 Интервал расчёта
        PAR_dmax,         //5 Мax значение входной величины.
        PAR_dmin,         //6 Мin значение входной величины.
        PAR_AccelTime,    //7 Время выхода на режим регулирования.
        PAR_IsManualMode, //8 Ручной режим.
        PAR_UManual,      //9 Заданное ручное значение выходного сигнала.
        PAR_Uk,           //10 Выход ПИД.

        PAR_k2,           //11 Параметр k2.
        PAR_Ti2,          //12 Параметр Ti2.
        PAR_Td2,          //13 Параметр Td2. 
        }; 

    int start_work_params_index;
    enum WORK_PARAMS 
        {
        WPAR_Uk,          //1 Выход ПИД-регулятора.
        WPAR_Z,           //2 Требуемое значение ПИД.
        };

    enum ADDITIONAL_PARAM 
        {    
   
        };

    int   used_par_n;

    public:
        PID( saved_params_float* par, run_time_params_float *w_par,
            int start_param_index = 0, 
            int start_work_params_index = 0 );

        ~PID(); 
                
        void  on( char is_down_to_inaccel_mode = 0 );
        void  off();
        float eval( float current_value, int delta_sign = 1 );

        void  reset();
        void  reset( float new_uk_1 );
        void  set( float new_z );         //Установить новое задание ПИД.

        void init_param( int par_n, float val );
        void init_work_param( int par_n, float val );
        void save_param();

        void set_used_par ( int par_n ); //Использовать kN, TiN, TdN.       
    };
#endif
