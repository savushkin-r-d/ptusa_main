//#pragma once

#include "iot_base.h"
#include <tcp_client.h>

class wages_eth : public i_iot_node
	{
	public:
		wages_eth(unsigned int id, const char* ip, unsigned int port);
		~wages_eth();

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
