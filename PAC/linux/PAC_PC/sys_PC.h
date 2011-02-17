/// @file sys_w750.h
/// @brief Содержит описания классов, которые реализуют аппаратную часть 
/// PAC WAGO 750.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef SYS_7186_H
#define SYS_7186_H

#include "sys_linux.h"
//-----------------------------------------------------------------------------
/// @brief Работа с энергонезависимой ОЗУ. 
class NV_memory_manager_PC: public NV_memory_manager
    {    
    public:
        NV_memory_manager_PC();       
    };
//-----------------------------------------------------------------------------
/// @brief Работа с диодными индикаторами.
class led_PC : public led
    {
    public:
        void on( LEDS_NAMES led, COLOUR led_color )
            {
            }

        void off( LEDS_NAMES led )
            {
            }

        void blink( LEDS_NAMES led, COLOUR led_color )
            {
            }
    };
//-----------------------------------------------------------------------------
#endif // SYS_7186_H
