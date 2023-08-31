/// @file l_tcp_cmctr.h
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

#include "dtime.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <stdio.h>
#include <vector>
//-----------------------------------------------------------------------------
/// @brief Cостояние сокета.
struct socket_state
    {
    int socket;
    int active;      ///< Сокет активен.
    int init;        ///< Сокет только что был активирован.
    int is_listener; ///< Сокет является инициатором соединения ( = 0 )/сокет является слушателем ( != 0 ).
    int evaluated;   ///< В данном цикле уже произошел обмен информацией по данному сокету.
    int ismodbus;
    sockaddr_in sin; ///< Адрес клиента.


    stat_time recv_stat;  ///< Статистика работы с сокетом.
    stat_time send_stat;  ///< Статистика работы с сокетом.
    };
//-----------------------------------------------------------------------------
/// @brief Коммуникатор для Linux - обмен данными PAC<->сервер.
class tcp_communicator_linux : public tcp_communicator
    {
        public:
            tcp_communicator_linux( const char *name_rus,
                const char *name_eng );
            virtual ~tcp_communicator_linux();

            /// @brief Итерация обмена данными с сервером.
            int evaluate();

    private:
            sockaddr_in ssin; 	        ///< Адрес клиента.
            u_int       sin_len;    	///< Длина адреса.
            int         master_socket;	///< Мастер-сокет для прослушивания.

            int modbus_socket;          ///< Модбас сокет.
            int slave_socket; ///< Слейв-сокет, получаемый при подключении клиента.
            int rc; ///< Код возврата select.

            /// @brief Посылка ответных данных на сервер.
            ///
            /// @param skt - сокет.
            int do_echo( int idx );

            u_long glob_last_transfer_time;  ///< Время последней успешной передачи данных.

            timeval tv;                      ///< Задержка ожидания функции опроса состояний сокетов, 0 по умолчанию.
            fd_set rfds;                     ///< Набор дескрипторов сокетных файлов для чтения.
            std::vector< socket_state > sst; ///< Таблица состояния сокетов.
            int netOK;                       ///< Признак успешной инициализации сети.

            /// @brief Уничтожение сокетов.
            void killsockets ();

            /// @brief Инициализация сети.
            int  net_init();

            /// @brief Закрытие сети.
            void net_terminate();

        public:
            static int sendall (int sockfd, unsigned char *buf, int len,
                int sec, int usec, const char* IP, const char* name,
                stat_time *stat );

            /// @brief Получение данных с таймаутом.
            ///
            /// @param s        - сокет.
            /// @param buf      - буфер для записи полученных данных.
            /// @param len      - количество считываемых байт.
            /// @param timeout  - время ожидания, сек.
            /// @param usec     - время ожидания, мк сек.
            /// @param usec     - время ожидания, мк сек.
            /// @param IP       - IP-адрес источника.
            /// @param name     - имя источника.
            ///
            /// @return -1   - ошибка работы с сокетом.
            /// @return -2   - ошибка таймаута.
            /// @return >= 0 - размер реально считанных данных.
            static int  recvtimeout( int s, u_char *buf, int len,
                long int sec, long int usec, const char* IP, const char* name,
                stat_time *stat, char first_connect = 0 );
            static bool checkBuff( int s );
    };
//-----------------------------------------------------------------------------
#endif //TCP_CMCTR_LINUX
