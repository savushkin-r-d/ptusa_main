#if !defined WIN_OS && !defined LINUX_OS && !defined MINIOS7
#error You must define OS!
#endif

#include "tcp_cmctr.h"

#ifdef WIN_OS
#include "w_tcp_cmctr.h"
#endif // WIN_OS

#ifdef LINUX_OS
#include "l_tcp_cmctr.h"
#endif

#ifdef MINIOS7
#include "mos7_tcp_cmctr.h"
#endif

auto_smart_ptr < tcp_communicator > tcp_communicator::instance = 0;
//------------------------------------------------------------------------------
tcp_communicator::tcp_communicator()
    {
    is_going_to_reboot  = 0;
    max_cycles          = 20;
    glob_cmctr_ok       = 1;
    for ( int i = 0; i < TC_MAX_SERVICE_NUMBER; i++ ) services[ i ] = NULL;

    memset( host_name_rus, 0, TC_MAX_HOST_NAME );
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
char* tcp_communicator::get_host_name_rus()
    {
    return host_name_rus;
    }
//------------------------------------------------------------------------------
char* tcp_communicator::get_host_name_eng()
    {
    return host_name_eng;
    }
//------------------------------------------------------------------------------
tcp_communicator::~tcp_communicator()
    {
    }
//------------------------------------------------------------------------------
void tcp_communicator::init_instance( const char *name_rus, const char *name_eng )
    {
#ifdef WIN_OS
    instance = new tcp_communicator_win( name_rus, name_eng );
#endif // WIN_OS

#ifdef LINUX_OS
    instance = new tcp_communicator_linux( name_rus, name_eng );
#endif

#ifdef MINIOS7
    instance = new tcp_communicator_mos7( name_rus );
#endif
    }
//------------------------------------------------------------------------------
