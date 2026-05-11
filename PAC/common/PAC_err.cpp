#include "PAC_err.h"

#include <stdio.h>
#include <vector>
#include <fmt/core.h>
#include <cstring>

#include "log.h"

#ifdef WIN_OS
extern const char* WSA_Last_Err_Decode ();
#endif // WINDOWS_OS

auto_smart_ptr < PAC_critical_errors_manager > PAC_critical_errors_manager::instance;
//-----------------------------------------------------------------------------
PAC_critical_errors_manager::~PAC_critical_errors_manager(
    )
    {
    reset_all_error();
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::show_errors() const
    {
    static u_char show_step = 0;
    static uint32_t start_time = get_millisec();

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
    ALARM_SUBCLASS p1, unsigned int p2 )
    {
    int b = 0;

    //1.try to find
    for ( u_int i = 0; i < errors.size(); i++ )
        {
        if ( errors[ i ].err_class == eclass &&
            static_cast<unsigned int>( p1 ) == errors[ i ].err_sub_class &&
            p2 == errors[ i ].param )
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
void PAC_critical_errors_manager::reset_all_error()
    {
    errors.clear();
    errors_id = 0;
    }
//-----------------------------------------------------------------------------
void PAC_critical_errors_manager::reset_global_error( ALARM_CLASS eclass,
    ALARM_SUBCLASS p1, unsigned int p2, bool is_print_msg /* = true */ )
    {
    int idx = -1;
    for ( u_int i = 0; i < errors.size(); i++ )
        {
        if ( errors[ i ].err_class == eclass &&
            static_cast<unsigned int>( p1 ) == errors[ i ].err_sub_class &&
            p2 == errors[ i ].param )
            {
            idx = i;
            break;
            }
        }

    if ( idx >= 0 )
        {
        errors.erase( errors.begin() + idx );

        if ( is_print_msg )
            {
            sprintf( G_LOG->msg, "%s", get_alarm_descr( eclass, p1, p2, false ) );
            G_LOG->write_log( i_log::P_INFO );
            }

        errors_id++;
        }
    }
//-----------------------------------------------------------------------------
int PAC_critical_errors_manager::save_as_Lua_str( char *str, u_int_2 &id )
    {
    int res = 0;
    str[ 0 ] = 0;
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
    ALARM_SUBCLASS err_sub_class, unsigned int par, bool is_set )
    {
    const auto BUFF_SIZE = 200;
    static char tmp[ BUFF_SIZE ]{};
    std::memset( tmp, 0, BUFF_SIZE );

    // LCOV_EXCL_START
    auto res = fmt::format_to_n( tmp, BUFF_SIZE, "{}-{}-{}",
        static_cast<int>( err_class ), static_cast<int>( err_sub_class ),
        par ).size;
    // LCOV_EXCL_STOP

    switch( err_class )
        {
    case AC_SERVICE:
        switch ( err_sub_class )
            {
            case AS_IO_COUPLER:
                res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                    " : Узел I/O '{}' ('{}', '{}') - ",
                    G_IO_MANAGER()->get_node( par - 1 )->name,
                    G_IO_MANAGER()->get_node( par - 1 )->ip_address,
                    G_CMMCTR->get_host_name_rus() ).size;
                if ( is_set )
                    {
                    fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                        "отключен для обслуживания" );
                    }
                else
                    {
                    fmt::format_to_n( tmp + res, BUFF_SIZE - res, "включен" );
                    }
                break;
            }
        break;

    case AC_PP_MODE:
        switch ( err_sub_class )
            {
            case AS_IO_COUPLER:
                res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                    " : Узел I/O '{}' ('{}', '{}') - ",
                    G_IO_MANAGER()->get_node( par - 1 )->name,
                    G_IO_MANAGER()->get_node( par - 1 )->ip_address,
                    G_CMMCTR->get_host_name_rus() ).size;

                if ( is_set )
                    {
                    fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                        "активен PP mode (каналы управления заблокированы)" );
                    }
                else
                    {
                    fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                        "обычный режим" );
                    }
                break;
            }
        break;

    case AC_NO_CONNECTION:
        if ( is_set )
            {
            res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                " : Нет связи с " ).size;
            }
        else
            {
            res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                " : Есть связь с " ).size;
            }

        switch ( err_sub_class )
            {
            case AS_IO_COUPLER:
                fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                    "узлом I/O '{}' ('{}', '{}').",
                    G_IO_MANAGER()->get_node( par - 1 )->name,
                    G_IO_MANAGER()->get_node( par - 1 )->ip_address,
                    G_CMMCTR->get_host_name_rus() );
                break;

            case AS_MODBUS_DEVICE:
                fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                    "Modbus-device №{}.", par );
                break;

            case AS_EASYSERVER:
                fmt::format_to_n( tmp + res, BUFF_SIZE - res, "EasyServer." );
                break;

            default:
                fmt::format_to_n( tmp + res, BUFF_SIZE - res, "?." );
                break;
            }
        break;

    case AC_NET:
        if ( is_set )
            {
            res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                " : Network communication error : " ).size;
            }
        else
            {
            res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                " : Network communication OK : " ).size;
            }

        switch( par )
            {
            case 0:
                res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                    "master : " ).size;
                break;

            case 1:
                res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                    "modbus : " ).size;
                break;

            default:
                res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                    "? : " ).size;
                break;
            }

        switch( err_sub_class )
            {
            case AS_SOCKET_F:
                res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                    "calling function socket(...)" ).size;
                break;

            case AS_BIND_F:
                res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                    "calling function bind(...)" ).size;
                break;

            case AS_SETSOCKOPT_F:
                res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                    "calling function setsockopt(...)" ).size;
                break;

            case AS_LISTEN_F:
                res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                    "calling function listen(...)" ).size;
                break;

            default:
                res += fmt::format_to_n( tmp + res, BUFF_SIZE - res,
                    "?" ).size;
                break;
            }// switch( err_sub_class )

        if ( is_set )
            {
            fmt::format_to_n( tmp + res, BUFF_SIZE - res, " : {}",
#ifdef LINUX_OS
                strerror( errno )
#endif // LINUX_OS

#ifdef WIN_OS
                WSA_Last_Err_Decode()
#endif // WINDOWS_OS
            );
            }
        else
            {
            fmt::format_to_n( tmp + res, BUFF_SIZE - res, "." );
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
