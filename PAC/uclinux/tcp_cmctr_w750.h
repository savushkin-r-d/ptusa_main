#ifndef TCP_CMCTR_W750
#define TCP_CMCTR_W750

#include "tcp_cmctr.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <stdio.h>

typedef struct socket_state
	{
	int active;    ///< Сокет активен.
	int init;      ///< Сокет только что был активирован.
	int islistener;///< Сокет является инициатором соединения ( = 0 )/сокет является слушателем ( != 0 ).
	int evaluated; ///< В данном цикле уже произошел обмен информацией по данному сокету.
	int clID;      ///< Идентификатор клиента для идентификации того, не занят ли уже сокет другим клиентом.
	struct sockaddr_in sin;       ///< Адрес клиента.
	} socket_state;

/// @brief Коммуникатор для PAC WAGO 750 - обмен данными PAC-сервер.
class tcp_communicator_w750 : public tcp_communicator 
	{
	public:
        tcp_communicator_w750();
		~tcp_communicator_w750();

	private:
		struct sockaddr_in 	ssin; 			///< Адрес клиента.
		unsigned int 		sin_len;    	///< Длина адреса.
		int 				master_socket;	///< Мастер-сокет для прослушивания.

#ifdef MODBUS
		int modbus_socket;				     ///< Модбас сокет.
#endif
		int ss;                  ///< Слейв-сокет, получаемый при подключении клиента.
		int rc;                  ///< Код возврата selectsocket.
		
        int do_echo( int skt );
		int netInit();

		int reboot;
		int glob_cmctr_ok;
		unsigned char* buf;
		int max_cycles;                     ///< максимальное количество циклов обработки состояний сокетов за 1 проход.
		timeval tv;                         ///< задержка ожидания функции опроса состояний сокетов 0 по умолчанию.
		fd_set rfds;                        ///< набор дескрипторов сокетных файлов для чтения.
		int netOK;
		int tcpipClientID;

		void killsockets ();
		int net_init();
		void net_terminate();
		
		int recvtimeout( uint s, uchar *buf, int len, int timeout, int usec );
	};
	
#endif //TCP_CMCTR_W750
