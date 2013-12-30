#include "l_tcp_client.h"

int linux_tcp_client::Communicate( unsigned int bytestosend )
    {
    if(!is_initialized)
        {
        if (!InitLib())
            {
            return 0;
            }
        }
    if (!connectedstate)
        {
        if (!Connect())
            {
            return 0;
            }
        }

    if (send(socket_number, buff, bytestosend, 0 ) == SOCKET_ERROR)
        {
#ifdef DEBUG
        Print("tcp_client_%d Ошибка %d отсылки сообщения.\n", id, WSAGetLastError());
#endif //DEBUG
        Disconnect();
        return 0;
        }

    int res = recv(socket_number, buff, buff_size, 0);

    if (0 == res)
    {
#ifdef DEBUG
    Print("tcp_client_%d Сервер закрыл соединение\n", id);
#endif //DEBUG
    Disconnect();
    return 0;
    }

    if (res < 0)
        {
        if (res == -1)
            {
#ifdef DEBUG
            Print("tcp_client_%d Ошибка %d получения ответа\n", id, WSAGetLastError());
#endif //DEBUG
            }
        else
            {
#ifdef DEBUG
            Print("tcp_client_%d Ошибка получения ответа, истекло время ожидания\n", id);
#endif //DEBUG
            }
        Disconnect();
        return 0;
        }

    return res;
    }

linux_tcp_client::linux_tcp_client( const char* client_ip, unsigned int client_port, unsigned int client_id, 
                               unsigned char alarm_subclass, unsigned int exchange_buf_size /*= 256*/, 
                               unsigned long send_receive_timeout /*= 100*/ ):  tcp_client(
                               client_ip, client_port, client_id, alarm_subclass, exchange_buf_size, send_receive_timeout)
    {

    }

int linux_tcp_client::InitLib()
    {
    WSAData tmpWSAData;
    if (!is_initialized)
        {
        if ( WSAStartup( 0x202, &tmpWSAData ) )
            {
#ifdef DEBUG
            Print("tcp_client_%d Ошибка %d инициализации сетевой библиотеки.\n", id, WSAGetLastError());
#endif //DEBUG
            return 0;
            }
        }
    is_initialized = 1;
    return 1;
    }

void linux_tcp_client::DeinitLib()
    {
    if (is_initialized)
        {
        WSACleanup();
        }
    is_initialized = 0;
    }

int linux_tcp_client::Connect()
    {
    int res;
    if ( connectedstate )
        {
        return 1;
        }
    socket_number = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
    if (socket_number == INVALID_SOCKET)
        {
#ifdef DEBUG
        Print("tcp_client_%d: Ошибка создания сокета %d!\n", id, WSAGetLastError());    
#endif // DEBUG
    
        return 0;
        }

    int vlen = sizeof( timeout );

    if ( setsockopt(socket_number, SOL_SOCKET, SO_SNDTIMEO, ( char* )&timeout, vlen) == SOCKET_ERROR ||
        setsockopt(socket_number, SOL_SOCKET, SO_RCVTIMEO, ( char* )&timeout, vlen) == SOCKET_ERROR )
        {
#ifdef DEBUG
        Print("tcp_client_%d: Ошибка установления параметров сокета %d!\n", id, WSAGetLastError());        
#endif // DEBUG
        return 0;
        }

    //Переводим сокет в неблокирующий режим.
    u_long mode = 0;
    res = ioctlsocket( socket_number, FIONBIO, &mode );
    if ( res == SOCKET_ERROR )
        {
#ifdef DEBUG
        Print("tcp_client_%d: Ошибка перевода сокета в неблокирующий режим %d!\n", id, WSAGetLastError());     
#endif // DEBUG
   

        closesocket( socket_number);
        socket_number = 0;
        return 0;
        }

    sockaddr_in sock_address;
    memset(&sock_address,0,sizeof(sockaddr_in));
    sock_address.sin_family  = AF_INET;
    sock_address.sin_port = htons( ( u_short ) port);
    sock_address.sin_addr.s_addr = inet_addr(ip);

    res = connect( socket_number, ( SOCKADDR* ) &sock_address, sizeof( sockaddr_in ) );

    if (res)
        {
#ifdef DEBUG
        Print("tcp_client_%d: Ошибка соединения %d!\n", id, WSAGetLastError());
#endif // DEBUG
        closesocket( socket_number);
        socket_number = 0;
        return 0;
        }

    
    connectedstate = 1;
    return 1;
    }

void linux_tcp_client::Disconnect()
    {
    shutdown( socket_number, SD_BOTH );
    closesocket( socket_number );
    socket_number = 0;
    connectedstate = 0;
    }

linux_tcp_client::~linux_tcp_client()
    {
    if (connectedstate)
        {
        Disconnect();
        }
    }

