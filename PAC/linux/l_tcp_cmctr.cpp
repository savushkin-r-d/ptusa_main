#include <netdb.h>
#include <time.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "l_tcp_cmctr.h"
#include "PAC_err.h"
#include "tcp_client.h"

#include "log.h"

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
    tv.tv_usec = 1000;

    sin_len = sizeof( ssin );
    strcpy( host_name_rus, name_rus );
    strcpy( host_name_eng, name_eng );

#ifdef DEBUG
    printf ( "PAC name \"%s\" (\"%s\").\n", host_name_rus, host_name_eng );
#endif // DEBUG

    //net_init();
    slave_socket = 0;
    modbus_socket = 0;
    master_socket = 0;
    rc = 0;

    glob_last_transfer_time = get_millisec();
    }
//------------------------------------------------------------------------------
void tcp_communicator_linux::killsockets()
    {
    FD_ZERO( &rfds ); /* clear FD set */

    for ( u_int i = 0; i < sst.size(); i++ )
        {
        if ( sst[ i ].active )
            {
            shutdown( sst[ i ].socket, SHUT_RDWR );
            close( sst[ i ].socket );
            }
        }
    sst.clear();
    }
//------------------------------------------------------------------------------
int tcp_communicator_linux::net_init()
    {
    errno = 0;

    int type = SOCK_STREAM;
    int protocol = 0;        /* всегда 0 */
    int err = master_socket = socket( PF_INET, type, protocol ); // Cоздание мастер-сокета.

#ifdef DEBUG
    printf( "tcp_communicator_linux:net_init() - master socket created. Has number %d\n\r",
        master_socket );
#endif // DEBUG

    if ( master_socket < 0 )
        {
        sprintf( G_LOG->msg,
            "%s : cp_communicator_linux:net_init() - can't create master socket.",
            strerror( errno ) );
        G_LOG->write_log( i_log::P_CRIT );

        return -4;
        }

    // Переводим в неблокирующий режим.
    fcntl( master_socket, F_SETFL, O_NONBLOCK );

    // Адресация мастер-сокета.
    socket_state master_socket_state;
    memset( &master_socket_state.sin, 0, sizeof( master_socket_state.sin ) );
    master_socket_state.socket = master_socket;
    master_socket_state.sin.sin_family 	 = AF_INET;
    master_socket_state.sin.sin_addr.s_addr = INADDR_ANY;
    master_socket_state.sin.sin_port 		 = htons ( PORT );
    master_socket_state.ismodbus = 0;

    const int on = 1;

    if ( setsockopt( master_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( on ) ) )
        {
        sprintf( G_LOG->msg,
            "%s : tcp_communicator_linux:net_init() - error calling setsockopt.",
            strerror( errno ) );
        G_LOG->write_log( i_log::P_CRIT );

        close( master_socket );
        return -5;
        }

    // Привязка сокета.
    err = bind( master_socket, ( struct sockaddr * ) & master_socket_state.sin,
        sizeof( master_socket_state.sin ) );
    if ( err < 0 )
        {
        sprintf( G_LOG->msg,
            "%s : tcp_communicator_linux:net_init() - can't bind master socket to port %d.",
            strerror( errno ), PORT );
        G_LOG->write_log( i_log::P_CRIT );

        close( master_socket );
        return -5;
        }

    err = listen( master_socket, QLEN ); // Делаем мастер-сокет слушателем.
    if ( type == SOCK_STREAM && err < 0 )
        {
        sprintf( G_LOG->msg,
            "%s : tcp_communicator_linux:net_init() - listen.",
            strerror( errno ) );
        G_LOG->write_log( i_log::P_CRIT );

        close( master_socket );
        return -6;
        }

    int val = 1;
    setsockopt( master_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof( val ) );

    master_socket_state.active      = 1; // мастер-сокет всегда активный.
    master_socket_state.is_listener = 1; // сокет является слушателем.
    master_socket_state.evaluated   = 0;

    sst.push_back( master_socket_state );

    // Создание серверного сокета modbus_socket.
    err = modbus_socket = socket ( PF_INET, type, protocol );

#ifdef DEBUG
    printf( "tcp_communicator_linux:net_init() - modbus socket created. Has number %d\n\r",
        modbus_socket );
#endif // DEBUG

    if ( modbus_socket < 0 )
        {
        sprintf( G_LOG->msg,
            "%s : cp_communicator_linux:net_init() - can't create modbus socket.",
            strerror( errno ) );
        G_LOG->write_log( i_log::P_CRIT );

        return -4;
        }
    // Адресация modbus_socket сокета.
    socket_state modbus_socket_state;
    memset( &modbus_socket_state.sin, 0, sizeof ( modbus_socket_state.sin ) );
    modbus_socket_state.socket = modbus_socket;
    modbus_socket_state.sin.sin_family 	  = AF_INET;
    modbus_socket_state.sin.sin_addr.s_addr = 0;
    modbus_socket_state.sin.sin_port 		  = htons ( PORT_MODBUS ); // Порт.
    modbus_socket_state.ismodbus = 1;

    err = bind( modbus_socket, ( struct sockaddr * ) & modbus_socket_state.sin,
        sizeof ( modbus_socket_state.sin ) );	   // Привязка сокета.
    if ( err < 0 )
        {
        sprintf( G_LOG->msg,
            "%s : tcp_communicator_linux:net_init() - can't bind modbus socket to port %d.",
            strerror( errno ), PORT_MODBUS );
        G_LOG->write_log( i_log::P_CRIT );

        close( modbus_socket );
        return -5;
        }
    err = listen( modbus_socket, QLEN ); // Делаем слушателем.
    if ( type == SOCK_STREAM && err < 0 )
        {
        sprintf( G_LOG->msg,
            "%s : tcp_communicator_linux:net_init() - listen modbus.",
            strerror( errno ) );
        G_LOG->write_log( i_log::P_CRIT );

        close( modbus_socket );
        return -6;
        }

    modbus_socket_state.active      = 1;
    modbus_socket_state.is_listener = 1;
    modbus_socket_state.evaluated   = 0;

    sst.push_back(modbus_socket_state);

    FD_ZERO ( &rfds );

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
    if ( get_delta_millisec(glob_last_transfer_time) > 5000L )
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
    int max_sock_number = 0;
    while ( count_cycles < max_cycles )
        {
        /* service loop */
        count_cycles++;
        sleep_ms(1);
        max_sock_number = 0;

        FD_ZERO( &rfds );
        for ( u_int i = 0; i < sst.size(); i++ )
            {
            if ( sst[ i ].active &&
                sst[ i ].is_listener &&
                !sst[ i ].evaluated )
                {
                /* re-join active sockets */
                FD_SET( sst[ i ].socket, &rfds );
                if (sst[ i ].socket > max_sock_number)
                    {
                    max_sock_number = sst[ i ].socket;
                    }
                }
            }

        //Добавляем асинхронные сокеты в список прослушки
        for (std::map<int, tcp_client*>::iterator it = clients->begin(); it != clients->end(); ++ it)
            {
            FD_SET( it->second->get_socket(), &rfds);
            if (it->second->get_socket() > max_sock_number)
                {
                max_sock_number = it->second->get_socket();
                }
            }

        // Ждём события в одном из сокетов.
        rc = select( max_sock_number + 1, &rfds, NULL, NULL, &tv );

        if ( 0 == rc ) break; // Ничего не произошло.

        if ( rc < 0 )
            {
            sprintf( G_LOG->msg,
                "%s : tcp_communicator_linux:evaluate() - select socket.",
                strerror( errno ) );
            G_LOG->write_log( i_log::P_ERR );

            continue;
            }

        for ( u_int i = 0; i < sst.size(); i++ )  /* scan all possible sockets */
            {
            // Поступил новый запрос на соединение.
            if ( FD_ISSET ( sst[ i ].socket, &rfds ) )
                {
                if ( sst[ i ].socket == master_socket || sst[ i ].socket == modbus_socket )
                     {
                    /* master socket */
                    memset( &ssin, 0, sizeof ( ssin ) );
                    slave_socket = accept ( sst[ i ].socket,
                        ( struct sockaddr * ) &ssin, &sin_len );

                    if ( slave_socket <= 0 )    // Ошибка.
                        {
                        sprintf( G_LOG->msg,
                            "%s : tcp_communicator_linux:evaluate() - accept.",
                            strerror( errno ) );
                        G_LOG->write_log( i_log::P_ERR );

                        continue;
                        }

                    if ( sst[ i ].socket != modbus_socket )
                        {
                        char Message1[] = "PAC accept";
                        send( slave_socket, Message1, strlen ( Message1 ), MSG_NOSIGNAL );
                        }
                    // Установка сокета в неблокирующий режим.
                    if ( fcntl( slave_socket, F_SETFL, O_NONBLOCK ) < 0 )
                        {
                        // Ошибка, разрушаем сокет.
                        shutdown( slave_socket, 0 );
                        close( slave_socket );

                        sprintf( G_LOG->msg,
                            "%s : tcp_communicator_linux:evaluate() - fcntl.",
                            strerror( errno ) );
                        G_LOG->write_log( i_log::P_ERR );

                        continue;
                        }

#ifdef DEBUG
//                    if (i != modbus_socket)
//                    {
//                    // Определение имени клиента.
//                    hostent *client = gethostbyaddr( &ssin.sin_addr, 4, AF_INET );
//                    if ( client )
//                        {
//                        printf( "Accepted connection on %d socket from %s [ %s ].\n",
//                            slave_socket, inet_ntoa( ssin.sin_addr ), client->h_name  );
//                        }
//                    else
//                        {
//                        herror( "Error getting client name" );
//                        Print( "Accepted connection on %d socket from %s.\n",
//                            slave_socket, inet_ntoa( ssin.sin_addr ) );
//                        }
//                    }
//                    else
//                    {
//                    Print( "Accepted connection on %d socket from %s.\n",
//                        slave_socket, inet_ntoa( ssin.sin_addr ) );
//                    }
#endif // DEBUG
                    FD_SET( slave_socket, &rfds );
                    socket_state slave_socket_state;
                    slave_socket_state.socket = slave_socket;
                    slave_socket_state.active = 1;
                    slave_socket_state.init   = 1;
                    slave_socket_state.is_listener = 1;
                    slave_socket_state.evaluated = 0;
                    memcpy( &slave_socket_state.sin, &ssin, sin_len );
                    sst.push_back(slave_socket_state);
                    }
                else         /* slave socket */
                    {
                    do_echo( i );
                    glob_last_transfer_time = get_millisec();
                    }
                }
            }

        //проверка асинхронных сокетов на предмет поступления данных
         for (std::map<int, tcp_client*>::iterator it = clients->begin(); it != clients->end();)
             {
             int is_removed = 0;
             if (FD_ISSET(it->second->get_socket(), &rfds)) //если есть событие на сокете
                 {
                 int err = recvtimeout(it->second->get_socket(),
                     (unsigned char*)it->second->buff, it->second->buff_size,
		     1, 0, it->second->ip, "async client", 0 );
                 if (err <= 0) //Ошибка чтения
                     {
                     it->second->Disconnect();
                     it->second->set_async_result(it->second->AR_SOCKETERROR);
                     }
                 else //Получены данные
                     {
                     in_buffer_count = err;
                     it->second->set_async_result(in_buffer_count);
                     }
                 is_removed = 1;
                 }
             else //проверяем на таймаут
                 {
                 if (get_delta_millisec(it->second->async_queued) > it->second->async_timeout)
                     {
                     it->second->Disconnect();
                     it->second->set_async_result(it->second->AR_TIMEOUT);
                     is_removed = 1;
                     }
                 }

             if (is_removed)
                 {
                 clients->erase(it++);
                 }
             else
                 {
                 it++;
                 }
             }

        }  /* service loop */



    for ( u_int i = 0; i < sst.size(); i++ )
        {
        sst[ i ].evaluated = 0;
        }

    return 0;
    }
//------------------------------------------------------------------------------
int tcp_communicator_linux::recvtimeout( int s, u_char *buf,
    int len, int sec, int usec, const char* IP, const char* name,
    stat_time *stat )
    {

    //Network performance info.
    if ( stat )
	{
	static time_t t_;
	struct tm *timeInfo_;
	t_ = time( 0 );
	timeInfo_ = localtime( &t_ );

	//Once per hour writes performance info.
	if ( stat->print_cycle_last_h != timeInfo_->tm_hour )
	    {
	    stat->print_cycle_last_h = timeInfo_->tm_hour;

	    u_long avg_time = stat->all_time / stat->cycles_cnt;
	    sprintf( G_LOG->msg,
		    "tcp_communicator_linux:recvtimeout() socket %d->\"%s\":\"%s\""
		    " avg = %lu, min = %u, max = %u (ms).",
		    s, name, IP,
		    avg_time, stat->min_iteration_cycle_time,
		    stat->max_iteration_cycle_time );
	    G_LOG->write_log( i_log::P_DEBUG );

	    u_int t = G_PAC_INFO()->par[ PAC_info::P_WAGO_TCP_NODE_WARN_ANSWER_TIME ];

	    if ( t < avg_time )
		{
		sprintf( G_LOG->msg,
			"tcp_communicator_linux:recvtimeout() socket %d->\"%s\":\"%s\""
			" avg time exceeds limit %lu > %u (ms).",
			s, name, IP, avg_time, t );
		G_LOG->write_log( i_log::P_DEBUG );
		}

	    stat->clear();
	    }
	}

    errno = 0;

    // Настраиваем  file descriptor set.
    fd_set fds;
    FD_ZERO( &fds );
    FD_SET( s, &fds );

    // Настраиваем время на таймаут.
    timeval rec_tv;
    rec_tv.tv_sec = sec;
    rec_tv.tv_usec = usec;

    static u_long st_time;
    static u_int select_wait_time;

    st_time = get_millisec();

    // Ждем таймаута или полученных данных.
    int n = select( s + 1, &fds, NULL, NULL, &rec_tv );

    if ( 0 == n )
        {
        sprintf( G_LOG->msg,
            "tcp_communicator_linux:recvtimeout() socket %d->\"%s\":\"%s\""
             " disconnected on read try - timeout (%d ms).",
             s, name, IP,
	     usec / 1000  );

        G_LOG->write_log( i_log::P_ERR );

        return -2;  // timeout!
        }

    if ( -1 == n )
        {
        sprintf( G_LOG->msg,
            "%s : tcp_communicator_linux:recvtimeout() socket %d->\"%s\":\"%s\""
            " disconnected on read try.",
            strerror( errno ), s, name, IP );
        G_LOG->write_log( i_log::P_ERR );

        return -1; // error
        }

    // Данные должны быть здесь, поэтому делаем обычный recv().
    int res = recv( s, buf, len, MSG_NOSIGNAL );

    select_wait_time = get_delta_millisec( st_time );

    if ( 0 == res )
        {
        sprintf( G_LOG->msg,
            "tcp_communicator_linux:recvtimeout() socket %d->\"%s\":\"%s\""
            " was closed.",
            s, name, IP );
        G_LOG->write_log( i_log::P_WARNING );
        }

    if ( res < 0 )
        {
        sprintf( G_LOG->msg,
            "%s : tcp_communicator_linux:recvtimeout() socket %d->\"%s\":\"%s\""
            " disconnected on read try (unknown).",
            strerror( errno ), s, name, IP );
        G_LOG->write_log( i_log::P_ERR );
        }

    //Network performance info.
    if ( stat )
	{
	stat->cycles_cnt++;
	stat->all_time += select_wait_time;

	if ( select_wait_time > stat->max_iteration_cycle_time )
	    {
	    stat->max_iteration_cycle_time = select_wait_time;
	    }
	if ( select_wait_time < stat->min_iteration_cycle_time )
	    {
	    stat->min_iteration_cycle_time = select_wait_time;
	    }
	}

    return res;
    }
//------------------------------------------------------------------------------
int tcp_communicator_linux::do_echo ( int idx )
    {
    socket_state &sock_state = sst[ idx ];
    FD_CLR( sock_state.socket, &rfds );

    int err = 0, res;

    if ( sock_state.init )         /* socket is just initiated */
        {
        sock_state.init = 0;
        }

    sock_state.evaluated = 1;
    memset( buf, 0, BUFSIZE );

    static const char* const SERVER = "server";
    static const char* const MODBUS_DEV = "modbus device";
    const char *dev_name = SERVER;

    if ( sock_state.ismodbus )
	{
	dev_name = MODBUS_DEV;
	}

    // Ожидаем данные с таймаутом 5 сек.
    err = in_buffer_count = recvtimeout( sock_state.socket, buf, BUFSIZE, 50, 0,
        inet_ntoa( sock_state.sin.sin_addr ), dev_name, &sock_state.stat );


    if ( err <= 0 )               /* read error */
        {
        shutdown( sock_state.socket, 0 );
        close( sock_state.socket );
        sst.erase(sst.begin() + idx, sst.begin() + idx + 1);
        return err;
        }

    if ( in_buffer_count > max_buffer_use )
        {
        sprintf( G_LOG->msg,
            "tcp_communicator_linux::do_echo max buffer use %u (in).",
	    in_buffer_count );
        G_LOG->write_log( i_log::P_WARNING );

        max_buffer_use = in_buffer_count + 0.1 * in_buffer_count;
        }

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

                if ( ( unsigned int ) res > max_buffer_use )
                    {
                    sprintf( G_LOG->msg,
                        "tcp_communicator_linux::do_echo max buffer use %u (out).",
                        res );
                    G_LOG->write_log( i_log::P_WARNING );

                    max_buffer_use = res + 0.1 * res;
                    }

                if ( res == 0 )
                    {
                    _AknOK();
                    }
                else
                    {
                    _AknData( res );
                    }
                break;

            default:
                _ErrorAkn( ERR_WRONG_CMD );

                sprintf( G_LOG->msg,
                    "tcp_communicator_linux::do_echo wrong command received on socket %d->\"%s\".",
                    sock_state.socket, inet_ntoa( sock_state.sin.sin_addr ) );
                G_LOG->write_log( i_log::P_WARNING );
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
            sock_state.evaluated = 0;
            }
        else
            {
            _ErrorAkn( ERR_WRONG_SERVICE );

            sprintf( G_LOG->msg,
                "No such service %d at socket %d->\"%s\".",
                buf[ 1 ], sock_state.socket, inet_ntoa( sock_state.sin.sin_addr ) );
            G_LOG->write_log( i_log::P_WARNING );
            }
        }

    err = sendall( sock_state.socket, buf, in_buffer_count, MSG_NOSIGNAL );
    if ( is_going_to_reboot )
        {
        killsockets();
        sleep_ms( 800 );
        }

    if ( err <= 0 )               /* write error */
        {
        sprintf( G_LOG->msg,
            "%s : tcp_communicator_linux::do_echo socket %d->\"%s\" disconnected on write try.",
            strerror( errno ), sock_state.socket, inet_ntoa( sock_state.sin.sin_addr ) );
        G_LOG->write_log( i_log::P_ERR );

        shutdown( sock_state.socket, 0 );
        close( sock_state.socket );
        sst.erase(sst.begin() + idx, sst.begin() + idx + 1);

        return err;
        }

    return err;
    }
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
