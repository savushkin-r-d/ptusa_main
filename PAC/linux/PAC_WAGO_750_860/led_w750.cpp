#include "led_w750.h"

#include "string"
#include "string.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
led_W750::led_W750()
    {
    const char *green_status_str  = "/sys/class/leds/wago:green:status/brightness";
    const char *red_status_str    = "/sys/class/leds/wago:red:status/brightness";

    green_status_fd  = 0;
    red_status_fd    = 0;

    green_status_fd = open( green_status_str, O_WRONLY );
    if ( green_status_fd < 0 )
        {
        perror( "Error opening green status lamp" );
        }
    red_status_fd = open( red_status_str, O_WRONLY );
    if ( red_status_fd < 0 )
        {
        perror( "Error opening red status lamp" );
        }
    }
//-----------------------------------------------------------------------------
led_W750::~led_W750()
    {
     if ( green_status_fd > 0 )
        {
        close( green_status_fd );
        green_status_fd = 0;
        }
    if ( red_status_fd > 0 )
        {
        close( red_status_fd );
        red_status_fd = 0;
        }
    }
//-----------------------------------------------------------------------------
void led_W750::on( LEDS_NAMES led, COLOUR led_color )
    {
    static const char *cmd = "255";

    switch ( led )
        {
        case L_ALL:
            break;

        case L_STATUS:
            switch ( led_color )
                {
                case C_GREEN:
                    if ( green_status_fd > 0 )
                        {
                        write( green_status_fd, cmd, strlen( cmd ) );
                        }
                    break;

                case C_YELLOW:
                    if ( green_status_fd > 0 )
                        {
                        write( green_status_fd, cmd, strlen( cmd ) );
                        }
                    if ( red_status_fd > 0 )
                        {
                        write( red_status_fd, cmd, strlen( cmd ) );
                        }                    
                    break;

                case C_RED:
                    if ( red_status_fd > 0 )
                        {
                        write( red_status_fd, cmd, strlen( cmd ) );
                        }
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
void led_W750::off( LEDS_NAMES led )
    {
    static const char *off_cmd = "0";

    switch ( led )
        {
        case L_ALL:
            break;

        case L_STATUS:
           if ( green_status_fd > 0 )
                {
                write( green_status_fd, off_cmd, strlen( off_cmd ) );
                }
           if ( red_status_fd > 0 )
                {
                write( red_status_fd, off_cmd, strlen( off_cmd ) );
                }
            break;

        case L_SERVICE:
            break;

        case L_USER:
            break;
        }
    }
//-----------------------------------------------------------------------------
void led_W750::blink( LEDS_NAMES led, COLOUR led_color )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
