#include "sys_w750.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NV_memory_manager_W750::NV_memory_manager_W750():NV_memory_manager()
    {
    PAC_NVRAM  = new SRAM( "/dev/nvram", 32768, 0, 30 );
    PAC_EEPROM = new SRAM( "/dev/nvram", 32768, 31, 32767 );
    last_NVRAM_pos  = PAC_NVRAM->get_available_start_pos();
    last_EEPROM_pos = PAC_EEPROM->get_available_start_pos();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
led_W750::led_W750()
    {
    fd = open( "/dev/ledman", O_RDWR );
#ifdef DEBUG
    if ( fd <= 0 )
        {
        perror( "Error init led" );
        }
#endif // DEBUG
    }
//-----------------------------------------------------------------------------
led_W750::~led_W750()
    {
    if ( fd > 0 )
        {
        close( fd );
        }
    fd = 0;
    }
//-----------------------------------------------------------------------------
void led_W750::on( LEDS_NAMES led, COLOUR led_color )
    {
    set_cmd( WAGO_FBK_LED_CMD_ON, led, led_color );
    }
//-----------------------------------------------------------------------------
void led_W750::off( LEDS_NAMES led )
    {
    set_cmd( WAGO_FBK_LED_CMD_OFF, led, C_YELLOW );
    }
//-----------------------------------------------------------------------------
void led_W750::blink( LEDS_NAMES led, COLOUR led_color )
    {    
    set_cmd( WAGO_FBK_LED_CMD_FLASH, led, led_color );
    }
//-----------------------------------------------------------------------------
void led_W750::set_cmd( int cmd, int led, int led_color )
    {
    if ( fd != -1 )
        {
        int led_green = 0;
        int led_red = 0;
        switch ( led )
            {
            case L_ALL:
                led_green = WAGO_FBK_LED_ALL;
                led_red = WAGO_FBK_LED_ALL;
                break;

            case L_STATUS:
                led_green = WAGO_FBK_LED_STATUS_GREEN;
                led_red = WAGO_FBK_LED_STATUS_RED;
                break;

            case L_SERVICE:
                led_green = WAGO_FBK_LED_SERVICE_GREEN;
                led_red = WAGO_FBK_LED_SERVICE_RED;
                break;

            case L_USER:
                led_green = WAGO_FBK_LED_USER_GREEN;
                led_red = WAGO_FBK_LED_USER_RED;
                break;
            }
    
        ioctl( fd, WAGO_FBK_LED_CMD_OFF, led_green );
        ioctl( fd, WAGO_FBK_LED_CMD_OFF, led_red );

        switch ( led_color )
            {
            case C_GREEN:
                ioctl( fd, cmd, led_green );
                break;

            case C_YELLOW:
                ioctl( fd, cmd, led_green );
                ioctl( fd, cmd, led_red );
                break;

            case C_RED:
                ioctl( fd, cmd, led_red );
                break;
            }
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

