#include "w_tcp_client.h"

int win_tcp_client::Communicate( unsigned int bytestosend )
	{
	int res;
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

	FD_ZERO(&rfds);
	FD_SET(socket_number, &rfds);
	res = select(0, &rfds, 0, 0, &tv);

	if (res <= 0)
		{
#ifdef DEBUG
		Print("tcp_client_%d Ошибка получения ответа\n", id);
#endif //DEBUG
		Disconnect();
		return 0;
		}

	res = recv(socket_number, buff, buff_size, 0);

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

win_tcp_client::win_tcp_client( const char* client_ip, unsigned int client_port, unsigned int client_id, 
							   unsigned char alarm_subclass, unsigned int exchange_buf_size /*= 256*/, 
							   unsigned long send_receive_timeout /*= 100*/ ):	tcp_client(
							   client_ip, client_port, client_id, alarm_subclass, exchange_buf_size, send_receive_timeout)
	{
	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;
	}

int win_tcp_client::InitLib()
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

void win_tcp_client::DeinitLib()
	{
	if (is_initialized)
		{
		WSACleanup();
		}
	is_initialized = 0;
	}

int win_tcp_client::Connect()
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
		setsockopt(socket_number, SOL_SOCKET, SO_RCVTIMEO, ( char* )&timeout, vlen) == SOCKET_ERROR)
		{
#ifdef DEBUG
		Print("tcp_client_%d: Ошибка установления параметров сокета %d!\n", id, WSAGetLastError());        
#endif // DEBUG
		return 0;
		}

	//Переводим сокет в неблокирующий режим.
	u_long mode = 1;
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

	connect( socket_number, ( SOCKADDR* ) &sock_address, sizeof( sockaddr_in ) );

	FD_ZERO(&rfds);
	FD_SET(socket_number, &rfds);
	res = select(0, 0, &rfds, 0, &tv);

	if (res <= 0)
		{
		if (0 == res)
			{
#ifdef DEBUG
			Print("tcp_client_%d: Ошибка соединения. Таймаут\n", id);
#endif // DEBUG
			}
		else
			{
#ifdef DEBUG
			Print("tcp_client_%d: Ошибка соединения %d!\n", id, WSAGetLastError());
#endif // DEBUG
			}
		closesocket( socket_number);
		socket_number = 0;
		return 0;
		}

	int sock_error;
	int sock_err_len = sizeof(sock_error);

	if (FD_ISSET(socket_number, &rfds))
		{
		res = getsockopt(socket_number, SOL_SOCKET, SO_ERROR, (char*)&sock_error, &sock_err_len);
		if (res < 0 || sock_error != 0)
			{
#ifdef DEBUG
			Print("tcp_client_%d: Ошибка соединения(select) %d!\n", id, sock_error);
#endif // DEBUG
			closesocket( socket_number);
			socket_number = 0;
			return 0;
			}
		}

	
	connectedstate = 1;
	return 1;
	}

void win_tcp_client::Disconnect()
	{
	shutdown( socket_number, SD_BOTH );
	closesocket( socket_number );
	socket_number = 0;
	connectedstate = 0;
	}

win_tcp_client::~win_tcp_client()
	{
	if (connectedstate)
		{
		Disconnect();
		}
	}

