#include "iot_wages_eth.h"
#include "tcp_client.h"

iot_wages_eth::iot_wages_eth( unsigned int id, char* ip, unsigned int port )
	{
	tc = tcp_client::Create( ip, port, id, 0 );
	}

void iot_wages_eth::evaluate()
	{
	status = tc->Communicate( 0 );

	if ( !status )
		{
		state = 0;
		return;
		}
	}

int iot_wages_eth::get_wages_state() const
	{
	return state;
	}

float iot_wages_eth::get_wages_value()
	{
	if ( tc->buff[ 2 ] != '+' )
		{
		state = -1;
		return value;
		}

	std::swap( tc->buff[ 6 ], tc->buff[ 7 ] );
	std::swap( tc->buff[ 8 ], tc->buff[ 9 ] );
	std::swap( tc->buff[ 10 ], tc->buff[ 11 ] );
	state = 1;
	value = static_cast<float>( atof( tc->buff + 6 ) );
	return value;
	}

void iot_wages_eth::direct_set_buff( char* new_value )
	{
	tc->buff = new_value;
	}

void iot_wages_eth::set_wages_value( float new_value )
	{
	if ( new_value >= .0f ) value = new_value;
	}

