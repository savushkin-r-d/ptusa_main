#include <string.h>

#include "PAC_info.h"

auto_smart_ptr < PAC_info > PAC_info::instance;///< Ёкземпл€р класса.

const u_int_4 PAC_info::MSEC_IN_DAY = 24UL * 60UL * 60UL * 1000UL;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PAC_info::PAC_info() :
    par( saved_params_u_int_4( P_PARAMS_COUNT ) ),
    up_days( 0 ),
    up_hours( 0 ),
    up_mins( 0 ),
    up_secs( 0 ),
    up_msec( 0 ),
    last_check_time( get_millisec() ),
    reset_type( 1 ) //+ IsResetByWatchDogTimer()
    {
    strcpy( up_time_str, "0 дн. 0:0:0" );
    }
//-----------------------------------------------------------------------------
PAC_info::~PAC_info()
    {   
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
    par[ P_MIX_FLIP_PERIOD ] = 60;
    par[ P_MIX_FLIP_TIME ]   = 1000;
    par.save_all();
    }
//-----------------------------------------------------------------------------
int PAC_info::save_device( char *buff )
    {
    int answer_size = sprintf( buff, "t.SYSTEM = \n\t{\n" );
    
    answer_size += sprintf( buff + answer_size, "\tRESET_BY=%d,\n", reset_type );    
    answer_size += sprintf( buff + answer_size, "\tUP_DAYS=%d,\n", up_days );  
    answer_size += sprintf( buff + answer_size, "\tUP_HOURS=%d,\n", up_hours );   
    answer_size += sprintf( buff + answer_size, "\tUP_MINS=%d,\n", up_mins );
    answer_size += sprintf( buff + answer_size, "\tUP_SECS=%d,\n", up_secs );
    answer_size += sprintf( buff + answer_size, "\tUP_TIME=\"%s\",\n",
        up_time_str );

    answer_size += sprintf( buff + answer_size, "\tWASH_VALVE_SEAT_PERIOD=%d,\n",
        par[ P_MIX_FLIP_PERIOD ] );    
    answer_size += sprintf( buff + answer_size, "\tWASH_VALVE_SEAT_TIME=%d,\n",
        par[ P_MIX_FLIP_TIME ] );
    
    answer_size += sprintf( buff + answer_size, "\t}\n" );
    
    return answer_size;
    }
//-----------------------------------------------------------------------------
PAC_info* PAC_info::get_instance()
    {
    if ( instance.is_null() )
        {
        instance = new PAC_info();
        }

    return instance;
    }
//-----------------------------------------------------------------------------
void PAC_info::print() const
    {
    Print( "PAC_info\n" );
    }
//-----------------------------------------------------------------------------
int PAC_info::set_cmd( const char *prop, u_int idx, double val )
    {
    if ( strcmp( prop, "WASH_VALVE_SEAT_PERIOD" ) == 0 )
        {        
        par.save( P_MIX_FLIP_PERIOD, ( u_int_4 ) val );

        return 0;
        }

    if ( strcmp( prop, "WASH_VALVE_SEAT_TIME" ) == 0 )
        {
        par.save( P_MIX_FLIP_TIME, ( u_int_4 ) val );

        return 0;
        }


    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PAC_info* G_PAC_INFO()
    {
    return PAC_info::get_instance();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
