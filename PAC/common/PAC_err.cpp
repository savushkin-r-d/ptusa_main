#include <stdio.h>

#include "PAC_err.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

PAC_critical_errors_manager* PAC_critical_errors_manager::instance;
//--------------------------------------------------------------------------------
/// Kommandos fuer das Interface Programm <--> Hardware
//--------------------------------------------------------------------------------
#define WAGO_FBK_LED_CMD_SET	0x01	/* turn on briefly to show activity */
#define WAGO_FBK_LED_CMD_ON     0x02	/* turn LED on permanently */
#define WAGO_FBK_LED_CMD_OFF	0x03	/* turn LED off permanently */
#define WAGO_FBK_LED_CMD_FLASH	0x04	/* flash this LED */

//--------------------------------------------------------------------------------
/// Hardware Index Programm <--> Hardware
//--------------------------------------------------------------------------------
#define WAGO_FBK_LED_STATUS_RED       2
#define WAGO_FBK_LED_STATUS_GREEN     3
#define WAGO_FBK_LED_SERVICE_RED      4
#define WAGO_FBK_LED_SERVICE_GREEN    5
#define WAGO_FBK_LED_USER_RED	      6
#define WAGO_FBK_LED_USER_GREEN	      7
#define WAGO_FBK_LED_ALL              8
//-----------------------------------------------------------------------------
int ledman_cmd( int cmd, int led )
    {
	int fd;
	if ( ( fd = open( "/dev/ledman", O_RDWR ) ) != -1 )
        {
		ioctl( fd, cmd, led );
		close( fd );
        return 0;
        }

    return 1;
    }
//-----------------------------------------------------------------------------
PAC_critical_errors_manager::PAC_critical_errors_manager(
    ): errors_id( 0 ),
    global_ok( 0 )
    {
    errors.clear();
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::show_errors()
    {
    static u_char is_error = 0;
    static u_char is_ok = 0;

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
                    ledman_cmd( WAGO_FBK_LED_CMD_ON, WAGO_FBK_LED_STATUS_RED );
                    start_time = get_millisec();
                    }
                break;

            case 1:
                if ( get_delta_millisec( start_time ) > 500 )
                    {
                    show_step = 0;
                    ledman_cmd( WAGO_FBK_LED_CMD_OFF, WAGO_FBK_LED_STATUS_RED );
                    start_time = get_millisec();
                    }
                break;
            }
        if ( 0 == is_error )
            {
            is_error = 1;
            is_ok = 0;

            ledman_cmd( WAGO_FBK_LED_CMD_OFF, WAGO_FBK_LED_ALL );
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
                    ledman_cmd( WAGO_FBK_LED_CMD_ON, WAGO_FBK_LED_STATUS_GREEN );
                    start_time = get_millisec();
                    }
                break;

            case 1:
                if ( get_delta_millisec( start_time ) > 500 )
                    {
                    show_step = 0;
                    ledman_cmd( WAGO_FBK_LED_CMD_OFF, WAGO_FBK_LED_STATUS_GREEN );
                    start_time = get_millisec();
                    }
                break;
            }

        if ( 0 == is_ok )
            {
            is_ok = 1;
            is_error = 0;

            ledman_cmd( WAGO_FBK_LED_CMD_OFF, WAGO_FBK_LED_ALL );
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
            ( unsigned int ) p1 == errors[ i ].p1 && 
            ( unsigned int ) p2 == errors[ i ].p2 )
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
            ( unsigned int ) p1 == errors[ i ].p1 &&
            ( unsigned int ) p2 == errors[ i ].p2 )
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
        buff[ 1 ] = errors[ i ].p1;
        buff[ 2 ] = errors[ i ].p2;
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
        buff[ 1 ] = errors[ i ].p1;
        buff[ 2 ] = errors[ i ].p2;
        buff += 3;
        answer_size += 3;
        }

    return answer_size; 
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
