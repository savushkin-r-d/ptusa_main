#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "console.h"

//-----------------------------------------------------------------------------
unsigned long get_sec()
    {
    timespec start_tv = {0,0};
    clock_gettime( 0, &start_tv );
    unsigned long now = start_tv.tv_sec + (start_tv.tv_nsec - start_tv.tv_nsec % 100000000L) / 1000000000UL;
    return now;
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
