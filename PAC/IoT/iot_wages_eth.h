#pragma once

#include "iot_base.h"
#include <memory>

class tcp_client;

class iot_wages_eth : public i_iot_node
    {
    public:
        iot_wages_eth( unsigned int id, char* ip, unsigned int port );

        void evaluate();

        int get_wages_state() const;

        float get_wages_value() const;

        void set_wages_value( float new_value );

        void convert_value();

        void direct_set_tcp_buff( char* new_value, int new_status );

    private:
        enum class CONSTANTS
            {
            BUFF_SIZE = 256,
            };

        int status = 0;
        int state = 0;
        float value = .0f;
        std::unique_ptr < tcp_client > tc;
    };
