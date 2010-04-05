#include <string.h>

#include "tcp_cmctr.h"

tcp_communicator * tcp_communicator::instance = 0;

//------------------------------------------------------------------------------
tcp_communicator::tcp_communicator()
    {
    buf = new u_char[ BUFSIZE ];
    reboot = 0;
    max_cycles = 1;
    glob_cmctr_ok = 1;
    for ( int i = 0; i < TC_MAX_SERVICE_NUMBER; i++ ) services[ i ] = NULL;
    }
//------------------------------------------------------------------------------
srv_ptr tcp_communicator::reg_service( u_char srv_id, srv_ptr fk );
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
	buf[ 0 ] = NetId;
	buf[ 1 ] = AknErr;
	buf[ 2 ] = pidx;
	buf[ 3 ] = 0;
	buf[ 4 ] = 1;
	buf[ 5 ] = error ;
	inBufCnt = 6;
	}
//------------------------------------------------------------------------------
void tcp_communicator::_AknData( u_long len )
	{
	buf[ 0 ] = NetId;
	buf[ 1 ] = AknOK;
	buf[ 2 ] = pidx;
	buf[ 3 ] = ( len >> 8 ) & 0xFF;
	buf[ 4 ] = len & 0xFF;
	inBufCnt = len + 5;
	}
//------------------------------------------------------------------------------
void tcp_communicator::_AknOK()
	{
	buf[ 0 ] = NetId;
	buf[ 1 ] = AknOK;
	buf[ 2 ] = pidx;
	buf[ 3 ] = 0;
	buf[ 4 ] = 0;
	inBufCnt = 5;
	}
//------------------------------------------------------------------------------
tcp_communicator* tcp_communicator::get_instance()
    {
    return instance;
    }
//------------------------------------------------------------------------------
char* tcp_communicator::get_host_name()
    {
    return host_name;
    }
//------------------------------------------------------------------------------

