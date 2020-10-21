/// @file w_rm_tech_def.h
/// @brief Реализована функциональность технологических объектов для управления
/// удаленно (через мастер-контроллер) для Windows.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev: 678 $.\n
/// @$Author: id $.\n
/// @$Date:: 2013-05-29 16:44:39#$.

#ifndef W_RM_TECH_DEFINITION_H
#define W_RM_TECH_DEFINITION_H

#include <winsock2.h>

#include "dtime.h"
#include "rm_tech_def.h"
#include "WSA_err_decode.h"

#include "lua_manager.h"

//-----------------------------------------------------------------------------
class win_rm_cmmctr: public i_rm_cmmctr
    {
    public:
        /// <summary>
        /// Конструктор.
        /// </summary>
        /// <param name="name">Имя удаленного PAC.</param>
        /// <param name="IP_address">IP адрес удаленного PAC.</param>
        /// <returns></returns>
        win_rm_cmmctr( char* name, char* IP_address, int remote_PAC_id );

        /// @brief Отсылка заданной команды PAC.
        int send_2_PAC( int service_n, const char *cmd_str, int length );

        /// @brief Обмен данными от PAC.
        int evaluate();

    private:
        /// <summary>
        /// Инициализация сетевой библиотеки.
        /// </summary>
        /// <returns>0 - успех.</returns>
        /// <returns>1 - ошибка.</returns>
        int net_init();

        /// <summary>
        /// Подключение к удаленному PAC.
        /// </summary>
        /// <returns>0 - успех.</returns>
        /// <returns>1 - ошибка.</returns>
        int connect_to_PAC();

        /// <summary>
        /// Отключение от удаленного PAC.
        /// </summary>
        void disconnect();

        // @brief Флаг успешной инициализации сети.
        bool is_initialized;

        // @brief Флаг успешного соединения с удаленным PAC.
        bool is_connected;

        // @brief Время последнего удачного обмена с удаленным PAC.
        u_long last_transfer_time;

        // @brief Флаг ошибки связи с удаленным PAC.
        bool cmctr_err;   

        // @brief Сокет связи с удаленным PAC.
        int remote_PAC_socket;

        int port;

        UCHAR pidx;

        // @brief Флаг получения устройств удаленного PAC.
        bool got_devices;

        /// @brief Номер запроса устройств в PAC. Устанавливается в 0 при
        /// загрузке PAC.
        u_int_2 devices_request_id; 
    };
//-----------------------------------------------------------------------------
#endif // W_RM_TECH_DEFINITION_H
