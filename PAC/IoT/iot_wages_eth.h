#pragma once

#include "iot_base.h"
#include "smart_ptr.h"
class tcp_client;

class iot_wages_eth : public i_iot_node
	{
	public:
		iot_wages_eth( unsigned int id, char* ip, unsigned int port );

		void evaluate();

		int get_wages_state() const;

		float get_wages_value() const;

		void set_wages_value( float new_value );

	private:
		int status = 0;
		int state = 0;
		float value = .0f;
		auto_smart_ptr < tcp_client > tc;
	};