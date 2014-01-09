#include <bits/errno.h>

#include "l_wago.h"

extern int errno;
//-----------------------------------------------------------------------------
int wago_manager_linux::net_init( wago_node *node )
    {
    int type = SOCK_STREAM;
    int protocol = 0; /* всегда 0 */
    int err;
    int sock = err = socket( AF_INET, type, protocol ); // Cоздание сокета.

    if( sock < 0 )
        {
#ifdef DEBUG
        perror( "wago_manager_linux:net_init(...) - can't create socket" );
#endif // DEBUG
        return -4;
        }

    // Адресация мастер-сокета.
    struct sockaddr_in socket_remote_server;
    const int PORT = 502;
    memset( &socket_remote_server, 0, sizeof( socket_remote_server ) );
    socket_remote_server.sin_family 	 = AF_INET;
    socket_remote_server.sin_addr.s_addr = inet_addr( node->ip_address );
    socket_remote_server.sin_port        = htons ( PORT );

    const int C_ON = 1;

    if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &C_ON, sizeof( C_ON ) ) )
        {
        perror( "wago_manager_linux:net_init(...) - ошибка  вызова  setsockopt" );
        close( sock );
        return -5;
        }

    // Переводим в неблокирующий режим.
    err = fcntl( sock, F_SETFL, O_NONBLOCK );
    if ( err != 0 )
        {
#ifdef DEBUG
        perror( "wago_manager_linux:net_init(...) - ошибка  вызова  fcntl" );
#endif
        close( sock );
        return -5;
        }

    // Привязка сокета. Сразу возвращает управление в неблокирующем режиме.
    err = connect( sock, ( struct sockaddr * ) &socket_remote_server,
        sizeof( socket_remote_server ) );

    fd_set rdevents;
    struct timeval tv;
    FD_ZERO( &rdevents );
    FD_SET( sock, &rdevents );

    tv.tv_sec  = 0;
    tv.tv_usec = wago_node::C_TIMEOUT_US;

    err = select( sock + 1, 0, &rdevents, 0, &tv );

    if ( err <= 0 )
        {
#ifdef DEBUG
        if ( node->is_set_err == false )
            {
            if ( err < 0 )
                {
                printf( "wago_manager_linux:net_init(...) - can't connect to \"%s\":%d : %s\n",
                    node->ip_address, PORT, strerror( err ) );
                }
            else // = 0
                {
                printf( "wago_manager_linux:net_init(...) - can't connect to \"%s\":%d : timeout\n",
                    node->ip_address, PORT );
                }
            }
#endif // DEBUG

        close( sock );
        return -5;
        }

    if( FD_ISSET( sock, &rdevents ) )
        {
        socklen_t err_len;
        int error;

        err_len = sizeof( error );
        if( getsockopt( sock, SOL_SOCKET, SO_ERROR, &error, &err_len ) < 0 || error != 0 )
            {
#ifdef DEBUG
            if ( node->is_set_err == false )
                {
                perror( "wago_manager::net_init(...) select error" );
                }
#endif // DEBUG

            close( sock );
            return -6;
            }
        }

#ifdef DEBUG
    printf( "Socket %d is successfully connected to \"%s\":%d\n",
        sock, node->ip_address, PORT );
#endif // DEBUG

    node->sock   = sock;
    node->state  = wago_node::ST_OK;

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_manager_linux::e_communicate( wago_node *node, int bytes_to_send,
    int bytes_to_receive )
    {
    // Проверка связи с узлом Wago.
    if( get_sec() - node->last_poll_time > wago_node::C_MAX_WAIT_TIME )
        {
        if( false == node->is_set_err )
            {
            node->is_set_err = true;
            PAC_critical_errors_manager::get_instance( )->set_global_error(
                PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_WAGO, node->number );
            }
        }
    else
        {
        if( node->is_set_err )
            {
            node->is_set_err = false;
            PAC_critical_errors_manager::get_instance( )->reset_global_error(
                PAC_critical_errors_manager::PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_WAGO, node->number );
            }
        }
    // Проверка связи с узлом Wago.-!>

    // Инициализация сетевого соединения, при необходимости.
    if( node->state != wago_node::ST_OK )
        {
        if ( get_delta_millisec( node->last_init_time ) < node->delay_time )
            {
            return 1;
            }

        net_init( node );
        if( node->state != wago_node::ST_OK )
            {
            node->last_init_time = get_millisec();
            if ( node->delay_time < wago_node::C_MAX_DELAY )
                {
                node->delay_time += 10000 + node->number * 200;
                }

            return -100;
            }
        }
    // Инициализация сетевого соединения, при необходимости.-!>

    node->delay_time = 0;

    // Посылка данных.
    int res = send( node->sock, buff, bytes_to_send, 0 );
    if( res < 0 )
        {
#ifdef DEBUG
        if ( node->is_set_err == false )
            {
            perror( "wago_manager::e_communicate(...) send error" );
            }
#endif
        disconnect( node );

        return -101;
        }

    // Получение данных.
    res = tcp_communicator_linux::recvtimeout( node->sock, buff,
        bytes_to_receive, 0, wago_node::C_TIMEOUT_US, node->ip_address );

    if( res <= 0 ) /* read error */
        {
#ifdef DEBUG
        if ( node->is_set_err == false )
            {
            perror( "wago_manager::e_communicate(...) recvtimeout error\n" );
            }
#endif
        disconnect( node );

        return -102;
        }

    node->last_poll_time = get_sec();

    return 0;
    }
//-----------------------------------------------------------------------------
void wago_manager_linux::disconnect( wago_node *node )
    {
    shutdown( node->sock, SHUT_RDWR );
    close( node->sock );
    node->sock = 0;

    node->state = wago_node::ST_NO_CONNECT;
    }
//-----------------------------------------------------------------------------
wago_manager_linux::wago_manager_linux()
    {
    }
//-----------------------------------------------------------------------------
wago_manager_linux::~wago_manager_linux()
    {
    }
//-----------------------------------------------------------------------------
