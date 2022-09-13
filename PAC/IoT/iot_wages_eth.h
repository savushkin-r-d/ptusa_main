#pragma once

#include "iot_base.h"
#include <tcp_client.h>

class iot_wages_eth : public i_iot_node
	{
	public:
		iot_wages_eth(unsigned int id, const char* ip, unsigned int port);
		~iot_wages_eth();

		void evaluate();

		int get_state();

		float get_value();

		void tare();

	private:
		unsigned int status;
		int state;
		float value;
		tcp_client* tc;
	};