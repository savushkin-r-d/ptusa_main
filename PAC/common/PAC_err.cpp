#include <stdio.h>

#include "PAC_err.h"


//-----------------------------------------------------------------------------
int PAC_critical_errors_manager::get_no( char ch ) 
    {
    if ( ch >= '0' && ch <= '9' ) return ch - '0';
    if ( ch >= 'A' && ch <= 'F' ) return ch - 'A' + 10;
    return 17;
    }
//-----------------------------------------------------------------------------
PAC_critical_errors_manager::PAC_critical_errors_manager(
    ): erros_id( 0 ),
    global_ok( 0 )
    {
    //Init5DigitLed();
    //LedOff();
    for ( int i = 0; i < GE_ERRORS_MAX_COUNT; i++ )
        {
        errors[ i ].eclass = -1;
        }
    error_show_params.tm = get_millisec();
    error_show_params.cur_err = -1;
    error_show_params.cur_pos = -1;
    error_show_params.speed = 500;
    error_show_params.MSG[ GE_ERRORS_STRING_LENGTH - 1 ] = 0;    
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::show_errors()
    {
    static unsigned long st_time = get_millisec();
    static char intense = 0;
    static char intense_sign = 1;

    if ( get_delta_millisec( st_time ) > 100 )
        {
        //Set5DigitLedIntensity( intense );
        intense += intense_sign;
        st_time = get_millisec();
        if ( 0 == intense || 15 == intense )
            {
            intense_sign *= -1;
            }
        }   

    int i, idx;
    if ( get_delta_millisec( error_show_params.tm ) >= error_show_params.speed )
        {
        if ( error_show_params.cur_err >= 0 ) 
            {
            error_show_params.tm = get_millisec();
            error_show_params.cur_pos++;
            sprintf( error_show_params.MSG, "     E%u %u %u", 
                ( unsigned int )errors[ error_show_params.cur_err ].eclass,
                errors[ error_show_params.cur_err ].p1, 
                errors[ error_show_params.cur_err ].p2 );
            if ( error_show_params.MSG[ error_show_params.cur_pos ] == 0 )
                {
                error_show_params.cur_pos = -1;
                //find next error;
                idx = 0;
                for ( i = error_show_params.cur_err + 1; i < GE_ERRORS_MAX_COUNT; i++ )
                    {
                    if ( errors[ i ].eclass >= 0 )
                        {
                        error_show_params.cur_err = i;
                        idx = 1;
                        break;
                        }
                    }
                if ( idx == 0 )
                    {
                    for ( i = 0; i < error_show_params.cur_err; i++ )
                        {
                        if ( errors[ i ].eclass >= 0 ) 
                            {
                            error_show_params.cur_err = i;
                            break;
                            }
                        }
                    }
                }
            for ( i = 1; i < 6; i++ )
                {
                //Show5DigitLed( i, get_no( error_show_params.MSG[
                //    error_show_params.cur_pos + i ] ) );
                }
            } 
        else 
            {
            for ( i = 0; i < GE_ERRORS_MAX_COUNT; i++ )
                {
                if ( errors[ i ].eclass >= 0 )
                    {
                    error_show_params.cur_err = i;
                    break;
                    }
                }
            if ( error_show_params.cur_err < 0 && error_show_params.cur_pos >= 0 )
                {
                error_show_params.cur_pos = -1;
                //Init5DigitLed();
                }
            if ( error_show_params.cur_err < 0 )
                {
                if ( !global_ok ) 
                    {
                    //LedOn();
                    //Init5DigitLed();
                    //Show5DigitLedSeg( 1, 103 );
                    //Show5DigitLed( 2, 10 );
                    //Show5DigitLedSeg( 3, 223 );
                    //Show5DigitLedSeg( 4, 0 );
                    //Show5DigitLedSeg( 5, 0 );
                    //if (IsResetByWatchDogTimer() )
                    //    {
                    //    Show5DigitLed( 5,17 );
                    //    }
                    global_ok = 1;
                    }
                } 
            else 
                {
                if ( global_ok )
                    {
                    //LedOff();
                    global_ok = 0;
                    }
                }                
            }
        }
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::set_global_error( ERRORS_CLASS eclass,
                                                   ERRORS_SUBCLASS p1, unsigned long p2 )
    {
    int i, b, idx;
    b = 0;
    idx = -1;
    //1.try to find
    for ( i = 0; i < GE_ERRORS_MAX_COUNT; i++ ) 
        {
        if ( errors[ i ].eclass == -1 && idx == -1 )
            idx = i;

        if ( errors[ i ].eclass == eclass && 
            ( unsigned int ) p1 == errors[ i ].p1 && 
            ( unsigned int ) p2 == errors[ i ].p2 )
            {
            b = 1;
            break;
            }
        }
    if ( b == 0 && idx != -1 )
        {
#ifdef DEBUG
        Print( "Set Error: class: %d, p1: %d, p2: %lu.\n\r", eclass, p1, p2 );
#endif // DEBUG
        errors[ idx ].eclass = eclass;
        errors[ idx ].p1 = p1;
        errors[ idx ].p2 = p2;

        erros_id++;
        }
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::reset_global_error( ERRORS_CLASS eclass, 
                                                     ERRORS_SUBCLASS p1, unsigned long p2 )
    {
    int idx = -1;
    for ( int i = 0; i < GE_ERRORS_MAX_COUNT; i++ )
        {
        if ( errors[ i ].eclass == eclass && 
            ( unsigned int ) p1 == errors[ i ].p1 &&
            ( unsigned int ) p2 == errors[ i ].p2 )
            {
            idx = i;
            break;
            }
        }
    if ( idx >= 0 )
        {
        errors[ idx ].eclass = -1;
#ifdef DEBUG
        Print( "Reset Error: class: %d, p1: %d, p2: %lu \n\r", eclass, p1, p2 );
#endif // DEBUG
        if ( error_show_params.cur_err == idx ) error_show_params.cur_err = -1;

        erros_id++;
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

    *( ( unsigned int* ) buff ) = erros_id;
    buff += 2;
    answer_size += 2;
    unsigned char *erros_cnt = ( unsigned char* ) buff;
    *erros_cnt = 0;
    buff++;
    answer_size++;

    for ( int i = 0; i < GE_ERRORS_MAX_COUNT; i++ ) 
        {
        if ( errors[ i ].eclass > -1 )
            {
            erros_cnt[ 0 ]++;
            buff[ 0 ] = errors[ i ].eclass;
            buff[ 1 ] = errors[ i ].p1;
            buff[ 2 ] = errors[ i ].p2;
            buff += 3;
            answer_size += 3;
            }            
        }

#ifdef DEBUG_PAC_ERR   
    Print( " erros_id = %u\n", erros_id );
    Print( " erros_cnt = %d\n", erros_cnt[ 0 ] );
    Print( " answer_size = %d\n", answer_size );
#endif // DEBUG_PAC_ERR

    return answer_size;
    }
//-----------------------------------------------------------------------------
unsigned char PAC_critical_errors_manager::save_to_stream_2( char *buff )
    {
    unsigned char answer_size = 0;
    for ( int i = 0; i < GE_ERRORS_MAX_COUNT; i++ ) 
        {
        if ( errors[ i ].eclass > -1 )
            {           
            buff[ 0 ] = errors[ i ].eclass;
            buff[ 1 ] = errors[ i ].p1;
            buff[ 2 ] = errors[ i ].p2;
            buff += 3;
            answer_size += 3;
            }            
        }

    return answer_size; 
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PAC_critical_errors_manager *g_pac_critical_errors = 0;
//-----------------------------------------------------------------------------
void InitGlobalErrors ()
    {  
    g_pac_critical_errors = new PAC_critical_errors_manager;
    }
//-----------------------------------------------------------------------------
void ShowErrors() 
    {
    if ( g_pac_critical_errors )
        {
        g_pac_critical_errors->show_errors();
        }    
    }
//-----------------------------------------------------------------------------
void SetGlobalError( ERRORS_CLASS eclass, ERRORS_SUBCLASS p1,
                    unsigned long p2 )
    {
    if ( g_pac_critical_errors )
        {
        g_pac_critical_errors->set_global_error( eclass, p1, p2 );
        }
    }
//-----------------------------------------------------------------------------
void ResetGlobalError( ERRORS_CLASS eclass, ERRORS_SUBCLASS p1,
                      unsigned long p2 )
    {
    if ( g_pac_critical_errors )
        {
        g_pac_critical_errors->reset_global_error( eclass, p1, p2 );
        }    
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


