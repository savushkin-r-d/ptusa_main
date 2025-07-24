#ifndef W_DTIME_H
#define W_DTIME_H

#include <windows.h>

#include <time.h>
#include <sys\timeb.h>
#include <limits.h>
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
unsigned long get_millisec()
    {
    struct _timeb tstruct;
    _ftime_s( &tstruct );

    static time_t first_time = 0;
    if ( 0 == first_time )
        {
        first_time = tstruct.time;
        }

    return ( unsigned long ) ( tstruct.time - first_time ) * 1000 + tstruct.millitm;
    }
//-----------------------------------------------------------------------------
unsigned long get_sec()
    {
    time_t ltime;
    time( &ltime );

    return ( unsigned long )ltime;
    }
//-----------------------------------------------------------------------------
unsigned long get_delta_millisec( unsigned long time1 )
    {
    unsigned long now = get_millisec();

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
