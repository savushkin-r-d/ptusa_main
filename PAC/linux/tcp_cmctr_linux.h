/// @file tcp_cmctr_linux.h
/// @brief Содержит описания классов, которые предназначены для обмена данными
/// PAC-сервер по протоколу TCP для OC Linux.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef TCP_CMCTR_LINUX
#define TCP_CMCTR_LINUX

#include "tcp_cmctr.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <stdio.h>
//-----------------------------------------------------------------------------
/// @brief Cостояние сокета.
struct socket_state
    {
    int active;      ///< Сокет активен.
    int init;        ///< Сокет только что был активирован.
    int is_listener; ///< Сокет является инициатором соединения ( = 0 )/сокет является слушателем ( != 0 ).
    int evaluated;   ///< В данном цикле уже произошел обмен информацией по данному сокету.
    int clID;        ///< Идентификатор клиента для идентификации того, не занят ли уже сокет другим клиентом.
    sockaddr_in sin; ///< Адрес клиента.
    };
//-----------------------------------------------------------------------------
/// @brief Коммуникатор для Linux - обмен данными PAC<->сервер.
class tcp_communicator_linux : public tcp_communicator 
    {
        public:
            tcp_communicator_linux( const char *name );
            virtual ~tcp_communicator_linux();

            /// @brief Итерация обмена данными с сервером.
            int evaluate();

	private:
            sockaddr_in ssin; 	        ///< Адрес клиента.
            u_int       sin_len;    	///< Длина адреса.
            int         master_socket;	///< Мастер-сокет для прослушивания.

#ifdef MODBUS
            int modbus_socket;          ///< Модбас сокет.
#endif
            int slave_socket; ///< Слейв-сокет, получаемый при подключении клиента.
            int rc; ///< Код возврата select.

            /// @brief Посылка ответных данных на сервер.
            ///
            /// @param skt - сокет.
            int do_echo( int skt );
            
            u_long glob_last_transfer_time;  ///< Время последней успешной передачи данных.

            timeval tv;                      ///< Задержка ожидания функции опроса состояний сокетов, 0 по умолчанию.
            fd_set rfds;                     ///< Набор дескрипторов сокетных файлов для чтения.
            socket_state sst[ MAX_SOCKETS ]; ///< Таблица состояния сокетов.
            int netOK;                       ///< Признак успешной инициализации сети.

            /// @brief Уничтожение сокетов.
            void killsockets ();

            /// @brief Инициализация сети.
            int  net_init();

            /// @brief Закрытие сети.
            void net_terminate();

            /// @brief Получение данных с таймаутом.
            ///
            /// @param s        - сокет.
            /// @param buf      - буфер для записи полученных данных.
            /// @param len      - количество считываемых байт.
            /// @param timeout  - время ожидания, сек.
            /// @param usec     - время ожидания, мк сек.
            ///
            /// @return -1   - ошибка работы с сокетом.
            /// @return -2   - ошибка таймаута.
            /// @return >= 0 - размер реально считанных данных.
            int  recvtimeout( u_int s, u_char *buf, int len,
                int timeout, int usec );
	};
//-----------------------------------------------------------------------------
#endif //TCP_CMCTR_LINUX
