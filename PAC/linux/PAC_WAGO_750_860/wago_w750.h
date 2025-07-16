/// @file wago_w750.h
/// @brief Работа с Wago для PAC WAGO 750-860.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG      - компиляция c выводом отладочной информации в консоль.
/// @c DEBUG_KBUS - компиляция c выводом отладочной информации работы KBUS в
/// консоль.
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef WAGO_750_H
#define WAGO_750_H

#include "l_wago.h"

extern "C"
    {
    #include "kbusapi.h"
    }
//-----------------------------------------------------------------------------
/// @brief Работа с модулями ввода/вывода Wago для PAC WAGO 750-860.
///
///
class wago_manager_w750 : public wago_manager_linux
    {
     public:
        wago_manager_w750();

        virtual ~wago_manager_w750();

        int read_inputs();
        int write_outputs();
    };
//-----------------------------------------------------------------------------
#endif // WAGO_750_H
