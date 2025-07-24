#if !defined WIN_OS && \
    !( defined LINUX_OS && defined PAC_PC ) && \
    !( defined LINUX_OS && defined PAC_WAGO_750_860 ) && \
    !( defined LINUX_OS && defined PAC_WAGO_PFC200 ) && \
	!( defined LINUX_OS && defined PAC_PLCNEXT )
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

#if defined LINUX_OS && defined PAC_WAGO_PFC200
#include "led_PFC200.h"
#endif

#if defined LINUX_OS && defined PAC_PLCNEXT
#include "led_PC.h"
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
        
#if defined LINUX_OS && defined PAC_WAGO_PFC200
        led_instance = new led_PFC200();
#endif // defined LINUX_OS && defined PAC_WAGO_750_860

#if defined LINUX_OS && defined PAC_PLCNEXT
        led_instance = new led_PC();
#endif
        }

    return led_instance;
    }
