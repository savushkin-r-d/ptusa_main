/// @file wago_PC.h
/// @brief Работа с Wago для PC.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG   - компиляция c выводом отладочной информации в консоль.
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef WAGO_PC_H
#define WAGO_PC_H

#include "wago.h"

//-----------------------------------------------------------------------------
/// @brief Работа с модулями ввода/вывода Wago для PC.
///
///
class wago_manager_PC : public wago_manager
    {
    public:
        wago_manager_PC();

        virtual ~wago_manager_PC();

        int read_inputs();
        int write_outputs();
    };
//-----------------------------------------------------------------------------
#endif // WAGO_PC_H


