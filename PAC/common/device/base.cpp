#include "base.h"
#include "PAC_info.h"

#include <fmt/core.h>

#ifdef WIN_OS
#pragma warning(push)
#pragma warning(disable: 26812) //Prefer 'enum class' over 'enum'.
#endif // WIN_OS

const std::array<const char*, device::DEVICE_TYPE::C_DEVICE_TYPE_CNT> device::DEV_NAMES = 
    {
    "V",       ///< Клапан.
    "VC",      ///< Управляемый клапан.
    "M",       ///< Двигатель.
    "LS",      ///< Уровень (есть/нет).
    "TE",      ///< Температура.
    "FS",      ///< Расход (есть/нет).
    "GS",      ///< Датчик положения.
    "FQT",     ///< Счетчик.
    "LT",      ///< Уровень (значение).
    "QT",      ///< Концентрация.

    "HA",      ///< Аварийная звуковая сигнализация.
    "HL",      ///< Аварийная световая сигнализация.
    "SB",      ///< Кнопка.
    "DI",      ///< Дискретный входной сигнал.
    "DO",      ///< Дискретный выходной сигнал.
    "AI",      ///< Аналоговый входной сигнал.
    "AO",      ///< Аналоговый выходной сигнал.
    "WT",      ///< Тензорезистор.
    "PT",      ///< Давление (значение).
    "F",       ///< Автоматический выключатель.

    "C",       ///< ПИД-регулятор.
    "HLA",     ///< Сигнальная колонна.
    "CAM",     ///< Камера.
    "PDS",     ///< Датчик разности давления.
    "TS",      ///< Сигнальный датчик температуры.
    "G",       ///< Блок питания.
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int par_device::save_device( char* str )
    {
    str[ 0 ] = 0;

    if ( par == nullptr )
        {
        return 0;
        }

    int size = 0;
    for ( u_int i = 0; i < par->get_count(); i++ )
        {
        if ( par_name[ i ] )
            {
            auto val = par[ 0 ][ i + 1 ];
            double tmp;
            int precision = modf( val, &tmp ) == 0 ? 0 : 2;
            size += fmt::format_to_n( str + size, MAX_COPY_SIZE, "{}={:.{}f}, ",
                par_name[ i ], val, precision ).size;
            }
        }
    return size;
    }
//-----------------------------------------------------------------------------
int par_device::set_par_by_name( const char* name, double val )
    {
    if ( par )
        {
        for ( u_int i = 0; i < par->get_count(); i++ )
            {
            if ( strcmp( par_name[ i ], name ) == 0 )
                {
                par->save( i + 1, (float)val );
                return 0;
                }
            }
        }

    if ( G_DEBUG )
        {
        printf( "par_device::set_cmd() - name = %s wasn't found.\n",
            name );
        }
    return 1;
    }
//-----------------------------------------------------------------------------
void par_device::set_par( u_int idx, u_int offset, float value )
    {
    if ( par )
        {
        par[ 0 ][ offset + idx ] = value;
        }
    }
//-----------------------------------------------------------------------------
par_device::par_device( u_int par_cnt )
    {
    if ( par_cnt )
        {
        par_name = new char* [ par_cnt ];
        for ( u_int i = 0; i < par_cnt; i++ )
            {
            par_name[ i ] = nullptr;
            }

        par = new saved_params_float( par_cnt );
        }
    }
//-----------------------------------------------------------------------------
par_device::~par_device()
    {
    if ( par )
        {
        for ( u_int i = 0; i < par->get_count(); i++ )
            {
            delete[] par_name[ i ];
            par_name[ i ] = nullptr;
            }

        delete[] par_name;
        par_name = nullptr;

        delete par;
        par = nullptr;
        }
    }
//-----------------------------------------------------------------------------
float par_device::get_par( u_int idx, u_int offset ) const
    {
    if ( par )
        {
        return par[ 0 ][ offset + idx ];
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void par_device::set_par_name( u_int idx, u_int offset, const char* name )
    {
    if ( par )
        {
        if ( offset + idx <= par->get_count() && ( offset + idx > 0 ) )
            {
            if ( strlen( name ) > C_MAX_PAR_NAME_LENGTH )
                {
                if ( G_DEBUG )
                    {
                    printf( "Error par_device::set_par_name( u_int idx, u_int offset, const char* name ) - "
                        "name length (%zu) > param C_MAX_PAR_NAME_LENGTH (%d).",
                        strlen( name ), C_MAX_PAR_NAME_LENGTH );
                    }

                return;
                }

            if ( nullptr == par_name[ offset + idx - 1 ] )
                {
                par_name[ offset + idx - 1 ] = new char[ strlen( name ) + 1 ];
                strcpy( par_name[ offset + idx - 1 ], name );
                }
            else
                {
                if ( G_DEBUG )
                    {
                    printf( "Error par_device::set_par_name (u_int idx, u_int offset, const char* name) - "
                        "param (%d %d) already has name (%s).",
                        offset, idx, par_name[ offset + idx - 1 ] );
                    }
                }
            }
        else
            {
            if ( G_DEBUG )
                {
                printf( "Error par_device::set_par_name (u_int idx, u_int offset, const char* name) - "
                    "offset (%d) + idx (%d) > param count ( %d ).",
                    offset, idx, par->get_count() );
                }
            }
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void device::set_descr( const char* new_description )
    {
    delete[] description;

    //Копирование с учетом нуль-символа.
    int len = strlen( new_description ) + 1;
    description = new char[ len ];
    strcpy( description, new_description );
    }
//-----------------------------------------------------------------------------
void device::set_article( const char* new_article )
    {
    delete[] article;

    //Копирование  с учетом нуль-символа.
    int len = strlen( new_article ) + 1;
    article = new char[ len ];
    strcpy( article, new_article );
    }
//-----------------------------------------------------------------------------
void device::print() const
    {
    printf( "%s\t", name );
    }
//-----------------------------------------------------------------------------
void device::direct_off()
    {
    state = 0;
    value = 0;
    }
//-----------------------------------------------------------------------------
void device::direct_on()
    {
    state = 1;
    }
//-----------------------------------------------------------------------------
void device::direct_set_state( int new_state )
    {
    state = new_state;
    }
//-----------------------------------------------------------------------------
void device::direct_set_value( float new_value )
    {
    value = new_value;
    }
//-----------------------------------------------------------------------------
void device::off()
    {
    if ( !get_manual_mode() )
        {
        direct_off();
        }
    }
//-----------------------------------------------------------------------------
int device::get_state()
    {
    return state;
    }
//-----------------------------------------------------------------------------
float device::get_value()
    {
    return value;
    }
//-----------------------------------------------------------------------------
void device::set_property( const char* field, device* dev )
    {
    // By default do nothing.
    }
//-----------------------------------------------------------------------------
void device::set_string_property( const char* field, const char* new_value )
    {
    if ( G_DEBUG )
        {
        G_LOG->debug( "%s\t device::set_string_property() - "
            "field = %s, val = \"%s\"",
            name, field, new_value );
        }
    }
//-----------------------------------------------------------------------------
int device::save_device( char* buff, const char* prefix )
    {
    int res = fmt::format_to_n( buff, MAX_COPY_SIZE, "{}{}={{M={:d}, ",
        prefix, name, is_manual_mode ).size;

    if ( type != DT_AO )
        {
        res += fmt::format_to_n( buff + res, MAX_COPY_SIZE, "ST={}, ",
            get_state() ).size;
        }

    if ( type != DT_V &&

        type != DT_FS &&
        type != DT_GS &&

        type != DT_HA &&
        type != DT_HL &&
        type != DT_SB &&
        !( type == DT_LS && ( sub_type == DST_LS_MAX || sub_type == DST_LS_MIN ) ) &&

        type != DT_DI &&
        type != DT_DO )
        {
        auto val = get_value();
        double tmp;
        int precision = modf( val, &tmp ) == 0 ? 0 : 2;
        res += fmt::format_to_n( buff + res, MAX_COPY_SIZE, "V={:.{}f}, ",
            val, precision ).size;
        }

    res += save_device_ex( buff + res );
    res += par_device::save_device( buff + res );

    const int extra_symbols_length = 2;                     //Remove last " ,".
    if ( res > extra_symbols_length ) res -= extra_symbols_length;
    res += fmt::format_to_n( buff + res, MAX_COPY_SIZE, "}},\n" ).size;
    return res;
    }
//-----------------------------------------------------------------------------
void device::evaluate_io()
    {
    //Do nothing by default.
    }
//-----------------------------------------------------------------------------
int device::set_cmd( const char* prop, u_int idx, char* val )
    {
    if ( G_DEBUG )
        {
        printf( "%s\t device::set_cmd() - prop = %s, idx = %d, val = %s\n",
            name, prop, idx, val );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int device::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_DEBUG )
        {
        sprintf( G_LOG->msg,
            "%s\t device::set_cmd() - prop = %s, idx = %d, val = %f",
            name, prop, idx, val );
        G_LOG->write_log( i_log::P_DEBUG );
        }

    switch ( prop[ 0 ] )
        {
        case 'S':
            direct_set_state( (int)val );
            break;

        case 'V':
            direct_set_value( (float)val );
            break;

        case 'M':
            if ( val == 0. )
                is_manual_mode = false;
            else if ( val == 1. )
                is_manual_mode = true;
            break;

        case 'P': //Параметры.
            return par_device::set_par_by_name( prop, val );

        default:
            if ( G_DEBUG )
                {
                printf( "Error device::set_cmd() - prop = %s, val = %f\n",
                    prop, val );
                }
            return 1;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
device::device( const char* dev_name, DEVICE_TYPE type, DEVICE_SUB_TYPE sub_type,
    u_int par_cnt ) : par_device( par_cnt ), type( type ),
    sub_type( sub_type )
    {
    if ( dev_name )
        {
        strcpy( this->name, dev_name );
        }
    else
        {
        strcpy( this->name, "?" );
        }

    description = new char[ 1 ];
    description[ 0 ] = 0;

    article = new char[ 2 ];
    article[ 0 ] = ' ';
    article[ 1 ] = 0;
    }
//-----------------------------------------------------------------------------
const char* device::get_type_str() const
    {
    if ( type >= 0 && type < C_DEVICE_TYPE_CNT ) return DEV_NAMES[ type ];

    return "NONE";
    }
//-----------------------------------------------------------------------------
const char* device::get_type_name() const
    {
    switch ( type )
        {
        case DT_V:
            return "Клапан";
        case DT_VC:
            return "Управляемый клапан";
        case DT_M:
            return "Двигатель";

        case DT_LS:
            return "Уровень";
        case DT_TE:
            return "Температура";
        case DT_FS:
            return "Расход";
        case DT_GS:
            return "Датчик положения";
        case DT_FQT:
            return "Счетчик";
        case DT_LT:
            return "Текущий уровень";
        case DT_QT:
            return "Концентрация";
        case DT_HA:
            return "Аварийная звуковая сигнализация";
        case DT_HL:
            return "Аварийная световая сигнализация";
        case DT_SB:
            return "Кнопка";
        case DT_DI:
            return "Дискретный входной сигнал";
        case DT_DO:
            return "Дискретный выходной сигнал";
        case DT_AI:
            return "Аналоговый входной сигнал";
        case DT_AO:
            return "Аналоговый выходной сигнал";
        case DT_WT:
            return "Тензорезистор";
        case DT_PT:
            return "Давление";
        case DT_F:
            return "Автоматический выключатель";
        case DT_REGULATOR:
            return "ПИД-регулятор";
        case DT_HLA:
            return "Сигнальная колонна";
        case DT_CAM:
            return "Камера";
        case DT_G:
            return "Блок питания";

        default:
            return "???";
        }
    }
//-----------------------------------------------------------------------------
void device::param_emulator( float math_expec, float stddev )
    {
    emulator.param( math_expec, stddev );
    }
//-----------------------------------------------------------------------------
analog_emulator& device::get_emulator()
    {
    return emulator;
    }
//-----------------------------------------------------------------------------
bool device::is_emulation() const
    {
    return emulation;
    }
//-----------------------------------------------------------------------------
void device::set_emulation( bool new_emulation_state )
    {
    emulation = new_emulation_state;
    }
//-----------------------------------------------------------------------------
device::~device()
    {
    delete[] description;
    description = nullptr;
    delete[] article;
    article = nullptr;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
i_motor::i_motor( const char* dev_name, device::DEVICE_SUB_TYPE sub_type,
    int params_count ) :
    device( dev_name, DT_M, sub_type, params_count )
    {
    }
//-----------------------------------------------------------------------------
void i_motor::reverse()
    {
    set_state( 2 );
    }
//-----------------------------------------------------------------------------
float i_motor::get_linear_speed() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
float i_motor::get_amperage() const
    {
    return 0.0f;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void digital_io_device::print() const
    {
    device::print();
    }
//-----------------------------------------------------------------------------
digital_io_device::digital_io_device( const char* dev_name, device::DEVICE_TYPE type,
    device::DEVICE_SUB_TYPE sub_type, u_int par_cnt ) :
    device( dev_name, type, sub_type, par_cnt ),
    io_device( dev_name )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AO1::AO1( const char* dev_name,
    device::DEVICE_TYPE type,
    device::DEVICE_SUB_TYPE sub_type,
    u_int par_cnt ) :
    analog_io_device( dev_name, type, sub_type, par_cnt )
    {
    }
//-----------------------------------------------------------------------------
float AO1::get_value()
    {
    if ( G_PAC_INFO()->is_emulator() ) return analog_io_device::get_value();

    return get_AO( AO_INDEX, get_min_value(), get_max_value() );
    }
//-----------------------------------------------------------------------------
void AO1::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() )
        return analog_io_device::direct_set_value( new_value );

    set_AO( AO_INDEX, new_value, get_min_value(), get_max_value() );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
virtual_counter::virtual_counter( const char* dev_name ) :
    device( dev_name, DT_FQT, DST_FQT_VIRT, 0 )
    {
    device::direct_set_state( static_cast<int>( STATES::S_WORK ) );
    }
//-----------------------------------------------------------------------------
int virtual_counter::get_state()
    {
    return device::get_state();
    }
//-----------------------------------------------------------------------------
void virtual_counter::direct_on()
    {
    start();
    }

void virtual_counter::direct_off()
    {
    reset();
    }
//-----------------------------------------------------------------------------
void virtual_counter::direct_set_state( int new_state )
    {
    switch ( static_cast<STATES>( new_state ) )
        {
        case STATES::S_WORK:
            start();
            break;

        case STATES::S_PAUSE:
            pause();
            break;

        default:
            device::direct_set_state( new_state );
            break;
        }
    }
//-----------------------------------------------------------------------------
void virtual_counter::pause()
    {
    device::direct_set_state( static_cast<int>( STATES::S_PAUSE ) );
    }
//-----------------------------------------------------------------------------
void virtual_counter::start()
    {
    device::direct_set_state( static_cast<int>( STATES::S_WORK ) );
    }
//-----------------------------------------------------------------------------
void virtual_counter::reset()
    {
    device::direct_set_value( 0.f );
    }
//-----------------------------------------------------------------------------
u_int virtual_counter::get_quantity()
    {
    return static_cast<u_int>( device::get_value() );
    }
//-----------------------------------------------------------------------------
float virtual_counter::get_flow()
    {
    return flow_value;
    }
//-----------------------------------------------------------------------------
/// @brief Получение абсолютного значения счетчика (без учета
/// состояния паузы).
u_int virtual_counter::get_abs_quantity()
    {
    return abs_value;
    }

/// @brief Сброс абсолютного значения счетчика.
void  virtual_counter::abs_reset()
    {
    abs_value = 0;
    }
//-----------------------------------------------------------------------------
int virtual_counter::set_cmd( const char* prop, u_int idx, double val )
    {
    switch ( prop[ 0 ] )
        {
        case 'A': //ABS_V
            abs_value = (u_int)val;
            break;

        case 'F':
            flow_value = (float)val;
            break;

        default:
            return device::set_cmd( prop, idx, val );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void virtual_counter::set( u_int new_value, u_int new_abs_value, float flow )
    {
    device::direct_set_value( static_cast<float>( new_value ) );
    abs_value = new_abs_value;
    flow_value = flow;
    }
//-----------------------------------------------------------------------------
void virtual_counter::eval( u_int read_value, u_int abs_read_value,
    float read_flow = 0 )
    {    
    if ( !is_first_read )
        {
        if ( read_value > last_read_value )
            {
            auto val = device::get_value() +
                static_cast<float>( read_value - last_read_value );
            device::direct_set_value( val );
            }

        if ( abs_read_value > abs_last_read_value )
            {
            abs_value += abs_read_value - abs_last_read_value;
            }
        }
    else
        {
        is_first_read = false;
        }    
    last_read_value = read_value;
    abs_last_read_value = abs_read_value;
    flow_value = read_flow;
    }
//-----------------------------------------------------------------------------
//Lua.
int virtual_counter::save_device_ex( char* buff )
    {
    int res = sprintf( buff, "ABS_V=%u, ", get_abs_quantity() );
    res += sprintf( buff + res, "F=%.2f, ", get_flow() );

    return res;
    }

u_long virtual_counter::get_pump_dt() const
    {
    return 0;
    }

float virtual_counter::get_min_flow() const
    {
    return .0f;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AI1::AI1( const char* dev_name, device::DEVICE_TYPE type,
    device::DEVICE_SUB_TYPE sub_type, u_int par_cnt ) :
    analog_io_device( dev_name, type, sub_type,
    par_cnt + ADDITIONAL_PARAM_COUNT )
    {
    device::set_state( 1 );
    set_par_name( P_ZERO_ADJUST_COEFF, 0, "P_CZ" );
    }
//-----------------------------------------------------------------------------
int AI1::get_state()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return device::get_state();
        }

    if ( get_AI( C_AI_INDEX, 0, 0 ) == -1. )
        {
        return -2;
        }
    if ( get_AI( C_AI_INDEX, 0, 0 ) == -2. )
        {
        return -3;
        }
    return 1;
    }
//-----------------------------------------------------------------------------
int AI1::get_params_count() const
    {
    return ADDITIONAL_PARAM_COUNT;
    }
//-----------------------------------------------------------------------------
float AI1::get_value()
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return analog_io_device::get_value();
        }

    return get_par( P_ZERO_ADJUST_COEFF, 0 ) +
        get_AI( C_AI_INDEX, get_min_val(), get_max_val() );
    }
//-----------------------------------------------------------------------------
void AI1::direct_set_value( float new_value )
    {
    if ( G_PAC_INFO()->is_emulator() )
        {
        return device::direct_set_value( new_value );
        }
    }
//-----------------------------------------------------------------------------
/// @brief Получение максимального значения выхода устройства.
float AI1::get_max_val()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
/// @brief Получение минимального значения выхода устройства.
float AI1::get_min_val()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
level::level( const char* dev_name, device::DEVICE_SUB_TYPE sub_type,
    u_int par_cnt ) :AI1(
    dev_name, DT_LT, sub_type, par_cnt + LAST_PARAM_IDX - 1 )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( P_ERR, start_param_idx, "P_ERR" );
    }
//-----------------------------------------------------------------------------
int level::get_volume()
    {
    if ( get_state() < 0 )
        {
        return (int)get_par( P_ERR, start_param_idx );
        }

    return calc_volume();
    }
//-----------------------------------------------------------------------------
int level::calc_volume()
    {
    return (int)get_value();
    }
//-----------------------------------------------------------------------------
int level::save_device_ex( char* buff )
    {
    int res = sprintf( buff, "CLEVEL=%d, ", get_volume() );

    return res;
    }
//-----------------------------------------------------------------------------
float level::get_max_val()
    {
    return 100;
    }
//-----------------------------------------------------------------------------
float level::get_min_val()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int level::get_params_count() const
    {
    return start_param_idx + LAST_PARAM_IDX - 1;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
signal_column::signal_column( const char* dev_name, DEVICE_SUB_TYPE sub_type,
    int red_lamp_channel, int yellow_lamp_channel,
    int green_lamp_channel, int blue_lamp_channel, int siren_channel ) :
    device( dev_name, DT_HLA, sub_type, 0 ),
    io_device( dev_name ),
    show_state( show_states::idle ),
    is_const_red( 0 ),
    red_lamp_channel( red_lamp_channel ),
    yellow_lamp_channel( yellow_lamp_channel ),
    green_lamp_channel( green_lamp_channel ),
    blue_lamp_channel( blue_lamp_channel ),
    siren_channel( siren_channel ),
    siren_step( STEP::off )
    {
    }
//-----------------------------------------------------------------------------
void signal_column::direct_off()
    {
    if ( red_lamp_channel ) turn_off_red();
    if ( yellow_lamp_channel ) turn_off_yellow();
    if ( green_lamp_channel ) turn_off_green();
    if ( blue_lamp_channel ) turn_off_blue();

    if ( siren_channel ) turn_off_siren();
    }
//-----------------------------------------------------------------------------
void signal_column::direct_on()
    {
    if ( red_lamp_channel ) turn_off_red();
    if ( yellow_lamp_channel ) turn_off_yellow();
    if ( green_lamp_channel ) turn_off_green();
    if ( blue_lamp_channel ) turn_off_blue();

    if ( siren_channel ) turn_off_siren();

    turn_on_green();
    }
//-----------------------------------------------------------------------------
void signal_column::turn_off_red()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( red_lamp_channel, DO_state::OFF, RED_LAMP );

    red.step = STEP::off;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_off_yellow()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( yellow_lamp_channel, DO_state::OFF, YELLOW_LAMP );

    yellow.step = STEP::off;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_off_green()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( green_lamp_channel, DO_state::OFF, GREEN_LAMP );

    green.step = STEP::off;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_off_blue()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( blue_lamp_channel, DO_state::OFF, BLUE_LAMP );

    blue.step = STEP::off;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_on_red()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( red_lamp_channel, DO_state::ON, RED_LAMP );

    red.step = STEP::on;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_on_yellow()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( yellow_lamp_channel, DO_state::ON, YELLOW_LAMP );

    yellow.step = STEP::on;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_on_green()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( green_lamp_channel, DO_state::ON, GREEN_LAMP );

    green.step = STEP::on;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_on_blue()
    {
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( blue_lamp_channel, DO_state::ON, BLUE_LAMP );

    blue.step = STEP::on;
    }
//-----------------------------------------------------------------------------
void signal_column::normal_blink_red()
    {
    if ( is_const_red )
        {
        blink( red_lamp_channel, red, (u_long)CONSTANTS::NORMAL_BLINK_TIME );
        }
    else
        {
        if ( !G_PAC_INFO()->is_emulator() )
            process_DO( red_lamp_channel, DO_state::ON, RED_LAMP );

        red.step = STEP::on;
        }
    }
//-----------------------------------------------------------------------------
void signal_column::normal_blink_yellow()
    {
    blink( yellow_lamp_channel, yellow, (u_long)CONSTANTS::NORMAL_BLINK_TIME );
    }
//-----------------------------------------------------------------------------
void signal_column::normal_blink_green()
    {
    blink( green_lamp_channel, green, (u_long)CONSTANTS::NORMAL_BLINK_TIME );
    }
//-----------------------------------------------------------------------------
void signal_column::normal_blink_blue()
    {
    blink( blue_lamp_channel, blue, (u_long)CONSTANTS::NORMAL_BLINK_TIME );
    }
//-----------------------------------------------------------------------------
void signal_column::slow_blink_red()
    {
    if ( is_const_red )
        {
        blink( red_lamp_channel, red, (u_long)CONSTANTS::SLOW_BLINK_TIME );
        }
    else
        {
        if ( !G_PAC_INFO()->is_emulator() )
            process_DO( red_lamp_channel, DO_state::ON, RED_LAMP );

        red.step = STEP::on;
        }
    }
//-----------------------------------------------------------------------------
void signal_column::slow_blink_yellow()
    {
    blink( yellow_lamp_channel, yellow, (u_long)CONSTANTS::SLOW_BLINK_TIME );
    }
//-----------------------------------------------------------------------------
void signal_column::slow_blink_green()
    {
    blink( green_lamp_channel, green, (u_long)CONSTANTS::SLOW_BLINK_TIME );
    }
//-----------------------------------------------------------------------------
void signal_column::slow_blink_blue()
    {
    blink( blue_lamp_channel, blue, (u_long)CONSTANTS::SLOW_BLINK_TIME );
    }
//-----------------------------------------------------------------------------
void signal_column::turn_on_siren()
    {
    siren_step = STEP::blink_on;
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( siren_channel, DO_state::ON, SIREN );
    }
//-----------------------------------------------------------------------------
void signal_column::turn_off_siren()
    {
    siren_step = STEP::off;
    if ( !G_PAC_INFO()->is_emulator() )
        process_DO( siren_channel, DO_state::OFF, SIREN );
    }
//-----------------------------------------------------------------------------
void signal_column::set_rt_par( u_int idx, float value )
    {
    switch ( idx )
        {
        case 1:
            is_const_red = (u_int)value;
            break;

        default:
            set_rt_par( idx, value );
            break;
        }
    }
//-----------------------------------------------------------------------------
void signal_column::direct_set_value( float new_value )
    {
    }
//-----------------------------------------------------------------------------
int signal_column::get_state()
    {
    int res = green.step != STEP::off || yellow.step == STEP::off ||
        red.step == STEP::off || siren_step != STEP::off;

    return res;
    }
//-----------------------------------------------------------------------------
float signal_column::get_value()
    {
    return .0f;
    }
//-----------------------------------------------------------------------------
int signal_column::save_device_ex( char* buff )
    {
    int res = sprintf( buff, "L_GREEN=%d, ",
        green.step == STEP::on || green.step == STEP::blink_on ? 1 : 0 );
    res += sprintf( buff + res, "L_YELLOW=%d, ",
        yellow.step == STEP::on || yellow.step == STEP::blink_on ? 1 : 0 );
    res += sprintf( buff + res, "L_RED=%d, ",
        red.step == STEP::on || red.step == STEP::blink_on ? 1 : 0 );
    res += sprintf( buff + res, "L_SIREN=%d, ",
        siren_step == STEP::on ? 1 : 0 );

    return res;
    }
//-----------------------------------------------------------------------------
void signal_column::evaluate_io()
    {
    //Так как колонну могут использовать несколько аппаратов
    //(агрегатов), то отключаем отображение событий в начале каждого
    // цикла управляющей программы.
    show_state = show_states::idle;
    }
//-----------------------------------------------------------------------------
void signal_column::show_error_exists()
    {
    if ( get_manual_mode() ) return;

    show_state = show_states::error_exists;
    normal_blink_red();
    turn_off_yellow();
    turn_off_green();
    turn_on_siren();
    };
//-----------------------------------------------------------------------------
void signal_column::show_message_exists()
    {
    if ( get_manual_mode() ) return;

    if ( show_state != show_states::error_exists )
        {
        show_state = show_states::message_exists;
        turn_off_red();
        normal_blink_yellow();
        turn_off_green();
        turn_off_siren();
        }
    };
//-----------------------------------------------------------------------------
void signal_column::show_batch_is_not_running()
    {
    if ( get_manual_mode() ) return;

    if ( show_state != show_states::error_exists &&
        show_state != show_states::message_exists )
        {
        show_state = show_states::batch_is_not_running;
        turn_off_red();
        turn_on_yellow();
        turn_off_green();
        turn_off_siren();
        }
    };
//-----------------------------------------------------------------------------
void signal_column::show_batch_is_running()
    {
    if ( get_manual_mode() ) return;

    if ( show_state != show_states::error_exists &&
        show_state != show_states::message_exists &&
        show_state != show_states::batch_is_not_running )
        {
        show_state = show_states::batch_is_running;
        turn_off_red();
        turn_off_yellow();
        turn_on_green();
        turn_off_siren();
        }
    };
//-----------------------------------------------------------------------------
void signal_column::show_operation_is_not_running()
    {
    if ( get_manual_mode() ) return;

    if ( show_state != show_states::error_exists &&
        show_state != show_states::message_exists &&
        show_state != show_states::batch_is_not_running )
        {
        show_state = show_states::operation_is_not_running;
        turn_off_red();
        turn_on_yellow();
        turn_off_green();
        turn_off_siren();
        }
    };
//-----------------------------------------------------------------------------
void signal_column::show_operation_is_running()
    {
    if ( get_manual_mode() ) return;

    if ( show_state != show_states::error_exists &&
        show_state != show_states::message_exists &&
        show_state != show_states::batch_is_not_running &&
        show_state != show_states::operation_is_not_running )
        {
        show_state = show_states::operation_is_running;
        turn_off_red();
        turn_off_yellow();
        turn_on_green();
        turn_off_siren();
        }
    };
//-----------------------------------------------------------------------------
void signal_column::show_idle()
    {
    if ( get_manual_mode() ) return;

    if ( show_state != show_states::error_exists &&
        show_state != show_states::message_exists &&
        show_state != show_states::batch_is_not_running &&
        show_state != show_states::operation_is_not_running &&
        show_state != show_states::batch_is_running &&
        show_state != show_states::operation_is_running )
        {
        show_state = show_states::idle;
        turn_off_red();
        turn_off_yellow();
        turn_off_green();
        turn_off_siren();
        }
    };
//-----------------------------------------------------------------------------
void signal_column::direct_set_state( int new_state )
    {
    switch ( (STATE)new_state )
        {
        case STATE::TURN_OFF:
            direct_off();
            break;

        case STATE::TURN_ON:
            direct_on();
            break;

        case STATE::LIGHTS_OFF:
            turn_off_red();
            turn_off_yellow();
            turn_off_green();
            break;

        case STATE::GREEN_ON:
            turn_on_green();
            break;

        case STATE::YELLOW_ON:
            turn_on_yellow();
            break;

        case STATE::RED_ON:
            turn_on_red();
            break;

        case STATE::GREEN_NORMAL_BLINK:
            normal_blink_green();
            break;

        case STATE::GREEN_OFF:
            turn_off_green();
            break;

        case STATE::YELLOW_OFF:
            turn_off_yellow();
            break;

        case STATE::RED_OFF:
            turn_off_red();
            break;

        case STATE::YELLOW_NORMAL_BLINK:
            normal_blink_yellow();
            break;

        case STATE::RED_NORMAL_BLINK:
            normal_blink_red();
            break;

        case STATE::GREEN_SLOW_BLINK:
            slow_blink_green();
            break;

        case STATE::YELLOW_SLOW_BLINK:
            slow_blink_yellow();
            break;

        case STATE::RED_SLOW_BLINK:
            slow_blink_red();
            break;

        case STATE::SIREN_ON:
            turn_on_siren();
            break;

        case STATE::SIREN_OFF:
            turn_off_siren();
            break;

        default:
            break;
        }
    }
//-----------------------------------------------------------------------------
signal_column::state_info::state_info()
    {
    step = STEP::off;
    start_blink_time = 0;
    start_wait_time = 0;
    }
//-----------------------------------------------------------------------------
void signal_column::blink( int lamp_DO, state_info& info, u_int delay_time )
    {
    switch ( info.step )
        {
        case STEP::off:
        case STEP::on:
            info.start_blink_time = get_millisec();
            info.step = STEP::blink_on;
            break;

        case STEP::blink_on:
            process_DO( lamp_DO, DO_state::ON, "?" );
            if ( get_delta_millisec( info.start_blink_time ) > delay_time )
                {
                info.start_wait_time = get_millisec();
                info.step = STEP::blink_off;
                }
            break;

        case STEP::blink_off:
            process_DO( lamp_DO, DO_state::OFF, "?" );
            if ( get_delta_millisec( info.start_wait_time ) > delay_time )
                {
                info.start_blink_time = get_millisec();
                info.step = STEP::blink_on;
                }
            break;
        }
    };
