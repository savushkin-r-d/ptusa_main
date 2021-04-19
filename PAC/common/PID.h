/// @file PID.h
/// @brief Содержит описание класса ПИД-контроллера.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
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

class PID: public device, public i_Lua_save_device
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

    int state;
    float out_value;
    float set_value;

    int used_par_n;
    float start_value;

    bool is_old_style;
    public:
        enum PARAM
            {
            P_k = 1,               ///< Параметр k.
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

            P_out_max,             ///< Мax значение выходной величины.
            P_out_min,             ///< Мin значение выходной величины.

            P_is_reverse,	       ///Обратного (реверсивного) действия.
            P_is_zero_start,	   ///Нулевое стартовое значение.

            PARAMS_COUNT
            };

        /// TODO Удалить после обновления, оставлено для совместимости.
        enum WORK_PARAM
            {
            WP_Z = 1,  ///< Требуемое значение.
            WP_U,      ///< Выход ПИД.
            };

        /// @param n - номер.
        ///
        /// Устаревший вариант. Оставлен для совместимости.
        PID( int n );

        /// @param name - имя.
        PID( const char *name );

        virtual ~PID();

        void  direct_on();

        void  direct_set_state( int st )
            {
            }

        /// @brief Выключение ПИД.
        void  direct_off()
            {
            if ( state != STATE_OFF )
                {
                state = STATE_OFF;
                }
            }

        void  direct_set_value( float val )
            {
            }

        float get_value()
            {
            return out_value;
            }

		/// @brief Сброс ПИД
		void reset();

        /// @brief Работа ПИД.
        float eval( float current_value, int delta_sign );

        /// @brief Установка нового задания ПИД.
        void set( float new_z );

        /// @brief Получение задания ПИД.
        float get_assignment();

        /// @brief Инициализация параметра ПИД.
        void init_param( PARAM par_n, float val );

        /// @brief Инициализация рабочего параметра ПИД.
        void init_work_param( int par_n, float val );

        /// @brief Сохранение параметров ПИД.
        void save_param();

        /// @brief Использование kN, TiN, TdN.
        void set_used_par ( int par_n );

        int save_device( char *buff );

#ifdef RM_PAC
        int rm_save_device_state( char *buff )
            {
            return 0;
            }
#endif // RM_PAC

		/// @brief Состояние регулятора
		int get_state();

		int set_cmd( const char *prop, u_int idx, double val );

        const char* get_name_in_Lua() const;
	};
#endif
