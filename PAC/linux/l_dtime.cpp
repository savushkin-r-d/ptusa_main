#include <stdio.h>
#include <limits.h>
#include <sys/time.h>
#include <unistd.h>

//-----------------------------------------------------------------------------
unsigned long get_millisec()
    {
    static timeval start_tv;
    static char is_init = 0;
    if ( 0 == is_init )
        {
        gettimeofday( &start_tv, NULL );
        is_init = 1;
        }

    timeval tv;
    gettimeofday( &tv, NULL );

    unsigned long now = 1000UL * ( tv.tv_sec - start_tv.tv_sec ) +
            ( tv.tv_usec - start_tv.tv_usec ) / 1000;

    return now;
    }
//-----------------------------------------------------------------------------
unsigned long get_sec()
    {
    timeval tv;
    gettimeofday( &tv, NULL );

    return tv.tv_sec;
    }
//-----------------------------------------------------------------------------
unsigned long get_delta_millisec( unsigned long time1 )
    {
    unsigned long now = get_millisec();

    return now >= time1 ? now - time1 : ULONG_MAX - time1 + now;
    }
//-----------------------------------------------------------------------------
void sleep_ms( unsigned int ms )
    {
    usleep( 1000 * ms );
    }
//-----------------------------------------------------------------------------
