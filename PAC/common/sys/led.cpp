#if !defined WIN_OS && !( defined LINUX_OS && defined PAC_PC ) && \
    !( defined LINUX_OS && defined PAC_WAGO_750_860 ) && \
    !( defined MINIOS7 && defined UPAC_7186E ) && \
    !( defined MINIOS7 && defined UPAC_5000 )
#error You must define OS!
#endif 

#include "led.h"
#include "smart_ptr.h"

#ifdef WIN_OS
#include "w_led.h"
#endif // WIN_OS

#if defined LINUX_OS && defined PAC_PC
#include "led_PC.h"
#endif

#if defined LINUX_OS && defined PAC_WAGO_750_860
#include "led_w750.h"
#endif

#if defined MINIOS7 && defined UPAC_7186E
#include "mos7_led.h"
#endif

led* get_led()
    {
    // Статический экземпляр класса для вызова методов.
    static auto_smart_ptr < led > led_instance;

    if ( led_instance.is_null() )
        {
#ifdef WIN_OS
        led_instance = new led_PC();
#endif // WIN_OS

#if defined LINUX_OS && defined PAC_PC
        led_instance = new led_PC();
#endif

#if defined LINUX_OS && defined PAC_WAGO_750_860
        led_instance = new led_W750();
#endif

#if defined MINIOS7 && defined UPAC_7186E
        led_instance = new led_mos7();
#endif
        }

    return led_instance;
    }
