#include <errno.h>

#include "l_bus_coupler_io.h"
#include "log.h"


extern int errno;
//-----------------------------------------------------------------------------
int io_manager_linux::net_init( io_node* node )
    {
    int type = SOCK_STREAM;
    int protocol = 0; /* всегда 0 */
    int err;
    int sock = err = socket( AF_INET, type, protocol ); // Cоздание сокета.

    if ( sock < 0 )
        {
        sprintf( G_LOG->msg,
            "Network communication : can't create I/O node socket : %s.",
            strerror( errno ) );
        G_LOG->write_log( i_log::P_CRIT );

        return -4;
        }

    // Адресация мастер-сокета.
    struct sockaddr_in socket_remote_server;
    const int PORT = 502;
    memset( &socket_remote_server, 0, sizeof( socket_remote_server ) );
    socket_remote_server.sin_family = AF_INET;
    socket_remote_server.sin_addr.s_addr = inet_addr( node->ip_address );
    socket_remote_server.sin_port = htons( PORT );

    const int C_ON = 1;

    if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &C_ON, sizeof( C_ON ) ) )
        {
        sprintf( G_LOG->msg,
            "Network communication : can't setsockopt I/O node socket : %s.",
            strerror( errno ) );
        G_LOG->write_log( i_log::P_CRIT );

        close( sock );
        return -5;
        }

    // Переводим в неблокирующий режим.
    err = fcntl( sock, F_SETFL, O_NONBLOCK );
    if ( err != 0 )
        {
        sprintf( G_LOG->msg,
            "Network communication : can't fcntl I/O node socket : %s.",
            strerror( errno ) );
        G_LOG->write_log( i_log::P_CRIT );

        close( sock );
        return -5;
        }

    // Привязка сокета. Сразу возвращает управление в неблокирующем режиме.
    err = connect( sock, ( struct sockaddr* ) & socket_remote_server,
        sizeof( socket_remote_server ) );

    fd_set rdevents;
    struct timeval tv;
    FD_ZERO( &rdevents );
    FD_SET( sock, &rdevents );

    tv.tv_sec = 0;
    tv.tv_usec = io_node::C_CNT_TIMEOUT_US;

    err = select( sock + 1, 0, &rdevents, 0, &tv );

    if ( err <= 0 )
        {
        if ( node->is_set_err == false )
            {
            if ( err < 0 )
                {
                sprintf( G_LOG->msg,
                    "Network device : s%d->\"%s\":\"%s\""
                    " can't connect : %s.",
                    sock, node->name, node->ip_address, strerror( errno ) );
                G_LOG->write_log( i_log::P_CRIT );
                }
            else // = 0
                {
                sprintf( G_LOG->msg,
                    "Network device : s%d->\"%s\":\"%s\""
                    " can't connect : timeout (%d ms).",
                    sock, node->name, node->ip_address,
                    io_node::C_CNT_TIMEOUT_US / 1000 );

                G_LOG->write_log( i_log::P_CRIT );
                }
            }

        close( sock );
        return -5;
        }

    if ( FD_ISSET( sock, &rdevents ) )
        {
        socklen_t err_len;
        int error;

        err_len = sizeof( error );
        if ( getsockopt( sock, SOL_SOCKET, SO_ERROR, &error, &err_len ) < 0 || error != 0 )
            {
            if ( node->is_set_err == false )
                {
                sprintf( G_LOG->msg,
                    "Network device : s%d->\"%s\":\"%s\""
                    " error during connect : %s.",
                    sock, node->name, node->ip_address, strerror( errno ) );
                G_LOG->write_log( i_log::P_CRIT );
                }

            close( sock );
            return -6;
            }
        }

    if ( G_DEBUG )
        {
        printf( "io_manager_linux:net_init() : socket %d is successfully"
            " connected to \"%s\":\"%s\":%d\n",
            sock, node->name, node->ip_address, PORT );
        }

    node->sock = sock;
    node->state = io_node::ST_OK;

    return 0;
    }
//-----------------------------------------------------------------------------
int io_manager_linux::write_outputs()
    {
    if ( 0 == nodes_count ) return 0;

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        io_node* nd = nodes[ i ];
        if ( nd->type == io_node::WAGO_750_XXX_ETHERNET || 
			nd->type == io_node::PHOENIX_BK_ETH )
            {
            if ( !nd->is_active )
                {
                continue;
                }

            if ( nd->DO_cnt > 0 )
                {
                u_int bytes_cnt = nd->DO_cnt / 8 + ( nd->DO_cnt % 8 > 0 ? 1 : 0 );

                buff[ 0 ] = 's';
                buff[ 1 ] = 's';
                buff[ 2 ] = 0;
                buff[ 3 ] = 0;
                buff[ 4 ] = 0;
                buff[ 5 ] = 7 + bytes_cnt;
                buff[ 6 ] = 0; //nodes[ i ]->number;
                buff[ 7 ] = 0x0F;
                buff[ 8 ] = 0;
                buff[ 9 ] = 0;
                buff[ 10 ] = (unsigned char)nd->DO_cnt >> 8;
                buff[ 11 ] = (unsigned char)nd->DO_cnt & 0xFF;
                buff[ 12 ] = bytes_cnt;

                for ( u_int j = 0, idx = 0; j < bytes_cnt; j++ )
                    {
                    u_char b = 0;
                    for ( u_int k = 0; k < 8; k++ )
                        {
                        if ( idx < nd->DO_cnt )
                            {
                            b = b | ( nd->DO_[ idx ] & 1 ) << k;
                            idx++;
                            }
                        }
                    buff[ j + 13 ] = b;
                    }

                if ( e_communicate( nd, bytes_cnt + 13, 12 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x0F )
                        {
                        memcpy( nd->DO, nd->DO_, nd->DO_cnt );
                        }
                    }// if ( e_communicate( nd, bytes_cnt + 13, 12 ) > 0 )
                else
                    {
                    if ( G_DEBUG )
                        {
                        //printf("\nWrite DO: returned error...\n");
                        }
                    }
                }// if ( nd->DO_cnt > 0 )

            if ( nd->AO_cnt > 0 )
                {
                u_int bytes_cnt = nd->AO_size;

                buff[ 0 ] = 's';
                buff[ 1 ] = 's';
                buff[ 2 ] = 0;
                buff[ 3 ] = 0;
                buff[ 4 ] = 0;
                buff[ 5 ] = 7 + bytes_cnt;
                buff[ 6 ] = 0; //nodes[ i ]->number;
                buff[ 7 ] = 0x10;
                buff[ 8 ] = 0;
                buff[ 9 ] = 0;
                buff[ 10 ] = bytes_cnt / 2 >> 8;
                buff[ 11 ] = bytes_cnt / 2 & 0xFF;
                buff[ 12 ] = bytes_cnt;

                int l = 0;
                for ( unsigned int idx = 0; idx < nd->AO_cnt; idx++ )
                    {
                    switch ( nd->AO_types[ idx ] )
                        {
                        case 638:
                            buff[ 13 + l ] = 0;
                            buff[ 13 + l + 1 ] = 0;
                            buff[ 13 + l + 2 ] = 0;
                            buff[ 13 + l + 3 ] = 0;
                            l += 4;
                            break;

                        default:
                            buff[ 13 + l ] = (u_char)( ( nd->AO_[ idx ] >> 8 ) & 0xFF );
                            buff[ 13 + l + 1 ] = (u_char)( nd->AO_[ idx ] & 0xFF );
                            l += 2;
                            break;
                        }
                    }

                if ( e_communicate( nd, bytes_cnt + 13, 12 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x10 )
                        {
                        memcpy( nd->AO, nd->AO_, sizeof( nd->AO ) );
                        }
                    }// if ( e_communicate( nd, 2 * bytes_cnt + 13, 12 ) == 0 )
                else
                    {
                    if ( G_DEBUG )
                        {
                        //printf("\nWrite AO: returned error...\n");
                        }
                    }
                }// if ( nd->AO_cnt > 0 )

            }// if ( nd->type == io_node::T_750_341 || ...
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return 0;
    }
//-----------------------------------------------------------------------------
int io_manager_linux::e_communicate( io_node* node, int bytes_to_send,
    int bytes_to_receive )
    {
    // Проверка связи с узлом I/O.
    if ( get_delta_millisec( node->last_poll_time ) > io_node::C_MAX_WAIT_TIME )
        {
        if ( false == node->is_set_err )
            {
            node->is_set_err = true;
            PAC_critical_errors_manager::get_instance()->set_global_error(
                PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_IO_COUPLER, node->number );
            }
        }
    else
        {
        if ( node->is_set_err )
            {
            node->is_set_err = false;
            PAC_critical_errors_manager::get_instance()->reset_global_error(
                PAC_critical_errors_manager::PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_IO_COUPLER, node->number );
            }
        }
    // Проверка связи с узлом I/O.-!>

    // Инициализация сетевого соединения, при необходимости.
    if ( node->state != io_node::ST_OK )
        {
        if ( get_delta_millisec( node->last_init_time ) < node->delay_time )
            {
            return 1;
            }

        net_init( node );
        if ( node->state != io_node::ST_OK )
            {
            node->last_init_time = get_millisec();
            if ( node->delay_time < io_node::C_MAX_DELAY )
                {
                node->delay_time += 10000 + node->number * 200;
                }
            return -100;
            }
        }
    // Инициализация сетевого соединения, при необходимости.-!>

    node->delay_time = 0;

    // Посылка данных.
    int res = tcp_communicator_linux::sendall( node->sock, buff,
        bytes_to_send, 0, io_node::C_RCV_TIMEOUT_US, node->ip_address,
        node->name, &node->send_stat );

    if ( res < 0 )
        {
        disconnect( node );
        return -101;
        }

    // Получение данных.
    res = tcp_communicator_linux::recvtimeout( node->sock, buff,
        bytes_to_receive, 0, io_node::C_RCV_TIMEOUT_US, node->ip_address,
        node->name, &node->recv_stat );

    if ( res <= 0 ) /* read error */
        {
        disconnect( node );
        return -102;
        }
    node->last_poll_time = get_millisec();

    return 0;
    }
//-----------------------------------------------------------------------------
int io_manager_linux::read_inputs()
    {
    if ( 0 == nodes_count ) return 0;

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        io_node* nd = nodes[ i ];

        if ( nd->type == io_node::WAGO_750_XXX_ETHERNET ||
			nd->type == io_node::PHOENIX_BK_ETH ) // Ethernet I/O nodes.
            {
            if ( !nd->is_active )
                {
                continue;
                }

            if ( nd->DI_cnt > 0 )
                {
                buff[ 0 ] = 's';
                buff[ 1 ] = 's';
                buff[ 2 ] = 0;
                buff[ 3 ] = 0;
                buff[ 4 ] = 0;
                buff[ 5 ] = 6;
                buff[ 6 ] = 0; //nd->number;
                buff[ 7 ] = 0x02;
                buff[ 8 ] = 0;
                buff[ 9 ] = 0;
                buff[ 10 ] = (unsigned char)nd->DI_cnt >> 8;
                buff[ 11 ] = (unsigned char)nd->DI_cnt & 0xFF;

                u_int bytes_cnt = nd->DI_cnt / 8 + ( nd->DI_cnt % 8 > 0 ? 1 : 0 );

                if ( e_communicate( nd, 12, bytes_cnt + 9 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x02 && buff[ 8 ] == bytes_cnt )
                        {
                        for ( u_int j = 0, idx = 0; j < bytes_cnt; j++ )
                            {
                            for ( int k = 0; k < 8; k++ )
                                {
                                if ( idx < nd->DI_cnt )
                                    {
                                    nd->DI[ idx ] = ( buff[ j + 9 ] >> k ) & 1;
#ifdef DEBUG_KBUS
                                    printf( "%d -> %d, ", idx, nd->DI[ idx ] );
#endif // DEBUG_KBUS
                                    idx++;
                                    }
                                }
                            }
#ifdef DEBUG_KBUS
                        printf( "\n" );
#endif // DEBUG_KBUS
                        }
                    else
                        {
                        if ( G_DEBUG )
                            {
                            //printf("\nRead DI:I/O returned error...\n");
                            }
                        }
                    }// if ( e_communicate( nd, 12, bytes_cnt + 9 ) == 0 )
                }// if ( nd->DI_cnt > 0 )

            if ( nd->AI_cnt > 0 )
                {
                buff[ 0 ] = 's';
                buff[ 1 ] = 's';
                buff[ 2 ] = 0;
                buff[ 3 ] = 0;
                buff[ 4 ] = 0;
                buff[ 5 ] = 6;
                buff[ 6 ] = 0; //nd->number;
                buff[ 7 ] = 0x04;
                buff[ 8 ] = 0;
                buff[ 9 ] = 0;

                u_int bytes_cnt = nd->AI_size;

                buff[ 10 ] = (unsigned char)bytes_cnt / 2 >> 8;
                buff[ 11 ] = (unsigned char)bytes_cnt / 2 & 0xFF;

                if ( e_communicate( nd, 12, bytes_cnt + 9 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x04 && buff[ 8 ] == bytes_cnt )
                        {
                        int idx = 0;
                        for ( unsigned int l = 0; l < nd->AI_cnt; l++ )
                            {
                            switch ( nd->AI_types[ l ] )
                                {
                                case 638:
                                    nd->AI[ l ] = 256 * buff[ 9 + idx + 2 ] +
                                        buff[ 9 + idx + 3 ];
                                    idx += 4;
                                    break;

                                default:
                                    nd->AI[ l ] = 256 * buff[ 9 + idx ] +
                                        buff[ 9 + idx + 1 ];
                                    idx += 2;
                                    break;
                                }
                            }
                        }
                    else
                        {
                        if ( G_DEBUG )
                            {
                            printf( "\nRead AI:bus coupler returned error. Node %d.\n",
                                nd->number );
                            printf( "bytes_cnt = %d, %d %d \n",
                                (int)buff[ 7 ], (int)buff[ 8 ], bytes_cnt );
                            }
                        }
                    }
                }// if ( nd->AI_cnt > 0 )

            }// if ( nd->type == io_node::T_750_341 || ...
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return 0;
    }
//-----------------------------------------------------------------------------
void io_manager_linux::disconnect( io_node* node )
    {
    if ( node->sock )
        {
        shutdown( node->sock, SHUT_RDWR );
        close( node->sock );
        node->sock = 0;
        }
    node->state = io_node::ST_NO_CONNECT;
    }
//-----------------------------------------------------------------------------
io_manager_linux::io_manager_linux()
    {
    }
//-----------------------------------------------------------------------------
io_manager_linux::~io_manager_linux()
    {
    }
//-----------------------------------------------------------------------------
