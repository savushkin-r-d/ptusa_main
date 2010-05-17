#include <netdb.h>
#include <time.h>
#include <sys/ioctl.h>
#include <string.h>

#include "tcp_cmctr_lin.h"

#ifdef DEBUG
unsigned int max_buffer_use = 0;
#endif

//------------------------------------------------------------------------------
tcp_communicator_linux::tcp_communicator_linux():tcp_communicator(),
netOK( 0 )
    {
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    max_cycles = 1;

    glob_cmctr_ok = 1;
    sin_len = sizeof( ssin );

    gethostname( host_name, TC_MAX_HOST_NAME );
#ifdef DEBUG
    printf ( "tcp_communicator_linux() - host name is \"%s\".\n", host_name );
#endif // DEBUG

    net_init();
    }
//------------------------------------------------------------------------------
void tcp_communicator_linux::killsockets()
    {
    FD_ZERO( &rfds ); /* clear FD set */

    for ( int i = 0; i < MAX_SOCKETS; i++ )
        {
        if ( sst[ i ].active )
            {
            sst[ i ].clID = -1;
            shutdown( i, 0 );
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
        printf( "tcp_communicator_linux:net_init() - can't create master socket! Error %d\n\r",
            err );
        perror( "tcp_communicator_linux:net_init() " );
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
    // Привязка сокета.
    err = bind( master_socket, ( struct sockaddr * ) & sst[ master_socket ].sin,
        sizeof( sst[ master_socket ].sin ) );
    if ( err < 0 )
        {
#ifdef DEBUG
        printf( "tcp_communicator_linux:net_init() - can't bind master socket to port %d, error %d\n\r",
            PORT, err );
        perror( "tcp_communicator_linux:net_init()" );
#endif // DEBUG
        close( master_socket );
        return -5;
        }

    err = listen( master_socket, QLEN ); // Делаем мастер-сокет слушателем.
    if ( type == SOCK_STREAM && err < 0 )
        {
        close ( master_socket );
#ifdef DEBUG
        perror( "tcp_communicator_linux:net_init()" );
#endif // DEBUG
        return -6;
        }

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
        printf ( "tcp_communicator_linux:net_init() - can't crmodbus_sockette modbus socket! error %modbus_socketn\r",
            err );
        perror( "tcp_communimodbus_sockettor:net_init() " );
#endif //DEBUG

        return -4;
        }
    // Адресация modbus_socket сокета.
    memset( &sst[ modbus_socket ].simodbus_socket 0, sizeof ( sst[ modbus_socket ].sin ) );
    sst[ modbus_socket ].sin.sin_family 	  = AF_INET;
    sst[ modbus_socket ].sin.sin_addr.s_addr = 0;
    sst[ modbus_socket ].sin.sin_port 		  = htons ( 502 ); // Порт.
    err = bind( modbus_socket, ( struct sockaddr * ) & sst[ modbus_socket ].sin,
        sizeof ( sst[ modbus_socket ].sin ) );	   // Привязка сокета.
    if ( err < 0 )
        {
#ifdef DEBUGmodbus_socket	printf( "tcp_communicator_linux:net_init(modbus_socket- can't bind modbus socket to port %d, error %d\n\r",
        502, err );
        perror( "tcp_communicator_linux:net_init() " );
#endif // DEBUG

        close( modbus_socket );
        return -5;
        }
    err = listen( modbus_socket, QLEN ); // Делаем слушателем.
    if ( type == SOCK_STREAM && err < 0 )
        {
        close( modbus_socket );
#ifdef DEBUG
        perror( "tcp_communicator_linux:net_init() " );
#endif // DEBUG
        return -6;
        }
#endif //MODBUS

    FD_ZERO ( &rfds );
    for ( int i = 0; i < MAX_SOCKETS; i++ )
        {
        sst[ i ].active = 0;
        sst[ i ].init   = 0;
        }
    sst[ master_socket ].active = 1;  	 // мастер-сокет всегда активный.
    sst[ master_socket ].islistener = 1; // сокет является слушателем.

#ifdef MODBUS
    sst[ modbus_socket ].active = 1;
    sst[ modbus_socket ].islistener = 1;
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
    killsockets();
    delete buf;
    }
//------------------------------------------------------------------------------
int tcp_communicator_linux::evaluate()
    {
    int err = 0;
    int count_cycles = 0;

    // Проверка связи с сервером.
    if ( difftime( time( NULL ), glob_last_transfer_time ) > 5 )
        {
        if ( glob_cmctr_ok )
            {
            glob_cmctr_ok = 0;
            //SetGlobalError ( EC_NO_CONNECTION, ES_EASYSERVER, ES_EASYSERVER );
            }
        }
    else
        {
        if ( glob_cmctr_ok == 0 )
            {
            glob_cmctr_ok = 1;
            //ResetGlobalError ( EC_NO_CONNECTION, ES_EASYSERVER, ES_EASYSERVER );
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

    while ( count_cycles < max_cycles )
        {
        /* service loop */
        count_cycles++;
        for ( int i = 0; i < MAX_SOCKETS; i++ )
            {
            if ( sst[ i ].active && sst[ i ].islistener && !sst[ i ].evaluated )
                {
                /* re-join active sockets */
                FD_SET( i, &rfds );
                }
            }

        // Задаем таймаут.
        tv.tv_sec  = 0;
        tv.tv_usec = 600000; // 0.6 сек.

        // Ждём события в одном из сокетов.
        rc = select( MAX_SOCKETS, &rfds, NULL, NULL, &tv );
        if ( rc < 0 )
            {
#ifdef DEBUG
            printf( "selectsocket error %d\n\r", rc );
            perror( "select" );
#endif
            break;
            }

        if ( !rc )
            break;

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
                    err = ss = accept ( i, ( struct sockaddr * ) &ssin, &sin_len );

#ifdef DEBUG
                    hostent *client = gethostbyaddr( &ssin.sin_addr, 4, PF_INET );
                    if ( client )
                        {
                        printf( "Accepted connection on %d socket from %s [ %s ].\n\r",
                            ss, inet_ntoa( ssin.sin_addr ), client->h_name  );
                        }
                    else
                        {
                        printf( "Accepted connection on %d socket from %s.\n\r",
                            ss, inet_ntoa( ssin.sin_addr ) );
                        }
#endif // DEBUG

                    if ( err < 0 )
                        {
#ifdef DEBUG
                        printf ( "accept error %d\n\r", err );
                        perror( "accept" );
#endif
                        break;
                        }
#ifdef MODBUS
                    if ( i != modbus_socket )
                        {
#endif
                        char Message1[] = "I7188e accept";
                        err = send ( ss, Message1, strlen ( Message1 ), 0 );
#ifdef MODBUS
                        }
#endif
                    FD_SET ( ss, &rfds );
                    sst[ ss ].active = 1;
                    sst[ ss ].init   = 1;
                    sst[ ss ].islistener = 1;
                    sst[ ss ].evaluated = 0;
                    memcpy ( &sst[ ss ].sin, &ssin, sin_len );
                    }
                else         /* slave socket */
                    {
                    err = do_echo ( i );
                    glob_last_transfer_time = time( NULL );
                    }
                }
            }
        }  /* service loop */

    for ( int i = 0; i < MAX_SOCKETS; i++ )
        {
        sst[ i ].evaluated = 0;
        }
    return err;
    }
//------------------------------------------------------------------------------
int tcp_communicator_linux::recvtimeout( u_int s, u_char *buf,
    int len, int timeout, int usec )
    {
    // Настраиваем  file descriptor set.
    fd_set fds;
    FD_ZERO( &fds );
    FD_SET( s, &fds );

    // Настраиваем время на таймаут.
    timeval rec_tv;
    rec_tv.tv_sec = timeout;
    rec_tv.tv_usec = usec;

    // Ждем таймаута или полученных данных.
    int n = select( s + 1, &fds, NULL, NULL, &rec_tv );
    if ( 0 == n ) return -2;  // timeout!
    if ( -1 == n ) return -1; // error

    // Данные должны быть здесь, поэтому делаем обычный recv().
    return recv( s, buf, len, 0 );
    }
//------------------------------------------------------------------------------
int tcp_communicator_linux::do_echo ( int skt )
    {
    int err = 0, res;

    if ( sst[ skt ].init )         /* socket is just initiated */
        {
        sst[ skt ].init = 0;
#ifdef DEBUG
        printf( "Socket connected\n" );
#endif // DEBUG

#ifdef  MODBUS
        return err;
#endif // MODBUS
        }
    sst[ skt ].evaluated = 1;

    err = in_buffer_count = recvtimeout( skt, buf, BUFSIZE, 0, 500000 );

    if ( err < 0 )               /* read error */
        {
        shutdown( skt, 0 );
        close( skt );
        FD_CLR( skt, &rfds );
        sst[ skt ].active = 0;
#ifdef DEBUG
        printf( "Socket %d disconnected on read try.\n", skt );
        perror( "" );
#endif // DEBUG
        return err;
        }
#ifdef DEBUG
    if ( in_buffer_count > max_buffer_use )
        {
        max_buffer_use = in_buffer_count;
        printf( "Max buffer use %u\n", max_buffer_use );
        }
#endif // DEBUG

    net_id = buf[ 0 ];
    pidx = buf[ 3 ];
    if ( net_id != 0x73 && ( buf[ 2 ] + buf[ 3 ] ) )
        {
        FD_CLR( skt, &rfds );
#ifdef DEBUG
        printf( "Incorrect input data at %d socket\n", skt );
#endif // DEBUG
        return ERR_RETRIVE;
        }

    if ( buf[ 1 ] < TC_MAX_SERVICE_NUMBER && services[ buf[ 1 ] ] != NULL &&
        ( buf[ 2 ] + buf[ 3 ] != 0 ) )
        {
        switch ( buf[ 2 ] )
            {
            case FRAME_SINGLE:
                res = services[ buf[ 1 ] ] ( ( u_int ) ( buf[ 4 ] * 256 + buf[ 5 ] ),
                    buf + 6, buf + 5 );
                if ( res == 0 )
                    {
                    _AknOK();
                    }
                else
                    {
                    _AknData( res );
#ifdef DEBUG
                    if ( ( unsigned int ) res > max_buffer_use )
                        {
                        max_buffer_use = res;
                        printf( "Max buffer use %u\n", res );
                        }
#endif
                    }
                break;

            default:
                _ErrorAkn( ERR_WRONG_CMD );
#ifdef DEBUG
                printf( "Wrong command received on %d socket\n\r", skt );
#endif // DEBUG
                break;
            }
        }
    else
        {
        if ( ( services[ 15 ] != NULL ) && ( 0 == buf[ 2 ] + buf[ 3 ] ) ) //MODBUS
            {
            res = services[ 15 ] ( ( u_int ) ( buf[4] * 256 + buf[5] ),
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
            printf( "No such service at %d socket\n", skt );
#endif // DEBUG
            }
        }

    err = send( skt, buf, in_buffer_count, 0 );
    if ( is_going_to_reboot )
        {
        killsockets();
        usleep( 800000 );
        }

    if ( err < 0 )               /* write error */
        {
#ifdef DEBUG
        printf( "echo write error %d\n", err );
#endif
        shutdown( skt, 0 );
        close( skt );
        FD_CLR( skt, &rfds );
        sst[ skt ].active = 0;
#ifdef DEBUG
        printf( "Socket %d disconnected on write try\n", skt );
#endif // DEBUG
        return err;
        }
    FD_CLR( skt, &rfds );
    return err;
    }
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

