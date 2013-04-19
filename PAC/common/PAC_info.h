#ifndef PAC_INFO_H
#define PAC_INFO_H

#include "dtime.h"

#include "g_device.h"
#include "param_ex.h"

class PAC_info: public i_Lua_save_device
    {
    public:
        virtual ~PAC_info();

        void eval();

        enum PARAMETERS
            {
            P_PARAMS_COUNT  = 3,   ///< Количество параметров.

            P_CTR_ERROR_TIME  = 1, ///< Время ожидания ошибки счетчика, мсек.
            P_MIX_FLIP_PERIOD,     ///< Интервал промывки седел клапанов, сек.
            P_MIX_FLIP_TIME,       ///< Время промывки седел клапанов, мсек.
            };

        saved_params_u_int_4 *par;

        void reset_params();

        int save_device( char *buff );

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
    };
//-----------------------------------------------------------------------------
PAC_info* G_PAC_INFO();
//-----------------------------------------------------------------------------
#endif // PAC_INFO_H
