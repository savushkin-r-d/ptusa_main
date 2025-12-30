#include "dtime.h"

#include <chrono>
#include <cstdint>

#ifdef PTUSA_TEST
tm get_time_next_hour()
    {
    thread_local static struct tm timeInfo_;
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

tm get_fixed_time()
    {
    thread_local static struct tm timeInfo_;
    static const time_t FIXED_TIMESTAMP = 1741737600;  // 2025-03-12 00.00.00
#ifdef LINUX_OS
    gmtime_r( &FIXED_TIMESTAMP, &timeInfo_ );
#else
    gmtime_s( &timeInfo_, &FIXED_TIMESTAMP );
#endif
    return timeInfo_;
    }
#endif
//-----------------------------------------------------------------------------
uint32_t get_millisec()
    {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>( 
        duration ).count();

    return static_cast<uint32_t>( ms );
    }
//-----------------------------------------------------------------------------
uint32_t get_delta_millisec( uint32_t time1 )
    {
    uint32_t now = get_millisec();
    return now >= time1 ? now - time1 : UINT32_MAX - time1 + now;
    }
