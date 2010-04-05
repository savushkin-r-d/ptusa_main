/// @file tcp_cmctr.h
/// @brief Содержит описания классов, которые предназначены для обмена данными
/// PAC-сервер по протоколу TCP.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// 
/// @par Текущая версия:
/// @$Rev: 39 $.\n
/// @$Author: id@BMK $.\n
/// @$Date:: 2010-04-02 17:22:01#$.

#ifndef TCP_CMCTR_H
#define TCP_CMCTR_H

#include "sys.h"

//-----------------------------------------------------------------------------
/// @brief Базовый класс коммуникатор - обмен данными PAC-сервер.
class tcp_communicator
    {
    public:
        /// @brief Определение функции сервиса.
        typedef long int srv_proc( long int, u_char *, u_char * );
        typedef srv_proc *srv_ptr;

        /// @brief Получение единственного экземпляра класса для работы с 
        /// коммуникатором.
        ///
        /// @return - указатель на единственный объект класса @ref
        /// tcp_communicator.
        static tcp_communicator* get_instance();

        /// @brief Инициализация единственного экземпляра класса для работы с 
        /// коммуникатором.
        ///
        /// @param - указатель на единственный объект класса, производного от
        /// @ref tcp_communicator.
        static void set_instance( tcp_communicator* new_instance );

        /// @brief Итерация обмена данными с сервером.
        virtual int evaluate() = 0;

        /// @brief Добавление пользовательской функции по обмену данными - 
        /// сервиса.
        ///
        /// @param srv_id - номер, за которым будет закреплен сервис.
        /// @param fk     - указатель на объект выделенного блока памяти.        
        srv_ptr reg_service( u_char srv_id, srv_ptr fk );

        /// @brief Получение сетевого имени PAC.
        ///
        /// @return - сетевое имя PAC.
        char* get_host_name();

    protected:
        tcp_communicator();

        //ERRORS DEFINITION
        enum ERRORS
            {
            ERR_RETRIVE       = 1,
            ERR_WRONG_SERVICE = 3,
            ERR_TRANSMIT      = 4,
            ERR_WRONG_CMD     = 5,
            };

        //COMMANDS DEFINITION  
        enum COMMANDS 
            {
            FRAME_SINGLE = 1,
            AKN_ERR      = 7,
            AKN_DATA     = 8,
            AKN_OK       = 12,
            };

        enum CONSTANTS
            {
            BUFSIZE     = 16384,           ///< Размер буфера.
            PORT 	= 10000,           ///< Порт.
            MAX_SOCKETS = 32,              ///< Максимальное количество сокетов.
            QLEN        = MAX_SOCKETS - 1, ///< Максимальное количество соединений.

            TC_MAX_HOST_NAME      = 20,
            TC_MAX_SERVICE_NUMBER = 16,

            };

        static tcp_communicator* instance;          ///< Экземпляр класса.
        
        srv_ptr services[ TC_MAX_SERVICE_NUMBER ];  ///< Массив сервисов.
        char    host_name[ TC_MAX_HOST_NAME ];      ///< Сетевое имя PAC.

        int max_cycles;         ///< Максимальное количество циклов обработки состояний сокетов за 1 проход.
        int is_going_to_reboot; ///< Флаг перезагрузки PAC.
        int glob_cmctr_ok;      ///< Флаг активности обмена с сервером.

        u_int   in_buffer_count;///< Количество данных в буфере.
        u_char* buf;            ///< Буфер.

        u_char pidx;            ///< Номер ответа.
        int    net_id;          ///< Номер PAC.

        void _ErrorAkn( u_char error );
        void _AknData( u_long len );
        void _AknOK();
    };
//-----------------------------------------------------------------------------
#endif //TCP_CMCTR_H
