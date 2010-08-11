#include "tcp_cmctr.h"

auto_smart_ptr < tcp_communicator > tcp_communicator::instance = 0;
//------------------------------------------------------------------------------
tcp_communicator::tcp_communicator()
    {
    buf                 = new u_char[ BUFSIZE ];
    is_going_to_reboot  = 0;
    max_cycles          = 4;
    glob_cmctr_ok       = 1;
    for ( int i = 0; i < TC_MAX_SERVICE_NUMBER; i++ ) services[ i ] = NULL;
    }
//------------------------------------------------------------------------------
tcp_communicator::srv_ptr tcp_communicator::reg_service( u_char srv_id,
    srv_ptr fk )
	{
	if ( services[ srv_id ] == NULL )
		{
		services[ srv_id ] = fk;
		return NULL;
		}
	else
		{
		return services[ srv_id ];
		}
	}
//------------------------------------------------------------------------------
void tcp_communicator::_ErrorAkn( u_char error )
	{
	buf[ 0 ] = net_id;
	buf[ 1 ] = AKN_ERR;
	buf[ 2 ] = pidx;
	buf[ 3 ] = 0;
	buf[ 4 ] = 1;
	buf[ 5 ] = error;
	in_buffer_count = 6;
	}
//------------------------------------------------------------------------------
void tcp_communicator::_AknData( u_long len )
	{
	buf[ 0 ] = net_id;
	buf[ 1 ] = AKN_OK;
	buf[ 2 ] = pidx;
	buf[ 3 ] = ( len >> 8 ) & 0xFF;
	buf[ 4 ] = len & 0xFF;
	in_buffer_count = len + 5;
	}
//------------------------------------------------------------------------------
void tcp_communicator::_AknOK()
	{
	buf[ 0 ] = net_id;
	buf[ 1 ] = AKN_OK;
	buf[ 2 ] = pidx;
	buf[ 3 ] = 0;
	buf[ 4 ] = 0;
	in_buffer_count = 5;
	}
//------------------------------------------------------------------------------
tcp_communicator* tcp_communicator::get_instance()
    {
    return instance;
    }
//------------------------------------------------------------------------------
void tcp_communicator::set_instance( tcp_communicator* new_instance )
    {
    instance = new_instance;
    }
//------------------------------------------------------------------------------
char* tcp_communicator::get_host_name()
    {
    return host_name;
    }
//------------------------------------------------------------------------------
tcp_communicator::~tcp_communicator()
    {
    if ( buf )
        {
        delete [] buf;
        buf = 0;
        }
    }
//------------------------------------------------------------------------------


