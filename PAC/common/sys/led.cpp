#if !defined WIN_OS
#error You must define OS!
#endif // !defined WIN_OS

#include "led.h"
#include "smart_ptr.h"

#ifdef WIN_OS
#include "w_led.h"
#endif // WIN_OS

led* get_led()
    {
    // Статический экземпляр класса для вызова методов.
    static auto_smart_ptr < led > led_instance;

    if ( led_instance.is_null() )
        {
#ifdef WIN_OS
        led_instance = new led_PC();
#endif // WIN_OS
        }

    return led_instance;
    }