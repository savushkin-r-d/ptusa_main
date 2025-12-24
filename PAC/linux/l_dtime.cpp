#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#include "console.h"

//-----------------------------------------------------------------------------
unsigned long get_millisec()
    {
    timespec start_tv = {0, 0};
#ifdef PAC_WAGO_750_860
    clock_gettime( CLOCK_REALTIME, &start_tv );
#else
    clock_gettime( CLOCK_MONOTONIC, &start_tv );
#endif

    unsigned long now = 1000UL * start_tv.tv_sec + start_tv.tv_nsec / 1'000'000UL;

    return now;
    }
//-----------------------------------------------------------------------------
unsigned long get_sec()
    {
    timespec start_tv = {0,0};
    clock_gettime( 0, &start_tv );
    unsigned long now = start_tv.tv_sec + (start_tv.tv_nsec -
        start_tv.tv_nsec % 100'000'000L) / 1'000'000'000UL;
    return now;
    }
//-----------------------------------------------------------------------------
uint64_t get_delta_millisec( uint64_t time1 )
    {
    uint64_t now = get_millisec();
    if (now < time1)
    {
        now += 2UL;
    }
    return now >= time1 ? now - time1 : UINT64_MAX - time1 + now;
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
