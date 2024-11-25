#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <fmt/core.h>
#include <algorithm>

#include "device.h"
#include "manager.h"

#include "tech_def.h"

#include "g_errors.h"
#include "lua_manager.h"
#include "log.h"
#include "PID.h"

#ifdef WIN_OS
#pragma warning(push)
#pragma warning(disable: 26812) //Prefer 'enum class' over 'enum'.
#endif // WIN_OS

circuit_breaker::F_data_out circuit_breaker::stub_out_info{};

power_unit::process_data_out power_unit::stub_p_data_out{};
unsigned int power_unit::WAIT_DATA_TIME = 300;
unsigned int power_unit::WAIT_CMD_TIME = 1000;

analog_valve_iolink::out_data analog_valve_iolink::stub_out_info{};
signal_column_iolink::out_data signal_column_iolink::stub_out_info{};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int DO1::get_state()
    {
    if ( G_PAC_INFO()->is_emulator() ) return digital_io_device::get_state();

    return get_DO( DO_INDEX );
    }
//-----------------------------------------------------------------------------
void DO1::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return digital_io_device::direct_on();

    set_DO( DO_INDEX, 1 );
    }
//-----------------------------------------------------------------------------
void DO1::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return digital_io_device::direct_off();

    set_DO( DO_INDEX, 0 );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
signal_column_discrete::signal_column_discrete( const char* dev_name,
    int red_lamp_channel, int yellow_lamp_channel,
    int green_lamp_channel, int blue_lamp_channel,
    int siren_channel ) :
    signal_column( dev_name, DST_HLA,
        red_lamp_channel, yellow_lamp_channel, green_lamp_channel,
        blue_lamp_channel, siren_channel )
    {
    }
//-----------------------------------------------------------------------------
void signal_column_discrete::process_DO( u_int n, DO_state state, const char* name )
    {
    set_DO( n - 1, static_cast<char>( state ) );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
signal_column_iolink::signal_column_iolink( const char* dev_name ) :
    signal_column( dev_name, DST_HLA_IOLINK )
    {
    }
//-----------------------------------------------------------------------------
void signal_column_iolink::set_string_property( const char* field, const char* value )
    {
    if ( G_DEBUG )
        {
        printf( "Set string property %s value %s\n", field, value );
        }

    if ( strcmp( field, "SIGNALS_SEQUENCE" ) == 0 )
        {
        auto pos = strchr( value, 'R' );
        if ( pos )
            {
            red_lamp_channel = pos - value + 1;
            }
        pos = strchr( value, 'Y' );
        if ( pos )
            {
            yellow_lamp_channel = pos - value + 1;
            }
        pos = strchr( value, 'G' );
        if ( pos )
            {
            green_lamp_channel = pos - value + 1;
            }
        pos = strchr( value, 'B' );
        if ( pos )
            {
            blue_lamp_channel = pos - value + 1;
            }
        pos = strchr( value, 'A' );
        if ( pos )
            {
            siren_channel = pos - value + 1;
            }
        }
    }
//-----------------------------------------------------------------------------
void signal_column_iolink::process_DO( u_int n, DO_state state, const char* name )
    {
    bool ch_state = state == DO_state::ON;
    switch ( n )
        {
        case 0:
            G_LOG->warning( "%s\t signal_column_iolink::process_DO() - "
                "no \'%s\'.", get_name(), name );
            break;

        case 1:
            out_info->switch_ch1 = ch_state;
            break;
        case 2:
            out_info->switch_ch2 = ch_state;
            break;
        case 3:
            out_info->switch_ch3 = ch_state;
            break;
        case 4:
            out_info->switch_ch4 = ch_state;
            break;
        case 5:
            out_info->switch_ch5 = ch_state;
            break;

        default:
            G_LOG->warning( "%s\t signal_column_iolink::process_DO() - "
                "unknown \'%s\'.", get_name(), name );
            break;
        }
    }
//-----------------------------------------------------------------------------
void signal_column_iolink::evaluate_io()
    {
    signal_column::evaluate_io();

    if ( G_PAC_INFO()->is_emulator() )
        {
        // Ничего не делаем.
        }
    else
        {
        out_info = (out_data*)get_AO_write_data( 0 );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
camera::camera( const char* dev_name, DEVICE_SUB_TYPE sub_type,
    int params_count, bool is_ready ) :
    device( dev_name, DT_CAM, sub_type, params_count ),
    io_device( dev_name ),
    is_cam_ready( is_ready )
    {
    }

void camera::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return device::direct_set_state( new_state );
        }

    if ( new_state ) direct_on(); else direct_off();
    }

void camera::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::direct_off();

    set_DO( static_cast<u_int>( CONSTANTS::INDEX_DO ), 0 );
    }

void camera::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::direct_on();

    set_DO( static_cast<u_int>( CONSTANTS::INDEX_DO ), 1 );
    }

int camera::get_result( int n )
    {
    if ( !G_PAC_INFO()->is_emulator() )
        {
        result = get_DI( static_cast<u_int>( CONSTANTS::INDEX_DI_RES_1 ) );
        }

    return result;
    }

int camera::save_device_ex( char* buff )
    {
    auto res = ( fmt::format_to_n( buff, MAX_COPY_SIZE, "RESULT=%d, READY=%d, ",
        get_result(), is_cam_ready ) ).size;
    return res;
    }

int camera::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( strcmp( prop, "RESULT" ) == 0 )
        {
        result = static_cast<int>( val );
        }
    else if ( strcmp( prop, "READY" ) == 0 )
        {
        is_cam_ready = val > 0;
        }
    else device::set_cmd( prop, idx, val );

    return 0;
    }

void camera::set_string_property( const char* field, const char* value )
    {
    if ( G_DEBUG )
        {
        printf( "Set string property %s value %s\n", field, value );
        }

    if ( strcmp( field, "IP" ) == 0 )
        {
        ip = std::string( value );
        }
    }

bool camera::is_ready() const
    {
    return is_cam_ready;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
camera_DI2::camera_DI2( const char* dev_name, DEVICE_SUB_TYPE sub_type ) :
    camera( dev_name, sub_type, static_cast<int>( PARAMS::PARAMS_CNT ) - 1, false )
    {
    set_par_name( static_cast<u_int>( PARAMS::P_READY_TIME ), 0, "P_READY_TIME" );
    }

void camera_DI2::evaluate_io()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        {
        int o = get_DO( static_cast<u_int>( CONSTANTS::INDEX_DO ) );
        int i = get_DI( static_cast<u_int>( CONSTANTS::INDEX_DI_READY ) );

        if ( 1 == i )
            {
            start_switch_time = get_millisec();
            camera::direct_set_state( o );
            }
        else if ( get_delta_millisec( start_switch_time ) <
            get_par( static_cast<u_int>( PARAMS::P_READY_TIME ), 0 ) )
            {
            camera::direct_set_state( o );
            }
        else
            {
            camera::direct_set_state( -1 );
            }

        is_cam_ready = i > 0;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
camera_DI3::camera_DI3( const char* dev_name ) :
    camera_DI2( dev_name, DEVICE_SUB_TYPE::DST_CAM_DO1_DI3 )
    {
    }

void camera_DI3::evaluate_io()
    {
    camera_DI2::evaluate_io();

    if ( !G_PAC_INFO()->is_emulator() )
        {
        result_2 = get_DI( static_cast<u_int>( CONSTANTS::INDEX_DI_RES_2 ) );
        }
    }

int camera_DI3::get_result( int n )
    {
    switch ( n )
        {
        case 1:
            return camera::get_result();

        case 2:
            return result_2;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void i_counter::restart()
    {
    reset();
    start();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool i_DI_device::is_active()
    {
    return get_state() == 0 ? false : true;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void i_DO_device::on()
    {
    if ( !get_manual_mode() )
        {
        direct_on();
        }
    }
//-----------------------------------------------------------------------------
void i_DO_device::instant_off()
    {
    if ( !get_manual_mode() )
        {
        direct_off();
        }
    }
//-----------------------------------------------------------------------------
void i_DO_device::set_state( int new_state )
    {
    if ( !get_manual_mode() )
        {
        direct_set_state( new_state );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief Установка текущего состояния устройства с учетом ручного режима.
void i_AO_device::set_value( float new_value )
    {
    if ( !get_manual_mode() )
        {
        direct_set_value( new_value );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
virtual_valve::virtual_valve( const char* dev_name ) :
    valve( dev_name, DT_V, DST_V_VIRT )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
dev_stub::dev_stub() : valve( "STUB", DT_NONE, DST_NONE ),
    camera( "STUB", DST_NONE ),
    signal_column( "STUB", DST_NONE )
    {
    }
//-----------------------------------------------------------------------------
float dev_stub::get_value()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_set_value( float new_value )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::off()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::on()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::set_value( float new_value )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::set_state( int new_state )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE dev_stub::get_valve_state()
    {
    return VALVE_STATE::V_OFF;
    }
//-----------------------------------------------------------------------------
int dev_stub::get_state()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_on()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_off()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_set_state( int new_state )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
u_int_4 dev_stub::get_serial_n() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::print() const
    {
    printf( "virtual device" );
    }
//-----------------------------------------------------------------------------
void dev_stub::pause()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::start()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::reset()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
u_int dev_stub::get_quantity()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
float dev_stub::get_flow()
    {
    return 0.;
    }
//-----------------------------------------------------------------------------
u_long dev_stub::get_pump_dt() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
float dev_stub::get_min_flow() const
    {
    return .0f;
    }
//-----------------------------------------------------------------------------
void dev_stub::abs_reset()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
u_int dev_stub::get_abs_quantity()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::tare()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::pause_daily( DAY_CTR n )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::start_daily( DAY_CTR n )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void dev_stub::process_DO( u_int n, DO_state state, const char* name )
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
threshold_regulator::threshold_regulator( const char* name ) :device( name,
    device::DEVICE_TYPE::DT_REGULATOR,
    device::DEVICE_SUB_TYPE::DST_REGULATOR_THLD,
    static_cast<int>( PARAM::PARAMS_COUNT ) - 1 )
    {
    set_par_name( static_cast<int>( PARAM::P_delta ), 0, "P_delta" );
    set_par_name( static_cast<int>( PARAM::P_is_reverse ), 0, "P_is_reverse" );
    }
//-----------------------------------------------------------------------------
int threshold_regulator::get_state()
    {
    return static_cast<int>( state );
    };
//-----------------------------------------------------------------------------
void threshold_regulator::direct_off()
    {
    if ( state != STATE::OFF )
        {
        state = STATE::OFF;
        if ( actuator )
            {
            actuator->off();
            }
        }
    };
//-----------------------------------------------------------------------------
void threshold_regulator::direct_set_state( int new_state )
    {
    switch ( static_cast<STATE>( new_state ) )
        {
        case STATE::OFF:
            direct_off();
            break;

        case STATE::ON:
            direct_on();
            break;
        }
    };
//-----------------------------------------------------------------------------
void threshold_regulator::direct_on()
    {
    if ( state != STATE::ON )
        {
        state = STATE::ON;
        }
    };
//-----------------------------------------------------------------------------
const char* threshold_regulator::get_name_in_Lua() const
    {
    return get_name();
    };
//-----------------------------------------------------------------------------
float threshold_regulator::get_value()
    {
    return static_cast<float>( out_state );
    };
//-----------------------------------------------------------------------------
void threshold_regulator::direct_set_value( float val )
    {
    set_value = val;
    if ( sensor && actuator )
        {
        auto in_value = sensor->get_value();
        if ( sensor->get_type() == DT_FQT )
            {
            in_value = dynamic_cast <i_counter*> ( sensor )->get_flow();
            }

        auto idx = static_cast<int>( PARAM::P_is_reverse );
        auto is_reverse = get_par( idx ) > 0;
        if ( STATE::OFF == state )
            {
            out_state = 0;
            actuator->off();
            }
        else
            {
            auto delta = get_par( static_cast<int>( PARAM::P_delta ) );
            if ( in_value > set_value + delta )
                {
                out_state = is_reverse ? 1 : 0;
                }
            else if ( in_value < set_value - delta )
                {
                out_state = is_reverse ? 0 : 1;
                }
            }

        actuator->set_state( out_state );
        }
    };
//-----------------------------------------------------------------------------
void threshold_regulator::set_string_property( const char* field, const char* value )
    {
    if ( !field ) return;

    device::set_string_property( field, value );
    switch ( field[ 0 ] )
        {
        //IN_VALUE
        case 'I':
            sensor = G_DEVICE_MANAGER()->get_device( value );
            break;

        //OUT_VALUE
        case 'O':
            actuator = G_DEVICE_MANAGER()->get_device( value );
            break;

        default:
            break;
        }
    }
//-----------------------------------------------------------------------------
int threshold_regulator::save_device( char* buff )
    {
    return device::save_device( buff, "\t" );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
power_unit::power_unit( const char* dev_name,
    device::DEVICE_SUB_TYPE sub_type  ) :
    analog_io_device( dev_name, device::DT_G, sub_type, 0 )
    {
    memset( &p_data_in, 0, sizeof( p_data_in ) );    

    static_assert( sizeof( process_data_in ) == 18,
        "Struct `process_data_in` must be the 18 bytes size." );
    static_assert( sizeof( process_data_out ) == 7,
        "Struct `process_data_out` must be the 7 bytes size." );
    }
//-----------------------------------------------------------------------------
float power_unit::get_value()
    {
    return v;
    }
//-----------------------------------------------------------------------------
int power_unit::get_state()
    {
    return st;
    }
//-----------------------------------------------------------------------------
void power_unit::direct_set_value( float val )
    {
    // Здесь нет возможности управлять состоянием - управляем через отдельные 
    // каналы.
    }
//-----------------------------------------------------------------------------
void power_unit::evaluate_io()
    {
    auto data = reinterpret_cast<char*>( get_AI_data( C_AIAO_INDEX ) );

    if ( !data ) return; // Return, if data is nullptr (in debug mode).

    std::copy( data, data + sizeof( p_data_in ),
        reinterpret_cast<char*>( &p_data_in ) );
    v = .1f *
        static_cast<float>( ( ( p_data_in.sum_currents_2 << 8 ) +
        p_data_in.sum_currents ) );
    err = p_data_in.DC_not_OK;
    st = p_data_in.status_ch1 || p_data_in.status_ch2 ||
        p_data_in.status_ch3 || p_data_in.status_ch4 ||
        p_data_in.status_ch5 || p_data_in.status_ch6 ||
        p_data_in.status_ch7 || p_data_in.status_ch8;

#ifdef DEBUG_IOLINK_POWER_UNIT
    auto res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
        "{:b} {:b} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x}",
        data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ],
        data[ 4 ], data[ 5 ], data[ 6 ], data[ 7 ],
        data[ 8 ], data[ 9 ], data[ 10 ], data[ 11 ],
        data[ 12 ], data[ 13 ], data[ 14 ], data[ 15 ],
        data[ 16 ], data[ 17 ] );
    *res.out = 0;
    G_LOG->write_log( i_log::P_WARNING );

    res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
        "voltage = {}, out_voltage_2 = {}, DC_not_OK = {}",
        p_data_in.out_voltage, +p_data_in.out_voltage_2, +p_data_in.DC_not_OK );
    *res.out = 0;
    G_LOG->write_log( i_log::P_WARNING );
    res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
        "out voltage {}",
        .1f * ( ( p_data_in.out_voltage_2 << 8 ) + p_data_in.out_voltage ) );
    *res.out = 0;
    G_LOG->write_log( i_log::P_INFO );

    res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
        "nominal_current_ch1 = {}, load_current_ch1 = {}, st_ch1 = {}",
        +p_data_in.nominal_current_ch1,
        +p_data_in.load_current_ch1, +p_data_in.status_ch1 );
    *res.out = 0;
    G_LOG->write_log( i_log::P_WARNING );
    res = fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
        "nominal_current_ch4 = {}, load_current_ch4 = {}, st_ch4 = {}\n",
        +p_data_in.nominal_current_ch4,
        +p_data_in.load_current_ch4, +p_data_in.status_ch4 );
    *res.out = 0;
    G_LOG->write_log( i_log::P_WARNING );


    char buff[ 500 ] = { 0 };
    save_device( buff, "" );
    fmt::println( "{}", buff );
#endif

    p_data_out = reinterpret_cast<process_data_out*>(
        get_AO_write_data( C_AIAO_INDEX ) );


    if ( get_AI_IOLINK_state( C_AIAO_INDEX ) == io_device::IOLINKSTATE::OK )
        {
        if ( is_processing_cmd )
            {
            // Устанавливаем управляющий бит через некоторое время после
            // записи выходных данных.
            if ( get_delta_millisec( cmd_time ) > WAIT_DATA_TIME )
                {
                p_data_out->valid_flag = true;
                }
            if ( get_delta_millisec( cmd_time ) > WAIT_CMD_TIME )
                {
                is_processing_cmd = false;
                sync_pdout();
                }
            }
        else
            {
            sync_pdout();
            }
        }
    }
//-----------------------------------------------------------------------------
void power_unit::sync_pdout()
    {
    p_data_out->valid_flag = false;
    p_data_out->switch_ch1 = p_data_in.status_ch1;
    p_data_out->switch_ch2 = p_data_in.status_ch2;
    p_data_out->switch_ch3 = p_data_in.status_ch3;
    p_data_out->switch_ch4 = p_data_in.status_ch4;
    p_data_out->switch_ch5 = p_data_in.status_ch5;
    p_data_out->switch_ch6 = p_data_in.status_ch6;
    p_data_out->switch_ch7 = p_data_in.status_ch7;
    p_data_out->switch_ch8 = p_data_in.status_ch8;
    p_data_out->nominal_current_ch1 = p_data_in.nominal_current_ch1;
    p_data_out->nominal_current_ch2 = p_data_in.nominal_current_ch2;
    p_data_out->nominal_current_ch3 = p_data_in.nominal_current_ch3;
    p_data_out->nominal_current_ch4 = p_data_in.nominal_current_ch4;
    p_data_out->nominal_current_ch5 = p_data_in.nominal_current_ch5;
    p_data_out->nominal_current_ch6 = p_data_in.nominal_current_ch6;
    p_data_out->nominal_current_ch7 = p_data_in.nominal_current_ch7;
    p_data_out->nominal_current_ch8 = p_data_in.nominal_current_ch8;
    }
//-----------------------------------------------------------------------------
int power_unit::save_device_ex( char* buff )
    {
    auto res = fmt::format_to_n( buff, MAX_COPY_SIZE,
        "NOMINAL_CURRENT_CH={{{},{},{},{},{},{},{},{}}}, ",
        +p_data_in.nominal_current_ch1, +p_data_in.nominal_current_ch2,
        +p_data_in.nominal_current_ch3, +p_data_in.nominal_current_ch4,
        +p_data_in.nominal_current_ch5, +p_data_in.nominal_current_ch6,
        +p_data_in.nominal_current_ch7, +p_data_in.nominal_current_ch8 );
    auto size = static_cast<int>( res.size );

    res = fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "LOAD_CURRENT_CH={{{:.1f},{:.1f},{:.1f},{:.1f},{:.1f},{:.1f},{:.1f},{:.1f}}}, ",
        .1f * p_data_in.load_current_ch1, .1f * p_data_in.load_current_ch2,
        .1f * p_data_in.load_current_ch3, .1f * p_data_in.load_current_ch4,
        .1f * p_data_in.load_current_ch5, .1f * p_data_in.load_current_ch6,
        .1f * p_data_in.load_current_ch7, .1f * p_data_in.load_current_ch8 );
    size += res.size;

    res = fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "ST_CH={{{},{},{},{},{},{},{},{}}}, ",
        +p_data_in.status_ch1, +p_data_in.status_ch2,
        +p_data_in.status_ch3, +p_data_in.status_ch4,
        +p_data_in.status_ch5, +p_data_in.status_ch6,
        +p_data_in.status_ch7, +p_data_in.status_ch8 );
    size += res.size;    

    res = fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "SUM_CURRENTS={:.1f}, ", v );
    size += res.size;
    res = fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "VOLTAGE={:.1f}, ",
        .1f * static_cast<float>( ( ( p_data_in.out_voltage_2 << 8 ) +
        p_data_in.out_voltage ) ) );
    size += res.size;
    res = fmt::format_to_n( buff + size, MAX_COPY_SIZE,
        "OUT_POWER_90={}, ", +p_data_in.out_power_90 );
    size += res.size;    
    res = fmt::format_to_n( buff + size, MAX_COPY_SIZE, "ERR={}, ", err );
    size += res.size;

    *res.out = 0;
    return size;
    }
//-----------------------------------------------------------------------------
void power_unit::direct_on()
    {
    p_data_out->switch_ch1 = true;
    p_data_out->switch_ch2 = true;
    p_data_out->switch_ch3 = true;
    p_data_out->switch_ch4 = true;
    p_data_out->switch_ch5 = true;
    p_data_out->switch_ch6 = true;
    p_data_out->switch_ch7 = true;
    p_data_out->switch_ch8 = true;

    if ( G_PAC_INFO()->is_emulator() )
        {
        p_data_in.status_ch1 = true;
        p_data_in.status_ch2 = true;
        p_data_in.status_ch3 = true;
        p_data_in.status_ch4 = true;
        p_data_in.status_ch5 = true;
        p_data_in.status_ch6 = true;
        p_data_in.status_ch7 = true;
        p_data_in.status_ch8 = true;
        st = 1;
        }

    is_processing_cmd = true;
    cmd_time = get_millisec();
    }
//-----------------------------------------------------------------------------
void power_unit::direct_off()
    {
    p_data_out->switch_ch1 = false;
    p_data_out->switch_ch2 = false;
    p_data_out->switch_ch3 = false;
    p_data_out->switch_ch4 = false;
    p_data_out->switch_ch5 = false;
    p_data_out->switch_ch6 = false;
    p_data_out->switch_ch7 = false;
    p_data_out->switch_ch8 = false;

    if ( G_PAC_INFO()->is_emulator() )
        {
        p_data_in.status_ch1 = false;
        p_data_in.status_ch2 = false;
        p_data_in.status_ch3 = false;
        p_data_in.status_ch4 = false;
        p_data_in.status_ch5 = false;
        p_data_in.status_ch6 = false;
        p_data_in.status_ch7 = false;
        p_data_in.status_ch8 = false;
        st = 0;
        }

    is_processing_cmd = true;
    cmd_time = get_millisec();
    }
//-----------------------------------------------------------------------------
int power_unit::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_DEBUG )
        {
        fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
            "{}\t power_unit::set_cmd() - prop = {}, idx = {}, val = {}",
            get_name(), prop, idx, val );
        G_LOG->write_log( i_log::P_DEBUG );
        }

    if ( strcmp( prop, "ST" ) == 0 )
        {
        auto new_val = static_cast<int>( val );
        if ( new_val )
            {
            on();
            }
        else
            {
            off();
            }

        return 0;
        }
    auto new_st = static_cast<bool>( val );
    auto status = static_cast<int8_t>( val );    

    if ( strcmp( prop, "ST_CH" ) == 0 )
        {
        switch ( idx )
            {
            case 1:
                p_data_out->switch_ch1 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch1 = status;
                break;
            case 2:
                p_data_out->switch_ch2 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch2 = status;
                break;
            case 3:
                p_data_out->switch_ch3 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch3 = status;
                break;
            case 4:
                p_data_out->switch_ch4 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch4 = status;
                break;
            case 5:
                p_data_out->switch_ch5 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch5 = status;
                break;
            case 6:
                p_data_out->switch_ch6 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch6 = status;
                break;
            case 7:
                p_data_out->switch_ch7 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch7 = status;
                break;
            case 8:
                p_data_out->switch_ch8 = new_st;
                if ( G_PAC_INFO()->is_emulator() ) p_data_in.status_ch8 = status;
                break;

            default:
                // Do nothing.
                break;
            }
        is_processing_cmd = true;
        cmd_time = get_millisec();
        if ( G_PAC_INFO()->is_emulator() )
            st = p_data_in.status_ch1 || p_data_in.status_ch2 ||
                p_data_in.status_ch3 || p_data_in.status_ch4 ||
                p_data_in.status_ch5 || p_data_in.status_ch6 ||
                p_data_in.status_ch7 || p_data_in.status_ch8;

        return 0;
        }

    auto nom_curr = static_cast<int8_t>( val );
    if ( strcmp( prop, "NOMINAL_CURRENT_CH" ) == 0 )
        {
        switch ( idx )
            {
            case 1:
                p_data_out->nominal_current_ch1 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch1 = nom_curr;
                    }
                break;
            case 2:
                p_data_out->nominal_current_ch2 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch2 = nom_curr;
                    }
                break;
            case 3:
                p_data_out->nominal_current_ch3 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch3 = nom_curr;
                    }
                break;
            case 4:
                p_data_out->nominal_current_ch4 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch4 = nom_curr;
                    }
                break;
            case 5:
                p_data_out->nominal_current_ch5 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch5 = nom_curr;
                    }
                break;
            case 6:
                p_data_out->nominal_current_ch6 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch6 = nom_curr;
                    }
                break;
            case 7:
                p_data_out->nominal_current_ch7 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch7 = nom_curr;
                    }
                break;
            case 8:
                p_data_out->nominal_current_ch8 = nom_curr;
                if ( G_PAC_INFO()->is_emulator() )
                    {
                    p_data_in.nominal_current_ch8 = nom_curr;
                    }
                break;

            default:
                // Do nothing.
                break;
            }
        is_processing_cmd = true;
        cmd_time = get_millisec();
        return 0;
        }

    return analog_io_device::set_cmd( prop, idx, val );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
base_counter::base_counter( const char* dev_name, DEVICE_SUB_TYPE sub_type,
    int extra_par_cnt ) :
    device( dev_name, DT_FQT, sub_type, extra_par_cnt ),
    io_device( dev_name )
    {
    device::direct_set_state( static_cast<int>( STATES::S_WORK ) );
    }
//-----------------------------------------------------------------------------
int base_counter::get_state()
    {
    bool is_pump_working = false;
    if ( !motors.empty() )
        {
        for ( auto m : motors )
            {
            if ( m->get_state() == 1 )
                {
                is_pump_working = true;
                }
            }
        }

    auto min_flow = get_min_flow();

    // Насос не работает (при его наличии) или расход ниже минимального.
    if ( ( !motors.empty() && !is_pump_working ) || get_flow() <= min_flow )
        {
        start_pump_working_time = 0;
        }
    // Насос работает (при его наличии) или расход выше минимального.
    else
        {
        if ( device::get_state() == static_cast<int>( STATES::S_PAUSE ) ||
            0 == start_pump_working_time )
            {
            start_pump_working_time = get_millisec();
            counter_prev_value = get_abs_quantity();
            return device::get_state();
            }

        // Работа. 
        // Проверяем счетчик на ошибку - он должен изменить свои показания.
        if ( get_abs_quantity() != counter_prev_value )
            {
            start_pump_working_time = get_millisec();
            counter_prev_value = get_abs_quantity();
            device::direct_set_state( static_cast<int>( STATES::S_WORK ) );
            }
        else
            {
            auto dt = get_pump_dt();
            if ( get_delta_millisec( start_pump_working_time ) < dt )
                {
                return device::get_state();
                }

            device::direct_set_state( static_cast<int>( STATES::S_ERROR ) );
            }
        }

    return device::get_state();
    };
//-----------------------------------------------------------------------------
void base_counter::direct_on()
    {
    start();
    }
//-----------------------------------------------------------------------------
void base_counter::direct_off()
    {
    // Ничего не делаем.
    }
//-----------------------------------------------------------------------------
void base_counter::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        device::direct_set_state( new_state );
        return;
        }
    
    switch ( new_state )
        {
        case 0:
            direct_off();
            break;

        case static_cast<int>( STATES::S_WORK ):
            direct_on();
            break;

        case static_cast<int>( STATES::S_PAUSE ):
            pause();
            break;
        }
    }
//-----------------------------------------------------------------------------
void base_counter::direct_set_value( float new_value )
    {
    value = new_value;
    }
//-----------------------------------------------------------------------------
void base_counter::set_abs_value( float new_value )
    {
    abs_value = new_value;
    };
//-----------------------------------------------------------------------------
float base_counter::calculate_quantity( float& val, float& last_read_val,
    bool& is_first ) const
    {
    float current = get_raw_value();
    float delta = 0;

    if ( is_first )
        {
        if ( current != 0 )
            {
            last_read_val = current;
            is_first = false;
            }
        }
    else
        {
        
        if ( current < last_read_val )
            {
            delta = get_max_raw_value() - last_read_val + current;
            if ( delta > MAX_OVERFLOW && current < delta )
                {
                delta = current;
                }
            }
        else
            {
            delta = current - last_read_val;
            }

        last_read_val = current;
        if ( delta > 0 )
            {
            val += delta;
            }
        }

    return delta;
    }
//-----------------------------------------------------------------------------
float base_counter::get_value()
    {
    return value;
    }
//-----------------------------------------------------------------------------
u_int base_counter::get_quantity()
    {
    return static_cast<u_int>( value );
    }
//-----------------------------------------------------------------------------
u_int base_counter::get_abs_quantity()
    {
    return static_cast<u_int>( abs_value );
    }
//-----------------------------------------------------------------------------
void base_counter::set_property( const char* field, device* dev )
    {
    if ( field && field[ 0 ] == 'M' ) //Связанные насосы.
        {
        motors.push_back( dev );
        }
    }
//-----------------------------------------------------------------------------
void base_counter::pause()
    {
    get_quantity(); // Пересчитываем значение счетчика.

    device::direct_set_state( static_cast<int>( STATES::S_PAUSE ) );
    }
//-----------------------------------------------------------------------------
void base_counter::start()
    {
    if ( static_cast<int>( STATES::S_PAUSE ) == device::get_state() )
        {
        device::direct_set_state( static_cast<int>( STATES::S_WORK ) );
        last_read_value = get_raw_value();
        start_pump_working_time = 0;
        }
    else if ( device::get_state() < 0 ) // Есть какая-либо ошибка.
        {
        start_pump_working_time = 0;
        device::direct_set_state( static_cast<int>( STATES::S_WORK ) );
        }
    }
//-----------------------------------------------------------------------------
void base_counter::reset()
    {
    value = .0f;
    }
//-----------------------------------------------------------------------------
void base_counter::abs_reset()
    {
    abs_value = .0f;
    }
//-----------------------------------------------------------------------------
void base_counter::restart()
    {
    reset();
    start();
    }
//-----------------------------------------------------------------------------
void base_counter::evaluate_io()
    {
    if ( STATES::S_WORK == static_cast<STATES>( get_state() ) )
        {
        calculate_quantity( value, last_read_value, is_first_read );
        }
    
    auto delta = calculate_quantity( abs_value, abs_last_read_value,
        abs_is_first_read );

    if ( STATES::S_WORK == day_t1_state )
        {
        day_t1_value += delta;
        }
    if ( STATES::S_WORK == day_t2_state )
        {
        day_t2_value += delta;
        }

    if ( c_day != get_time().tm_yday ) // Наступил новый день.
        {
        prev_day_t1_value = day_t1_value;
        day_t1_value = 0;
        prev_day_t2_value = day_t2_value;
        day_t2_value = 0;

        c_day = get_time().tm_yday;
        }
    }
//-----------------------------------------------------------------------------
void base_counter::print() const
    {
    device::print();
    }
//-----------------------------------------------------------------------------
int base_counter::set_cmd( const char* prop, u_int idx, double val )
    {
    auto f_value = static_cast<float>( val );

    switch ( prop[ 0 ] )
        {
        case 'A': //ABS_V
            set_abs_value( f_value );
            break;

        case 'D': //DAY_T1, DAY_T2
            if ( prop[ 5 ] == '1' ) day_t1_value = f_value;
            else day_t2_value = f_value;
            break;

        default:
            if ( strcmp( prop, "PREV_DAY_T1" ) == 0 ) 
                prev_day_t1_value = f_value;
            else if ( strcmp( prop, "PREV_DAY_T2" ) == 0 )
                prev_day_t2_value = f_value;
            else return device::set_cmd( prop, idx, val );
        }

    return 0;
    };
//-----------------------------------------------------------------------------
int base_counter::save_device_ex( char* buff )
    {
    return fmt::format_to_n( buff, MAX_COPY_SIZE,
        "ABS_V={}, DAY_T1={}, PREV_DAY_T1={}, DAY_T2={}, PREV_DAY_T2={}, ",
        get_abs_quantity(), day_t1_value, prev_day_t1_value,
        day_t2_value, prev_day_t2_value ).size;
    }
//-----------------------------------------------------------------------------
const char* base_counter::get_error_description()
    {
    if ( device::get_state() < 0 )
        {
        switch ( device::get_state() )
            {
            case static_cast<int>( STATES::S_ERROR ):
                prev_error_state = STATES::S_ERROR;
                return "счет импульсов";

            case static_cast<int>( STATES::S_LOW_ERR ):
                prev_error_state = STATES::S_LOW_ERR;
                return "канал потока (нижний предел)";

            case static_cast<int>( STATES::S_HI_ERR ):
                prev_error_state = STATES::S_HI_ERR;
                return "канал потока (верхний предел)";

            default:
                return device::get_error_description();
            }
        }

    switch ( prev_error_state )
        {
        case STATES::S_ERROR:
            return "счет импульсов (rtn)";

        case STATES::S_LOW_ERR:
            return "канал потока (нижний предел, rtn)";

        case STATES::S_HI_ERR:
            return "канал потока (верхний предел, rtn)";

        default:
            // Ничего не делаем. Вернем в конце функции строку, что всё хорошо.
            break;
        }

    return "нет ошибок";
    }
//-----------------------------------------------------------------------------
void base_counter::pause_daily( DAY_CTR n )
    {
    if ( n == DAY_CTR::DAY_T1 )
        day_t1_state = STATES::S_PAUSE;
    else if ( n == DAY_CTR::DAY_T2 )
        day_t2_state = STATES::S_PAUSE;
    }
//-----------------------------------------------------------------------------
void base_counter::start_daily( DAY_CTR n )
    {
    if ( n == DAY_CTR::DAY_T1 )
        day_t1_state = STATES::S_WORK;
    else if ( n == DAY_CTR::DAY_T2 )
        day_t2_state = STATES::S_WORK;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
counter::counter( const char *dev_name, DEVICE_SUB_TYPE sub_type,
                     int extra_par_cnt ):
    base_counter( dev_name, sub_type, extra_par_cnt )
    {
    }
//-----------------------------------------------------------------------------
float counter::get_raw_value() const
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return 0;
        }

    return static_cast<float>( *( (u_int_2*)get_AI_data( AI_Q_INDEX ) ) );
    }
//-----------------------------------------------------------------------------
float counter::get_max_raw_value() const
    {
    return USHRT_MAX;
    }
//-----------------------------------------------------------------------------
float counter::get_flow()
    {
    return .0f;
    }
//-----------------------------------------------------------------------------
u_long counter::get_pump_dt() const
    {
    return 0;
    };
//-----------------------------------------------------------------------------
float counter::get_min_flow() const
    {
    return .0f;
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
counter_f::counter_f( const char *dev_name ) :
    counter( dev_name, DST_FQT_F, LAST_PARAM_IDX - 1 )
    {
    set_par_name( P_MIN_FLOW, 0, "P_MIN_FLOW" );
    set_par_name( P_MAX_FLOW, 0, "P_MAX_FLOW" );
    set_par_name( P_CZ, 0, "P_CZ" );
    set_par_name( P_DT, 0, "P_DT" );
    set_par_name( P_ERR_MIN_FLOW, 0, "P_ERR_MIN_FLOW" );    
    }
//-----------------------------------------------------------------------------
int counter_f::get_state()
    {
    if ( get_flow() == -1. )
        {
        return (int) STATES::S_LOW_ERR;
        }
    if ( get_flow() == -2. )
        {
        return (int) STATES::S_HI_ERR;
        }

    return base_counter::get_state();
    }
//-----------------------------------------------------------------------------
float counter_f::get_flow()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return get_par( P_CZ, 0 ) + flow_value;
        }
    else
        {
        return get_par( P_CZ, 0 ) +
            get_AI( AI_FLOW_INDEX, get_par( P_MIN_FLOW, 0 ),
            get_par( P_MAX_FLOW, 0 ) );
        }
    }
//-----------------------------------------------------------------------------
int counter_f::save_device_ex( char *buff )
    {
    int res = counter::save_device_ex( buff );
    res += fmt::format_to_n( buff + res, MAX_COPY_SIZE, "F={:.2f}, ",
        get_flow() ).size;

    return res;
    }
//-----------------------------------------------------------------------------
u_long counter_f::get_pump_dt() const
    {
    return static_cast<u_long>( get_par( P_DT, 0 ) );
    }
//-----------------------------------------------------------------------------
float counter_f::get_min_flow() const
    {
    return get_par( P_ERR_MIN_FLOW, 0 );
    }
//-----------------------------------------------------------------------------
int counter_f::set_cmd( const char* prop, u_int idx, double val )
    {
    switch ( prop[ 0 ] )
        {
        case 'F':
            flow_value = static_cast<float>( val );
            break;

        default:
            return counter::set_cmd( prop, idx, val );
        }

    return 0;
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
counter_iolink::counter_iolink( const char* dev_name ) :base_counter( dev_name,
    device::DST_FQT_IOLINK,
    static_cast<int>( CONSTANTS::LAST_PARAM_IDX ) - 1 )
    {
    set_par_name( static_cast<u_int>( CONSTANTS::P_CZ ), 0, "P_CZ" );
    set_par_name( static_cast<u_int>( CONSTANTS::P_DT ), 0, "P_DT" );
    set_par_name( static_cast<u_int>( CONSTANTS::P_ERR_MIN_FLOW ), 0,
        "P_ERR_MIN_FLOW" );    
    };
//-----------------------------------------------------------------------------
void counter_iolink::evaluate_io()
    {
    auto data = (char*)get_AI_data( 0 );
    if ( data )
        {
        auto buff = (char*)&in_info;

        const int SIZE = 8;
        std::copy( data, data + SIZE, buff );

        //Reverse byte order to get correct float.
        std::swap( buff[ 3 ], buff[ 0 ] );
        std::swap( buff[ 1 ], buff[ 2 ] );
        //Reverse byte order to get correct int16.
        std::swap( buff[ 4 ], buff[ 5 ] );
        //Reverse byte order to get correct int16.
        std::swap( buff[ 6 ], buff[ 7 ] );

#ifdef DEBUG_FQT_IOLINK
        sprintf( G_LOG->msg,
            "Totalizer %.2f, flow %d, temperature %d, status2 %d, status1 %d",
            in_info.totalizer, in_info.flow, in_info.temperature,
            in_info.out2, in_info.out1 );
        G_LOG->write_log( i_log::P_NOTICE );
        sprintf( G_LOG->msg,
            "get_quantity() %d, get_flow() %f, get_temperature() %f",
            get_quantity(), get_flow(), get_temperature() );
        G_LOG->write_log( i_log::P_NOTICE );
#endif
        }

    base_counter::evaluate_io();
    };
//-----------------------------------------------------------------------------
float counter_iolink::get_temperature() const
    {
    return 0.1f * in_info.temperature;
    }
//-----------------------------------------------------------------------------
int counter_iolink::get_state()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        {
        IOLINKSTATE res = get_AI_IOLINK_state( 
            static_cast<u_int>( CONSTANTS::AI_INDEX ) );
        if ( res != io_device::IOLINKSTATE::OK )
            {
            return -static_cast<int>( res );
            }
        }

    return base_counter::get_state();
    }
//-----------------------------------------------------------------------------
u_long counter_iolink::get_pump_dt() const
    {
    return static_cast<u_long>(
        get_par( static_cast<u_int>( CONSTANTS::P_DT ), 0 ) );
    }
//-----------------------------------------------------------------------------
float counter_iolink::get_min_flow() const
    {
    return get_par( static_cast<u_int>( CONSTANTS::P_ERR_MIN_FLOW ), 0 );
    }
//-----------------------------------------------------------------------------
float counter_iolink::get_raw_value() const
    {
    return in_info.totalizer;
    };
//-----------------------------------------------------------------------------
float counter_iolink::get_max_raw_value() const
    {
    return 499999999.99f; ///< Максимальное значение счетчика.
    }
//-----------------------------------------------------------------------------
float counter_iolink::get_flow()
    {
    return get_par( static_cast<u_int>( CONSTANTS::P_CZ ), 0 )
        + in_info.flow * 0.01f;
    }
//-----------------------------------------------------------------------------
int counter_iolink::save_device_ex( char* buff )
    {
    int res = base_counter::save_device_ex( buff );
    res += fmt::format_to_n( buff + res, MAX_COPY_SIZE, "F={:.2f}, T={:.1f}, ",
        get_flow(), get_temperature() ).size;

    return res;
    }
//-----------------------------------------------------------------------------
int counter_iolink::set_cmd( const char* prop, u_int idx, double val )
    {
    switch ( prop[ 0 ] )
        {
        case 'F':
            in_info.flow = static_cast<int16_t>( val * 100 ) ;
            break;

        case 'T':
            in_info.temperature = static_cast<int16_t>( val * 10 );
            break;

        default:
            return base_counter::set_cmd( prop, idx, val );
        }

    return 0;
    };
//-----------------------------------------------------------------------------
u_int counter_iolink::get_quantity()
    {
    return static_cast<u_int>( get_value() );
    }
//-----------------------------------------------------------------------------
u_int counter_iolink::get_abs_quantity()
    {
    return static_cast<u_int>( mL_in_L * get_abs_value() );
    }
//-----------------------------------------------------------------------------
float counter_iolink::get_value()
    {
    return base_counter::get_value() * mL_in_L;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DI1::DI1( const char* dev_name, device::DEVICE_TYPE type,
    device::DEVICE_SUB_TYPE sub_type, u_int par_cnt, 
    int current_state_init_val ) :
    digital_io_device( dev_name, type, sub_type,
    ADDITIONAL_PARAMS_COUNT + par_cnt ),
    current_state( current_state_init_val )
    {
    set_par_name( P_DT, 0, "P_DT" );
    }
//-----------------------------------------------------------------------------
void DI1::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) digital_io_device::direct_on();
    }
//-----------------------------------------------------------------------------
void DI1::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) digital_io_device::direct_off();
    }
//-----------------------------------------------------------------------------
int DI1::get_state()
    {
    if ( G_PAC_INFO()->is_emulator() ) return digital_io_device::get_state();

    auto dt = ( u_int_4 ) get_par( P_DT, 0 );

    if ( dt > 0 )
        {
        if ( current_state != get_DI( DI_INDEX ) )
            {
            if ( get_delta_millisec( time ) > dt )
                {
                current_state = get_DI( DI_INDEX );
                time = get_millisec();
                }
            }
        else
            {
            time = get_millisec();
            }
        }
    else current_state = get_DI( DI_INDEX );

    return current_state;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
temperature_e::temperature_e( const char* dev_name ) : AI1( dev_name, DT_TE, DST_TE,
    ADDITIONAL_PARAM_COUNT )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( P_ERR_T, start_param_idx, "P_ERR_T" );
    param_emulator( 20, 2 );    //Average room temperature.
    }
//-----------------------------------------------------------------------------
float temperature_e::get_value()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        float v = AI1::get_value();
        return -1000 == v ? get_par( P_ERR_T, start_param_idx ) : v;
        }
    else
        {
        float v = get_AI( C_AI_INDEX, 0, 0 );
        return -1000 == v ? get_par( P_ERR_T, start_param_idx ) :
            get_par( P_ZERO_ADJUST_COEFF, 0 ) + v;
        }
    }
//-----------------------------------------------------------------------------
int temperature_e::get_state()
    {
    if ( G_PAC_INFO()->is_emulator() ) return AI1::get_state();

    float v = get_AI( C_AI_INDEX, 0, 0 );
    if ( -1000 == v )
        {
        return -1;
        }

    return 1;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
temperature_e_analog::temperature_e_analog( const char* dev_name ) :
    AI1( dev_name, DT_TE, DST_TE_ANALOG, LAST_PARAM_IDX - 1 )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( P_ERR_T, start_param_idx, "P_ERR_T" );
    set_par_name( P_MIN_V, start_param_idx, "P_MIN_V" );
    set_par_name( P_MAX_V, start_param_idx, "P_MAX_V" );
    param_emulator( 20, 2 );    //Average room temperature.
    }
//-----------------------------------------------------------------------------
float temperature_e_analog::get_value()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return analog_io_device::get_value();
        }

    float v = get_AI( C_AI_INDEX, 0, 0 );
    if ( v < 0 )
        {
        return get_par( P_ERR_T, start_param_idx );
        }
    else
        {
        auto min = get_par( P_MIN_V, start_param_idx );
        auto max = get_par( P_MAX_V, start_param_idx );
        v = get_AI( C_AI_INDEX, min, max );
        return get_par( P_ZERO_ADJUST_COEFF ) + v;
        }
    }
//-----------------------------------------------------------------------------
int temperature_e_analog::get_state()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return AI1::get_state();
        }

    float v = get_AI( C_AI_INDEX, 0, 0 );
    if ( v < 0 )
        {
        return -1;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
temperature_e_iolink::temperature_e_iolink( const char *dev_name ):
    AI1(dev_name, DT_TE, DST_TE_IOLINK, ADDITIONAL_PARAM_COUNT)
    {
    start_param_idx = AI1::get_params_count();
    set_par_name(P_ERR_T, start_param_idx, "P_ERR_T");
    }
//-----------------------------------------------------------------------------
temperature_e_iolink::~temperature_e_iolink()
    {
    delete info;
    info = nullptr;
    }
//-----------------------------------------------------------------------------
float temperature_e_iolink::get_value()
    {
    if ( G_PAC_INFO()->is_emulator() ) return AI1::get_value();

    if (get_AI_IOLINK_state(C_AI_INDEX) != io_device::IOLINKSTATE::OK)
        {
        return get_par(P_ERR_T, start_param_idx);
        }
    else
        {
        auto data = (char*)get_AI_data(C_AI_INDEX);

        int16_t tmp = data[1] + 256 * data[0];
        memcpy(info, &tmp, 2);

        return get_par(P_ZERO_ADJUST_COEFF, 0) + 0.1f * info->v;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void virtual_wages::direct_off()
    {
    state = 0;
    value = 0;
    }

void virtual_wages::direct_set_value( float new_value )
    {
    value = new_value;
    }

float virtual_wages::get_value()
    {
    return value;
    }

void virtual_wages::direct_set_state( int new_state )
    {
    state = new_state;
    }

void virtual_wages::direct_on()
    {
    state = 1;
    }

int virtual_wages::get_state()
    {
    return state;
    }

void virtual_wages::tare()
    {
    // Ничего не делаем.
    }

virtual_wages::virtual_wages( const char* dev_name ) :
    device( dev_name, device::DT_WT, device::DST_WT_VIRT, 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
wages_RS232::wages_RS232( const char* dev_name ) :
    analog_io_device( dev_name, device::DT_WT, device::DST_WT_RS232,
    static_cast<int>( CONSTANTS::LAST_PARAM_IDX ) - 1 )
    {
    set_par_name( static_cast<int>( CONSTANTS::P_CZ ), 0, "P_CZ" );
    }

float wages_RS232::get_value_from_wages()
    {
    //Получение массива данных (1). Если данные пустые, вернуть старое
    //значение (2). Если 1 - буфер не пустой, переключиться в режим считывания
    //данных, вернуть старое значение (3). После переключения в режим
    //считывания данных получаем данные и обрабатываем (4)
    //Если данные корректные, то в 4м байте символ "+" (ASCII - 43), иначе -
    //ошибка (5).

    auto data = (char*)get_AI_data(
        static_cast<int>( CONSTANTS::C_AIAO_INDEX ) );                     //1

    if ( !data )
        {
        state = -1;
        value = 0.0f;
        return value;
        }

    if ( data[ 0 ] == 0 && data[ 1 ] == 0 ) return value;                  //2
    else if ( data[ 0 ] == 1 && data[ 1 ] == 0 )                           //3
        {
        set_command( static_cast<int>( STATES::TOGGLE_COMMAND ) );
        return value;
        }

    set_command( static_cast<int>( STATES::BUFFER_MOD ) );                 //4

    if ( data[ 2 ] != '+' )                                                //5
        {
        state = -1;
        value = 0.0f;
        return value;
        }

    std::swap( data[ 6 ], data[ 7 ] );
    std::swap( data[ 8 ], data[ 9 ] );
    std::swap( data[ 10 ], data[ 11 ] );
    state = 1;
    value = static_cast<float>( atof( data + 6 ) );
    return value;
    }

float wages_RS232::get_value()
    {
    return value + get_par( static_cast<u_int>( CONSTANTS::P_CZ ) );
    }

void wages_RS232::set_command( int new_state )
    {
    //Установить состояние чтения состояния буфера (1).
    //Установить состояние чтения данных (2).
    auto out =
        (int*)get_AO_write_data( static_cast<int>( CONSTANTS::C_AIAO_INDEX ) );
    if ( !out ) return;

    auto new_st = static_cast<STATES>( new_state );
    if ( new_st == STATES::BUFFER_MOD )                                    //1                                      
        {
        *out = 0;
        }
    else if ( new_st == STATES::TOGGLE_COMMAND )                           //2
        {
        *out = static_cast<u_int>( STATES::READ_CHARACTER );
        }
    }

int wages_RS232::get_state()
    {
    //Здесь мы должны получать состояние весов. Например: идёт взвешивание,
    //взвешивание успешно завершилось, ошибка взвешивания (отрицательные
    //значения), ожидание взвешивания и т.п.
    return state;
    }

void wages_RS232::evaluate_io()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        value = analog_io_device::get_value();
        }
    else
        {
        value = get_value_from_wages();
        }
    }

void wages_RS232::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() ) 
        return analog_io_device::direct_set_value( new_value );
    }

void wages_RS232::tare()
    {
    //Этот метод нужен для тарировки весов (когда текущий вес устанавливается 
    //в качестве нулевого).
    }
//-----------------------------------------------------------------------------
wages_eth::wages_eth( const char* dev_name ) :
    analog_io_device( dev_name, device::DT_WT, device::DST_WT_ETH,
        static_cast< int >( CONSTANTS::LAST_PARAM_IDX ) - 1 )
    {
    set_par_name( static_cast<int>( CONSTANTS::P_CZ ), 0, "P_CZ" );
    }

float wages_eth::get_value()
    {
    return weth->get_wages_value() + get_par( static_cast<u_int>( CONSTANTS::P_CZ ) );
    }

int wages_eth::get_state()
    {
    return weth->get_wages_state();
    }

void wages_eth::evaluate_io()
    {
    if ( G_PAC_INFO()->is_emulator() ) return;

    weth->evaluate();
    }

void wages_eth::tare()
    {
    //Этот метод нужен для тарировки весов (когда текущий вес устанавливается 
    //в качестве нулевого).
    }

void wages_eth::set_string_property( const char* field, const char* value )
    {
    if ( !weth && strcmp( field, "IP" ) == 0 )
        {
        int port = 1001;
        int id = 0;
        weth = new iot_wages_eth( id, value, port );
        }
    }

void wages_eth::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() ) return weth->set_wages_value( new_value );
    }

void wages_eth::direct_set_state( int state )
    {
    if ( G_PAC_INFO()->is_emulator() ) return weth->set_wages_state( state );
    }

void wages_eth::direct_off()
    {
    weth->set_state( 0 );
    }

void wages_eth::direct_on()
    {
    weth->set_state( 1 );
    }

void wages_eth::direct_set_tcp_buff( const char* new_value, size_t size,
    int new_status )
    {
    weth->direct_set_tcp_buff( new_value, size, new_status );
    }
//-----------------------------------------------------------------------------
wages_pxc_axl::wages_pxc_axl( const char* dev_name ) :
    analog_io_device( dev_name, device::DT_WT, device::DST_WT_PXC_AXL,
    static_cast<int>( CONSTANTS::LAST_PARAM_IDX ) - 1 )
    {
    set_par_name( static_cast<int>( CONSTANTS::P_DT ), 0, "P_DT" );
    set_par_name( static_cast<int>( CONSTANTS::P_CZ ), 0, "P_CZ" );
    set_par_name( static_cast<int>( CONSTANTS::P_K ), 0, "P_K" );
    }

void wages_pxc_axl::evaluate_io()
    {
    auto idx = static_cast<u_int>( CONSTANTS::C_AIAO_INDEX );
    auto data = reinterpret_cast<char*>( get_AI_data( idx ) );

    if ( !data ) return;

    //Reverse byte order to get correct int32.
    std::swap( data[ 0 ], data[ 2 ] );
    std::swap( data[ 1 ], data[ 3 ] );
    int weigth = 0;
    std::memcpy( &weigth, data, sizeof( weigth ) );
    w = 0.001f * static_cast<float>( weigth ) *
        get_par( static_cast<int>( CONSTANTS::P_K ) );

    switch ( static_cast<ERR_VALUES>( weigth ) )
        {
        case ERR_VALUES::ERR_OVERRANGE:
            st = static_cast<int>( ERR_STATES::ERR_OVERRANGE );
            break;

        case ERR_VALUES::ERR_WIRE_BREAK:
            st = static_cast<int>( ERR_STATES::ERR_WIRE_BREAK );
            break;

        case ERR_VALUES::ERR_SHORT_CIRCUIT:
            st = static_cast<int>( ERR_STATES::ERR_SHORT_CIRCUIT );
            break;

        case ERR_VALUES::ERR_INVALID_VALUE:
            st = static_cast<int>( ERR_STATES::ERR_INVALID_VALUE );
            break;

        case ERR_VALUES::ERR_FAULTY_SUPPLY_VOLTAGE:
            st = static_cast<int>( ERR_STATES::ERR_FAULTY_SUPPLY_VOLTAGE );
            break;

        case ERR_VALUES::ERR_FAULTY_DEVICE:
            st = static_cast<int>( ERR_STATES::ERR_FAULTY_DEVICE );
            break;

        case ERR_VALUES::ERR_UNDERRANGE:
            st = static_cast<int>( ERR_STATES::ERR_UNDERRANGE );
            break;

        default:
            st = 0;
            break;
        }
    }

void wages_pxc_axl::tare()
    {
    set_par( static_cast<int>( CONSTANTS::P_CZ ), 0, -w );
    }

void wages_pxc_axl::reset_tare()
    {
    set_par( static_cast<int>( CONSTANTS::P_CZ ), 0, 0 );
    }

float wages_pxc_axl::get_value()
    {
    return w + get_par( static_cast<int>( CONSTANTS::P_CZ ) );
    }

int wages_pxc_axl::get_state()
    {
    return st;
    }

void wages_pxc_axl::direct_set_state( int new_state )
    {
    switch ( static_cast<CMDS>( new_state ) )
        {
        case CMDS::TARE:
            tare();
            break;

        case CMDS::RESET_TARE:
            reset_tare();
            break;
        }
    }

void wages_pxc_axl::direct_set_value( float new_value )
    {
    // Do nothing.
    }
//-----------------------------------------------------------------------------
wages::wages( const char *dev_name ) : analog_io_device(
    dev_name, DT_WT, DST_NONE, ADDITIONAL_PARAM_COUNT )
    {
    set_par_name( P_NOMINAL_W,  0, "P_NOMINAL_W" );
    set_par_name( P_RKP, 0, "P_RKP");
    set_par_name( P_C0, 0, "P_CZ" );
    set_par_name( P_DT, 0, "P_DT");
    }
//-----------------------------------------------------------------------------
void wages::tare()
    {
    set_par(P_C0, 0, -weight);
    return;
    }
//-----------------------------------------------------------------------------
float wages::get_weight()
    {
    if (get_delta_millisec(filter_time) > 500)
        {
        filter_time = get_millisec();
        float rkp = get_par(P_RKP, 0);
        if (0 == rkp) return -1001;
        float uref = get_AI(C_AI_Uref);
        if (0 == uref) return -1002;
        float filterval = get_par(P_DT, 0);
        float now_weight = get_AI(C_AI_Ud) / rkp / uref * get_par(P_NOMINAL_W, 0);
        if (fabs(now_weight - weight) > filterval)
            {
            weight = now_weight;
            }
        if (filterval >= 1)
            {
            weight = ceilf(weight);
            }
        }
    return weight + get_par(P_C0, 0);
    }
//-----------------------------------------------------------------------------
float wages::get_value()
    {
    if ( G_PAC_INFO()->is_emulator() ) return weight + get_par(P_C0, 0);

    return get_weight();
    }
//-----------------------------------------------------------------------------
void wages::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() ) weight = new_value;
    }
//-----------------------------------------------------------------------------
void wages::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        switch ( new_state )
            {
            case S_TARE:
                tare();
                break;
            }
        }
    }
//-----------------------------------------------------------------------------
int wages::get_state()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int wages::save_device_ex( char* buff )
    {
    return static_cast<int>( fmt::format_to_n( buff, MAX_COPY_SIZE, "W={:.3f}, ",
        get_value() ).size );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
level_e::level_e( const char* dev_name ) : level(
    dev_name, DST_LT, 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
level_e_cyl::level_e_cyl( const char* dev_name ) : level(
    dev_name, DST_LT_CYL, LAST_PARAM_IDX - 1 )
    {
    start_param_idx = level::get_params_count();
    set_par_name( P_MAX_P, start_param_idx, "P_MAX_P" );
    set_par_name( P_R, start_param_idx, "P_R" );
    }
//-----------------------------------------------------------------------------
int level_e_cyl::calc_volume()
    {
    float v = get_par( P_R, start_param_idx );
    v = (float)M_PI * v * v *  get_value() *
        get_par( P_MAX_P, start_param_idx ) / 9.81f;

    int v_kg = 10 * (int)( v * 100 + 0.5f ); //Переводим в килограммы.

    return v_kg;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
level_e_cone::level_e_cone( const char* dev_name ) : level(
    dev_name, DST_LT_CONE, LAST_PARAM_IDX - 1 )
    {
    start_param_idx = level::get_params_count();
    set_par_name( P_MAX_P, start_param_idx, "P_MAX_P" );
    set_par_name( P_R, start_param_idx, "P_R" );
    set_par_name( P_H_CONE, start_param_idx, "P_H_CONE" );
    }
//-----------------------------------------------------------------------------
int level_e_cone::calc_volume()
    {
    float r = get_par( P_R, start_param_idx );
    float tg_a = 0;
    if ( get_par( P_H_CONE, start_param_idx ) > 0 )
        {
        tg_a = r / get_par( P_H_CONE, start_param_idx );
        }

    float h_cone = get_par( P_H_CONE, start_param_idx );
    float h_curr = get_par( P_MAX_P, start_param_idx ) * get_value() / 9.81f;

    float v = 0;
    if ( h_curr <= h_cone )
        {
        v = (float)M_PI * h_curr * tg_a * h_curr * tg_a * h_curr / 3;
        }
    else
        {
        v = (float)M_PI * r * r * ( h_curr - h_cone * 2 / 3 );
        }
    int v_kg = 10 * (int)( v * 100 + 0.5f ); //Переводим в килограммы.

    return v_kg;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_DO1::valve_DO1( const char* dev_name ) : valve( dev_name, DT_V, DST_V_DO1 )
    {
    }
//-----------------------------------------------------------------------------
void valve_DO1::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_on();

    set_DO( DO_INDEX, 1 );
    }
//-----------------------------------------------------------------------------
void valve_DO1::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return valve::direct_off();

    set_DO( DO_INDEX, 0 );
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_DO1::get_valve_state()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return (VALVE_STATE)digital_io_device::get_state();
        }

    return (VALVE_STATE)get_DO( DO_INDEX );
    };
//-----------------------------------------------------------------------------
bool valve_DO1::get_fb_state()
    {
    return true;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void virtual_motor::direct_off()
    {
    state = 0;
    value = 0;
    }

void virtual_motor::direct_set_value( float new_value )
    {
    value = new_value;
    }

float virtual_motor::get_value()
    {
    return value;
    }

void virtual_motor::direct_set_state( int new_state )
    {
    state = new_state;
    }

void virtual_motor::direct_on()
    {
    state = 1;
    }

int virtual_motor::get_state()
    {
    return state;
    }

virtual_motor::virtual_motor( const char* dev_name ):
    i_motor( dev_name, device::DST_M_VIRT, 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
motor::motor( const char* dev_name, device::DEVICE_SUB_TYPE sub_type ) :
    i_motor( dev_name, sub_type, ADDITIONAL_PARAM_COUNT ),
    io_device( dev_name )
    {
    set_par_name( P_ON_TIME, 0, "P_ON_TIME" );
    }
//-----------------------------------------------------------------------------
float motor::get_value()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::get_value();

    auto sub_type = get_sub_type();
    if ( sub_type == device::DST_M_FREQ || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        return get_AO( AO_INDEX, C_MIN_VALUE, C_MAX_VALUE );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void motor::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() )
        return device::direct_set_value( new_value );

    auto sub_type = get_sub_type();
    if ( sub_type == device::DST_M_FREQ || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        set_AO( AO_INDEX, new_value, C_MIN_VALUE, C_MAX_VALUE );
        }
    }
//-----------------------------------------------------------------------------
void motor::direct_set_state( int new_state )
    {
    if ( G_PAC_INFO()->is_emulator() )
        return device::direct_set_state( new_state );

    auto sub_type = get_sub_type();
    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ )
        {
        if ( new_state == 2 )
            {
            // Включение прямого пуска.
            int o = get_DO( DO_INDEX );
            if ( 0 == o )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX, 1 );
                }

            // Включение реверса.
            o = get_DO( DO_INDEX_REVERSE );
            if ( 0 == o )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_REVERSE, 1 );
                }

            return;
            }
        }

    if ( sub_type == device::DST_M_REV_2 ||
        sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        if ( new_state == 2 )
            {
            // Выключение прямого пуска.
            int o = get_DO( DO_INDEX );
            if ( 1 == o )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX, 0 );
                }

            // Включение реверса.
            o = get_DO( DO_INDEX_REVERSE );
            if ( 0 == o )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_REVERSE, 1 );
                }

            return;
            }
        }

    if ( new_state )
        {
        direct_on();
        }
    else
        {
        direct_off();
        }
    }
//-----------------------------------------------------------------------------
int motor::get_state()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::get_state();

    int o = get_DO( DO_INDEX );
    auto sub_type = get_sub_type();

    if ( sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        int err = get_DI( DI_INDEX_ERROR );

        if ( 1 == err )
            {
            if ( get_delta_millisec( start_switch_time ) > get_par( P_ON_TIME, 0 ) )
                {
                return -1;
                }
            }
        else
            {
            start_switch_time = get_millisec();
            }

        int ro = get_DO( DO_INDEX_REVERSE );
        if ( 1 == ro )
            {
            return 2;
            }

        if ( 1 == o )
            {
            return 1;
            }

        return 0;
        }

    int i = get_DI( DI_INDEX );

    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 )
        {
        int ro = get_DO( DO_INDEX_REVERSE );

        if ( 0 == ro && 0 == o && 0 == i )
            {
            start_switch_time = get_millisec();
            return 0;
            }

        if ( 0 == ro && 1 == o && 1 == i )
            {
            start_switch_time = get_millisec();
            return 1;
            }

        if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ )
            {
            if ( 1 == ro && 1 == o && 1 == i )
                {
                start_switch_time = get_millisec();
                return 2;
                }
            }

        if ( sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 )
            {
            if ( 1 == ro && 0 == o && 1 == i )
                {
                start_switch_time = get_millisec();
                return 2;
                }
            }
        }
    else
        {
        if ( o == i )
            {
            start_switch_time = get_millisec();
            return i;
            }
        }

    if ( get_delta_millisec( start_switch_time ) > get_par( P_ON_TIME, 0 ) )
        {
        return -1;
        }
    else
        {
        return i;
        }
    }
//-----------------------------------------------------------------------------
void motor::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::direct_on();

    auto sub_type = get_sub_type();
    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        // Выключение реверса.
        int o = get_DO( DO_INDEX_REVERSE );
        if ( 0 != o )
            {
            start_switch_time = get_millisec();
            set_DO( DO_INDEX_REVERSE, 0 );
            }
        }

    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void motor::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::direct_off();

    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    auto sub_type = get_sub_type();
    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        // Отключение реверса.
        o = get_DO( DO_INDEX_REVERSE );
        if ( o != 0 )
            {
            start_switch_time = get_millisec();
            set_DO( DO_INDEX_REVERSE, 0 );
            }
        }

    direct_set_value( 0 );
    }
//-----------------------------------------------------------------------------
int motor::save_device_ex( char *buff )
    {
    int res = 0;
    if ( G_PAC_INFO()->is_emulator() )
        return static_cast<int>(
        fmt::format_to_n( buff, MAX_COPY_SIZE, "R=0, ERRT=0, " ).size );

    auto sub_type = get_sub_type();
    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        if ( sub_type == device::M_REV_2_ERROR ||
            sub_type == device::DST_M_REV_FREQ_2_ERROR )
            {
            res = static_cast<int>( fmt::format_to_n( buff, MAX_COPY_SIZE, "R={}, ERRT={}, ",
                get_DO( DO_INDEX_REVERSE ), get_DI( DI_INDEX_ERROR ) ).size );
            }
        else
            {
            res = static_cast<int>( fmt::format_to_n( buff, MAX_COPY_SIZE, "R={}, ",
                get_DO( DO_INDEX_REVERSE ) ).size );
            }
        }

    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool level_s::is_active()
    {
    switch ( get_sub_type() )
        {
        case DST_LS_MIN:
            return get_state() == 0 ? false : true;

        case DST_LS_MAX:
            return get_state() == 0 ? true : false;

        default:
            return get_state() == 0 ? false : true;
        }
    }
//-----------------------------------------------------------------------------
level_s::level_s( const char *dev_name, device::DEVICE_SUB_TYPE sub_type ):
    DI1( dev_name, DT_LS, sub_type, 0,
        sub_type == DST_LS_MAX ? 1 : 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
level_s_iolink::level_s_iolink( const char *dev_name,
    device::DEVICE_SUB_TYPE sub_type ):
    analog_io_device( dev_name, DT_LS, sub_type, LAST_PARAM_IDX - 1 ),
    current_state( sub_type == device::LS_IOLINK_MAX ? 1 : 0 )    
    {
    set_par_name( P_DT, 0, "P_DT" );
    set_par_name( P_ERR, 0, "P_ERR" );
    }

void level_s_iolink::evaluate_io()
    {
    auto data = (char*)get_AI_data( C_AI_INDEX );

    if ( !data ) return;

    switch ( n_article )
        {
        case ARTICLE::IFM_LMT100:   //IFM.LMT100
        case ARTICLE::IFM_LMT102:   //IFM.LMT102
        case ARTICLE::IFM_LMT104:   //IFM.LMT104
        case ARTICLE::IFM_LMT105:   //IFM.LMT105
            {
            LS_data info{};
            std::reverse_copy( data, data + sizeof( info ), (char*)&info );
            v = (float) info.v;
            st = info.st1;
            break;
            }

        case ARTICLE::EH_FTL33:     //E&H.FTL33-GR7N2ABW5J
            {
            rev_LS_data info{};
            std::reverse_copy( data, data + sizeof( info ), (char*) &info );
            v = 0.1f *info.v;
            st = info.st1;
            break;
            }

        case ARTICLE::DEFAULT:
            v = get_par( P_ERR, 0 );
            st = 0;
            break;
        }
    }

void level_s_iolink::set_article( const char* new_article )
    {
    device::set_article( new_article );

    auto article = get_article();
    if ( strcmp( article, "IFM.LMT100" ) == 0 )
        {
        n_article = ARTICLE::IFM_LMT100;
        return;
        }
    if (strcmp(article, "IFM.LMT102") == 0)
        {
        n_article = ARTICLE::IFM_LMT102;
        return;
        }
    if (strcmp(article, "IFM.LMT104") == 0)
        {
        n_article = ARTICLE::IFM_LMT104;
        return;
        }
    if (strcmp(article, "IFM.LMT105") == 0)
        {
        n_article = ARTICLE::IFM_LMT105;
        return;
        }
    if ( strcmp( article, "E&H.FTL33-GR7N2ABW5J" ) == 0 )
        {
        n_article = ARTICLE::EH_FTL33;
        return;
        }

    if ( G_DEBUG )
        {
        G_LOG->warning( "%s unknown article \"%s\"",
            get_name(), new_article );
        }
    }

float level_s_iolink::get_value()
    {
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_value();

	if (get_AI_IOLINK_state(C_AI_INDEX) != io_device::IOLINKSTATE::OK)
		{
		return get_par( P_ERR, 0 );
		}
	else
		{
        return v;
		}
    }

int level_s_iolink::get_state()
	{
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_state();

	io_device::IOLINKSTATE devstate = get_AI_IOLINK_state(C_AI_INDEX);
	if (devstate != io_device::IOLINKSTATE::OK)
		{
		return get_sub_type() == device::LS_IOLINK_MAX ? 1 : 0;
		}

    auto dt = (u_int_4)get_par( P_DT, 0 );
    if ( dt > 0 )
        {
        if ( current_state != st )
            {
            if ( get_delta_millisec( time ) > dt )
                {
                current_state = st;
                time = get_millisec();
                }
            }
        else
            {
            time = get_millisec();
            }
        }
    else current_state = st;

    return current_state;
	}

bool level_s_iolink::is_active()
    {
    return get_state();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
flow_s::flow_s( const char* dev_name ) : DI1( dev_name, DT_FS, DST_NONE,
    0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DI_signal::DI_signal( const char* dev_name ) : DI1( dev_name, DT_DI, DST_NONE, 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
button::button( const char* dev_name ) : DI1( dev_name, DT_SB, DST_NONE, 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DO_signal::DO_signal( const char* dev_name ) : DO1( dev_name, DT_DO, DST_NONE )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
siren::siren( const char* dev_name ) : DO1( dev_name, DT_HA, DST_NONE )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
lamp::lamp( const char* dev_name ) : DO1( dev_name, DT_HL, DST_NONE )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
level_e_iolink::level_e_iolink( const char *dev_name ) :
    level( dev_name, DST_LT_IOLINK, LAST_PARAM_IDX - 1 )
    {
    start_param_idx = level::get_params_count();
    set_par_name( P_MAX_P, start_param_idx, "P_MAX_P" );
    set_par_name( P_R, start_param_idx, "P_R" );
    set_par_name( P_H_CONE, start_param_idx, "P_H_CONE" );
    }
//-----------------------------------------------------------------------------
int level_e_iolink::calc_volume()
    {
    int v_kg = 0;
    float v = 0.0;
    if ( G_PAC_INFO()->is_emulator() )
        {
        v = get_value();
        v = v / 100 * get_par( P_MAX_P, start_param_idx );
        }
    else
        {
        v = this->v;
        }

    if (PT_extra)
        {
        v -= PT_extra->get_value();
        }

    if ( get_par( P_H_CONE, start_param_idx ) <= 0 )
        {
        float val = get_par( P_R, start_param_idx );
        val = (float)M_PI * val * val * v * 100 / 9.81f;

        v_kg = 10 * (int)( val * 100 + 0.5f ); //Переводим в килограммы.
        }
    else
        {
        float r = get_par( P_R, start_param_idx );
        float tg_a = 0;
        if ( get_par( P_H_CONE, start_param_idx ) > 0 )
            {
            tg_a = r / get_par( P_H_CONE, start_param_idx );
            }

        float h_cone = get_par( P_H_CONE, start_param_idx );
        float h_curr = 100 * v / 9.81f;

        float val = 0;
        if ( h_curr <= h_cone )
            {
            val = (float)M_PI * h_curr * tg_a * h_curr * tg_a * h_curr / 3;
            }
        else
            {
            val = (float)M_PI * r * r * ( h_curr - h_cone * 2 / 3 );
            }
        v_kg = 10 * (int)( val * 100 + 0.5f ); //Переводим в килограммы.
        }

    return v_kg;
    }
//-----------------------------------------------------------------------------
float level_e_iolink::get_value()
    {
    if ( G_PAC_INFO()->is_emulator() ) return AI1::get_value();

    if (get_AI_IOLINK_state(C_AI_INDEX) != io_device::IOLINKSTATE::OK)
        {
        return get_par( level::P_ERR, get_start_param_idx() );
        }
    else
        {
        return v / get_par( P_MAX_P, start_param_idx ) * 100;
        }
    }
//-----------------------------------------------------------------------------
int level_e_iolink::get_state()
    {
    if ( G_PAC_INFO()->is_emulator() ) return AI1::get_state();

    IOLINKSTATE res = get_AI_IOLINK_state(C_AI_INDEX);
    if (res != io_device::IOLINKSTATE::OK)
        {
        return -(int)res;
        }
    else
        {
        return st;
        }
    }
//-----------------------------------------------------------------------------
void level_e_iolink::set_article( const char* new_article )
    {
    device::set_article( new_article );
    pressure_e_iolink::read_article( new_article, n_article, this );
    }
//-----------------------------------------------------------------------------
void level_e_iolink::evaluate_io()
    {
    auto data = (char*)get_AI_data( C_AI_INDEX );

    if ( !data ) return;

    pressure_e_iolink::evaluate_io( get_name(), data, n_article, v, st );
    }

void level_e_iolink::set_string_property(const char* field, const char* value)
    {
    if (strcmp(field, "PT") == 0)
        {
        PT_extra = PT(value);
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
pressure_e::pressure_e( const char* dev_name ) : AI1( dev_name, DT_PT, DST_NONE,
    ADDITIONAL_PARAM_COUNT )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( P_MIN_V, start_param_idx, "P_MIN_V" );
    set_par_name( P_MAX_V, start_param_idx, "P_MAX_V" );
    }
//-----------------------------------------------------------------------------
float pressure_e::get_max_val()
    {
    return get_par( P_MAX_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
float pressure_e::get_min_val()
    {
    return get_par( P_MIN_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
pressure_e_iolink::pressure_e_iolink( const char* dev_name ) :
    analog_io_device( dev_name, DT_PT, DST_PT_IOLINK, LAST_PARAM_IDX - 1 )
    {
    set_par_name( P_ERR, 0, "P_ERR" );
    }
//-----------------------------------------------------------------------------
void pressure_e_iolink::set_article( const char* new_article )
    {
    device::set_article( new_article );
    read_article( new_article, n_article, this );
    }
//-----------------------------------------------------------------------------
void pressure_e_iolink::read_article( const char* article,
    ARTICLE& n_article, const device* dev )
    {
    if ( strcmp( article, "IFM.PI2715" ) == 0 )
        {
        n_article = ARTICLE::IFM_PI2715;
        return;
        }
    if ( strcmp( article, "IFM.PI2794" ) == 0 )
        {
        n_article = ARTICLE::IFM_PI2794;
        return;
        }
    if ( strcmp( article, "IFM.PI2797" ) == 0 )
        {
        n_article = ARTICLE::IFM_PI2797;
        return;
        }

    if ( strcmp( article, "IFM.PM1704" ) == 0 )
        {
        n_article = ARTICLE::IFM_PM1704;
        return;
        }
    if ( strcmp( article, "IFM.PM1705" ) == 0 )
        {
        n_article = ARTICLE::IFM_PM1705;
        return;
        }
    if ( strcmp( article, "IFM.PM1707" ) == 0 )
        {
        n_article = ARTICLE::IFM_PM1707;
        return;
        }
    if ( strcmp( article, "IFM.PM1708" ) == 0 )
        {
        n_article = ARTICLE::IFM_PM1708;
        return;
        }
    if ( strcmp( article, "IFM.PM1709" ) == 0 )
        {
        n_article = ARTICLE::IFM_PM1709;
        return;
        }
    if ( strcmp( article, "IFM.PM1715" ) == 0 )
        {
        n_article = ARTICLE::IFM_PM1715;
        return;
        }

    if ( strcmp( article, "FES.8001446" ) == 0 )
        {
        n_article = ARTICLE::FES_8001446;
        return;
        }

    if ( G_DEBUG )
        {
        G_LOG->warning( "%s unknown article \"%s\"",
            dev->get_name(), article );
        }
    }
//-----------------------------------------------------------------------------
void pressure_e_iolink::evaluate_io( const char *name, char* data, ARTICLE n_article,
    float& v, int& st )
    {
    if ( !data ) return;

    switch ( n_article )
        {
        case ARTICLE::IFM_PI2715:
        case ARTICLE::IFM_PI2794:
        case ARTICLE::IFM_PI2797:
        case ARTICLE::FES_8001446:
            {
            PT_data info{};
            std::reverse_copy( data, data + sizeof( info ), (char*)&info );

            v = info.v;
            st = 0;
            }
            break;

        case ARTICLE::IFM_PM1704:
        case ARTICLE::IFM_PM1705:
        case ARTICLE::IFM_PM1707:
        case ARTICLE::IFM_PM1708:
        case ARTICLE::IFM_PM1709:
        case ARTICLE::IFM_PM1715:
            {
            ex_PT_data info{};

            std::swap( data[ 0 ], data[ 1 ] );
            std::swap( data[ 2 ], data[ 3 ] );
            std::copy( data, data + sizeof( info ), (char*)&info );

            v = info.v;
            st = info.status;
            }
            break;

        case ARTICLE::DEFAULT:
            v = 0;
            st = 0;
            break;
        }

    float alfa = 1;
    switch ( n_article )
        {
        case ARTICLE::IFM_PM1708:       //  0.01, mbar
            alfa = 0.00001f;
            break;

        case ARTICLE::IFM_PM1707:       //   0.1, mbar
        case ARTICLE::IFM_PM1709:       //   0.1, mbar
            alfa = 0.0001f;
            break;

        case ARTICLE::IFM_PI2715:       // 0.001, bar
        case ARTICLE::IFM_PI2797:       //     1, mbar

        case ARTICLE::IFM_PM1704:       // 0.001, bar
        case ARTICLE::IFM_PM1705:       // 0.001, bar
        case ARTICLE::IFM_PM1715:       // 0.001, bar
            alfa = 0.001f;
            break;

        case ARTICLE::IFM_PI2794:       // 0.01, bar
            alfa = 0.01f;
            break;

        case ARTICLE::FES_8001446:
            alfa = 0.000610388818f;
            break;

        case ARTICLE::DEFAULT:
            alfa = 1;
            break;
        }
    v = alfa * v;
    }
//-----------------------------------------------------------------------------
void pressure_e_iolink::evaluate_io()
    {
    evaluate_io( get_name(), (char*)get_AI_data( C_AI_INDEX ), n_article, v, st );
    }
//-----------------------------------------------------------------------------
float pressure_e_iolink::get_value()
    {
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_value();

    if (get_AI_IOLINK_state(C_AI_INDEX) != io_device::IOLINKSTATE::OK)
        {
        return get_par( P_ERR, 0 );
        }
    else
        {
        return v;
        }
    }
//-----------------------------------------------------------------------------
int pressure_e_iolink::get_state()
    {
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_state();

    IOLINKSTATE res = get_AI_IOLINK_state( C_AI_INDEX );
    if ( res != io_device::IOLINKSTATE::OK )
        {
        return -(int)res;
        }
    else
        {
        return st;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
circuit_breaker::circuit_breaker( const char* dev_name ):analog_io_device(
    dev_name, DT_F, DST_F, 0)
    {
    }
//-----------------------------------------------------------------------------
int circuit_breaker::save_device_ex( char *buff )
    {
    auto res = ( fmt::format_to_n( buff, MAX_COPY_SIZE, "ERR={}, M={}, ",
        err, m ) ).size;

    res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE,
        "NOMINAL_CURRENT_CH={{{},{},{},{}}}, ",
        +in_info.nominal_current_ch1, +in_info.nominal_current_ch2,
        +in_info.nominal_current_ch3, +in_info.nominal_current_ch4 ) ).size;
    res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE,
        "LOAD_CURRENT_CH={{{:.1f},{:.1f},{:.1f},{:.1f}}}, ",
        .1f * in_info.load_current_ch1, .1f * in_info.load_current_ch2,
        .1f * in_info.load_current_ch3, .1f * in_info.load_current_ch4 ) ).size;

    res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE,
        "ST_CH={{{},{},{},{}}}, ",
        +in_info.st_ch1, +in_info.st_ch2,
        +in_info.st_ch3, +in_info.st_ch4 ) ).size;
    res += ( fmt::format_to_n( buff + res, MAX_COPY_SIZE,
        "ERR_CH={{{},{},{},{}}}, ",
        +in_info.err_ch1, +in_info.err_ch2,
        +in_info.err_ch3, +in_info.err_ch4 ) ).size;

    return res;
    }
//-----------------------------------------------------------------------------
int circuit_breaker::set_cmd( const char *prop, u_int idx, double val )
    {
    if (G_DEBUG)
        {
        G_LOG->debug( 
            "%s\t circuit_breaker::set_cmd() - prop = %s, idx = %d, val = %f",
            get_name(), prop, idx, val);
        }

    if ( strcmp( prop, "ST" ) == 0 )
        {
        int new_val = ( int ) val;

        if ( new_val )
            {
            on();
            }
        else
            {
            off();
            }

        return 0;
        }


    if ( strcmp( prop, "ST_CH" ) == 0 )
        {
        switch ( idx )
            {
            case 1:
                out_info->switch_ch1 = val;
                if ( G_PAC_INFO()->is_emulator() ) in_info.st_ch1 = val;
                return 0;
            case 2:
                out_info->switch_ch2 = val;
                if ( G_PAC_INFO()->is_emulator() ) in_info.st_ch2 = val;
                return 0;
            case 3:
                out_info->switch_ch3 = val;
                if ( G_PAC_INFO()->is_emulator() ) in_info.st_ch3 = val;
                return 0;
            case 4:
                out_info->switch_ch4 = val;
                if ( G_PAC_INFO()->is_emulator() ) in_info.st_ch4 = val;
                return 0;
            }
        }

    return analog_io_device::set_cmd( prop, idx, val );
    }
//-----------------------------------------------------------------------------
void circuit_breaker::direct_set_value( float v )
    {
    if ( v )
        {
        on();
        }
    else
        {
        off();
        }
    }
//-----------------------------------------------------------------------------
void circuit_breaker::direct_on()
    {
    out_info->valid_flag = true;
    out_info->switch_ch1 = true;
    out_info->switch_ch2 = true;
    out_info->switch_ch3 = true;
    out_info->switch_ch4 = true;

    if ( G_PAC_INFO()->is_emulator() )
        {
        in_info.st_ch1 = true;
        in_info.st_ch2 = true;
        in_info.st_ch3 = true;
        in_info.st_ch4 = true;
        }
    }
//-----------------------------------------------------------------------------
void circuit_breaker::direct_off()
    {
    out_info->valid_flag = true;
    out_info->switch_ch1 = false;
    out_info->switch_ch2 = false;
    out_info->switch_ch3 = false;
    out_info->switch_ch4 = false;

    if ( G_PAC_INFO()->is_emulator() )
        {
        in_info.st_ch1 = false;
        in_info.st_ch2 = false;
        in_info.st_ch3 = false;
        in_info.st_ch4 = false;
        }
    }
//-----------------------------------------------------------------------------
float circuit_breaker::get_value()
    {
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_value();

    return v;
    }
//-----------------------------------------------------------------------------
int circuit_breaker::get_state()
    {
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_state();

    IOLINKSTATE res = get_AI_IOLINK_state( C_AI_INDEX );
    if ( res != io_device::IOLINKSTATE::OK )
        {
        return -(int)res;
        }
    else
        {
        return st;
        }
    }
//-----------------------------------------------------------------------------
void circuit_breaker::evaluate_io()
    {
    auto res = get_AO_write_data( 0 );
    if ( !res ) return;

    out_info = (F_data_out*)res;

    if ( get_AI_IOLINK_state(C_AI_INDEX) == io_device::IOLINKSTATE::OK )
        {
        if ( !is_read_OK )
            {
            out_info->switch_ch1 = in_info.st_ch1;
            out_info->switch_ch2 = in_info.st_ch2;
            out_info->switch_ch3 = in_info.st_ch3;
            out_info->switch_ch4 = in_info.st_ch4;
            out_info->nominal_current_ch1 = in_info.nominal_current_ch1;
            out_info->nominal_current_ch2 = in_info.nominal_current_ch2;
            out_info->nominal_current_ch3 = in_info.nominal_current_ch3;
            out_info->nominal_current_ch4 = in_info.nominal_current_ch4;

            is_read_OK = true;
            }
        }
    else
        {
        is_read_OK = false;
        }

    auto data = (char*)get_AI_data(C_AI_INDEX);
    std::copy(data, data + sizeof(in_info), (char*)&in_info);

#ifdef DEBUG_IOLINK_F
    char* tmp = (char*)data;
    sprintf(G_LOG->msg, "%x %x %x %x %x %x %x %x\n",
        tmp[0], tmp[1], tmp[2], tmp[3],
        tmp[4], tmp[5], tmp[6], tmp[7] );
    G_LOG->write_log(i_log::P_WARNING);

    sprintf(G_LOG->msg, "nominal_current_ch1 %u, load_current_ch1 %u, "
        "st_ch1 %x, err_ch1 %x, %.1f\n", in_info.nominal_current_ch1,
        in_info.load_current_ch1, in_info.st_ch1, in_info.err_ch1,
        .1f * in_info.v );
    G_LOG->write_log(i_log::P_WARNING);
    sprintf(G_LOG->msg, "nominal_current_ch2 %u, load_current_ch2 %u, "
        "st_ch2 %x, err_ch2 %x\n", in_info.nominal_current_ch2,
        in_info.load_current_ch2, in_info.st_ch2, in_info.err_ch2 );
    G_LOG->write_log(i_log::P_WARNING);
#endif
    v = .1f * in_info.v;
    st = in_info.st_ch1 || in_info.st_ch2 ||
        in_info.st_ch3 || in_info.st_ch4 ? 1 : 0;
    err = in_info.err_ch1 || in_info.err_ch2 ||
        in_info.err_ch3 || in_info.err_ch4 ? 1 : 0;
    }
//-----------------------------------------------------------------------------
circuit_breaker::F_data_out::F_data_out(): switch_ch1( false ), switch_ch2( false ),
    switch_ch3( false ), switch_ch4( false ),
    reserved( 0 ), valid_flag( false ),
    nominal_current_ch1( 0 ), nominal_current_ch2( 0 ),
    nominal_current_ch3( 0 ), nominal_current_ch4( 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
concentration_e::concentration_e( const char* dev_name,
    DEVICE_SUB_TYPE sub_type ) :
    AI1( dev_name, DT_QT, sub_type, ADDITIONAL_PARAM_COUNT )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( P_MIN_V, start_param_idx, "P_MIN_V" );
    set_par_name( P_MAX_V, start_param_idx, "P_MAX_V" );
    }
//-----------------------------------------------------------------------------
float concentration_e::get_max_val()
    {
    return get_par( P_MAX_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
float concentration_e::get_min_val()
    {
    return get_par( P_MIN_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
concentration_e_ok::concentration_e_ok( const char* dev_name ) : concentration_e( dev_name,
    DST_QT_OK )
    {
    }

int concentration_e_ok::get_state()
    {
    if ( G_PAC_INFO()->is_emulator() ) return concentration_e::get_state();

    int i = get_DI( DI_INDEX );
    return i == 1 ? concentration_e::get_state() : -1;
    }

int concentration_e_ok::save_device_ex( char* buff )
    {
    int res = 0;
    if ( G_PAC_INFO()->is_emulator() )
        {
        res = static_cast<int>( fmt::format_to_n( buff, MAX_COPY_SIZE,
            "OK={1}, " ).size );
        }
    else
        {
        res = static_cast<int>( fmt::format_to_n( buff, MAX_COPY_SIZE, "OK={}, ",
            get_DI( DI_INDEX ) ).size );
        }

    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
concentration_e_iolink::concentration_e_iolink( const char* dev_name ) :
    analog_io_device( dev_name,
    DT_QT, DST_QT_IOLINK, LAST_PARAM_IDX - 1 )
    {
    set_par_name( P_ERR, 0, "P_ERR" );
    };
//-----------------------------------------------------------------------------
concentration_e_iolink::~concentration_e_iolink()
    {
    delete info;
    info = nullptr;
    }
//-----------------------------------------------------------------------------
int concentration_e_iolink::save_device_ex( char *buff )
    {
    auto res = ( fmt::format_to_n( buff, MAX_COPY_SIZE, "T={:.1f}, ",
        get_temperature() ) ).size;

    return res;
    }
//-----------------------------------------------------------------------------
float concentration_e_iolink::get_temperature() const
    {
    return .1f * info->temperature;
    }
//-----------------------------------------------------------------------------
float concentration_e_iolink::get_value()
    {
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_value();

    if ( get_AI_IOLINK_state( C_AI_INDEX ) != io_device::IOLINKSTATE::OK )
        {
        return get_par( P_ERR, 0 );
        }
    else
        {
        return 0.001f * info->conductivity;
        }
    }
//-----------------------------------------------------------------------------
int concentration_e_iolink::get_state()
	{
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_state();

    IOLINKSTATE res = get_AI_IOLINK_state( C_AI_INDEX );
    if ( res != io_device::IOLINKSTATE::OK )
        {
        return -(int)res;
        }
    else
        {
        return info->status;
        }
	}
//-----------------------------------------------------------------------------
void concentration_e_iolink::evaluate_io()
    {
    auto data = (char*)get_AI_data(0);
    if ( !data ) return;

    const int SIZE = 12;
    std::reverse_copy (data, data + SIZE, (char*) info);

#ifdef DEBUG_IOLINK_QT
    char *tmp = (char*) info;
    sprintf( G_LOG->msg, "%x %x %x %x %x %x %x %x %x %x %x %x\n",
        tmp[ 0 ], tmp[ 1 ], tmp[ 2 ], tmp[ 3 ],
        tmp[ 4 ], tmp[ 5 ], tmp[ 6 ], tmp[ 7 ],
        tmp[ 8 ], tmp[ 9 ], tmp[ 10 ], tmp[ 11 ] );
    G_LOG->write_log( i_log::P_WARNING );

    sprintf( G_LOG->msg, "conductivity %u, temperature %u, "
            "status %x\n", info->conductivity,
            info->temperature, info->status);
    G_LOG->write_log(i_log::P_NOTICE);

    sprintf( G_LOG->msg,
            "conductivity %.3f, temperature %.1f, status %x\n",
            get_value(), get_temperature(), get_state());
    G_LOG->write_log(i_log::P_NOTICE);
#endif
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
analog_input::analog_input( const char* dev_name ) : AI1( dev_name, DT_AI,
    DST_NONE, ADDITIONAL_PARAM_COUNT )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( P_MIN_V, start_param_idx, "P_MIN_V" );
    set_par_name( P_MAX_V, start_param_idx, "P_MAX_V" );
    }
//-----------------------------------------------------------------------------
float analog_input::get_max_val()
    {
    return get_par( P_MAX_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
float analog_input::get_min_val()
    {
    return get_par( P_MIN_V, start_param_idx );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
analog_io_device::analog_io_device( const char* dev_name, 
    device::DEVICE_TYPE type, device::DEVICE_SUB_TYPE sub_type,
    u_int par_cnt ) : device( dev_name, type, sub_type, par_cnt ),
    io_device( dev_name )
    {
    }
//-----------------------------------------------------------------------------
void analog_io_device::print() const
    {
    device::print();
    //io_device::print();
    }
//-----------------------------------------------------------------------------
int analog_io_device::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_DEBUG )
        {
        fmt::format_to( G_LOG->msg,
            "{}\t analog_io_device::set_cmd() - prop = {}, idx = {}, val = {}",
            get_name(), prop, idx, val );
        G_LOG->write_log( i_log::P_DEBUG );
        }

    analog_emulator& emulator = get_emulator();
    if ( strcmp( prop, "M_EXP" ) == 0 )
        {
        emulator.param( static_cast<float>( val ), emulator.get_st_deviation() );
        }
    else if ( strcmp( prop, "S_DEV" ) == 0 )
        {
        emulator.param( emulator.get_m_expec(), static_cast<float>( val ) );
        }
    else if ( prop[ 0 ] == 'E' )
        {
        set_emulation( val != 0 );
        }
    else
        {
        return device::set_cmd( prop, idx, val );
        }

    return 0;
    }

int analog_io_device::save_device_ex( char* buff )
    {
    auto res = fmt::format_to_n( 
        buff, MAX_COPY_SIZE, 
        "E={}, M_EXP={:.1f}, S_DEV={:.1f}, ",
        is_emulation() ? 1 : 0, get_emulator().get_m_expec(),
        get_emulator().get_st_deviation());
    return static_cast<int>( res.size );
    }
//-----------------------------------------------------------------------------
float analog_io_device::get_value()
    {
    if ( is_emulation() ) return get_emulator().get_value();
    
    return device::get_value();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int timer::save( char *buff )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int timer::load( char *buff )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int timer::get_saved_size() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
timer::timer()
    {
    }
//-----------------------------------------------------------------------------
void timer::start()
    {
    if ( STATE::S_STOP == state )
        {
        work_time = 0;
        }

    if ( STATE::S_PAUSE == state || STATE::S_STOP == state )
        {
        state = STATE::S_WORK;
        last_time = get_millisec();
        }
    }
//-----------------------------------------------------------------------------
void timer::reset()
    {
    state = STATE::S_STOP;
    work_time = 0;
    }
//-----------------------------------------------------------------------------
void timer::pause()
    {
    if ( STATE::S_WORK == state )
        {
        work_time += get_delta_millisec( last_time );
        }
    state = STATE::S_PAUSE;
    }
//-----------------------------------------------------------------------------
bool timer::is_time_up() const
    {
    if ( STATE::S_WORK == state )
        {
        u_int time = work_time + get_delta_millisec( last_time );
        if (  time <= countdown_time )
            {
            return 0;
            }
        else
            {
            return 1;
            }
        }
    return 0;
    }
//-----------------------------------------------------------------------------
u_long timer::get_work_time() const
    {
    if ( STATE::S_WORK == state )
        {
        return work_time + get_delta_millisec( last_time );
        }
    else
        {
        return work_time;
        }
    }
//-----------------------------------------------------------------------------
void timer::set_countdown_time( u_long new_countdown_time )
    {
    if ( G_DEBUG )
        {
        if ( 0 == new_countdown_time )
            {
            printf( "Error void timer::set_countdown_time( u_long time ), time = %lu!\n",
                new_countdown_time );
            }
        }

    countdown_time = new_countdown_time;
    }
//-----------------------------------------------------------------------------
u_long timer::get_countdown_time() const
    {
    return countdown_time;
    }
//-----------------------------------------------------------------------------
timer::STATE timer::get_state() const
    {
    return state;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
timer_manager::timer_manager( u_int timers_count ) : timers_cnt( timers_count )
    {
    if ( timers_cnt )
        {
        timers = new timer[ timers_cnt ];
        }
    }
//-----------------------------------------------------------------------------
timer_manager::~timer_manager()
    {
    if ( timers )
        {
        delete [] timers;
        timers     = nullptr;
        timers_cnt = 0;
        }
    }
//-----------------------------------------------------------------------------
timer* timer_manager::operator[]( unsigned int index )
    {
    index -= 1;

    if ( index < timers_cnt )
        {
        return &timers[ index ];
        }
    else
        {
        if ( G_DEBUG )
            {
            printf( "timer_manager[] - error: index[ %u ] > count [ %u ]\n",
                index, timers_cnt );
            }
        }

    return &stub;
    }
//-----------------------------------------------------------------------------
int timer_manager::get_count() const
    {
    return timers_cnt;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void virtual_device::direct_off()
    {
    state = 0;
    value = 0;
    }

void virtual_device::direct_set_value( float new_value )
    {
    value = new_value;
    }

float virtual_device::get_value()
    {
    return value;
    }

void virtual_device::direct_set_state( int new_state )
    {
    state = new_state;
    }

void virtual_device::direct_on()
    {
    state = 1;
    }

int virtual_device::get_state()
    {
    return state;
    }

bool virtual_device::is_active()
    {
    bool ret = ((state == 0) ? false : true);
    return (level_logic_invert ? !ret : ret);
    }

void virtual_device::set_rt_par(unsigned int idx, float value)
    {
    switch (idx)
        {
        case 1:
            level_logic_invert = value != 0;
            break;
        }
    }

virtual_device::virtual_device( const char *dev_name,
    device::DEVICE_TYPE dev_type,
    device::DEVICE_SUB_TYPE dev_sub_type ) : device (dev_name, dev_type, dev_sub_type, 0)
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
analog_output::analog_output( const char* dev_name ) :
    AO1( dev_name, DT_AO, DST_NONE, ADDITIONAL_PARAM_COUNT )
    {
    set_par_name( P_MIN_VALUE, 0, "P_MIN_V" );
    set_par_name( P_MAX_VALUE, 0, "P_MAX_V" );
    }
//-----------------------------------------------------------------------------
float analog_output::get_min_value() const
    {
    return get_par( P_MIN_VALUE, 0 );
    }
//-----------------------------------------------------------------------------
float analog_output::get_max_value() const
    {
    return get_par( P_MAX_VALUE, 0 );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
motor_altivar::motor_altivar( const char* dev_name,
    device::DEVICE_SUB_TYPE sub_type, u_int par_cnt ) :
    i_motor( dev_name, sub_type, par_cnt + ADDITIONAL_PARAM_COUNT ),
    io_device( dev_name )
    {
    set_par_name( P_ON_TIME, 0, "P_ON_TIME" );
    }

int motor_altivar::save_device_ex(char * buff)
    {
    int res = 0;
    if ( G_PAC_INFO()->is_emulator() )
        {
        res = fmt::format_to_n( buff, MAX_COPY_SIZE,
            "R={}, FRQ={:.1f}, RPM={}, EST={}, AMP={:.1f}, MAX_FRQ=0.0, ",
            reverse, freq, rpm, est, amperage ).size;
        }
    else
        {
        res = fmt::format_to_n( buff, MAX_COPY_SIZE,
            "R={}, FRQ={:.1f}, RPM={}, EST={}, AMP={:.1f}, MAX_FRQ={:.1f}, ",
            atv->reverse, atv->frq_value, atv->rpm_value, atv->remote_state,
            atv->amperage, atv->frq_max ).size;
        }

    return res;
    }

float motor_altivar::get_value()
    {
    if ( G_PAC_INFO()->is_emulator() ) return freq;

    return atv->get_output_in_percent();
    }

void motor_altivar::direct_set_value(float value)
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        freq = value;
        return;
        }

    atv->set_output_in_percent( value );
    }

void motor_altivar::direct_set_state(int new_state)
    {
    if ( G_PAC_INFO()->is_emulator() )
        return device::direct_set_state( new_state );

    if (new_state == 2)
        {
        atv->cmd = 2;
        atv->reverse = 1;
        return;
        }

    if (new_state == 100)
        {
        atv->Disable();
        return;
        }

    if (new_state == 101)
        {
        atv->Enable();
        return;
        }

    if (new_state)
        {
        direct_on();
        }
    else
        {
        direct_off();
        }
    }

int motor_altivar::get_state()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::get_state();

    return atv->state;
    }

void motor_altivar::direct_on()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::direct_on();

    atv->cmd = 1;
    atv->reverse = 0;
    }

void motor_altivar::direct_off()
    {
    if ( G_PAC_INFO()->is_emulator() ) return device::direct_off();

    if (atv->state < 0)
        {
            atv->cmd = 4; //2 bit - fault reset
        }
    else
        {
        atv->cmd = 0;
        }
    atv->reverse = 0;
    }

void motor_altivar::set_string_property(const char * field, const char * value)
    {
    if ( G_DEBUG )
        {
        printf( "Set string property %s value %s\n", field, value );
        }

    if (strcmp(field, "IP") == 0)
        {
        int port = 502;
        int timeout = 300;
        auto nodeip = std::string(value);
        nodeip.append(":");
        nodeip.append(std::to_string(port));
        nodeip.append(" ");
        nodeip.append(std::to_string(timeout));
        if (!atv)
            {
            atv = G_ALTIVAR_MANAGER()->get_node(nodeip.c_str());
            if (!atv)
                {
                G_ALTIVAR_MANAGER()->add_node(value, port, timeout, get_article() );
                atv = G_ALTIVAR_MANAGER()->get_node(nodeip.c_str());
                }
            }
        }
    }

void motor_altivar::print() const
    {
    device::print();
    }

int motor_altivar::get_params_count() const
    {
    return ADDITIONAL_PARAM_COUNT;
    }

float motor_altivar::get_amperage() const
    {
    if ( G_PAC_INFO()->is_emulator() ) return amperage;

    return atv->amperage;
    }

int motor_altivar::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        printf( "motor_altivar::set_cmd() - prop = %s, idx = %d, val = %f\n",
            prop, idx, val );

        if ( strcmp( prop, "R" ) == 0 )
            {
            reverse = static_cast<int>( val );
            return 0;
            }
        else if ( strcmp( prop, "FRQ" ) == 0 )
            {
            freq = static_cast<float>( val );
            return 0;
            }
        else if ( strcmp( prop, "RPM" ) == 0 )
            {
            rpm = static_cast<int>( val );
            return 0;
            }
        else if ( strcmp( prop, "EST" ) == 0 )
            {
            est = static_cast<int>( val );
            return 0;
            }
        else if ( strcmp( prop, "AMP" ) == 0 )
            {
            amperage = static_cast<float>( val );
            return 0;
            }
        }

    return device::set_cmd( prop, idx, val );    
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float motor_altivar_linear::get_linear_speed() const
    {
    float d = get_par( P_SHAFT_DIAMETER, start_param_idx );
    float n = get_par( P_TRANSFER_RATIO, start_param_idx );
    float v = .0f;

    if ( 0 != d && 0 != n )
        {
        if ( G_PAC_INFO()->is_emulator() )
            {
            v = ( get_rpm() * (float)M_PI * d ) / ( n * SEC_IN_MIN );
            }
        else
            {
            v = ( get_atv()->rpm_value * (float)M_PI * d ) / ( n * SEC_IN_MIN );
            }
        }

    return v;
    }

motor_altivar_linear::motor_altivar_linear( const char* dev_name ) :
    motor_altivar( dev_name, device::M_ATV_LINEAR, ADDITIONAL_PARAM_COUNT )
    {
    start_param_idx = motor_altivar::get_params_count();
    set_par_name( P_SHAFT_DIAMETER, start_param_idx, "P_SHAFT_DIAMETER" );
    set_par_name( P_TRANSFER_RATIO, start_param_idx, "P_TRANSFER_RATIO" );
    }

#ifdef WIN_OS
#pragma warning(pop)
#endif // WIN_OS
