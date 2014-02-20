#include "led_PFC200.h"
extern "C" {
    #include <diagnostic/diagnostic_API.h>
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
led_PFC200::led_PFC200( )
    {
    }
//-----------------------------------------------------------------------------
led_PFC200::~led_PFC200( )
    {
    off( L_ALL );
    }
//-----------------------------------------------------------------------------
void led_PFC200::on( LEDS_NAMES led, COLOUR led_color )
    {    
    switch ( led )
        {
        case L_ALL:
            break;

        case L_STATUS: //USR1
            switch ( led_color )
                {
                case C_GREEN:
                    log_EVENT_LogId( 0x00050010, true );
                    break;

                case C_RED:
                    log_EVENT_LogId( 0x00050011, true );
                    break;
                    
                case C_YELLOW:
                    log_EVENT_LogId( 0x00050012, true );
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
void led_PFC200::off( LEDS_NAMES led )
    {    
    switch ( led )
        {
        case L_ALL:
            log_EVENT_LogId( 0x00050000, true ); //Switch off U1-LED.
            break;

        case L_STATUS:
            log_EVENT_LogId( 0x00050000, true ); //Switch off U1-LED.
            break;

        case L_SERVICE:
            break;

        case L_USER:
            break;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
