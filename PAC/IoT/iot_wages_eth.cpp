#include <iot_wages_eth.h>

wages_eth::wages_eth(unsigned int id, const char* ip, unsigned int port)
	{
	tcp_client* tc = tcp_client::Create((char*)ip, port, id, 0);
	}

wages_eth::~wages_eth()
	{
	delete tc;
	}

void wages_eth::evaluate()
	{
	status = tc->Communicate(0);

	if (!status)
	{
		state = 0;
		value = .0f;
	}

	if (tc->buff[2] != '+')                                                
	{
		state = -1;
		value = 0.0f;
	}

	std::swap(tc->buff[6], tc->buff[7]);
	std::swap(tc->buff[8], tc->buff[9]);
	std::swap(tc->buff[10], tc->buff[11]);
	state = 1;
	value = static_cast<float>(atof(tc->buff + 6));
	}

int wages_eth::get_state()
	{
	return state;
	}

float wages_eth::get_value()
	{
	return value;
	}

void wages_eth::tare()
	{	
	}