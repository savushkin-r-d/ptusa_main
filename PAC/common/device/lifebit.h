#pragma once

#include "dtime.h"

#include "param_ex.h"
#include "device/device.h"
#include "device/manager.h"

class lifebit : public device, public i_Lua_save_device
    {
    public:
        enum class PARAM
            {
            DT = 1,                 ///< Интервал расчёта.

            PARAMS_COUNT
            };

        /// @param name - имя.
        lifebit( const char* name );

        virtual ~lifebit() = default;

        void evaluate_io() override;

        void set_string_property( const char* field, const char* value ) override;

        int save_device( char* buff ) override;

        const char* get_name_in_Lua() const override;

    private:
        device* di_device = nullptr;
        int prev_di_state = 0;
        unsigned long start_time = get_millisec();
    };
