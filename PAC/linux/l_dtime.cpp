#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <chrono>

#include "console.h"

//-----------------------------------------------------------------------------
unsigned long get_millisec()
    {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
        duration ).count();
    
    return static_cast<unsigned long>( millis );
    }
//-----------------------------------------------------------------------------
unsigned long get_sec()
    {
    timespec start_tv = {0,0};
    clock_gettime( 0, &start_tv );
    unsigned long now = start_tv.tv_sec + (start_tv.tv_nsec - start_tv.tv_nsec % 100000000L) / 1000000000UL;
    return now;
    }
//-----------------------------------------------------------------------------
unsigned long get_delta_millisec( unsigned long time1 )
    {
    unsigned long now = get_millisec();
    if (now < time1)
    {
        now += 2UL;
    }
    return now >= time1 ? now - time1 : ULONG_MAX - time1 + now;
    }
//-----------------------------------------------------------------------------
void sleep_ms( unsigned int ms )
    {
    usleep( 1000 * ms );
    }
//-----------------------------------------------------------------------------
tm get_time()
    {
    static time_t t_;
    struct tm *timeInfo_;
    t_ = time( 0 );
    timeInfo_ = localtime( &t_ );

    return *timeInfo_;
    }
//-----------------------------------------------------------------------------
