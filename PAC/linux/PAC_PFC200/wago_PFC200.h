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
/// @$Rev: 588 $.\n
/// @$Author: id $.\n
/// @$Date:: 2013-01-21 08:47:30#$.

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
class wago_manager_PFC200 : public wago_manager_linux
    {
     public:
        wago_manager_PFC200()
            {
            
            }

        virtual ~wago_manager_PFC200()
            {
            
            }

        int read_inputs()
            {
            return wago_manager_linux::read_inputs();
            }
        
        int write_outputs()
            {
            return wago_manager_linux::write_outputs();
            }
    };
//-----------------------------------------------------------------------------
#endif // WAGO_750_H
