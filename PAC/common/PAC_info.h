#ifndef PAC_INFO_H
#define PAC_INFO_H

#include "dtime.h"

#include "g_device.h"
#include "param_ex.h"
#include "version_info.h"

class PAC_info: public i_Lua_save_device
    {
    public:
        virtual ~PAC_info();

        void eval();

        enum PARAMETERS
            {
            P_PARAMS_COUNT  = 10,   ///< Количество параметров.

            P_MIX_FLIP_PERIOD = 1, ///< Интервал промывки седел клапанов, сек.
            P_MIX_FLIP_UPPER_TIME, ///< Время промывки верхних седел клапанов, мсек.
			P_MIX_FLIP_LOWER_TIME, ///< Время промывки нижних седел клапанов, мсек

            P_V_OFF_DELAY_TIME,    ///< Время задержки закрытия клапанов, мсек.

            ///< Время задержки закрытия для донных клапанов, мсек.
            P_V_BOTTOM_OFF_DELAY_TIME,

	        ///< Среднее время задержки получения ответа от узла I/O, мсек.
	        P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME,
            ///< Среднее время цикла программы, мсек.
            P_MAIN_CYCLE_WARN_ANSWER_AVG_TIME,


            ///< Работа модуля ограничений.
            /// 0 - авто, 1 - ручной, 2 - полуручной (через время
            /// @P_RESTRICTIONS_MANUAL_TIME вернется в автоматический режим).
            P_RESTRICTIONS_MODE,

            ///< Работа модуля ограничений в ручном режиме заданное время.
            P_RESTRICTIONS_MANUAL_TIME,

            ///< Переход на паузу операции при ошибке устройств,
            /// 0 - авто (есть), 1 - ручной (нет).
            P_AUTO_PAUSE_OPER_ON_DEV_ERR,
            };

        saved_params_u_int_4 par;

        void reset_params();

        int save_device( char *buff );

        bool is_emulator();

        void print() const;

        /// @brief Получение единственного экземпляра класса для работы.
        ///
        /// @return - указатель на единственный объект класса @ref PAC_info.
        static PAC_info* get_instance();

        int set_cmd( const char *prop, u_int idx, double val );

        const char* get_name_in_Lua() const
            {
            return "SYSTEM";
            }

        enum COMMANDS
            {
            RELOAD_RESTRICTIONS = 100,
            RESET_PARAMS,
            };

    private:
        PAC_info();

        static auto_smart_ptr < PAC_info > instance;///< Экземпляр класса.

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

        char up_time_str[ C_MAX_STR_LENGTH ];

        int  cmd;
        char cmd_answer[ 200 ];

        u_int_4 restrictions_set_to_off_time;
    };
//-----------------------------------------------------------------------------
PAC_info* G_PAC_INFO();
//-----------------------------------------------------------------------------
#endif // PAC_INFO_H
