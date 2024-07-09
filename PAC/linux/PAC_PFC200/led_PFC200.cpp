#include "led_PFC200.h"
extern "C" {
#include <diagnostic/diagnostic_API.h>

#include "diagnostic/led_server.h"
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
led_PFC200::led_PFC200() {}
//-----------------------------------------------------------------------------
led_PFC200::~led_PFC200() { off(L_ALL); }
//-----------------------------------------------------------------------------
void led_PFC200::on(LEDS_NAMES led, COLOUR led_color) {
  switch (led) {
    case L_ALL:
      break;

    case L_STATUS:  // USR1
      switch (led_color) {
        case C_GREEN:
          log_EVENT_LogId(DIAG_LED_USER1_STATIC_GRN, true);
          break;

        case C_RED:
          log_EVENT_LogId(DIAG_LED_USER1_STATIC_RED, true);
          break;

        case C_YELLOW:
          log_EVENT_LogId(DIAG_LED_USER1_STATIC_YLW, true);
          break;
      }
      break;

    case L_SERVICE:
      break;

    case L_USER:
      break;
  }
}
//-----------------------------------------------------------------------------
void led_PFC200::off(LEDS_NAMES led) {
  switch (led) {
    case L_ALL:
      log_EVENT_LogId(DIAG_LED_USER1_STATIC_OFF, true);
      break;

    case L_STATUS:
      log_EVENT_LogId(DIAG_LED_USER1_STATIC_OFF, true);
      break;

    case L_SERVICE:
      break;

    case L_USER:
      break;
  }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
