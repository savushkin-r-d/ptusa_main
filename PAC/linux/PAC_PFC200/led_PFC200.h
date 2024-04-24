/// @file led_PFC200.h
/// @brief Работа со светодиодным индикатором в ОС Linux PAC PFC200.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev: 220 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-15 16:58:56#$.

#ifndef LED_PFC200_H
#define LED_PFC200_H

#include "led.h"
//-----------------------------------------------------------------------------
/// @brief Работа с диодными индикаторами.

class led_PFC200 : public led {
 public:
  led_PFC200();

  virtual ~led_PFC200();

  void on(LEDS_NAMES led, COLOUR led_color);

  void off(LEDS_NAMES led);
};
//-----------------------------------------------------------------------------
#endif  // LED_PFC200_H
