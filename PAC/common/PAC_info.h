#ifndef PAC_INFO_H
#define PAC_INFO_H

#include "dtime.h"

#include "g_device.h"
#include "param_ex.h"
#include "version_info.h"

class PAC_info: public i_Lua_save_device
    {
    public:
        ~PAC_info() override = default;

        void eval();

        enum PARAMETERS
            {
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

            ///< Время для включения операции по сигналам, мсек.
            P_AUTO_OPERATION_WAIT_TIME,

            ///< Время ожидания для повторной попытки включения операции по
            ///< сигналам, мсек.
            P_AUTO_OPERATION_WARN_TIME,

            ///< Активность сервера OPC UA,
            /// 0 - нет, 1 - да.
            P_IS_OPC_UA_SERVER_ACTIVE,

            ///< Возможность управлять через сервер OPC UA,
            /// 0 - нет, 1 - да.
            P_IS_OPC_UA_SERVER_CONTROL,

            ///< Количество параметров.
            P_PARAMS_COUNT
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

        const char* get_up_time_str() const
            {
            return up_time_str;
            }

        enum COMMANDS
            {
            CLEAR_RESULT_CMD = 0,

            RELOAD_RESTRICTIONS = 100,
            RESET_PARAMS = 101,
            };

#ifdef PTUSA_TEST
        void emulation_on();

        void emulation_off();

        bool emulator_state = true;
#endif

        void set_cycle_time( uint32_t current_cycle_time );

        /// @brief Get nodes communication error status.
        /// @return 0 - all OK, 1 - at least one node has error or PP mode.
        int get_nodes_comm_error() const
            {
            return nodes_comm_error;
            }

        /// @brief Get watchdog error status.
        /// @return 0 - all OK, 1 - at least one watchdog has error (state less than 0).
        int get_watchdog_error() const
            {
            return watchdog_error;
            }

    private:
        PAC_info();

        static auto_smart_ptr < PAC_info > instance;///< Экземпляр класса.

        static const u_int_4 MSEC_IN_DAY;
        enum CONSTANTS
            {
            C_MAX_STR_LENGTH = 50,
            };

        uint32_t up_days;
        uint32_t up_hours;
        uint32_t up_mins;
        uint32_t up_secs;

        uint32_t up_msec;
        uint32_t last_check_time;

        u_int_4 reset_type;

        char up_time_str[ C_MAX_STR_LENGTH ];

        int  cmd;
        char cmd_answer[ 200 ];

        uint32_t restrictions_set_to_off_time;

        uint32_t cycle_time = 0;

        /// @brief Indicator: any node has communication error or warning.
        /// 0 - all OK, 1 - at least one node has error or PP mode active.
        int nodes_comm_error = 0;

        /// @brief Indicator: any watchdog has error state.
        /// 0 - all OK, 1 - at least one watchdog has error (state < 0).
        int watchdog_error = 0;
    };
//-----------------------------------------------------------------------------
PAC_info* G_PAC_INFO();
//-----------------------------------------------------------------------------
#endif // PAC_INFO_H
