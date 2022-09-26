#pragma once

#include "iot_base.h"
#include "smart_ptr.h"
class tcp_client;

class iot_wages_eth : public i_iot_node
	{
	public:
		iot_wages_eth( unsigned int id, char* ip, unsigned int port );

		void evaluate();

		int get_state() const;

		float get_value();

		void set_value(float new_value);

		void direct_set_buff( char* new_value );

	private:
		int status = 0;
		int state = 0;
		float value = .0f;
		auto_smart_ptr < tcp_client > tc;
	};