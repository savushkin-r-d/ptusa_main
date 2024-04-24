/// @file mos7_led.h
/// @brief Работа со светодиодным индикатором в ОС MINIOS7.
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

#ifndef LED_MINIOS7_H
#define LED_MINIOS7_H

#include "led.h"

//-----------------------------------------------------------------------------
/// @brief Работа с диодными индикаторами.
class led_mos7 : public led {
 public:
  void on(LEDS_NAMES led, COLOUR led_color) {}

  void off(LEDS_NAMES led) {}

  void blink(LEDS_NAMES led, COLOUR led_color) {}
};
//-----------------------------------------------------------------------------
#endif  // LED_MINIOS7_H
