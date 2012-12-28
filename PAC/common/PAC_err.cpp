#include <stdio.h>

#include "PAC_err.h"

auto_smart_ptr < PAC_critical_errors_manager > PAC_critical_errors_manager::instance;
//-----------------------------------------------------------------------------
PAC_critical_errors_manager::PAC_critical_errors_manager(
    ): errors_id( 0 ),
    global_ok( 0 )
    {
#ifndef __BORLANDC__
    errors.clear();
#endif // __BORLANDC__
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::show_errors()
    {
    static u_char show_step = 0;
    static u_long start_time = get_millisec();

    if ( errors.size() > 0 )    // Есть ошибки.
        {
        switch ( show_step )
            {
        case 0:
            if ( get_delta_millisec( start_time ) > 500 )
                {
                show_step = 1;
                get_led()->on( led::L_STATUS, led::C_RED );
                start_time = get_millisec();
                }
            break;

        case 1:
            if ( get_delta_millisec( start_time ) > 500 )
                {
                show_step = 0;
                get_led()->off( led::L_STATUS );
                start_time = get_millisec();
                }
            break;
            }
        }
    else                        // Нет ошибок.
        {
        switch ( show_step )
            {
        case 0:
            if ( get_delta_millisec( start_time ) > 500 )
                {
                show_step = 1;
                get_led()->on( led::L_STATUS, led::C_GREEN );
                start_time = get_millisec();
                }
            break;

        case 1:
            if ( get_delta_millisec( start_time ) > 500 )
                {
                show_step = 0;
                get_led()->off( led::L_STATUS );
                start_time = get_millisec();
                }
            break;
            }
        }
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::set_global_error( ALARM_CLASS eclass,
    ALARM_SUBCLASS p1, unsigned long p2 )
    {
    int b = 0;

    //1.try to find
    for ( u_int i = 0; i < errors.size(); i++ )
        {
        if ( errors[ i ].err_class == eclass &&
            ( unsigned int ) p1 == errors[ i ].err_sub_class &&
            ( unsigned int ) p2 == errors[ i ].param )
            {
            b = 1;
            break;
            }
        }

    if ( b == 0 )
        {
#ifdef DEBUG
        Print( "Set Error: class: %d, p1: %d, p2: %lu.\n\r", eclass, p1, p2 );
#endif // DEBUG
        errors.push_back( critical_error( eclass, p1, p2 ) );

        errors_id++;
        }
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::reset_global_error( ALARM_CLASS eclass,
    ALARM_SUBCLASS p1, unsigned long p2 )
    {
    int idx = -1;
    for ( u_int i = 0; i < errors.size(); i++ )
        {
        if ( errors[ i ].err_class == eclass &&
            ( unsigned int ) p1 == errors[ i ].err_sub_class &&
            ( unsigned int ) p2 == errors[ i ].param )
            {
            idx = i;
            break;
            }
        }

    if ( idx >= 0 )
        {
        errors.erase( errors.begin() + idx );
#ifdef DEBUG
        Print( "Reset Error: class: %d, p1: %d, p2: %lu \n\r", eclass, p1, p2 );
#endif // DEBUG

        errors_id++;
        }
    }
//-----------------------------------------------------------------------------
int PAC_critical_errors_manager::save_as_Lua_str( char *str, u_int_2 &id )
    {
    for ( u_int i = 0; i < errors.size(); i++ )
        {
        sprintf( str + strlen( str ), "\t%s\n", "{" );

        sprintf( str + strlen( str ), "\t%s%s\n",
           "description = \"", get_alarm_descr(
           ( ALARM_CLASS ) errors[ i ].err_class,
           ( ALARM_SUBCLASS ) errors[ i ].err_sub_class, errors[ i ].param ) );

        sprintf( str + strlen( str ), "\t%s\n", "type = AT_SPECIAL," );
        sprintf( str + strlen( str ), "\t%s%s%s\n", "group = '",
            get_alarm_group(), "'," );
        sprintf( str + strlen( str ), "\t%s%d%s\n", "priority = ",
            ALARM_CLASS_PRIORITY, "," );
        sprintf( str + strlen( str ), "\t%s\n", "state = AS_ALARM," );

        sprintf( str + strlen( str ), "\t%s\n", "}," );
        }

   id = errors_id;

#ifdef DEBUG_PAC_ERR
    Print( "%s\n", str );
#endif // DEBUG_PAC_ERR

    return 0;
    }
//-----------------------------------------------------------------------------
PAC_critical_errors_manager * PAC_critical_errors_manager::get_instance()
    {
    if ( instance.is_null() )
        {
        instance = new PAC_critical_errors_manager();
        }

    return instance;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PAC_critical_errors_manager::critical_error::critical_error( int err_class,
    u_int err_sub_class,
    u_int param ) :err_class( err_class ),
    err_sub_class( err_sub_class ),
    param( param )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
