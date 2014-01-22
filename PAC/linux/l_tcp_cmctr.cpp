#include <netdb.h>
#include <time.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "l_tcp_cmctr.h"
#include "PAC_err.h"

//#ifdef DEBUG
unsigned int max_buffer_use = 0;
//#endif

//------------------------------------------------------------------------------
tcp_communicator_linux::tcp_communicator_linux( const char *name_rus,
    const char *name_eng ):tcp_communicator(),
    netOK( 0 )
    {
    // Задаем таймаут.
    tv.tv_sec  = 0;
    tv.tv_usec = 5000; // 0.005 сек.

    sin_len = sizeof( ssin );
    strcpy( host_name_rus, name_rus );
    strcpy( host_name_eng, name_eng );

#ifdef DEBUG
    printf ( "PAC name \"%s\" (\"%s\").\n", host_name_rus, host_name_eng );
#endif // DEBUG

    //net_init();

    glob_last_transfer_time = get_sec();
    }
//------------------------------------------------------------------------------
void tcp_communicator_linux::killsockets()
    {
    FD_ZERO( &rfds ); /* clear FD set */

    for ( int i = 0; i < MAX_SOCKETS; i++ )
        {
        if ( sst[ i ].active )
            {
            shutdown( i, SHUT_RDWR );
            close( i );
            }
        }
    }
//------------------------------------------------------------------------------
int tcp_communicator_linux::net_init()
    {
    int type = SOCK_STREAM;
    int protocol = 0;        /* всегда 0 */
    int err = master_socket = socket( PF_INET, type, protocol ); // Cоздание мастер-сокета.

#ifdef DEBUG
    printf( "tcp_communicator_linux:net_init() - master socket created. Has number %d\n\r",
        master_socket );
#endif // DEBUG

    if ( master_socket < 0 )
        {
#ifdef DEBUG
        perror( "tcp_communicator_linux:net_init() - can't create master socket" );
#endif // DEBUG
        return -4;
        }

    // Переводим в неблокирующий режим.
    fcntl( master_socket, F_SETFL, O_NONBLOCK );

    // Адресация мастер-сокета.
    memset( &sst[ master_socket ].sin, 0, sizeof( sst[ master_socket ].sin ) );
    sst[ master_socket ].sin.sin_family 	 = AF_INET;
    sst[ master_socket ].sin.sin_addr.s_addr = INADDR_ANY;
    sst[ master_socket ].sin.sin_port 		 = htons ( PORT );

    const int on = 1;

    if ( setsockopt( master_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( on ) ) )
        {
        perror( "tcp_communicator_linux:net_init() - ошибка  вызова  setsockopt" );
        close( master_socket );
        return -5;
        }

    // Привязка сокета.
    err = bind( master_socket, ( struct sockaddr * ) & sst[ master_socket ].sin,
        sizeof( sst[ master_socket ].sin ) );
    if ( err < 0 )
        {
#ifdef DEBUG
        printf( "tcp_communicator_linux:net_init() - can't bind master socket to port %d : %s\n",
            PORT, strerror( errno ) );
#endif // DEBUG
        close( master_socket );
        return -5;
        }

    err = listen( master_socket, QLEN ); // Делаем мастер-сокет слушателем.
    if ( type == SOCK_STREAM && err < 0 )
        {
        close( master_socket );
#ifdef DEBUG
        perror( "tcp_communicator_linux:net_init() - listen" );
#endif // DEBUG
        return -6;
        }

    int val = 1;
    setsockopt( master_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof( val ) );
#ifdef MODBUS
    // Создание серверного сокета modbus_socket.
    err = modbus_socket = socket ( PF_INET, type, protocol );

#ifdef DEBUG
    printf( "tcp_communicator_linux:net_init() - modbus socket created. Has number %d\n\r",
        modbus_socket );
#endif // DEBUG

    if ( modbus_socket < 0 )
        {
#ifdef DEBUG
        perror( "tcp_communicator_linux:net_init() - can't create modbus socket" );
#endif //DEBUG

        return -4;
        }
    // Адресация modbus_socket сокета.
    memset( &sst[ modbus_socket ].sin, 0, sizeof ( sst[ modbus_socket ].sin ) );
    sst[ modbus_socket ].sin.sin_family 	  = AF_INET;
    sst[ modbus_socket ].sin.sin_addr.s_addr = 0;
    sst[ modbus_socket ].sin.sin_port 		  = htons ( 502 ); // Порт.
    err = bind( modbus_socket, ( struct sockaddr * ) & sst[ modbus_socket ].sin,
        sizeof ( sst[ modbus_socket ].sin ) );	   // Привязка сокета.
    if ( err < 0 )
        {
#ifdef DEBUG
        printf( "tcp_communicator_linux:net_init() - can't bind modbus socket to port %d : %s\n",
            502, strerror( errno ) );
#endif // DEBUG

        close( modbus_socket );
        return -5;
        }
    err = listen( modbus_socket, QLEN ); // Делаем слушателем.
    if ( type == SOCK_STREAM && err < 0 )
        {
        close( modbus_socket );
#ifdef DEBUG
        perror( "tcp_communicator_linux:net_init() - listen" );
#endif // DEBUG
        return -6;
        }
#endif //MODBUS

    FD_ZERO ( &rfds );
    for ( int i = 0; i < MAX_SOCKETS; i++ )
        {
        sst[ i ].active     = 0;
        sst[ i ].init       = 0;
        sst[ i ].evaluated  = 0;
        }
    sst[ master_socket ].active      = 1; // мастер-сокет всегда активный.
    sst[ master_socket ].is_listener = 1; // сокет является слушателем.
    sst[ master_socket ].evaluated   = 0;

#ifdef MODBUS
    sst[ modbus_socket ].active      = 1;
    sst[ modbus_socket ].is_listener = 1;
    sst[ modbus_socket ].evaluated   = 0;
#endif // MODBUS

    netOK = 1;
    return 0;
    }
//------------------------------------------------------------------------------
void tcp_communicator_linux::net_terminate()
    {
    killsockets();
    netOK = 0;
    }
//------------------------------------------------------------------------------
tcp_communicator_linux::~tcp_communicator_linux()
    {
    net_terminate();
    }
//------------------------------------------------------------------------------
int tcp_communicator_linux::evaluate()
    {
    // Проверка связи с сервером.
    if ( get_sec() - glob_last_transfer_time > 5 )
        {
        if ( glob_cmctr_ok )
            {
            glob_cmctr_ok = 0;
            PAC_critical_errors_manager::get_instance()->set_global_error(
                PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_EASYSERVER,
                PAC_critical_errors_manager::AS_EASYSERVER );
            }
        }
    else
        {
        if ( glob_cmctr_ok == 0 )
            {
            glob_cmctr_ok = 1;
            PAC_critical_errors_manager::get_instance()->reset_global_error(
                PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_EASYSERVER,
                PAC_critical_errors_manager::AS_EASYSERVER );
            }
        }
    // Проверка связи с сервером.-!>

	// Инициализация сети, при необходимости.
    if ( !netOK )
        {
        net_init();
        if ( !netOK ) return -100;
        }
    // Инициализация сети, при необходимости.-!>

    int count_cycles = 0;
    while ( count_cycles < max_cycles )
        {
        /* service loop */
        count_cycles++;

        FD_ZERO( &rfds );
        for ( int i = 0; i < MAX_SOCKETS; i++ )
            {
            if ( sst[ i ].active &&
                sst[ i ].is_listener &&
                !sst[ i ].evaluated )
                {
                /* re-join active sockets */
                FD_SET( i, &rfds );
                }
            }

        // Ждём события в одном из сокетов.
        rc = select( MAX_SOCKETS + 1, &rfds, NULL, NULL, &tv );

        if ( 0 == rc ) break; // Ничего не произошло.

        if ( rc < 0 )
            {
#ifdef DEBUG
            perror( "selectsocket() error" );
#endif
            continue;
            }

        for ( int i = 0; i < MAX_SOCKETS; i++ )  /* scan all possible sockets */
            {
            // Поступил новый запрос на соединение.
            if ( FD_ISSET ( i, &rfds ) )
                {
#ifndef MODBUS
                if ( i == master_socket )
#else
                if ( i == master_socket || i == modbus_socket )
#endif
                     {
                    /* master socket */
                    memset( &ssin, 0, sizeof ( ssin ) );
                    slave_socket = accept ( i,
                        ( struct sockaddr * ) &ssin, &sin_len );

                    if ( slave_socket <= 0 )    // Ошибка.
                        {
#ifdef DEBUG
                        perror( "accept() error" );
#endif
                        continue;
                        }

#ifdef MODBUS
                    if ( i != modbus_socket )
                        {
#endif
                        char Message1[] = "PAC accept";
                        send( slave_socket, Message1, strlen ( Message1 ), MSG_NOSIGNAL );
#ifdef MODBUS
                        }
#endif
                    // Установка сокета в неблокирующий режим.
                    if ( fcntl( slave_socket, F_SETFL, O_NONBLOCK ) < 0 )
                        {
                        // Ошибка, разрушаем сокет.
                        shutdown( slave_socket, 0 );
                        close( slave_socket );

#ifdef DEBUG
                        perror( "fcntl() error" );
#endif
                        continue;
                        }

#ifdef DEBUG
#ifndef MODBUS
                    // Определение имени клиента.
                    hostent *client = gethostbyaddr( &ssin.sin_addr, 4, AF_INET );
                    if ( client )
                        {
                        printf( "Accepted connection on %d socket from %s [ %s ].\n",
                            slave_socket, inet_ntoa( ssin.sin_addr ), client->h_name  );
                        }
                    else
                        {
                        herror( "Error getting client name" );
                        Print( "Accepted connection on %d socket from %s.\n",
                            slave_socket, inet_ntoa( ssin.sin_addr ) );
                        }
#else
                    Print( "Accepted connection on %d socket from %s.\n",
                        slave_socket, inet_ntoa( ssin.sin_addr ) );
#endif // MODBUS
#endif // DEBUG

                    FD_SET( slave_socket, &rfds );
                    sst[ slave_socket ].active = 1;
                    sst[ slave_socket ].init   = 1;
                    sst[ slave_socket ].is_listener = 1;
                    sst[ slave_socket ].evaluated = 0;
                    memcpy( &sst[ slave_socket ].sin, &ssin, sin_len );
                    }
                else         /* slave socket */
                    {
                    do_echo( i );
                    glob_last_transfer_time = get_sec();
                    }
                }
            }
        }  /* service loop */

    for ( int i = 0; i < MAX_SOCKETS; i++ )
        {
        sst[ i ].evaluated = 0;
        }

    return 0;
    }
//------------------------------------------------------------------------------
int tcp_communicator_linux::recvtimeout( int s, u_char *buf,
    int len, int sec, int usec, char* IP )
    {
    // Настраиваем  file descriptor set.
    fd_set fds;
    FD_ZERO( &fds );
    FD_SET( s, &fds );

    // Настраиваем время на таймаут.
    timeval rec_tv;
    rec_tv.tv_sec = sec;
    rec_tv.tv_usec = usec;

    // Ждем таймаута или полученных данных.
    int n = select( s + 1, &fds, NULL, NULL, &rec_tv );
    if ( 0 == n )
        {
#if DEBUG
        print_time( "Socket %d->\"%s\" disconnected on read try - timeout.\n",
            s, IP );
#endif

        return -2;  // timeout!
        }

    if ( -1 == n )
        {
#if DEBUG
        print_time( "Socket %d->\"%s\" disconnected on read try : %s\n",
            s, IP, strerror( errno ) );
#endif
        return -1; // error
        }

    // Данные должны быть здесь, поэтому делаем обычный recv().
    int res = recv( s, buf, len, MSG_NOSIGNAL );

    if ( 0 == res )
        {
#if DEBUG
        print_time( "Socket %d->\"%s\" was closed.\n",
            s, IP );
#endif
        }

    if ( res < 0 )
        {
#if DEBUG
        print_time( "Socket %d->\"%s\" disconnected on read try (unknown) : %s\n",
            s, IP, strerror( errno ) );
#endif
        }

    return res;
    }
//------------------------------------------------------------------------------
int tcp_communicator_linux::do_echo ( int skt )
    {
    FD_CLR( skt, &rfds );

    int err = 0, res;

    if ( sst[ skt ].init )         /* socket is just initiated */
        {
        sst[ skt ].init = 0;

#ifdef  MODBUS
        return err;
#endif // MODBUS
        }

    sst[ skt ].evaluated = 1;
    memset( buf, 0, BUFSIZE );

    // Ожидаем данные с таймаутом 1 сек.
    err = in_buffer_count = recvtimeout( skt, buf, BUFSIZE, 5, 0,
        inet_ntoa( sst[ skt ].sin.sin_addr ) );

    if ( err <= 0 )               /* read error */
        {
        shutdown( skt, 0 );
        close( skt );
        sst[ skt ].active = 0;
        return err;
        }

//#ifdef DEBUG
    if ( in_buffer_count > max_buffer_use )
        {
        max_buffer_use = in_buffer_count;
        printf( "Max buffer use %u\n", max_buffer_use );
        }
//#endif // DEBUG

    net_id = buf[ 0 ];
    pidx   = buf[ 3 ];

    if ( net_id == 's' && buf[ 1 ] < TC_MAX_SERVICE_NUMBER && services[ buf[ 1 ] ] != NULL &&
        buf[ 2 ] + buf[ 3 ] != 0 )
        {
        switch ( buf[ 2 ] )
            {
            case FRAME_SINGLE:

                res = services[ buf[ 1 ] ] (
                    ( u_int ) ( buf[ 4 ] * 256 + buf[ 5 ] ), buf + 6, buf + 5 );

                if ( res == 0 )
                    {
                    _AknOK();
                    }
                else
                    {
                    _AknData( res );
//#ifdef DEBUG
                    if ( ( unsigned int ) res > max_buffer_use )
                        {
                        max_buffer_use = res;
                        printf( "Max buffer use %u\n", res );
                        }
//#endif
                    }
                break;

            default:
                _ErrorAkn( ERR_WRONG_CMD );
#ifdef DEBUG
                printf( "Wrong command received on socket %d->\"%s\"\n",
                    skt, inet_ntoa( sst[ skt ].sin.sin_addr ) );
#endif // DEBUG
                break;
            }
        }
    else
        {
        if ( services[ 15 ] != NULL && 0 == buf[ 2 ] + buf[ 3 ] ) //MODBUS
            {
            res = services[ 15 ] ( ( u_int ) ( buf[ 4 ] * 256 + buf[ 5 ] ),
                buf + 6,
                buf + 6 );
            if ( res > 0 )
                {
                buf[ 4 ] = ( res >> 8 ) & 0xFF;
                buf[ 5 ] = res & 0xFF;
                in_buffer_count = res + 6;
                }
            sst[ skt ].evaluated = 0;
            }
        else
            {
            _ErrorAkn( ERR_WRONG_SERVICE );
#ifdef DEBUG
            printf( "No such service %d at socket %d->\"%s\"\n",
                buf[ 1 ], skt, inet_ntoa( sst[ skt ].sin.sin_addr ) );
#endif // DEBUG
            }
        }

    err = sendall( skt, buf, in_buffer_count, MSG_NOSIGNAL );
    if ( is_going_to_reboot )
        {
        killsockets();
        usleep( 800000 );
        }

    if ( err <= 0 )               /* write error */
        {
#ifdef DEBUG
        fprintf( stderr, "Socket %d->\"%s\" disconnected on write try : %s\n",
            skt, inet_ntoa( sst[ skt ].sin.sin_addr ), strerror( errno ) );
#endif // DEBUG

        shutdown( skt, 0 );
        close( skt );
        sst[ skt ].active = 0;
        return err;
        }

    return err;
    }
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
