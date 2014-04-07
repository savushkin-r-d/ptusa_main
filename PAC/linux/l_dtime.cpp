#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>

#include "console.h"

//-----------------------------------------------------------------------------
unsigned long get_millisec()
    {
    timespec start_tv;
    clock_gettime( 0, &start_tv );
    unsigned long now = 1000UL * start_tv.tv_sec + start_tv.tv_nsec / 1000000UL;
    return now;
    }
//-----------------------------------------------------------------------------
unsigned long get_sec()
    {
    timespec start_tv;
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
