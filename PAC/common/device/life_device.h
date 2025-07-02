#pragma once

#include "dtime.h"

#include "param_ex.h"
#include "device/device.h"
#include "device/manager.h"

class watchdog : public device, public i_Lua_save_device
    {
    public:
        enum class PARAM
            {
            P_DT = 1,               ///< Интервал расчёта.

            PARAMS_COUNT
            };

        /// @param name - имя.
        explicit watchdog( const char* name, 
            device::DEVICE_SUB_TYPE sub_type = device::DEVICE_SUB_TYPE::DST_WATCHDOG );

        ~watchdog() override = default;

        void evaluate_io() override;

        void set_string_property( const char* field, const char* value ) override;
        void set_property( const char* field, device* value ) override;

        int save_device( char* buff ) override;

        const char* get_name_in_Lua() const override;

#ifndef PTUSA_TEST
    private:
#endif

        device* dev = nullptr;
        int prev_dev_state = 0;     ///< Предыдущее состояние устройства.
        float prev_dev_value = 0.f; ///< Предыдущее значение устройства.
        unsigned long start_time = get_millisec();
    };
