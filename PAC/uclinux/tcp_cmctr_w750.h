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
//-----------------------------------------------------------------------------
typedef struct socket_state
    {
    int active;    ///< Сокет активен.
    int init;      ///< Сокет только что был активирован.
    int islistener;///< Сокет является инициатором соединения ( = 0 )/сокет является слушателем ( != 0 ).
    int evaluated; ///< В данном цикле уже произошел обмен информацией по данному сокету.
    int clID;      ///< Идентификатор клиента для идентификации того, не занят ли уже сокет другим клиентом.
    struct sockaddr_in sin;       ///< Адрес клиента.
    } socket_state;
//-----------------------------------------------------------------------------
/// @brief Коммуникатор для PAC WAGO 750 - обмен данными PAC-сервер.
class tcp_communicator_w750 : public tcp_communicator 
    {
        public:
            tcp_communicator_w750();
            virtual ~tcp_communicator_w750();

            int evaluate();

	private:
            struct sockaddr_in 	ssin; 	        ///< Адрес клиента.
            u_int               sin_len;    	///< Длина адреса.
            int                 master_socket;	///< Мастер-сокет для прослушивания.

#ifdef MODBUS
            int modbus_socket;	     ///< Модбас сокет.
#endif
            int ss;                  ///< Слейв-сокет, получаемый при подключении клиента.
            int rc;                  ///< Код возврата selectsocket.
		
            int do_echo( int skt );
            int netInit();

            u_long glob_last_trans;

            timeval tv;                         ///< Задержка ожидания функции опроса состояний сокетов 0 по умолчанию.
            fd_set rfds;                        ///< Набор дескрипторов сокетных файлов для чтения.
            socket_state sst[ MAX_SOCKETS ];    ///< Таблици состояния сокетов.
            int netOK;
            int tcpipClientID;

            void killsockets ();
            int  net_init();
            void net_terminate();
		
            int  recvtimeout( uint s, u_char *buf, int len,
                int timeout, int usec );
	};
	
#endif //TCP_CMCTR_W750
