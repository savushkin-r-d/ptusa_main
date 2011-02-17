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
/// @brief Работа с энергонезависимой ОЗУ. Представляет абстракцию от 
/// физического распределения памяти.
class NV_memory_manager_W750: public NV_memory_manager
    {    
    public:
        NV_memory_manager_W750();
    };
//-----------------------------------------------------------------------------
/// @brief Работа с диодными индикаторами.
class led_W750 : public led
    {
    public:
        led_W750();

        virtual ~led_W750();

        void on( LEDS_NAMES led, COLOUR led_color );

        void off( LEDS_NAMES led );

        void blink( LEDS_NAMES led, COLOUR led_color );

    private:
        enum WAGO_LED_CMD
            {
            WAGO_FBK_LED_CMD_SET = 1,   ///< Turn on briefly to show activity.
            WAGO_FBK_LED_CMD_ON,        ///< Turn LED on permanently.
            WAGO_FBK_LED_CMD_OFF,       ///< Turn LED off permanently.
            WAGO_FBK_LED_CMD_FLASH,     ///< Flash this LED.
            };

        enum WAGO_LEDS
            {
            WAGO_FBK_LED_STATUS_RED = 2,
            WAGO_FBK_LED_STATUS_GREEN,
            WAGO_FBK_LED_SERVICE_RED,
            WAGO_FBK_LED_SERVICE_GREEN,
            WAGO_FBK_LED_USER_RED,
            WAGO_FBK_LED_USER_GREEN,
            WAGO_FBK_LED_ALL
            };

        int fd;

        void set_cmd( int cmd, int led, int led_color );

    };
#endif // SYS_7186_H
