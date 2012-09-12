/// @file led_w750.h
/// @brief Работа со светодиодным индикатором в ОС Linux PAC 750-860.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG    - компиляция c выводом отладочной информации в консоль.@n@n
///
/// @par Текущая версия:
/// @$Rev: 220 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-15 16:58:56#$.

#ifndef LED_W750_H
#define LED_W750_H

#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "led.h"
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

        FILE *fd;

        void set_cmd( int cmd, int led, int led_color );
    };
//-----------------------------------------------------------------------------
#endif // LED_W750_H
