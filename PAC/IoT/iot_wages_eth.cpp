#include "iot_wages_eth.h"
#include "tcp_client.h"

iot_wages_eth::iot_wages_eth( unsigned int id, char* ip, unsigned int port )
	{
	tc = tcp_client::Create( ip, port, id, 0 , 256u, 300u); 
	}

void iot_wages_eth::evaluate()
	{
	status = tc->Communicate( 0 );
	convert_value();
	}

int iot_wages_eth::get_wages_state() const
	{
	return state;
	}

float iot_wages_eth::get_wages_value() const
	{
	return value;
	}

void iot_wages_eth::set_wages_value( float new_value )
	{
	if ( new_value >= .0f ) value = new_value;
	}

void iot_wages_eth::convert_value() 
	{
	if ( tc->buff[ 8 ] != 'k' || tc->buff[ 9 ] != 'g' || !status )
		{
		state = 0;
		return;
		}
	else
		{
		state = 1;
		value = static_cast<float>( atof( tc->buff + 1 ) );
		}
	}

void iot_wages_eth::direct_set_tcp_buff( char* new_value, int new_status )
	{
	strcpy( tc->buff, new_value );
	status = new_status;
	convert_value();
	}

