#ifndef W_DTIME_H
#define W_DTIME_H

#include <windows.h>

#include <time.h>
#include <sys\timeb.h>
#include <limits.h>
#include <chrono>
//-----------------------------------------------------------------------------
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
unsigned long get_sec()
    {
    time_t ltime;
    time( &ltime );

    return ( unsigned long )ltime;
    }
//-----------------------------------------------------------------------------
uint32_t get_delta_millisec( uint32_t time1 )
    {
    uint32_t now = get_millisec();

    return now >= time1 ? now - time1 : ULONG_MAX - time1 + now;
    }
//-----------------------------------------------------------------------------
void sleep_ms( u_int ms )
    {
    Sleep( ms );
    }
//-----------------------------------------------------------------------------
tm get_time()
    {
    static time_t t_;
    static struct tm timeInfo_;
    t_ = time( 0 );
    localtime_s( &timeInfo_, &t_ );

    return timeInfo_;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#endif // W_DTIME_H
