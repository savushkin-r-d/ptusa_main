/// @file win\PAC_PC\wago_PC.h
/// @brief Работа с I/O для PC.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef IO_PC_H
#define IO_PC_H

#include <winsock2.h>
#include <vector>

#include <errno.h>

#include "dtime.h"
#include "PAC_err.h"
#include "w_tcp_cmctr.h"

#include "bus_coupler_io.h"

//-----------------------------------------------------------------------------
/// @brief Работа с модулями ввода/вывода для PC.
///
///
class io_manager_PC : public io_manager
{
protected:
    enum CONSTANTS
    {
        MAX_MODBUS_REGISTERS_PER_QUERY = 123,
        BUFF_SIZE = 262,
        PHOENIX_INPUTREGISTERS_STARTADDRESS = 8000,
        PHOENIX_HOLDINGREGISTERS_STARTADDRESS = 9000,
    };

    u_char buff[BUFF_SIZE];
    u_char* resultbuff;
    u_char* writebuff;

    /// @brief Инициализация соединения с узлом I/O.
    ///
    /// @param node - узел I/O, с которым осуществляется соединение.
    ///
    /// @return -   0 - ок.
    /// @return - < 0 - ошибка.
    int net_init(io_node* node);

    /// @brief Обмен с узлом I/O.
    ///
    /// @param node             - узел I/O, с которым осуществляется обмен.
    /// @param bytes_to_send    - размер данных для отсылки.
    /// @param bytes_to_receive - размер данных для получения.
    ///
    /// @return -   0 - ок.
    /// @return - < 0 - ошибка.
    int e_communicate(io_node* node, int bytes_to_send, int bytes_to_receive);

    int read_input_registers(io_node* node, unsigned int address, unsigned int quantity, unsigned char station = 0);
    int write_holding_registers(io_node* node, unsigned int address, unsigned int quantity, unsigned char station = 0);

    int read_inputs();
    int write_outputs();

public:
    io_manager_PC();

    virtual ~io_manager_PC();

    /// @brief Отключение от узла.
    ///
    /// @param node - узел, от которого отключаемся.
    void disconnect(io_node* node) override;
};
//-----------------------------------------------------------------------------
#endif // IO_PC_H


