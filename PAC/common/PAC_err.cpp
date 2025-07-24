#include "PAC_err.h"

#include <stdio.h>
#include <vector>

#include "log.h"

#ifdef WIN_OS
extern const char* WSA_Last_Err_Decode ();
#endif // WINDOWS_OS

auto_smart_ptr < PAC_critical_errors_manager > PAC_critical_errors_manager::instance;
//-----------------------------------------------------------------------------
PAC_critical_errors_manager::PAC_critical_errors_manager(
    ): errors_id( 0 )
    {
#ifndef __BORLANDC__
    errors.clear();
#endif // __BORLANDC__
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::show_errors() const
    {
    static u_char show_step = 0;
    static u_long start_time = get_millisec();

    if ( !errors.empty() )    // Есть ошибки.
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
        sprintf( G_LOG->msg, "%s", get_alarm_descr( eclass, p1, p2, true ) );
        G_LOG->write_log( i_log::P_ERR );

        errors.emplace_back( eclass, p1, p2 );
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

        sprintf( G_LOG->msg, "%s", get_alarm_descr( eclass, p1, p2, false ) );
        G_LOG->write_log( i_log::P_INFO );

        errors_id++;
        }
    }
//-----------------------------------------------------------------------------
int PAC_critical_errors_manager::save_as_Lua_str( char *str, u_int_2 &id )
    {
    int res = 0;

    for ( u_int i = 0; i < errors.size(); i++ )
        {
        res += sprintf( str + res, "\t%s\n", "{" );

        res += sprintf( str + res, "\tdescription = \"%s\",\n",
            get_alarm_descr( ( ALARM_CLASS ) errors[ i ].err_class,
            ( ALARM_SUBCLASS ) errors[ i ].err_sub_class, errors[ i ].param, true ) );

        res += sprintf( str + res, "\t%s\n", "type = AT_SPECIAL," );
        res += sprintf( str + res, "\t%s%s%s\n", "group = '",
            get_alarm_group(), "'," );
        res += sprintf( str + res, "\t%s%d%s\n", "priority = ",
            ALARM_CLASS_PRIORITY, "," );
       res +=  sprintf( str + res, "\t%s\n", "state = AS_ALARM," );

        //Для идентификации ошибок.
        res += sprintf( str + res, "\tid_n = %d,\n", errors[ i ].param );

        res += sprintf( str + res, "\t%s\n", "}," );
        }

   id = errors_id;

#ifdef DEBUG_PAC_ERR
    printf( "%s\n", str );
#endif // DEBUG_PAC_ERR

    return res;
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
const char* PAC_critical_errors_manager::get_alarm_descr( ALARM_CLASS err_class,
    ALARM_SUBCLASS err_sub_class, int par, bool is_set )
    {
    static char tmp[ 200 ] = "";
    sprintf( tmp, "%d-%d-%d : ",
        ( int ) err_class, ( int ) err_sub_class, par );

    switch( err_class )
        {
    case AC_UNKNOWN:
        sprintf( tmp + strlen( tmp ), "?" );
        break;

	case AC_SERVICE:
		switch (err_sub_class)
			{
			case AS_IO_COUPLER:
				sprintf(tmp + strlen(tmp),
					"Узел ввода/вывода '%s' ('%s') - ",
					G_IO_MANAGER()->get_node(par - 1)->name,
					G_IO_MANAGER()->get_node(par - 1)->ip_address
					);
				break;
            default:
                break;
			}
		if (is_set)
			{
			sprintf(tmp + strlen(tmp), "%s", "отключен для обслуживания");
			}
		else
			{
			sprintf(tmp + strlen(tmp), "%s", "включен");
			}
		break;

    case AC_NO_CONNECTION:
        if ( is_set )
            {
            sprintf( tmp + strlen( tmp ), "%s", "Нет связи с " );
            }
        else
            {
            sprintf( tmp + strlen( tmp ), "%s", "Есть связь с " );
            }

        switch( err_sub_class )
            {
        case AS_IO_COUPLER:
            sprintf( tmp + strlen( tmp ),
                "узлом I/O '%s' ('%s', '%s')",
                G_IO_MANAGER()->get_node( par - 1 )->name,
                G_IO_MANAGER()->get_node( par - 1 )->ip_address,
                G_CMMCTR->get_host_name_rus() );
            break;

        case AS_PANEL:
            sprintf( tmp + strlen( tmp ), "panel EasyView №%d.", par );
            break;

        case AS_MODBUS_DEVICE:
            sprintf( tmp + strlen( tmp ), "Modbus-device №%d.", par );
            break;

        case AS_RFID_READER:
            sprintf( tmp + strlen( tmp ), "RFID-reader №%d.", par );
            break;

        case AS_EASYSERVER:
            sprintf( tmp + strlen( tmp ), "EasyServer." );
            break;

        case AS_REMOTE_PAC:
            sprintf( tmp + strlen( tmp ), "remote PAC." );
            break;

        default:
        	break;
            }//switch( err_sub_class )
        break;

    case AC_COM_DRIVER:
        return "?";

    case AC_RUNTIME_ERROR:
        switch( err_sub_class )
            {
        case AS_EMERGENCY_BUTTON:
            break;

        default:
            break;
            }// switch( err_sub_class )
        break;

    case AC_NET:
        if ( is_set )
            {
            sprintf( tmp + strlen( tmp ), "%s", "Network communication error : " );
            }
        else
            {
            sprintf( tmp + strlen( tmp ), "%s", "Network communication OK : " );
            }

        switch( par )
            {
            case 0:
                sprintf( tmp + strlen( tmp ),
                    "master : " );
                break;

            case 1:
                sprintf( tmp + strlen( tmp ),
                    "modbus : " );
                break;

            default:
                break;
            }

        switch( err_sub_class )
            {
            case AS_SOCKET_F:
                sprintf( tmp + strlen( tmp ),
                    "calling function socket(...) : " );
                break;

            case AS_BIND_F:
                sprintf( tmp + strlen( tmp ),
                    "calling function bind(...) : " );
                break;

            case AS_SETSOCKOPT_F:
                sprintf( tmp + strlen( tmp ),
                    "calling function setsockopt(...) : " );
                break;

            case AS_LISTEN_F:
                sprintf( tmp + strlen( tmp ),
                    "calling function listen(...) : " );
                break;

            default:
                sprintf( tmp + strlen( tmp ),
                    "? : " );
                break;
            }// switch( err_sub_class )

        if ( is_set )
            {
#ifdef LINUX_OS
            sprintf( tmp + strlen( tmp ), "%s.", strerror( errno ) );
#endif // LINUX_OS

#ifdef WIN_OS
            sprintf( tmp + strlen( tmp ), "%s", WSA_Last_Err_Decode() );
#endif // WINDOWS_OS
            }
        else
            {
            sprintf( tmp + strlen( tmp ) - 3, "." );
            }
        break;
    default:
        break;
        }

    return tmp;
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
