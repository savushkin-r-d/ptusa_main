#include <bits/errno.h>

#include "l_wago.h"
//-----------------------------------------------------------------------------
int wago_manager_linux::net_init( wago_node *node )
    {
#ifdef DEBUG
    printf( "int wago_manager_linux::net_init( wago_node *node ).\n" );
#endif // DEBUG

    int type = SOCK_STREAM;
    int protocol = 0; /* всегда 0 */
    int err;
    int sock = err = socket( AF_INET, type, protocol ); // Cоздание сокета.

#ifdef DEBUG
    printf( "wago_manager_linux:net_init(...) - socket created. Has number %d.\n",
        sock );
#endif // DEBUG

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

    // Привязка сокета.
    err = connect( sock, ( struct sockaddr * ) &socket_remote_server,
        sizeof( socket_remote_server ) );

    fd_set rdevents;
    struct timeval tv;
    FD_ZERO( &rdevents );
    FD_SET( sock, &rdevents );

    tv.tv_sec  = 0;
    tv.tv_usec = 200000;

    err = select( sock + 1, 0, &rdevents, 0, &tv );
    if ( err <= 0 )
        {
#ifdef DEBUG
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
#endif // DEBUG
        close( sock );
        return -5;
        }

#ifdef DEBUG
    Print( "Socket %d is successfully connected to \"%s\":%d\n",
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
    static bool is_set_err = false;
    
    if( get_sec() - node->last_poll_time > wago_node::C_MAX_WAIT_TIME )
        {
        if( false == is_set_err )
            {
            is_set_err = true;
            PAC_critical_errors_manager::get_instance( )->set_global_error(
                    AC_NO_CONNECTION, AS_WAGO, node->number );
            }
        }
    else
        {
        if( is_set_err )
            {
            is_set_err = false;
            PAC_critical_errors_manager::get_instance( )->reset_global_error(
                    AC_NO_CONNECTION, AS_WAGO, node->number );
            }
        }
    // Проверка связи с узлом Wago.-!>

    // Инициализация сетевого соединения, при необходимости.
    if( node->state != wago_node::ST_OK )
        {
        net_init( node );
        if( node->state != wago_node::ST_OK ) return -100;
        }
    // Инициализация сетевого соединения, при необходимости.-!>

    // Посылка данных.
    int res = send( node->sock, buff, bytes_to_send, 0 );
    if( res < 0 )
        {
#ifdef DEBUG
        perror( "wago_manager::e_communicate(...) send error\n" );
#endif
        disconnect( node );

        return -101;
        }

    // Получение данных.
    res = tcp_communicator_linux::recvtimeout( node->sock, &buff[ 0 ],
        bytes_to_receive, 1, 0, node->ip_address );

    if( res <= 0 ) /* read error */
        {
#ifdef DEBUG
        perror( "wago_manager::e_communicate(...) recvtimeout error\n" );
#endif
        disconnect( node );

        return -102;
        }

    node->last_poll_time = get_sec( );

    return 0;
    }
//-----------------------------------------------------------------------------
void wago_manager_linux::disconnect( wago_node *node )
    {
    Print( "void wago_manager_linux::disconnect( wago_node *node )\n" );
    
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
