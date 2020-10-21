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

#include "bus_coupler_io.h"

//-----------------------------------------------------------------------------
/// @brief Работа с модулями ввода/вывода для PC.
///
///
class io_manager_PC : public io_manager
    {
    public:
        io_manager_PC();

        virtual ~io_manager_PC();

        int read_inputs();
        int write_outputs();
    };
//-----------------------------------------------------------------------------
#endif // IO_PC_H


