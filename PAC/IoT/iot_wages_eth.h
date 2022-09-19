#pragma once

#include "iot_base.h"
class tcp_client;

class iot_wages_eth : public i_iot_node
	{
	public:
		iot_wages_eth(unsigned int id, const char* ip, unsigned int port);
		~iot_wages_eth();

		void evaluate();

		int get_state();

		float get_value();

		void direct_set_buff(char* new_value);

	private:
		unsigned int status;
		int state;
		float value;
		tcp_client* tc;
	};