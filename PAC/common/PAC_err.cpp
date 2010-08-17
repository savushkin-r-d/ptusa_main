#include <stdio.h>

#include "PAC_err.h"
#ifdef UCLINUX
#include "sys_w750.h"
#endif // UCLINUX
#ifdef LINUX
#include "sys_PC.h"
#endif // LINUX

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

auto_smart_ptr < PAC_critical_errors_manager > PAC_critical_errors_manager::instance;
//-----------------------------------------------------------------------------
PAC_critical_errors_manager::PAC_critical_errors_manager(
    ): errors_id( 0 ),
    global_ok( 0 )    
    {
    errors.clear();
    
#ifdef UCLINUX
    wago_led = new led_W750();
#endif // UCLINUX

#ifdef LINUX
    wago_led = new led_PC();
#endif // UCLINUX
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
                wago_led->on( led::L_STATUS, led::C_RED );
                start_time = get_millisec();
                }
            break;

        case 1:
            if ( get_delta_millisec( start_time ) > 500 )
                {
                show_step = 0;
                wago_led->off( led::L_STATUS );
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
                wago_led->on( led::L_STATUS, led::C_GREEN );
                start_time = get_millisec();
                }
            break;

        case 1:
            if ( get_delta_millisec( start_time ) > 500 )
                {
                show_step = 0;
                wago_led->off( led::L_STATUS );                
                start_time = get_millisec();
                }
            break;
            }
        }
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::set_global_error( ERRORS_CLASS eclass,
    ERRORS_SUBCLASS p1, unsigned long p2 )
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
void PAC_critical_errors_manager::reset_global_error( ERRORS_CLASS eclass, 
    ERRORS_SUBCLASS p1, unsigned long p2 )
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
// [ 0  ] - номер ошибок, unsigned, 2 байта. 
// [ 1  ]  
// [ 2  ] - количество ошибок, unsigned char.  
// [ 3  ] - класс ошибки    1, unsigned char.
// [ 4  ] - подклас ошибки  1, unsigned char.
// [ 5  ] - параметр ошибки 1, unsigned char.
// [ 6  ] - класс ошибки    2, unsigned char.
// [ 7  ] - подклас ошибки  2, unsigned char.
// [ 8  ] - параметр ошибки 2, unsigned char.
// [ .. ]
// [ x  ]
int PAC_critical_errors_manager::save_to_stream( char *buff )
    {
    int answer_size = 0;

    *( ( unsigned int* ) buff ) = errors_id;
    buff += 2;
    answer_size += 2;
    unsigned char *erros_cnt = ( unsigned char* ) buff;
    *erros_cnt = 0;
    buff++;
    answer_size++;

    for ( u_int i = 0; i < errors.size(); i++ )
        {
        erros_cnt[ 0 ]++;
        buff[ 0 ] = errors[ i ].err_class;
        buff[ 1 ] = errors[ i ].err_sub_class;
        buff[ 2 ] = errors[ i ].param;
        buff += 3;
        answer_size += 3;
        }

#ifdef DEBUG_PAC_ERR   
    Print( " erros_id = %u\n", errors_id );
    Print( " erros_cnt = %d\n", erros_cnt[ 0 ] );
    Print( " answer_size = %d\n", answer_size );
#endif // DEBUG_PAC_ERR

    return answer_size;
    }
//-----------------------------------------------------------------------------
unsigned char PAC_critical_errors_manager::save_to_stream_2( char *buff )
    {
    unsigned char answer_size = 0;
    for ( u_int i = 0; i < errors.size(); i++ )
        {
        buff[ 0 ] = errors[ i ].err_class;
        buff[ 1 ] = errors[ i ].err_sub_class;
        buff[ 2 ] = errors[ i ].param;
        buff += 3;
        answer_size += 3;
        }

    return answer_size; 
    }
//-----------------------------------------------------------------------------
int PAC_critical_errors_manager::set_instance( PAC_critical_errors_manager *new_instance )
    {
    instance = new_instance;
    return 0;
    }
//-----------------------------------------------------------------------------
PAC_critical_errors_manager * PAC_critical_errors_manager::get_instance()
    {
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
