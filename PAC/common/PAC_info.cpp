#include "PAC_info.h"

const u_int_4 PAC_info::MSEC_IN_DAY = 24UL * 60UL * 60UL * 1000UL;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PAC_info::PAC_info() :
    par( new saved_params_u_int_4( P_PARAMS_COUNT ) ),
    up_days( 0 ),
    up_msec( 0 ),        
    last_check_time( get_millisec() ),
    reset_type( 1 ), //+ IsResetByWatchDogTimer()
    str_datatime( 1, "UP_TIME", up_time_str, C_MAX_STR_LENGTH )    
    {
    com_dev = new complex_device( 1, "SYSTEM", 7, i_complex_device::COMPLEX_DEV );
    com_dev->sub_dev[ 0 ] = new var_state( "UP_DAYS",
        i_complex_device::ARRAY_DEV_LONG, up_days );
    com_dev->sub_dev[ 1 ] = new var_state( "UP_HOURS",
        i_complex_device::ARRAY_DEV_LONG, up_hours );
    com_dev->sub_dev[ 2 ] = new var_state( "UP_MINS",
        i_complex_device::ARRAY_DEV_LONG, up_mins );
    com_dev->sub_dev[ 3 ] = new var_state( "UP_SECS",
        i_complex_device::ARRAY_DEV_LONG, up_secs );    
    com_dev->sub_dev[ 4 ] = new var_state( "RESET_BY",
       i_complex_device::ARRAY_DEV_LONG, reset_type );
    com_dev->sub_dev[ 5 ] = &str_datatime;
    com_dev->sub_dev[ 6 ] = par;
    }
//-----------------------------------------------------------------------------
PAC_info::~PAC_info()
    {
    delete com_dev->sub_dev[ 0 ];
    delete com_dev->sub_dev[ 1 ];
    delete com_dev->sub_dev[ 2 ];
    delete com_dev->sub_dev[ 3 ];
    delete com_dev->sub_dev[ 4 ];

    delete com_dev;
    com_dev = 0;

    delete par;
    par = 0;
    }
//-----------------------------------------------------------------------------
void PAC_info::eval()
    {
    if ( get_delta_millisec( last_check_time ) > 1000 )
        {
        up_msec += get_delta_millisec( last_check_time );
        if ( up_msec >= MSEC_IN_DAY )
            {
            up_days++;
            up_msec %= MSEC_IN_DAY;            
            }
        last_check_time = get_millisec();

        up_hours = up_msec / (1000UL * 60 * 60 );
        up_mins = up_msec / ( 1000UL * 60 ) % 60 ;
        up_secs = up_msec / 1000 % 60;

        sprintf( up_time_str, "%lu дн. %02lu:%02lu:%02lu",
           ( u_long ) up_days, ( u_long ) up_hours,
           ( u_long ) up_mins, ( u_long ) up_secs );
        }
    }
//-----------------------------------------------------------------------------
void PAC_info::reset_params()
    {
    par[ 0 ] [ P_CTR_ERROR_TIME ] = 10000;
    par->save_all();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
