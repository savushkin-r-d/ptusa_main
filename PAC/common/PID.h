/// @file PID.h
/// @brief Содержит описание класса ПИД-контроллера.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG    - компиляция c выводом отладочной информации в консоль.@n@n
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef _PID_H
#define _PID_H

#include "dtime.h"

#include "param_ex.h"
#include "PAC_dev.h"
//#include "sys.h"

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

    /// @brief Надо ли при старте регулятора уменьшать, а не увеличивать
    /// выходную величину.
    char is_down_to_inaccel_mode;  

    saved_params_float    *par;
    run_time_params_float *w_par;

    u_int_4 state;

    int   used_par_n;

    void reset();
    void acceleration( float accel_time );

    float start_value;

    int number; ///< Номер ПИД.

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

        enum WORK_PARAM 
            {
            WP_Z,  ///< Требуемое значение.
            WP_U,  ///< Выход ПИД.
            };

        PID( int n );
        ~PID(); 
                
        /// @brief Включение ПИД.
        void  on( char is_down_to_inaccel_mode = 0 );

        /// @brief Выключение ПИД.
        void  off();

        /// @brief Работа ПИД.
        float eval( float current_value, int delta_sign = 1 );

        /// @brief Установка нового задания ПИД.
        void set( float new_z );    

        /// @brief Получение задания ПИД.
        float get_assignment();

        /// @brief Инициализация параметра ПИД.
        void init_param( PARAM par_n, float val );

        /// @brief Инициализация рабочего параметра ПИД.
        void init_work_param( WORK_PARAM par_n, float val );

        /// @brief Сохранение параметров ПИД.
        void save_param();

        /// @brief Отладочный вывод в консоль ПИД.
        void print() const;

        /// @brief Использование kN, TiN, TdN.     
        void set_used_par ( int par_n ); 
    };
#endif
