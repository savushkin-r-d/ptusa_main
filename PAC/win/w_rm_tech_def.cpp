#include "w_rm_tech_def.h"

//-----------------------------------------------------------------------------
int win_rm_cmmctr::net_init()
    {
    is_initialized = false;

    WSAData tmpWSAData;

    if ( WSAStartup( 0x202, &tmpWSAData ) )
        {
        fprintf( stderr, "Error initializing network library: " );
        fprintf( stderr, WSA_Last_Err_Decode() );
        fprintf( stderr, "\n" );
        return 1;
        }

    is_initialized = true;
    return 0;
    }
//-----------------------------------------------------------------------------
win_rm_cmmctr::win_rm_cmmctr( char* name, char* IP_address, int remote_PAC_id ) :
    i_rm_cmmctr( name, IP_address, remote_PAC_id ),
    is_initialized( false ),
    is_connected( false ),
    cmctr_err( false ),
    port( 10000 ),
    pidx( 0 ),
    remote_PAC_socket( 0 ),
    got_devices( false ),
    devices_request_id( 0 )
    {
    last_transfer_time = get_sec();
    }
//-----------------------------------------------------------------------------
int win_rm_cmmctr::send_2_PAC( int service_n, const char *cmd_str, int length )
    {
    answer_size = 0;

    // Инициализация сети, при необходимости.
    if ( !is_initialized )
        {
        net_init();
        if ( !is_initialized ) return 1;
        }

    // Подключение к удаленному PAC.
    if ( !is_connected )
        {
        connect_to_PAC();
        if ( !is_connected ) return 2;
        }

    in_buff[ 0 ] = 's';
    in_buff[ 1 ] = service_n;
    in_buff[ 2 ] = 1;                 // FrameSingle.
    in_buff[ 3 ] = ++pidx;            // Идентификатор пакета.
    in_buff[ 4 ] = ( char ) ( length >> 8 );
    in_buff[ 5 ] = length & 0xFF;
    memcpy( in_buff + 6, cmd_str, length );

    if ( send( remote_PAC_socket, in_buff, length + 6, 0 ) == SOCKET_ERROR )
        {
        fprintf( stderr, "rm_cmmctr: Ошибка отсылки сообщения для \"%s\"!\n",
            name.c_str() );
        disconnect();
        return 1;
        }

    memset( in_buff, 0, sizeof( in_buff ) );
    int res = recv( remote_PAC_socket, in_buff, 8000, 0 );

    if ( 0 == res )
        {
        fprintf( stderr, "PAC закрыл соединение." );
        disconnect();
        return 1;
        }

    if ( res < 0 /*res == SOCKET_ERROR*/ )
        {
        fprintf( stderr,  "Ошибка получения ответа!" );
        disconnect();
        return 1;
        }

    if ( in_buff[ 1 ] == 7 )
        {
        fprintf( stderr, "Возвращена ошибка!" );
        return 1;
        }
    unsigned char *work_buff = ( unsigned char* ) in_buff;

    //-Проверка на правильность заголовка блока данных от PAC.
    if ( !( work_buff[ 0 ] == 's'                   //NetId
        && pidx == work_buff[ 2 ] ) )
        {
        fprintf( stderr, "Возвращен неверный ответ!" );
        disconnect();
        if ( G_DEBUG )
            {
            //        _DebugBreak();
            }
        return 1;
        }

    answer_size = work_buff[ 3 ] * 256 + work_buff[ 4 ];
    if ( 0 == answer_size )
        {
        fprintf( stderr, "Длина ответа - 0!" );
        return 1;
        }

    if ( answer_size > P_MAX_BUFFER_SIZE )
        {
        fprintf( stderr,
            "Длина ответа[ %d ] > максимальной длины[ %d ]!",
            answer_size, P_MAX_BUFFER_SIZE );
        answer_size = 0;

        return 1;
        }

    int  tmp_answer_size = answer_size + 5 - res;
    char *tmp_buff = in_buff + res;
    while ( tmp_answer_size > 0 )
        {
        res = recv( remote_PAC_socket, tmp_buff, 8000, 0 );

        if ( res <= 0 /*res == SOCKET_ERROR*/ )
            {
            fprintf( stderr, "Получена часть ответа от PAC!" );

            answer_size = 0;

            //disconnect();
            return 1;
            }

        tmp_buff += res;
        tmp_answer_size -= res;
        }

    Lua_str = in_buff + 5;

    return 0;
    }
//-----------------------------------------------------------------------------
int win_rm_cmmctr::evaluate()
    {
    // Проверка связи с удаленным PAC.
    if ( get_sec() - last_transfer_time > 5 /*сек*/)
        {
        if ( cmctr_err == false )
            {
            cmctr_err = true;
            PAC_critical_errors_manager::get_instance()->set_global_error(
                PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_REMOTE_PAC,
                ( u_long ) remote_PAC_id );
            }
        }
    else
        {
        if ( cmctr_err == true )
            {
            cmctr_err = false;
            PAC_critical_errors_manager::get_instance()->reset_global_error(
                PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_REMOTE_PAC,
                ( u_long ) remote_PAC_id );
            }
        }

    if ( !got_devices )
        {
        //Получение устройств в первый раз.
        char buff[ 1 ] = { (char)device_communicator::CMD_RM_GET_DEVICES };
        send_2_PAC( PAC_CMMCTR_SERVICE_ID, buff, sizeof( buff ) );

        unsigned int answer_size;
        char *answer = get_out_data( answer_size );
        if ( answer_size > 0 )
            {
            devices_request_id = ( ( u_int_2* ) answer )[ 0 ];

            int res = G_LUA_MANAGER->exec_Lua_str( answer + 2,
                "Ошибка получения объектов remote PAC" );
            if( res != 0 )
                {
                return -1;
                }

            got_devices = true;
            last_transfer_time = get_sec();
            }
        }

    if ( got_devices )
        {
        //Получение состояния устройств.
        char buff[ 3 ] = { 0 };
        buff[ 0 ] = ( char ) device_communicator::CMD_RM_GET_DEVICES_STATES;

        ( ( u_int_2* ) ( buff + 1 ) )[ 0 ] = 0; //devices_request_id;
        const int cmd_length = 3;
        send_2_PAC( PAC_CMMCTR_SERVICE_ID, buff, cmd_length );

        unsigned int answer_size;
        char *answer = get_out_data( answer_size );
        if ( answer_size > 0 )
            {
            if ( devices_request_id != ( ( u_int_2* ) answer )[ 0 ] )
                {
                printf( "Устройства %s изменились.\n", name.c_str() );

                got_devices = false;
                }
            else
                {
                int res = G_LUA_MANAGER->exec_Lua_str( answer + 2,
                    "Ошибка получения состояния объектов remote PAC" );
                if( res != 0 )
                    {
                    return -1;
                    }
                }

            last_transfer_time = get_sec();
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int win_rm_cmmctr::connect_to_PAC()
    {
    static bool is_set_select_err = false;

    is_connected = false;

    static int err_cnt = 0;
    static int idle_cnt = 0;

    if ( err_cnt < idle_cnt)
        {
        err_cnt++;
        return 1;
        }

    err_cnt = 0;

    int type     = SOCK_STREAM;
    int protocol = 0;        /* всегда 0 */
    //-Cоздание мастер-сокета.
    remote_PAC_socket = socket( PF_INET, type, protocol );

    if ( remote_PAC_socket < 0 )
        {
if ( G_DEBUG )
 {
        printf( "rm_cmmct_win - can't create master socket: %s\n",
            WSA_Last_Err_Decode() );
}
        return -4;
        }

if ( G_DEBUG )
 {
    if( is_set_select_err == false )
        {
        printf( "rm_cmmct_win - master socket [ %d ] created.\n",
            remote_PAC_socket );
        }
}

    //-Адресация мастер-сокета.
    const int on = 1;
    if ( setsockopt( remote_PAC_socket, SOL_SOCKET, SO_REUSEADDR,
        ( char* ) &on, sizeof( on ) ) )
        {
        printf( "rm_cmmct_win - ошибка  вызова  setsockopt: %s\n",
            WSA_Last_Err_Decode() );
        closesocket( remote_PAC_socket );
        return -5;
        }

    //Переводим сокет в неблокирующий режим.
    u_long mode = 1;
    int res = ioctlsocket( remote_PAC_socket, FIONBIO, &mode );
    if ( res == SOCKET_ERROR )
        {
        fprintf( stderr, "rm_cmmct_win - Ошибка перевода сокета в неблокирующий режим!\n" );

        closesocket( remote_PAC_socket );
        return 0;
        }

    sockaddr_in sock_address;
    memset(&sock_address,0,sizeof(sockaddr_in));
    sock_address.sin_family  = AF_INET;
    sock_address.sin_port = htons( ( u_short ) port);
    sock_address.sin_addr.s_addr = inet_addr( IP_address.c_str() );

    connect( remote_PAC_socket, ( SOCKADDR* ) &sock_address, sizeof( sockaddr_in ) );

    fd_set wrevents{};
    struct timeval tv{};
    FD_ZERO( &wrevents );
    FD_SET( remote_PAC_socket, &wrevents );

    tv.tv_sec = 0;
    tv.tv_usec = 200;

    int rc = select( ( int ) remote_PAC_socket, 0, &wrevents, 0, &tv );

    if ( SOCKET_ERROR == rc || 0 == rc )
        {
        if ( is_set_select_err == false )
            {
            fprintf( stderr, "rm_cmmct_win - Ошибка установления соединения!\n" );
            is_set_select_err = true;
            }

        closesocket( remote_PAC_socket );

        if ( idle_cnt < 100 )
            {
            idle_cnt += 10;
            }

        return 0;
        }
    idle_cnt = 0;
    is_set_select_err = false;

    //Переводим сокет в блокирующий режим.
    mode = 0;
    res = ioctlsocket( remote_PAC_socket, FIONBIO, &mode );
    if ( res == SOCKET_ERROR )
        {
        fprintf( stderr, "rm_cmmct_win - Ошибка перевода сокета в блокирующий режим!\n" );
        }

    res = recv( remote_PAC_socket, in_buff, 255, 0 );
    if ( SOCKET_ERROR == res )
        {
        fprintf( stderr, "rm_cmmct_win - Ошибка получения ответа при подключении!\n" );
        closesocket( remote_PAC_socket );

        return 0;
        }

    is_connected = true;

    return 0;
    }
//-----------------------------------------------------------------------------
void win_rm_cmmctr::disconnect()
    {
    if ( is_connected )
        {
        closesocket( remote_PAC_socket );
        remote_PAC_socket = 0;
        }

    is_connected = 0;
    }
//-----------------------------------------------------------------------------
