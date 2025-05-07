#include "dtime.h"

#ifdef PTUSA_TEST
tm get_time_next_hour()
    {
    static struct tm timeInfo_;
#ifdef LINUX_OS
    auto t_ = time( nullptr );
    localtime_r( &t_, &timeInfo_ );
#else
    static time_t t_ = time( nullptr );
    localtime_s( &timeInfo_, &t_ );
#endif // LINUX_OS
    timeInfo_.tm_hour++;

    return timeInfo_;
    }
#endif
