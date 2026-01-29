#include <string.h>
#include <fmt/core.h>
#include <inttypes.h>

#include "PAC_info.h"
#include "PAC_err.h"

#include "lua_manager.h"

#include "bus_coupler_io.h"

#ifdef OPCUA
#include "OPCUAServer.h"
#endif

extern bool G_NO_IO_NODES;

auto_smart_ptr < PAC_info > PAC_info::instance;///< Экземпляр класса.

const u_int_4 PAC_info::MSEC_IN_DAY = 24UL * 60UL * 60UL * 1000UL;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PAC_info::PAC_info() :
    par( saved_params_u_int_4( P_PARAMS_COUNT - 1 ) ),
    up_days( 0 ),
    up_hours( 0 ),
    up_mins( 0 ),
    up_secs( 0 ),
    up_msec( 0 ),
    last_check_time( get_millisec() ),
    reset_type( 1 ), //+ IsResetByWatchDogTimer()
    cmd( 0 ),
    restrictions_set_to_off_time( 0 )
    {
    strcpy( up_time_str, "0 дн. 0:0:0" );
    cmd_answer[ 0 ] = 0;
    }
//-----------------------------------------------------------------------------
void PAC_info::eval()
    {
    if ( restrictions_set_to_off_time &&
        get_delta_millisec( restrictions_set_to_off_time ) >
        par[ P_RESTRICTIONS_MANUAL_TIME ] )
        {
        par[ P_RESTRICTIONS_MODE ] = 0;
        restrictions_set_to_off_time = 0;
        }

    if ( get_delta_millisec( last_check_time ) > 1000 )
        {
        up_msec += get_delta_millisec( last_check_time );
        if ( up_msec >= MSEC_IN_DAY )
            {
            up_days++;
            up_msec %= MSEC_IN_DAY;
            }
        last_check_time = get_millisec();

        up_hours = up_msec / ( 1000UL * 60 * 60 );
        up_mins = up_msec / ( 1000UL * 60 ) % 60;
        up_secs = up_msec / 1000 % 60;

        auto res = fmt::format_to_n( up_time_str, C_MAX_STR_LENGTH - 1,
            "{} дн. {:02}:{:02}:{:02}", up_days, up_hours, up_mins, up_secs );
        *res.out = '\0';
        }
    }
//-----------------------------------------------------------------------------
void PAC_info::reset_params()
    {
    par[ P_MIX_FLIP_PERIOD ] = 180;
    par[ P_MIX_FLIP_UPPER_TIME ] = 2000;
    par[ P_MIX_FLIP_LOWER_TIME ] = 1000;
    par[ P_V_OFF_DELAY_TIME ] = 1000;
    par[ P_V_BOTTOM_OFF_DELAY_TIME ] = 1200;

    par[ P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME ] = 50;
    par[ P_MAIN_CYCLE_WARN_ANSWER_AVG_TIME ] = 300;

    par[ P_RESTRICTIONS_MODE ] = 0;
    par[ P_RESTRICTIONS_MANUAL_TIME ] = 2 * 60 * 1000; // 2 min

    par[ P_AUTO_PAUSE_OPER_ON_DEV_ERR ] = 0;

    par[ P_AUTO_OPERATION_WAIT_TIME ] = 60000;
    par[ P_AUTO_OPERATION_WARN_TIME ] = 20000;

    par[ P_IS_OPC_UA_SERVER_ACTIVE ] = 0;
    par[ P_IS_OPC_UA_SERVER_CONTROL ] = 0;

    par.save_all();
    }
//-----------------------------------------------------------------------------
int PAC_info::save_device( char* buff )
    {
    int size = fmt::format_to_n( buff, MAX_COPY_SIZE, "t.SYSTEM = \n\t{{\n" ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tRESET_BY={},\n", reset_type ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tUP_DAYS={},\n", up_days ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tUP_HOURS={},\n", up_hours ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tUP_MINS={},\n", up_mins ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tUP_SECS={},\n", up_secs ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tUP_TIME=\"{}\",\n", up_time_str ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tCYCLE_TIME={},\n", cycle_time ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tWASH_VALVE_SEAT_PERIOD={},\n", par[ P_MIX_FLIP_PERIOD ] ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tWASH_VALVE_UPPER_SEAT_TIME={},\n", par[ P_MIX_FLIP_UPPER_TIME ] ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tWASH_VALVE_LOWER_SEAT_TIME={},\n",par[ P_MIX_FLIP_LOWER_TIME ] ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_V_OFF_DELAY_TIME={},\n", par[ P_V_OFF_DELAY_TIME ] ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_V_BOTTOM_ON_DELAY_TIME={},\n", par[ P_V_BOTTOM_OFF_DELAY_TIME ] ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME={},\n", par[ P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME ] ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_MAIN_CYCLE_WARN_ANSWER_AVG_TIME={},\n", par[ P_MAIN_CYCLE_WARN_ANSWER_AVG_TIME ] ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_RESTRICTIONS_MODE={},\n", par[ P_RESTRICTIONS_MODE ] ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_RESTRICTIONS_MANUAL_TIME={},\n", par[ P_RESTRICTIONS_MANUAL_TIME ] ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_AUTO_PAUSE_OPER_ON_DEV_ERR={},\n", par[ P_AUTO_PAUSE_OPER_ON_DEV_ERR ] ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tCMD={},\n", cmd ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tCMD_ANSWER=\"{}\",\n", cmd_answer ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tVERSION=\"{}\",\n", PRODUCT_VERSION_FULL_STR ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tNODEENABLED = \n\t{{\n\t" ).size;
    unsigned int nc = io_manager::get_instance()->get_nodes_count();
    for ( unsigned int i = 0; i < nc; i++ )
        {
        io_manager::io_node* wn = io_manager::get_instance()->get_node( i );
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
            wn->is_active ? "1, " : "0, " ).size;
        }
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t}},\n" ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tNODEST = \n\t{{\n\t" ).size;
    for ( unsigned int i = 0; i < nc; i++ )
        {
        const io_manager::io_node* wn = io_manager::get_instance()->get_node( i );
        size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
            "{}, ", wn->get_display_state() ).size;
        }
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\n\t}},\n" ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_IS_OPC_UA_SERVER_ACTIVE={},\n", par[ P_IS_OPC_UA_SERVER_ACTIVE ] ).size;
    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "\tP_IS_OPC_UA_SERVER_CONTROL={},\n", par[ P_IS_OPC_UA_SERVER_CONTROL ] ).size;

    size += fmt::format_to_n( buff + size, MAX_COPY_SIZE, "\t}}\n" ).size;

    buff[ size ] = '\0';

    return size;
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
    printf( "PAC_info\n" );
    }
//-----------------------------------------------------------------------------
int PAC_info::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( strcmp( prop, "CMD" ) == 0 )
        {
        switch ((COMMANDS)(int)val)
            {
            case CLEAR_RESULT_CMD:
                cmd = 0;
                break;

            case RELOAD_RESTRICTIONS:
                {
                if (G_DEBUG)
                    {
                    G_LOG->notice("Reload restrictions (remote monitor client command).");
                    }
                const int SCRIPT_N = 7;
                cmd = G_LUA_MANAGER->reload_script( SCRIPT_N, "restrictions",
                    cmd_answer, sizeof( cmd_answer ) );
                return cmd;
                }

            case RESET_PARAMS:
                if ( G_DEBUG )
                    {
                    G_LOG->notice( "Resetting params (remote monitor client command)." );
                    }
                params_manager::get_instance()->reset_params_size();
                params_manager::get_instance()->final_init();
                break;
            }

        return 0;
        }

    if ( strcmp( prop, "WASH_VALVE_SEAT_PERIOD" ) == 0 )
        {
        par.save( P_MIX_FLIP_PERIOD, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "WASH_VALVE_UPPER_SEAT_TIME" ) == 0 )
        {
        par.save( P_MIX_FLIP_UPPER_TIME, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "WASH_VALVE_LOWER_SEAT_TIME" ) == 0 )
        {
        par.save( P_MIX_FLIP_LOWER_TIME, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "P_V_OFF_DELAY_TIME" ) == 0 )
        {
        par.save( P_V_OFF_DELAY_TIME, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "P_V_BOTTOM_ON_DELAY_TIME" ) == 0 )
        {
        par.save( P_V_BOTTOM_OFF_DELAY_TIME, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME" ) == 0 )
        {
        par.save( P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME, (u_int_4)val );
        return 0;
        }
    if ( strcmp( prop, "P_MAIN_CYCLE_WARN_ANSWER_AVG_TIME" ) == 0 )
        {
        par.save( P_MAIN_CYCLE_WARN_ANSWER_AVG_TIME, (u_int_4)val );
        return 0;
        }


    if ( strcmp( prop, "P_RESTRICTIONS_MODE" ) == 0 )
        {
        par.save( P_RESTRICTIONS_MODE, (u_int_4)val );

        if ( val == 2 ) //Полуавтоматический режим.
            {
            restrictions_set_to_off_time = get_millisec();
            }
        else
            {
            restrictions_set_to_off_time = 0;
            }

        return 0;
        }
    if ( strcmp( prop, "P_RESTRICTIONS_MANUAL_TIME" ) == 0 )
        {
        par.save( P_RESTRICTIONS_MANUAL_TIME, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "P_AUTO_PAUSE_OPER_ON_DEV_ERR" ) == 0 )
        {
        par.save( P_AUTO_PAUSE_OPER_ON_DEV_ERR, (u_int_4)val );
        return 0;
        }

    if ( strcmp( prop, "NODEENABLED" ) == 0 )
        {
        if ( idx <= io_manager::get_instance()->get_nodes_count() )
            {
            io_manager::io_node* wn = io_manager::get_instance()->get_node( idx - 1 );
            if ( 1 == val )
                {
                if ( !wn->is_active )
                    {
                    wn->is_active = 1;
                    }
                PAC_critical_errors_manager::get_instance()->reset_global_error(
                    PAC_critical_errors_manager::AC_SERVICE,
                    PAC_critical_errors_manager::AS_IO_COUPLER, wn->number );
                }
            if ( 0 == val )
                {
                if ( wn->is_active )
                    {
                    io_manager::get_instance()->disconnect( wn );
                    wn->is_active = 0;
                    }
                PAC_critical_errors_manager::get_instance()->set_global_error(
                    PAC_critical_errors_manager::AC_SERVICE,
                    PAC_critical_errors_manager::AS_IO_COUPLER, wn->number );
                PAC_critical_errors_manager::get_instance()->reset_global_error(
                    PAC_critical_errors_manager::AC_NO_CONNECTION,
                    PAC_critical_errors_manager::AS_IO_COUPLER, wn->number);
                wn->is_set_err = false;
                }
            if ( 100 == val ) //Сброс ошибки.
                {
                PAC_critical_errors_manager::get_instance()->reset_global_error(
                    PAC_critical_errors_manager::AC_SERVICE,
                    PAC_critical_errors_manager::AS_IO_COUPLER, wn->number );
                PAC_critical_errors_manager::get_instance()->reset_global_error(
                    PAC_critical_errors_manager::AC_NO_CONNECTION,
                    PAC_critical_errors_manager::AS_IO_COUPLER, wn->number);
                wn->is_set_err = false;
                }
            }
        }

#ifdef OPCUA
    if ( strcmp( prop, "P_IS_OPC_UA_SERVER_ACTIVE" ) == 0 )
        {
        cmd_answer[ 0 ] = 0;

        auto prev_val = par[ P_IS_OPC_UA_SERVER_ACTIVE ];
        if ( val == 0 && prev_val == 1 )
            {
            par.save( P_IS_OPC_UA_SERVER_ACTIVE, 0 );

            G_OPCUA_SERVER.shutdown();
            }
        else if ( val == 1 && prev_val == 0 )
            {
            par.save( P_IS_OPC_UA_SERVER_ACTIVE, 1 );
            auto retval = G_OPCUA_SERVER.init_all_and_start();
            if ( retval != UA_STATUSCODE_GOOD )
                {
                G_LOG->error( "OPC UA server start failed. Returned error code 0x%X!",
                    retval );

                auto r = fmt::format_to_n( cmd_answer, sizeof( cmd_answer ) - 1,
                    "{}", G_LOG->msg );
                *r.out = '\0';

                G_OPCUA_SERVER.shutdown();
                return 1;
                }
            }
        }

    if ( strcmp( prop, "P_IS_OPC_UA_SERVER_CONTROL" ) == 0 )
        {
        par.save( P_IS_OPC_UA_SERVER_CONTROL, (u_int_4)val );
        return 0;
        }
    
#endif

    return 0;
    }

bool PAC_info::is_emulator()
    {
#ifdef PTUSA_TEST
    return emulator_state;
#else
#ifdef DEBUG_NO_IO_MODULES
    return true;
#else
    if ( G_NO_IO_NODES )
        return true;
    else
        return false;
#endif // DEBUG_NO_IO_MODULES
#endif // PTUSA_TEST
    }

#ifdef PTUSA_TEST
void PAC_info::emulation_on()
    {
    emulator_state = true;
    }

void PAC_info::emulation_off()
    {
    emulator_state = false;
    }
#endif
//-----------------------------------------------------------------------------
void PAC_info::set_cycle_time( uint32_t current_cycle_time )
    {
    cycle_time = current_cycle_time;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PAC_info* G_PAC_INFO()
    {
    return PAC_info::get_instance();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
