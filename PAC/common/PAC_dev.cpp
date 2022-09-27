#include "PAC_dev.h"
#include "tech_def.h"

#include "g_errors.h"
#include "lua_manager.h"
#include "log.h"
#include "PID.h"

#ifdef WIN_OS
#pragma warning(push)
#pragma warning(disable: 26812) //Prefer 'enum class' over 'enum'.
#endif // WIN_OS

auto_smart_ptr < device_manager > device_manager::instance;

std::vector<valve*> valve::to_switch_off;
std::vector<valve_DO2_DI2_bistable*> valve::v_bistable;

std::vector<valve_bottom_mix_proof*> valve_bottom_mix_proof::to_switch_off;

valve_iolink_mix_proof::out_data_swapped valve_iolink_mix_proof::stub_out_info;
valve_iolink_shut_off_thinktop::out_data_swapped valve_iolink_shut_off_thinktop::stub_out_info;
circuit_breaker::F_data_out circuit_breaker::stub_out_info;
analog_valve_iolink::out_data analog_valve_iolink::stub_out_info;
signal_column_iolink::out_data signal_column_iolink::stub_out_info;

const char* const device::DEV_NAMES[ device::DEVICE_TYPE::C_DEVICE_TYPE_CNT ] =
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
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int par_device::save_device ( char *str )
    {
    str[ 0 ] = 0;

    if ( par == 0 )
        {
        return 0;
        }

    int res = 0;
    for ( u_int i = 0; i < par->get_count(); i++ )
        {
        if ( par_name[ i ] )
            {
            res += sprintf( str + res, "%s=", par_name[ i ] );

            float val =  par[ 0 ][ i + 1 ];
            if ( 0. == val )
                {
                res += sprintf( str + res, "0, " );
                }
            else
                {
                double tmp;
                if ( modf( val, &tmp ) == 0 )
                    {
                    res += sprintf( str + res, "%d, ", ( int ) val );
                    }
                else
                    {
                    res += sprintf( str + res, "%.2f, ", val );
                    }
                }
            }
        }

    return res;
    }
//-----------------------------------------------------------------------------
int par_device::set_cmd( const char *name, double val )
    {
    if ( par )
        {
        for ( u_int i = 0; i < par->get_count(); i++ )
            {
            if ( strcmp( par_name[ i ], name ) == 0 )
                {
                par->save( i + 1, ( float ) val );
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
par_device::par_device ( u_int par_cnt ) : err_par( 0 ), par ( 0 ),
    par_name ( 0 )
    {
    if ( par_cnt )
        {
        par_name = new char*[ par_cnt ];
        for ( u_int i = 0; i < par_cnt; i++ )
            {
            par_name[ i ] = 0;
            }

        par = new saved_params_float ( par_cnt );
        }
    }
//-----------------------------------------------------------------------------
par_device::~par_device()
    {
    if ( par )
        {
        for ( u_int i = 0; i < par->get_count(); i++ )
            {
            delete [] par_name[ i ];
            par_name[ i ] = nullptr;
            }

        delete [] par_name;
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

            if ( 0 == par_name[ offset + idx - 1 ] )
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
void device::set_descr( const char *new_description )
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
void device::off()
    {
    if ( !get_manual_mode() )
        {
        direct_off();
        }
    }
//-----------------------------------------------------------------------------
int device::save_device( char* buff, const char* prefix )
    {
    int res = sprintf( buff, "%s%s={M=%d, ", prefix, name, is_manual_mode );

    if ( type != DT_AO )
        {
        res += sprintf( buff + res, "ST=%d, ", get_state() );
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
        if ( get_value() == 0 )
            {
            res += sprintf( buff + res, "V=0, " );
            }
        else
            {
            res += sprintf( buff + res, "V=%.2f, ", get_value() );
            }
        }

    res += save_device_ex( buff + res );
    res += par_device::save_device( buff + res );

    const int extra_symbols_length = 2;                     //Remove last " ,".
    if ( res > extra_symbols_length ) res -= extra_symbols_length;
    res += sprintf( buff + res, "},\n" );

    return res;
    }
//-----------------------------------------------------------------------------
int device::set_cmd( const char *prop, u_int idx, char *val )
    {
    if ( G_DEBUG )
        {
        printf( "%s\t device::set_cmd() - prop = %s, idx = %d, val = %s\n",
            name, prop, idx, val );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int device::set_cmd( const char *prop, u_int idx, double val )
    {
    if (G_DEBUG)
        {
        sprintf( G_LOG->msg,
                "%s\t device::set_cmd() - prop = %s, idx = %d, val = %f",
                name, prop, idx, val);
        G_LOG->write_log(i_log::P_DEBUG);
        }

    switch ( prop[ 0 ] )
        {
        case 'S':
            direct_set_state( ( int ) val );
            break;

        case 'V':
            direct_set_value( ( float ) val );
            break;

        case 'M':
            is_manual_mode = val == 0. ? false : true;
            break;

        case 'P': //Параметры.
            par_device::set_cmd( prop, val );
            break;

        default:
            if ( G_DEBUG )
                {
                printf( "Error device::set_cmd() - prop = %s, val = %f\n",
                    prop, val );
                }
        }
    return 0;
    }
//-----------------------------------------------------------------------------
device::device( const char* dev_name, DEVICE_TYPE type, DEVICE_SUB_TYPE sub_type,
    u_int par_cnt ) : par_device( par_cnt ), s_number( 0 ), type( type ),
    sub_type( sub_type ), is_manual_mode( false ), article( 0 )
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
    return DEV_NAMES[ type ];
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
            return "Уровень";
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

        default:
            return "???";
        }
    }
//-----------------------------------------------------------------------------
device::~device()
    {
    delete [] description;
    description = nullptr;
    delete [] article;
    article = nullptr;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES

int DO1::get_state()
    {
    return get_DO( DO_INDEX );
    }
//-----------------------------------------------------------------------------
void DO1::direct_on()
    {
    set_DO( DO_INDEX, 1 );
    }
//-----------------------------------------------------------------------------
void DO1::direct_off()
    {
    set_DO( DO_INDEX, 0 );
    }

#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
signal_column::signal_column( const char* dev_name, DEVICE_SUB_TYPE sub_type,
    int red_lamp_channel, int yellow_lamp_channel,
    int green_lamp_channel, int blue_lamp_channel, int siren_channel ):
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
#ifndef DEBUG_NO_IO_MODULES
    process_DO( red_lamp_channel, DO_state::OFF, RED_LAMP );
#endif // DEBUG_NO_IO_MODULES
    red.step = STEP::off;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_off_yellow()
    {
#ifndef DEBUG_NO_IO_MODULES
    process_DO( yellow_lamp_channel, DO_state::OFF, YELLOW_LAMP );
#endif // DEBUG_NO_IO_MODULES
    yellow.step = STEP::off;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_off_green()
    {
#ifndef DEBUG_NO_IO_MODULES
    process_DO( green_lamp_channel, DO_state::OFF, GREEN_LAMP );
#endif // DEBUG_NO_IO_MODULES
    green.step = STEP::off;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_off_blue()
    {
#ifndef DEBUG_NO_IO_MODULES
    process_DO( blue_lamp_channel, DO_state::OFF, BLUE_LAMP );
#endif // DEBUG_NO_IO_MODULES
    blue.step = STEP::off;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_on_red()
    {
#ifndef DEBUG_NO_IO_MODULES
    process_DO( red_lamp_channel, DO_state::ON, RED_LAMP );
#endif // DEBUG_NO_IO_MODULES
    red.step = STEP::on;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_on_yellow()
    {
#ifndef DEBUG_NO_IO_MODULES
    process_DO( yellow_lamp_channel, DO_state::ON, YELLOW_LAMP );
#endif // DEBUG_NO_IO_MODULES
    yellow.step = STEP::on;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_on_green()
    {
#ifndef DEBUG_NO_IO_MODULES
    process_DO( green_lamp_channel, DO_state::ON, GREEN_LAMP );
#endif // DEBUG_NO_IO_MODULES
    green.step = STEP::on;
    }
//-----------------------------------------------------------------------------
void signal_column::turn_on_blue()
    {
#ifndef DEBUG_NO_IO_MODULES
    process_DO( blue_lamp_channel, DO_state::ON, BLUE_LAMP );
#endif // DEBUG_NO_IO_MODULES
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
#ifndef DEBUG_NO_IO_MODULES
        process_DO( red_lamp_channel, DO_state::ON, RED_LAMP );
#endif // DEBUG_NO_IO_MODULES
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
#ifndef DEBUG_NO_IO_MODULES
        process_DO( red_lamp_channel, DO_state::ON, RED_LAMP );
#endif // DEBUG_NO_IO_MODULES
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
#ifndef DEBUG_NO_IO_MODULES
    process_DO( siren_channel, DO_state::ON, SIREN );
#endif // DEBUG_NO_IO_MODULES
    }
//-----------------------------------------------------------------------------
void signal_column::turn_off_siren()
    {
    siren_step = STEP::off;
#ifndef DEBUG_NO_IO_MODULES
    process_DO( siren_channel, DO_state::OFF, SIREN );
#endif // DEBUG_NO_IO_MODULES
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
#ifndef DEBUG_NO_IO_MODULES
    set_DO( n - 1, static_cast<char>( state ) );
#endif
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
signal_column_iolink::signal_column_iolink( const char* dev_name ) :
    signal_column( dev_name, DST_HLA_IOLINK ),
    out_info( 0 )
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
#ifndef DEBUG_NO_IO_MODULES
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
#endif // DEBUG_NO_IO_MODULES
    }
//-----------------------------------------------------------------------------
void signal_column_iolink::evaluate_io()
    {
#ifndef DEBUG_NO_IO_MODULES
    signal_column::evaluate_io();
    out_info = (out_data*)get_AO_write_data( 0 );
#endif // DEBUG_NO_IO_MODULES
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
camera::camera( const char* dev_name, DEVICE_SUB_TYPE sub_type, int params_count ) :
    device( dev_name, DT_CAM, sub_type, params_count ),
    io_device( dev_name ),
    is_cam_ready( true ),
    result( 0 ),
    state( 0 )
    {
    }

void camera::direct_set_state( int new_state )
    {
    if ( new_state ) direct_on(); else direct_off();
    }

void camera::direct_off()
    {
#ifndef DEBUG_NO_IO_MODULES
    set_DO( static_cast<u_int>( CONSTANTS::INDEX_DO ), 0 );
#endif
    state = 0;
    }

void camera::direct_on()
    {
#ifndef DEBUG_NO_IO_MODULES
    set_DO( static_cast<u_int>( CONSTANTS::INDEX_DO ), 1 );
#endif
    state = 1;
    }

void camera::direct_set_value( float new_value )
    {
    }

int camera::get_state()
    {
    return state;
    }

float camera::get_value()
    {
    return static_cast<float>( state );
    }

int camera::get_result( int n )
    {
#ifndef DEBUG_NO_IO_MODULES
    result = get_DI( static_cast<u_int>( CONSTANTS::INDEX_DI_RES_1 ) );
#endif
    return result;
    }

int camera::save_device_ex( char* buff )
    {
    int res = sprintf( buff, "RESULT=%d, READY=%d, ",
        get_result(), is_cam_ready );
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
    camera( dev_name, sub_type, static_cast<int>( PARAMS::PARAMS_CNT ) - 1 ),
    start_switch_time( get_millisec() )
    {
    is_cam_ready = false;
    set_par_name( static_cast<u_int>( PARAMS::P_READY_TIME ), 0, "P_READY_TIME" );
    }

int camera_DI2::get_state()
    {
    return state;
    }

void camera_DI2::evaluate_io()
    {
#ifndef DEBUG_NO_IO_MODULES
    int o = get_DO( static_cast<u_int>( CONSTANTS::INDEX_DO ) );
    int i = get_DI( static_cast<u_int>( CONSTANTS::INDEX_DI_READY ) );

    if ( 1 == i )
        {
        start_switch_time = get_millisec();
        state = o;
        }
    else if ( get_delta_millisec( start_switch_time ) <
        get_par( static_cast<u_int>( PARAMS::P_READY_TIME ), 0 ) )
        {
        state = o;
        }
    else
        {
        state = -1;
        }

    is_cam_ready = i > 0;
#endif
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
camera_DI3::camera_DI3( const char* dev_name ) :
    camera_DI2( dev_name, DEVICE_SUB_TYPE::DST_CAM_DO1_DI3 ),
    result_2( 0 )
    {
    }

void camera_DI3::evaluate_io()
    {
    camera_DI2::evaluate_io();
#ifndef DEBUG_NO_IO_MODULES
    result_2 = get_DI( static_cast<u_int>( CONSTANTS::INDEX_DI_RES_2 ) );
#endif
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
i_DO_device* device_manager::get_V( const char *dev_name )
    {
    return get_device( device::DT_V, dev_name );
    }
//-----------------------------------------------------------------------------
device_manager* device_manager::get_instance()
    {
    if ( instance.is_null() )
        {
        instance = new device_manager();
        }
    return instance;
    }
//-----------------------------------------------------------------------------
device* device_manager::get_device( int dev_type,
                                   const char *dev_name )
    {
    int dev_n = get_device_n( ( device::DEVICE_TYPE ) dev_type, dev_name );

    if ( dev_n >= 0 )
        {
        try
            {
            return project_devices[ dev_n ];
            }
        catch (...)
            {
            return get_stub_device();
            }
        }
    else
        {
        if ( dev_type < device::C_DEVICE_TYPE_CNT )
            {
            sprintf( G_LOG->msg, "%3s ", device::DEV_NAMES[ dev_type ] );
            }
        else
            {
            sprintf( G_LOG->msg, "unknown " );
            }
        sprintf( G_LOG->msg + strlen( G_LOG->msg ), "\"%s\" not found!",
            dev_name );

        G_LOG->write_log( i_log::P_ERR );
        }

    return get_stub_device();
    }
//-----------------------------------------------------------------------------
device* device_manager::get_device( const char* dev_name )
    {
    int dev_n = get_device_n( dev_name );

    if ( dev_n >= 0 )
        {
        try
            {
            return project_devices[ dev_n ];
            }
        catch ( ... )
            {
            return get_stub_device();
            }
        }
    else
        {
        sprintf( G_LOG->msg, "Device \"%s\" not found!", dev_name );
        G_LOG->write_log( i_log::P_ERR );
        }

    return get_stub_device();
    }
//-----------------------------------------------------------------------------
void device_manager::print() const
    {
    printf( "Device manager [%zu]:\n", project_devices.size() );
    for ( u_int i = 0; i < project_devices.size(); i++ )
        {
        printf( "    %3i. ", i + 1 );
        printf( "%-20s %s",
            project_devices[ i ]->get_name(), project_devices[ i ]->get_description() );
        printf( "\n" );
        }
    }
//-----------------------------------------------------------------------------
device_manager::device_manager(): project_devices( 0 )
    {
    G_DEVICE_CMMCTR->add_device( this );
    }
//-----------------------------------------------------------------------------
device_manager::~device_manager()
    {
    for ( u_int i = 0; i < project_devices.size(); i++ )
        {
        delete project_devices[ i ];
        }

#ifndef __BORLANDC__
    project_devices.clear();
#endif
    }
//-----------------------------------------------------------------------------
i_AO_device* device_manager::get_VC( const char *dev_name )
    {
    return get_device( device::DT_VC, dev_name );
    }
//-----------------------------------------------------------------------------
i_motor* device_manager::get_M( const char *dev_name )
    {
    return (i_motor*) get_device( device::DT_M, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_LS( const char *dev_name )
    {
    return get_device( device::DT_LS, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_FS( const char *dev_name )
    {
    return get_device( device::DT_FS, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_GS( const char *dev_name )
    {
    return get_device( device::DT_GS, dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_AI( const char *dev_name )
    {
    return get_device( device::DT_AI, dev_name );
    }
//-----------------------------------------------------------------------------
i_AO_device* device_manager::get_AO( const char *dev_name )
    {
    return get_device( device::DT_AO, dev_name );
    }
//-----------------------------------------------------------------------------
i_counter* device_manager::get_FQT( const char *dev_name )
    {
    int res = get_device_n( device::DT_FQT, dev_name );
    if ( res > -1 )
        {
        device *res_ctr = project_devices.at( res );
        switch ( res_ctr->get_sub_type() )
            {
            case device::DST_FQT_F:
                return ( counter_f* )res_ctr;

            case device::DST_FQT_F_OK:
                return ( counter_f_ok* )res_ctr;

            case device::DST_FQT:
                return ( counter* )res_ctr;

            case device::DST_FQT_VIRT:
                return ( virtual_counter* ) res_ctr;

            case device::DST_FQT_IOLINK:
                return ( counter_iolink* ) res_ctr;

            default:
                break;
            }
        }

     sprintf( G_LOG->msg, "FQT \"%s\" not found!", dev_name );
     G_LOG->write_log( i_log::P_ERR );

    return &stub;
    }
//-----------------------------------------------------------------------------
virtual_counter* device_manager::get_virtual_FQT( const char *dev_name )
    {
    int res = get_device_n( device::DT_FQT, dev_name );
    if ( res > -1 )
        {
        device *res_ctr = project_devices.at( res );
        switch ( res_ctr->get_sub_type() )
            {
            case device::DST_FQT_VIRT:
                return ( virtual_counter* ) res_ctr;

            default:
                break;
            }
        }

    sprintf( G_LOG->msg, "FQT \"%s\" not found!", dev_name );
    G_LOG->write_log( i_log::P_ERR );

    static virtual_counter stub_fqt( "stub" );
    return &stub_fqt;
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_TE( const char *dev_name )
    {
    return get_device( device::DT_TE, dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_LT( const char *dev_name )
    {
    return get_device( device::DT_LT, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_DI( const char *dev_name )
    {
    return get_device( device::DT_DI, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_SB( const char *dev_name )
    {
    return get_device( device::DT_SB, dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_DO( const char *dev_name )
    {
    return get_device( device::DT_DO, dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_HA( const char *dev_name )
    {
    return get_device( device::DT_HA, dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_HL( const char *dev_name )
    {
    return get_device( device::DT_HL, dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_PT( const char *dev_name )
    {
    return get_device( device::DT_PT, dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_QT( const char *dev_name )
    {
    return get_device( device::DT_QT, dev_name );
    }
//-----------------------------------------------------------------------------
wages* device_manager::get_WT( const char *dev_name )
    {
    return (wages*)get_device( device::DT_WT, dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_AO_device* device_manager::get_F(const char* dev_name)
    {
    return (i_DO_AO_device*)get_device(device::DT_F, dev_name);
    }
//-----------------------------------------------------------------------------
PID* device_manager::get_C( const char* dev_name )
    {
    return (PID*)get_device( device::DT_REGULATOR, dev_name );
    }
//-----------------------------------------------------------------------------
signal_column* device_manager::get_HLA( const char* dev_name )
    {
    return (signal_column*)get_device( device::DT_HLA, dev_name );
    }
//-----------------------------------------------------------------------------
camera* device_manager::get_CAM( const char* dev_name )
    {
    return (camera*)get_device( device::DT_CAM, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_PDS( const char* dev_name )
    {
    return get_device( device::DT_PDS, dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_TS( const char* dev_name )
    {
    return get_device( device::DT_TS, dev_name );
    }
//-----------------------------------------------------------------------------
io_device* device_manager::add_io_device( int dev_type, int dev_sub_type,
                        const char* dev_name, const char * descr, const char* article )
    {
    device* new_device = nullptr;
    io_device* new_io_device = nullptr;

    switch ( dev_type )
        {
        case device::DT_V:
            {
            switch ( dev_sub_type )
                {
                case device::DST_V_DO1:
                    new_device      = new valve_DO1( dev_name );
                    new_io_device = ( valve_DO1* ) new_device;
                    break;

                case device::DST_V_DO2:
                    new_device      = new valve_DO2( dev_name );
                    new_io_device = ( valve_DO2* ) new_device;
                    break;

                case device::DST_V_DO1_DI1_FB_OFF:
                    new_device      = new valve_DO1_DI1_off( dev_name );
                    new_io_device = ( valve_DO1_DI1_off* ) new_device;
                    break;

                case device::DST_V_DO1_DI1_FB_ON:
                    new_device      = new valve_DO1_DI1_on( dev_name );
                    new_io_device = ( valve_DO1_DI1_on* ) new_device;
                    break;

                case device::DST_V_DO1_DI2:
                    new_device      = new valve_DO1_DI2( dev_name );
                    new_io_device = ( valve_DO1_DI2* ) new_device;
                    break;

                case device::DST_V_DO2_DI2:
                    new_device      = new valve_DO2_DI2( dev_name );
                    new_io_device = ( valve_DO2_DI2* ) new_device;
                    break;

                case device::DST_V_MIXPROOF:
                    new_device      = new valve_mix_proof( dev_name );
                    new_io_device = ( valve_mix_proof* ) new_device;
                    break;

                case device::DST_V_AS_MIXPROOF:
                    new_device      = new valve_AS_mix_proof( dev_name );
                    new_io_device = ( valve_AS_mix_proof* ) new_device;
                    break;

                case device::DST_V_BOTTOM_MIXPROOF:
                    new_device      = new valve_bottom_mix_proof( dev_name );
                    new_io_device = ( valve_bottom_mix_proof* ) new_device;
                    break;

                case device::DST_V_AS_DO1_DI2:
                    new_device      = new valve_AS_DO1_DI2( dev_name );
                    new_io_device = ( valve_AS_DO1_DI2* ) new_device;
                    break;

                case device::V_DO2_DI2_BISTABLE:
                    new_device      = new valve_DO2_DI2_bistable( dev_name );
                    new_io_device = ( valve_DO2_DI2_bistable* ) new_device;
                    break;

                case device::V_IOLINK_VTUG_DO1:
                    new_device = new valve_iolink_vtug( dev_name,
                        device::V_IOLINK_VTUG_DO1 );
                    new_io_device = (valve_iolink_vtug*)new_device;
                    break;

                case device::V_IOLINK_VTUG_DO1_DI2:
                    new_device = new valve_iolink_vtug_DO2( dev_name );
                    new_io_device = (valve_iolink_vtug_DO2*)new_device;
                    break;

                case device::V_IOLINK_VTUG_DO1_FB_OFF:
                    new_device = new valve_iolink_vtug_off(dev_name);
                    new_io_device = (valve_iolink_vtug_off*)new_device;
                    break;

                case device::V_IOLINK_VTUG_DO1_FB_ON:
                    new_device = new valve_iolink_vtug_on(dev_name);
                    new_io_device = (valve_iolink_vtug_on*)new_device;
                    break;

                case device::V_IOLINK_MIXPROOF:
                    new_device = new valve_iolink_mix_proof( dev_name );
                    new_io_device = (valve_iolink_mix_proof*)new_device;
                    break;

                case device::V_IOLINK_DO1_DI2:
                    {
                    const char* SORIO_ARTICLE = "DEF.SORIO-1SV";
                    if ( strcmp( article, SORIO_ARTICLE ) == 0 )
                        {
                        new_device = new valve_iolink_shut_off_sorio( dev_name );
                        new_io_device = (valve_iolink_shut_off_sorio*)new_device;
                        }
                    else
                        {
                        new_device = new valve_iolink_shut_off_thinktop( dev_name );
                        new_io_device = (valve_iolink_shut_off_thinktop*)new_device;
                        }
                    break;
                    }

                case device::DST_V_VIRT:
                    new_device = new virtual_valve( dev_name );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown V device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;
            }

        case device::DT_VC:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_VC:
                    new_device = new analog_valve( dev_name );
                    new_io_device = (analog_valve*)new_device;
                    break;

                case device::DST_VC_IOLINK:
                    new_device = new analog_valve_iolink( dev_name );
                    new_io_device = (analog_valve_iolink*)new_device;
                    break;

                case device::DST_VC_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_VC, device::DST_VC_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown VC device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_M:
            switch ( dev_sub_type )
                {
                case device::DST_M:
                case device::DST_M_FREQ:
                case device::DST_M_REV:
                case device::DST_M_REV_FREQ:
                case device::DST_M_REV_2:
                case device::DST_M_REV_FREQ_2:
                case device::M_REV_2_ERROR:
                case device::DST_M_REV_FREQ_2_ERROR:
                    new_device = new motor( dev_name,
                        (device::DEVICE_SUB_TYPE)dev_sub_type );
                    new_io_device = (motor*)new_device;
                    break;

                case device::M_ATV:
                    new_device = new motor_altivar( dev_name,
                        (device::DEVICE_SUB_TYPE)dev_sub_type );
                    new_io_device = (motor_altivar*)new_device;
                    break;

                case device::M_ATV_LINEAR:
                    new_device = new motor_altivar_linear( dev_name );
                    new_io_device = (motor_altivar_linear*)new_device;
                    break;

                case device::DST_M_VIRT:
                    new_device = new virtual_motor( dev_name );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown M device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_LS:
            switch ( dev_sub_type )
                {
                case device::DST_LS_MIN:
                case device::DST_LS_MAX:
                    new_device = new level_s( dev_name,
                        ( device::DEVICE_SUB_TYPE ) dev_sub_type );
                    new_io_device = (level_s*)new_device;
                    break;

                case device::LS_IOLINK_MAX:
                case device::LS_IOLINK_MIN:
                    new_device = new level_s_iolink( dev_name,
                        ( device::DEVICE_SUB_TYPE ) dev_sub_type );
                    new_io_device = (level_s_iolink*)new_device;
                    break;

                case device::DST_LS_VIRT:
                    new_device = new virtual_device(dev_name, device::DT_LS, device::DST_LS_VIRT);
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown LS device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_TE:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_TE:
                    new_device      = new temperature_e( dev_name );
                    new_io_device = ( temperature_e* ) new_device;
                    break;

                case device::DST_TE_IOLINK:
                    new_device = new temperature_e_iolink(dev_name);
                    new_io_device = (temperature_e_iolink*)new_device;
                    break;

                case device::DST_TE_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_TE, device::DST_TE_VIRT );
                    break;

                case device::DST_TE_ANALOG:
                    new_device = new temperature_e_analog( dev_name );
                    new_io_device = (temperature_e_analog*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown TE device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_FS:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_FS:
                    new_device = new flow_s( dev_name );
                    new_io_device = (flow_s*)new_device;
                    break;

                case device::DST_FS_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_FS, device::DST_FS_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown FS device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_FQT:
            switch ( dev_sub_type )
                {
                case device::DST_FQT:
                    new_device      = new counter( dev_name );
                    new_io_device = ( counter* ) new_device;
                    break;

                case device::DST_FQT_F:
                    new_device      = new counter_f( dev_name );
                    new_io_device = ( counter_f* ) new_device;
                    break;

                case device::DST_FQT_F_OK:
                    new_device = new counter_f_ok( dev_name );
                    new_io_device = (counter_f_ok*)new_device;
                    break;

                case device::DST_FQT_VIRT:
                    new_device = new virtual_counter( dev_name );
                    break;

                case device::DST_FQT_IOLINK:
                    new_device = new counter_iolink( dev_name );
                    new_io_device = (counter_iolink*)new_device;
                    break;
                    
                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown FQT device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_AO:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_AO:
                    new_device      = new analog_output( dev_name );
                    new_io_device = ( analog_output* ) new_device;
                    break;

                case device::DST_AO_VIRT:
                    new_device      = new virtual_device( dev_name, device::DT_AO, device::DST_AO_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown AO device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_LT:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_LT:
                    new_device = new level_e( dev_name );
                    new_io_device = ( level_e* ) new_device;
                    break;

                case device::DST_LT_CYL:
                    new_device = new level_e_cyl( dev_name );
                    new_io_device = ( level_e_cyl* ) new_device;
                    break;

                case device::DST_LT_CONE:
                    new_device = new level_e_cone( dev_name );
                    new_io_device = (level_e_cone*)new_device;
                    break;

                case device::DST_LT_IOLINK:
                    new_device = new level_e_iolink( dev_name );
                    new_io_device = (level_e_iolink*)new_device;
                    break;

                case device::DST_LT_VIRT:
                    new_device = new virtual_device(dev_name, device::DT_LT, device::DST_LT_VIRT);
                    break;

                case device::DST_LT_TRUNC:
                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown LT device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_DI:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_DI:
                    new_device      = new DI_signal( dev_name );
                    new_io_device = ( DI_signal* ) new_device;
                    break;

                case device::DST_DI_VIRT:
                    new_device      = new virtual_device( dev_name, device::DT_DI, device::DST_DI_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown DI device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_DO:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_DO:
                    new_device      = new DO_signal( dev_name );
                    new_io_device = ( DO_signal* ) new_device;
                    break;

                case device::DST_DO_VIRT:
                    new_device      = new virtual_device( dev_name, device::DT_DO, device::DST_DO_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown DO device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_PT:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_PT:
                    new_device      = new pressure_e( dev_name );
                    new_io_device = ( pressure_e* ) new_device;
                    break;

                case device::DST_PT_IOLINK:
                    new_device      = new pressure_e_iolink( dev_name );
                    new_io_device = ( pressure_e_iolink* ) new_device;
                    break;

                case device::DST_PT_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_PT, device::DST_PT_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown PT device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_QT:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_QT:
                    new_device = new concentration_e( dev_name, device::DST_QT );
                    new_io_device = (concentration_e*)new_device;
                    break;

                case device::DST_QT_OK:
                    new_device = new concentration_e_ok( dev_name );
                    new_io_device = (concentration_e_ok*)new_device;
                    break;

                case device::DST_QT_IOLINK:
                    new_device = new concentration_e_iolink(dev_name);
                    new_io_device = (concentration_e_iolink*)new_device;
                    break;

                case device::DST_QT_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_QT, device::DST_QT_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown QT device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_AI:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_AI:
                    new_device      = new analog_input( dev_name );
                    new_io_device = ( analog_input* ) new_device;
                    break;

                case device::DST_AI_VIRT:
                    new_device      = new virtual_device( dev_name, device::DT_AI, device::DST_AI_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown AI device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_HA:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_HA:
                    new_device = new siren( dev_name );
                    new_io_device = (siren*)new_device;
                    break;

                case device::DST_HA_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_HA, device::DST_HA_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown HA device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_HL:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_HL:
                    new_device = new lamp( dev_name );
                    new_io_device = (lamp*)new_device;
                    break;

                case device::DST_HL_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_HL, device::DST_HL_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown HL device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_SB:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_SB:
                    new_device = new button( dev_name );
                    new_io_device = (button*)new_device;
                    break;

                case device::DST_SB_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_SB, device::DST_SB_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown SB device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_GS:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_GS:
                    new_device = new state_s( dev_name );
                    new_io_device = (state_s*)new_device;
                    break;

                case device::DST_GS_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_GS, device::DST_GS_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown GS device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_PDS:
            switch ( dev_sub_type )
                {
                case device::DST_PDS:
                    new_device = new diff_pressure( dev_name );
                    new_io_device = (state_s*)new_device;
                    break;

                case device::DST_PDS_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_PDS, device::DST_PDS_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown PDS device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_WT:
            switch ( dev_sub_type )
                {
                case device::DST_NONE:
                case device::DST_WT:
                    new_device = new wages( dev_name );
                    new_io_device = (wages*)new_device;

                case device::DST_WT_VIRT:
                    new_device = new virtual_wages( dev_name );
                    break;

                case device::DST_WT_RS232:
                    new_device = new wages_RS232( dev_name );
                    new_io_device = (wages_RS232*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown WT device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_F:
            switch (dev_sub_type)
                {
                case device::DST_NONE:
                case device::DST_F:
                    new_device = new circuit_breaker(dev_name);
                    new_io_device = (circuit_breaker*)new_device;
                    break;

                case device::DST_F_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_F, device::DST_F_VIRT );
                    break;

                default:
                    if (G_DEBUG)
                        {
                        printf("Unknown F device subtype %d!\n", dev_sub_type);
                        }
                    break;
                }
            break;

        case device::DT_REGULATOR:
            new_device = new PID( dev_name );
            new_io_device = 0;
            break;

        case device::DT_HLA:
            switch ( dev_sub_type )
                {
                case device::DST_HLA:
                    new_device = new signal_column_discrete( dev_name );
                    new_io_device = (signal_column_discrete*)new_device;
                    break;

                case device::DST_HLA_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_HLA, device::DST_HLA_VIRT );
                    break;

                case device::DST_HLA_IOLINK:
                    new_device = new signal_column_iolink( dev_name );
                    new_io_device = (signal_column_iolink*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown HLA device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_CAM:
            switch ( dev_sub_type )
                {
                case device::DST_CAM_DO1_DI1:
                    new_device = new camera( dev_name,
                        static_cast<device::DEVICE_SUB_TYPE> (dev_sub_type) );
                    new_io_device = (camera*)new_device;
                    break;

                case device::DST_CAM_DO1_DI2:
                    new_device = new camera_DI2( dev_name,
                        static_cast<device::DEVICE_SUB_TYPE> ( dev_sub_type ) );
                    new_io_device = (camera_DI2*)new_device;
                    break;

                case device::DST_CAM_DO1_DI3:
                    new_device = new camera_DI3( dev_name );
                    new_io_device = (camera_DI3*)new_device;
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown CAM device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        case device::DT_TS:
            switch ( dev_sub_type )
                {
                case device::DST_TS:
                    new_device = new temperature_signal( dev_name );
                    new_io_device = (state_s*)new_device;
                    break;

                case device::DST_TS_VIRT:
                    new_device = new virtual_device( dev_name, device::DT_TS, device::DST_TS_VIRT );
                    break;

                default:
                    if ( G_DEBUG )
                        {
                        printf( "Unknown TS device subtype %d!\n", dev_sub_type );
                        }
                    break;
                }
            break;

        default:
            if ( G_DEBUG )
                {
                printf( "Unknown device type %d!\n", dev_type );
                }
            break;
        }

    // Ошибки.
    if ( !new_device )
        {
        new_device = static_cast<device*>( static_cast<valve*>( new dev_stub() ) );
        }

    G_ERRORS_MANAGER->add_error( new tech_dev_error( new_device ) );

    u_int new_dev_index = project_devices.size();
    project_devices.push_back( new_device );
    new_device->set_serial_n( new_dev_index );
    new_device->set_article( article );

    if ( dev_type >= 0 && dev_type < device::C_DEVICE_TYPE_CNT )
        {
        if ( 0 == is_first_device[ dev_type ] )
            {
            dev_types_ranges[ dev_type ].start_pos = new_dev_index;
            is_first_device[ dev_type ] = 1;
            }
        dev_types_ranges[ dev_type ].end_pos = new_dev_index;
        }

    return new_io_device;
    }
//-----------------------------------------------------------------------------
void device_manager::clear_io_devices()
    {
    for ( size_t idx = 0; idx < project_devices.size(); idx++ )
        {
        delete project_devices[ idx ];
        project_devices[ idx ] = nullptr;
        }

    for ( size_t idx = 0; idx < device::C_DEVICE_TYPE_CNT; idx++ )
        {
        dev_types_ranges[ idx ].start_pos = -1;
        dev_types_ranges[ idx ].end_pos = -1;
        is_first_device[ idx ] = 0;
        }

    project_devices.clear();
    }
//-----------------------------------------------------------------------------
int device_manager::init_params()
    {
    lua_manager::get_instance()->void_exec_lua_method( "system",
        "init_devices_params", "device_manager::init_params()" );

    return 0;
    }
//-----------------------------------------------------------------------------
int device_manager::save_device( char *buff )
    {
    int res = sprintf( buff, "t=\n" );

    res += sprintf( buff + res, "\t{\n" );

    for ( u_int i = 0; i < project_devices.size(); i++)
        {
        res += project_devices[ i ]->save_device( buff + res, "\t" );
        }

    res += sprintf( buff + res, "\t}\n" );
    return res;
    }
//-----------------------------------------------------------------------------
int device_manager::get_device_n( device::DEVICE_TYPE dev_type, const char* dev_name )
    {
    int l = -1;
    int u = -1;

    if ( dev_type < device::C_DEVICE_TYPE_CNT && dev_type >= device::DT_V )
        {
        l = dev_types_ranges[ dev_type ].start_pos;
        u = dev_types_ranges[ dev_type ].end_pos;
        }

    if ( -1 == l ) return -1; // Нет устройств.

    while ( l <= u )
        {
        int i = ( l + u ) / 2;

        if ( strcmp( dev_name, project_devices[ i ]->get_name() ) == 0 )
            {
            return i;
            }

        if ( strcmp( dev_name, project_devices[ i ]->get_name() ) > 0 )
            {
            l = i + 1;
            }
        else
            {
            u = i - 1;
            }
        }

    return -1;
    }
//-----------------------------------------------------------------------------
int device_manager::get_device_n( const char* dev_name )
    {
    for ( int dev_type = device::DT_V; dev_type < device::C_DEVICE_TYPE_CNT; dev_type++ )
        {
        auto res = get_device_n( ( device::DEVICE_TYPE ) dev_type, dev_name );
        if ( res >= 0 ) return res;
        }

    return -1;
    }
//-----------------------------------------------------------------------------
int device_manager::init_rt_params()
    {
    lua_manager::get_instance()->void_exec_lua_method( "system",
        "init_devices_rt_params", "device_manager::init_rt_params()" );

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
i_counter::~i_counter()
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool i_DI_device::is_active()
    {
    return get_state() == 0 ? 0 : 1;
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
void virtual_valve::direct_off()
    {
    state = 0;
    value = 0;
    }

void virtual_valve::direct_set_value( float new_value )
    {
    value = new_value;
    }

float virtual_valve::get_value()
    {
    return value;
    }

void virtual_valve::direct_set_state( int new_state )
    {
    state = new_state;
    }

void virtual_valve::direct_on()
    {
    state = 1;
    }

int virtual_valve::get_state()
    {
    return state;
    }

virtual_valve::virtual_valve( const char* dev_name ) :
    valve( dev_name, DT_V, DST_V_VIRT ),
    value( 0 ),
    state( 0 )
    {
    }

valve::VALVE_STATE virtual_valve::get_valve_state()
    {
    return (valve::VALVE_STATE)state;
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
    }
//-----------------------------------------------------------------------------
void dev_stub::off()
    {
    }
//-----------------------------------------------------------------------------
void dev_stub::on()
    {
    }
//-----------------------------------------------------------------------------
void dev_stub::set_value( float new_value )
    {
    }
//-----------------------------------------------------------------------------
void dev_stub::set_state( int new_state )
    {
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
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_off()
    {
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_set_state( int new_state )
    {
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
    }
//-----------------------------------------------------------------------------
void dev_stub::start()
    {
    }
//-----------------------------------------------------------------------------
void dev_stub::reset()
    {
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
    }
//-----------------------------------------------------------------------------
u_int dev_stub::get_abs_quantity()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::tare()
    {
    }
//-----------------------------------------------------------------------------
void dev_stub::process_DO( u_int n, DO_state state, const char* name )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
base_counter::base_counter( const char* dev_name, DEVICE_SUB_TYPE sub_type,
    int extra_par_cnt ) :
    device( dev_name, DT_FQT, sub_type, extra_par_cnt ),
    io_device( dev_name )
    {
    }
//-----------------------------------------------------------------------------
int base_counter::get_state()
    {
    if ( !motors.empty() )
        {
        char is_pump_working = 0;

        for ( u_int i = 0; i < motors.size(); i++ )
            {
            if ( motors[ i ]->get_state() == 1 )
                {
                is_pump_working = 1;
                if ( 0 == start_pump_working_time )
                    {
                    start_pump_working_time = get_millisec();
                    counter_prev_value = get_abs_quantity();
                    }
                }
            }

        auto min_flow = get_min_flow();
        if ( 0 == is_pump_working ||        // Насос не работает или 
            get_flow() <= min_flow )        //расход ниже минимального. 
            {
            start_pump_working_time = 0;
            }
        else                            // Насос работает. 
            {
            if ( state == STATES::S_PAUSE )
                {
                start_pump_working_time = get_millisec();
                }
            else                        // Работа. 
                {
                auto dt = get_pump_dt();
                if ( get_delta_millisec( start_pump_working_time ) > dt )
                    {
                    // Проверяем счетчик на ошибку - он должен изменить свои показания. 
                    if ( get_abs_quantity() == counter_prev_value )
                        {
                        state = STATES::S_ERROR;
                        }
                    else
                        {
                        start_pump_working_time = get_millisec();
                        counter_prev_value = get_abs_quantity();
                        state = STATES::S_WORK;
                        }
                    }
                }
            }
        }// if ( motors.size() > 0 

    return static_cast<int>( state );
    };
//-----------------------------------------------------------------------------
void base_counter::direct_on()
    {
    start();
    }
//-----------------------------------------------------------------------------
void base_counter::direct_off()
    {
    }
//-----------------------------------------------------------------------------
void base_counter::direct_set_state( int new_state )
    {
#ifdef DEBUG_NO_IO_MODULES
    if ( new_state < 0 )
        {
        state = static_cast<STATES>( new_state );
        return;
        }
#endif
    
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
void base_counter::calculate_quantity( float& value, float& last_read_value,
    bool& is_first_read )
    {
    float current = get_raw_value();

    if ( is_first_read )
        {
        if ( current != 0 )
            {
            last_read_value = current;
            is_first_read = false;
            }
        }
    else
        {
        float delta;
        if ( current < last_read_value )
            {
            delta = get_max_raw_value() - last_read_value + current;
            if ( delta > MAX_OVERFLOW && current < delta )
                {
                delta = current;
                }
            }
        else
            {
            delta = current - last_read_value;
            }

        last_read_value = current;
        if ( delta > 0 )
            {
            value += delta;
            }
        }
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

    state = STATES::S_PAUSE;
    }
//-----------------------------------------------------------------------------
void base_counter::start()
    {
    if ( STATES::S_PAUSE == state )
        {
        state = STATES::S_WORK;
        last_read_value = get_raw_value();
        }
    else if ( STATES::S_ERROR == state )
        {
        start_pump_working_time = 0;
        state = STATES::S_WORK;
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
    calculate_quantity( abs_value, abs_last_read_value, abs_is_first_read );
    }
//-----------------------------------------------------------------------------
void base_counter::print() const
    {
    device::print();
    }
//-----------------------------------------------------------------------------
int base_counter::set_cmd( const char* prop, u_int idx, double val )
    {
    switch ( prop[ 0 ] )
        {
        case 'A': //ABS_V
            set_abs_value( static_cast<float>( val ) );
            break;

        default:
            return device::set_cmd( prop, idx, val );
        }

    return 0;
    };
//-----------------------------------------------------------------------------
int base_counter::save_device_ex( char* buff )
    {
    return sprintf( buff, "ABS_V=%u, ", get_abs_quantity() );
    }
//-----------------------------------------------------------------------------
const char* base_counter::get_error_description() const
    {
    if ( static_cast<int>( state ) < 0 )
        {
        switch ( state )
            {
            case STATES::S_ERROR:
                return "счет импульсов";

            case STATES::S_LOW_ERR:
                return "канал потока (нижний предел)";

            case STATES::S_HI_ERR:
                return "канал потока (верхний предел)";

            default:
                return device::get_error_description();
            }
        }

    return "";
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
#ifndef DEBUG_NO_IO_MODULES
    return static_cast<float>( *( (u_int_2*)get_AI_data( AI_Q_INDEX ) ) );
#else
    return 0;
#endif
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
    return get_par( P_CZ, 0 ) +
#ifdef DEBUG_NO_IO_MODULES
        flow_value;
#else
        get_AI( AI_FLOW_INDEX, get_par( P_MIN_FLOW, 0 ), get_par( P_MAX_FLOW, 0 ) );
#endif // NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
int counter_f::save_device_ex( char *buff )
    {
    int res = counter::save_device_ex( buff );

    res += sprintf( buff + res, "F=%.2f, ", get_flow() );

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
counter_f_ok::counter_f_ok( const char *dev_name ) : counter_f( dev_name )
    {
    sub_type = DST_FQT_F_OK;
    }
//-----------------------------------------------------------------------------
int counter_f_ok::save_device_ex( char *buff )
    {
    int res = counter_f::save_device_ex( buff );

#ifdef DEBUG_NO_IO_MODULES
    res += sprintf( buff + res, "OK=1, " );
#else
    res += sprintf( buff + res, "OK=%d, ", get_DI( DI_INDEX ) );
#endif //DEBUG_NO_IO_MODULES

    return res;
    }
//-----------------------------------------------------------------------------
int counter_f_ok::get_state()
    {
#ifndef DEBUG_NO_IO_MODULES
    int i = get_DI( DI_INDEX );

    return i == 1 ? counter_f::get_state() : static_cast<int>( STATES::S_ERROR );
#else
    return counter_f::get_state();
#endif
    }
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
    char* data = (char*)get_AI_data( 0 );
    if ( data )
        {
        char* buff = (char*)&in_info;

        const int SIZE = 8;
        std::copy( data, data + SIZE, buff );

        //Reverse byte order to get correct float.
        std::swap( buff[ 3 ], buff[ 0 ] );
        std::swap( buff[ 1 ], buff[ 2 ] );
        //Reverse byte order to get correct int16.
        std::swap( buff[ 4 ], buff[ 5 ] );
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
    IOLINKSTATE res = get_AI_IOLINK_state( static_cast<u_int>( CONSTANTS::AI_INDEX ) );
    if ( res != io_device::IOLINKSTATE::OK )
        {
        return -static_cast<int>( res );
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

    res += sprintf( buff + res, "F=%.2f, ", get_flow() );
    res += sprintf( buff + res, "T=%.1f, ", get_temperature() );

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
float digital_io_device::get_value()
    {
    return ( float ) get_state();
    }
//-----------------------------------------------------------------------------
void digital_io_device::direct_set_value( float new_value )
    {
    direct_set_state( ( int ) new_value );
    }
//-----------------------------------------------------------------------------
void digital_io_device::direct_set_state( int new_state )
    {
#ifdef DEBUG_NO_IO_MODULES
    state = new_state;
#else
    if ( new_state )
        {
        direct_on();
        }
    else direct_off();
#endif //DEBUG_NO_IO_MODULES
    }
//-----------------------------------------------------------------------------
void digital_io_device::print() const
    {
    device::print();
    //io_device::print();
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_IO_MODULES
int digital_io_device::get_state()
    {
    return state;
    }
//-----------------------------------------------------------------------------
void digital_io_device::direct_on()
    {
    state = 1;
    }
//-----------------------------------------------------------------------------
void digital_io_device::direct_off()
    {
    state = 0;
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
digital_io_device::digital_io_device( const char *dev_name, device::DEVICE_TYPE type,
                                         device::DEVICE_SUB_TYPE sub_type, u_int par_cnt ) :
    device( dev_name, type, sub_type, par_cnt ),
    io_device( dev_name )
#ifdef DEBUG_NO_IO_MODULES
    , state( 0 )
#endif // DEBUG_NO_IO_MODULES
    {
    }
//-----------------------------------------------------------------------------
digital_io_device::~digital_io_device()
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES

int DO2::get_state()
    {
    int b1 = get_DO( DO_INDEX_1 );
    int b2 = get_DO( DO_INDEX_2 );
    if ( b1 == b2 ) return -1;
    return b2;
    }
//-----------------------------------------------------------------------------
void DO2::direct_on()
    {
    set_DO( DO_INDEX_1, 0 );
    set_DO( DO_INDEX_2, 1 );
    }
//-----------------------------------------------------------------------------
void DO2::direct_off()
    {
    set_DO( DO_INDEX_1, 1 );
    set_DO( DO_INDEX_2, 0 );
    }

#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve::valve( bool is_on_fb, bool is_off_fb, const char *dev_name,
             device::DEVICE_TYPE type, device::DEVICE_SUB_TYPE sub_type ) :
digital_io_device( dev_name, type, sub_type, ADDITIONAL_PARAMS_COUNT ),
    is_on_fb( is_on_fb ),
    is_off_fb( is_off_fb ),
    on_fb( true ),
    off_fb( true ),
    was_on_auto( false ),
    is_switching_off( false ),
    start_off_time( 0 ),
    start_switch_time( get_millisec() ),
    wash_flag ( false )
    {
    set_par_name( P_ON_TIME, 0, "P_ON_TIME" );
    set_par_name( P_FB,  0, "P_FB" );
    }
//-----------------------------------------------------------------------------
valve::valve( const char *dev_name, device::DEVICE_TYPE type,
    device::DEVICE_SUB_TYPE sub_type ) :
    digital_io_device( dev_name, type, sub_type, 0 ),
    is_on_fb( false ),
    is_off_fb( false ),
    on_fb( false ),
    off_fb( false ),
    was_on_auto( false ),
    is_switching_off( false ),
    start_off_time( 0 ),
    start_switch_time( get_millisec() ),
    wash_flag ( false )
    {
    }
//-----------------------------------------------------------------------------
valve::valve( bool is_on_fb, bool is_off_fb, const char *dev_name,
    device::DEVICE_TYPE type, device::DEVICE_SUB_TYPE sub_type,
    int extra_params_cnt ) :
    digital_io_device( dev_name, type, sub_type, ADDITIONAL_PARAMS_COUNT + extra_params_cnt ),
    is_on_fb( is_on_fb ),
    is_off_fb( is_off_fb ),
    on_fb( true ),
    off_fb( true ),
    was_on_auto( false ),
    is_switching_off( false ),
    start_off_time( 0 ),
    start_switch_time( get_millisec() ),
    wash_flag( false )
    {
    set_par_name( P_ON_TIME, 0, "P_ON_TIME" );
    set_par_name( P_FB, 0, "P_FB" );
    }
//-----------------------------------------------------------------------------
int valve::save_device_ex( char *buff )
    {
    int res = 0;
    if ( is_on_fb )
        {
        res += sprintf( buff, "FB_ON_ST=%d, ", get_on_fb_value() );
        }

    if ( is_off_fb )
        {
        res += sprintf( buff + res, "FB_OFF_ST=%d, ", get_off_fb_value() );
        }
    return res;
    }
//-----------------------------------------------------------------------------
int valve::get_state()
    {
#ifdef DEBUG_NO_IO_MODULES
    return digital_io_device::get_state();
#else

    switch ( get_valve_state() )
        {
        case V_LOWER_SEAT:
            if ( get_manual_mode() )
                {
                return VX_LOWER_SEAT_MANUAL;
                }

            //Обратная связь отключена.
            if ( get_par( P_FB, 0 ) == FB_IS_AND_OFF )
                {
                return VX_LOWER_SEAT_FB_OFF;
                }

            return VX_LOWER_SEAT;

        case V_UPPER_SEAT:
            if ( get_manual_mode() )
                {
                return VX_UPPER_SEAT_MANUAL;
                }

            //Обратная связь отключена.
            if ( get_par( P_FB, 0 ) == FB_IS_AND_OFF )
                {
                return VX_UPPER_SEAT_FB_OFF;
                }

            return VX_UPPER_SEAT;

        case V_ON:
            if ( is_off_fb || is_on_fb ) //Обратная связь есть.
                {
                if ( get_par( P_FB, 0 ) == FB_IS_AND_OFF ) //Обратная связь отключена.
                    {
                    //start_switch_time = get_millisec();

                    if ( get_manual_mode() )
                        {
                        return VX_ON_FB_OFF_MANUAL;
                        }
                    else
                        {
                        return VX_ON_FB_OFF;
                        }
                    }
                else //Обратная связь включена.
                    {
                    if ( get_manual_mode() ) //Ручной режим включен.
                        {
                        if ( get_fb_state() == true )
                            {
                            //start_switch_time = get_millisec();

                            return VX_ON_FB_OK_MANUAL;
                            }
                        else
                            {
                            if ( get_delta_millisec( start_switch_time ) > get_par( P_ON_TIME, 0 ) )
                                {
                                return VX_ON_FB_ERR_MANUAL;
                                }
                            else
                                {
                                return VX_OPENING_MANUAL;
                                }
                            }
                        } // if ( get_manual_mode() )
                    else  //Ручной режим отключен.
                        {
                        if ( get_fb_state() == true )
                            {
                            //start_switch_time = get_millisec();

                            return VX_ON_FB_OK;
                            }
                        else
                            {
                            if ( get_delta_millisec( start_switch_time ) > get_par( P_ON_TIME, 0 ) )
                                {
                                return VX_ON_FB_ERR;
                                }
                            else
                                {
                                return VX_OPENING;
                                }
                            }
                        }
                    }
                }//if ( is_off_fb || is_on_fb ) //Обратная связь есть.
            else //Обратной связи нет.
                {
                if ( get_manual_mode() )
                    {
                    return VX_ON_FB_OK_MANUAL;
                    }
                else
                    {
                    return VX_ON_FB_OK;
                    }
                }
            break;

        case V_OFF:
            if ( is_off_fb || is_on_fb ) //Обратная связь есть.
                {
                if ( get_par( P_FB, 0 ) == FB_IS_AND_OFF ) //Обратная связь отключена.
                    {
                    //start_switch_time = get_millisec();

                    if ( get_manual_mode() )
                        {
                        return VX_OFF_FB_OFF_MANUAL;
                        }
                    else
                        {
                        return VX_OFF_FB_OFF;
                        }
                    }
                else //Обратная связь включена.
                    {
                    if ( get_manual_mode() ) //Ручной режим включен.
                        {
                        if ( get_fb_state() == true )
                            {
                            //start_switch_time = get_millisec();

                            return VX_OFF_FB_OK_MANUAL;
                            }
                        else
                            {
                            if ( get_delta_millisec( start_switch_time ) > get_par( P_ON_TIME, 0 ) )
                                {
                                return VX_OFF_FB_ERR_MANUAL;
                                }
                            else
                                {
                                return VX_CLOSING_MANUAL;
                                }
                            }
                        } // if ( get_manual_mode() )
                    else  //Ручной режим отключен.
                        {
                        if ( get_fb_state() == true )
                            {
                            //start_switch_time = get_millisec();

                            return VX_OFF_FB_OK;
                            }
                        else
                            {
                            if ( get_delta_millisec( start_switch_time ) > get_par( P_ON_TIME, 0 ) )
                                {
                                return VX_OFF_FB_ERR;
                                }
                            else
                                {
                                return VX_CLOSING;
                                }
                            }
                        }
                    }
                }//if ( is_off_fb || is_on_fb ) //Обратная связь есть.
            else //Обратной связи нет.
                {
                if ( get_manual_mode() )
                    {
                    return VX_OFF_FB_OK_MANUAL;
                    }
                else
                    {
                    return VX_OFF_FB_OK;
                    }
                }
            break;

        case V_STOP:
            return V_STOP;
            break;
        }

    return VX_UNKNOWN;

#endif // DEBUG_NO_IO_MODULES
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_IO_MODULES
int valve::set_cmd( const char *prop, u_int idx, double val )
    {
    printf( "valve::set_cmd() - prop = %s, idx = %d, val = %f\n",
        prop, idx, val );

    switch ( prop[ 0 ] )
        {
        case 'F':
            if ( strcmp( prop, "FB_ON_ST" ) == 0 )
                {
                on_fb = val != .0;
                }
            else
                {
                off_fb = val != .0;
                }
            break;

        default:
            device::set_cmd( prop, idx, val );
        }

    return 0;
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
void valve::evaluate()
    {
    if ( v_bistable.empty() == false )
        {
        std::vector< valve_DO2_DI2_bistable* >::iterator iter;
        for ( iter = v_bistable.begin(); iter != v_bistable.end(); iter++ )
            {
            valve_DO2_DI2_bistable* v = *iter;
            v->evaluate();
            }
        }

    if ( to_switch_off.empty() )
        {
        return;
        }

    u_int delay = G_PAC_INFO()->par[ PAC_info::P_V_OFF_DELAY_TIME ];

    for( std::vector< valve* >::iterator iter = to_switch_off.begin();
            iter != to_switch_off.end(); iter++ )
        {
        valve* v = *iter;

        if ( v->is_switching_off &&
            get_delta_millisec( v->start_off_time ) > delay )
            {
            if ( !v->get_manual_mode() )
                {
                v->direct_off();
                }

            v->is_switching_off = false;
            v->was_on_auto = false;
            }
        }

    to_switch_off.erase(
        std::remove_if( to_switch_off.begin(), to_switch_off.end(),
        is_switching_off_finished ), to_switch_off.end() );
    }
//-----------------------------------------------------------------------------
void valve::off()
    {
    if ( false == was_on_auto ||                //Если был включен вручную.
        get_valve_state() == VALVE_STATE::V_UPPER_SEAT ||
        get_valve_state() == VALVE_STATE::V_LOWER_SEAT )
        {
        if ( !get_manual_mode() )
            {
            direct_off();
            }

        return;
        }

    if ( false == is_switching_off )
        {
        is_switching_off = true;
        start_off_time = get_millisec();

        to_switch_off.push_back( this );
        }
    }
//-----------------------------------------------------------------------------
void valve::on()
    {
    was_on_auto = true;
    is_switching_off = false;
    digital_io_device::on();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_DO1_DI1_off::valve_DO1_DI1_off( const char *dev_name ) :
    valve( false, true, dev_name, DT_V, DST_V_DO1_DI1_FB_OFF )
    {
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES
void valve_DO1_DI1_off::direct_on()
    {
    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI1_off::direct_off()
    {
    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES
void valve_DO1_DI1_on::direct_on()
    {
    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI1_on::direct_off()
    {
    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    }

#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES

void valve_DO1_DI2::direct_on()
    {
    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI2::direct_off()
    {
    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    }

#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES

void valve_DO2_DI2::direct_on()
    {
    int o = get_DO( DO_INDEX_1 );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_1, 1 );
        set_DO( DO_INDEX_2, 0 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO2_DI2::direct_off()
    {
    int o = get_DO( DO_INDEX_2 );
    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_1, 0 );
        set_DO( DO_INDEX_2, 1 );
        }
    }

#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void valve_mix_proof::open_upper_seat()
    {
    direct_set_state( (int) VALVE_STATE::V_UPPER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::open_lower_seat()
    {
    direct_set_state( (int) VALVE_STATE::V_LOWER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::direct_set_state( int new_state )
    {
#ifdef DEBUG_NO_IO_MODULES
    state = ( char ) new_state;
#else
    switch ( new_state )
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

        case V_UPPER_SEAT:
            {
            direct_off();

            int u = get_DO( DO_INDEX_U );
            if( 0 == u )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_U, 1 );
                }

            break;
            }

        case V_LOWER_SEAT:
            {
            direct_off();

            int l = get_DO( DO_INDEX_L );
            if ( 0 == l )
                {
                start_switch_time = get_millisec();
                set_DO( DO_INDEX_L, 1 );
                }
            break;
            }

        default:
            direct_on();
            break;
        }
#endif //DEBUG_NO_IO_MODULES
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES

void valve_mix_proof::direct_on()
    {
    set_DO( DO_INDEX_U, 0 );
    set_DO( DO_INDEX_L, 0 );
    int o = get_DO( DO_INDEX );

    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::direct_off()
    {
    VALVE_STATE st = get_valve_state();
    bool was_seat = st == V_LOWER_SEAT || st == V_UPPER_SEAT;

    set_DO( DO_INDEX_U, 0 );
    set_DO( DO_INDEX_L, 0 );
    int o = get_DO( DO_INDEX );

    if ( o != 0 || was_seat )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief Определение завершения отключения клапана с задержкой.
bool valve_bottom_mix_proof::is_switching_off_finished(
    valve_bottom_mix_proof *v )
    {
    //Если открыли клапан раньше завершения закрытия, то его можно удалять из
    //вектора.
    if ( v->get_valve_state() == VALVE_STATE::V_ON )
        {
        return true;
        }

    //Если сняли флаг закрытия, то удаляем из вектора
    if (!v->is_closing_mini)
        {
        return true;
        }

    u_int delay = G_PAC_INFO()->par[ PAC_info::P_V_BOTTOM_OFF_DELAY_TIME ];

    //Если завершилось время задержки, выключаем мини клапан перед удалением
    //клапана из вектора.
    if ( get_delta_millisec( v->start_off_time ) > delay )
        {
        v->set_DO( DO_INDEX_MINI_V, 0 );
        v->is_closing_mini = 0;
        return true;
        }

    return false;
    };
//-----------------------------------------------------------------------------
void valve_bottom_mix_proof::evaluate()
    {
    if ( to_switch_off.empty() )
        {
        return;
        }

    to_switch_off.erase(
        std::remove_if( to_switch_off.begin(), to_switch_off.end(),
        is_switching_off_finished ), to_switch_off.end() );
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES
void valve_bottom_mix_proof::direct_on()
    {
    set_DO( DO_INDEX_L, 0 );
    int o = get_DO( DO_INDEX );

    if ( 0 == o )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 1 );

        set_DO( DO_INDEX_MINI_V, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_bottom_mix_proof::direct_off()
    {
    VALVE_STATE st = get_valve_state();
    bool was_seat = st == V_LOWER_SEAT;
    bool was_mini = st == V_UPPER_SEAT;
    int o = get_DO( DO_INDEX );

    if ( was_seat )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_L, 0 );
        }

    if ( was_mini && !is_closing_mini )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX_MINI_V, 0 );
        }

    if ( o != 0 )
        {
        start_switch_time = get_millisec();
        start_off_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        is_closing_mini = 1;

        to_switch_off.push_back( this );
        }
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iolink_mix_proof::valve_iolink_mix_proof( const char* dev_name ) :
    valve( true, true, dev_name, DT_V, V_IOLINK_MIXPROOF ), out_info( 0 )
    {
    in_info->err = 0;
    }
//-----------------------------------------------------------------------------
void valve_iolink_mix_proof::open_upper_seat()
    {
    direct_set_state( (int) VALVE_STATE::V_UPPER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_iolink_mix_proof::open_lower_seat()
    {
    direct_set_state( (int) VALVE_STATE::V_LOWER_SEAT );
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_iolink_mix_proof::get_valve_state()
    {
#ifdef DEBUG_NO_IO_MODULES
    return (VALVE_STATE)digital_io_device::get_state();
#else

    if ( in_info->de_en ) return V_OFF;
    if ( in_info->main ) return V_ON;
    if ( in_info->usl ) return V_UPPER_SEAT;
    if ( in_info->lsp ) return V_LOWER_SEAT;

    return V_OFF;
#endif // DEBUG_NO_IO_MODULES
    }

//-----------------------------------------------------------------------------
void valve_iolink_mix_proof::evaluate_io()
    {
    out_info = (out_data_swapped*)get_AO_write_data(
        static_cast<u_int>( CONSTANTS::C_AI_INDEX ) );
    if ( extra_offset < 0 )
        {
        out_info = (out_data_swapped*)( (char*)out_info + extra_offset );
        }

    char* data = (char*)get_AI_data(
        static_cast<u_int>( CONSTANTS::C_AI_INDEX ) );
    char* buff = (char*)in_info;

    const int SIZE = 4;
    std::copy( data, data + SIZE, buff );
    std::swap( buff[ 0 ], buff[ 1 ] );
    std::swap( buff[ 2 ], buff[ 3 ] );

//#define DEBUG_IOLINK_MIXPROOF
#ifdef DEBUG_IOLINK_MIXPROOF
    char* tmp = (char*)in_info;

    sprintf( G_LOG->msg, "%x %x %x %x\n",
        tmp[ 0 ], tmp[ 1 ], tmp[ 2 ], tmp[ 3 ] );
    G_LOG->write_log( i_log::P_WARNING );

    sprintf( G_LOG->msg,
        "de_en %u, main %u, usl %u, lsp %u, pos %.1f\n",
        in_info->de_en, in_info->main, in_info->usl,
        in_info->lsp, 0.1 * in_info->pos );
    G_LOG->write_log( i_log::P_NOTICE );
#endif
    }
//-----------------------------------------------------------------------------
void valve_iolink_mix_proof::set_rt_par( u_int idx, float value )
    {
    switch ( idx )
        {
        case 1:
            extra_offset = (int)value;
            break;

        default:
            valve::set_rt_par( idx, value );
            break;
        }
    }
//-----------------------------------------------------------------------------
int valve_iolink_mix_proof::save_device_ex( char *buff )
    {
    int res = valve::save_device_ex( buff );

    bool cs = out_info->sv1 || out_info->sv2 || out_info->sv3;
    int err = in_info->err;
    res += sprintf( buff + res, "BLINK=%d, CS=%d, ERR=%d, ", blink, cs, err );
    res += sprintf( buff + res, "V=%.1f, ", get_value() );

    return res;
    }
//-----------------------------------------------------------------------------
valve_iolink_mix_proof::~valve_iolink_mix_proof()
    {
    delete in_info;
    in_info = nullptr;
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES
bool valve_iolink_mix_proof::get_fb_state()
    {
    if ( get_AI_IOLINK_state( static_cast<u_int>( CONSTANTS::C_AI_INDEX ) ) !=
        io_device::IOLINKSTATE::OK )
        {
        return false;
        }

    if ( in_info->err ) return false;

    if ( get_delta_millisec( start_switch_time ) <
        get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    if ( out_info->sv1 == false && in_info->de_en && in_info->st ) return true;

    if ( out_info->sv1 == true && in_info->main && in_info->st ) return true;

    if ( out_info->sv2 == true && in_info->usl && in_info->st ) return true;

    if ( out_info->sv3 == true && in_info->lsp && in_info->st ) return true;

    return false;
    }
//-----------------------------------------------------------------------------
int valve_iolink_mix_proof::get_state()
    {
    switch ( get_valve_state() )
        {
        case V_LOWER_SEAT:
            if ( get_manual_mode() )
                {
                return VX_LOWER_SEAT_MANUAL;
                }

            //Обратная связь отключена.
            if ( get_par( P_FB, 0 ) == FB_IS_AND_OFF )
                {
                return VX_LOWER_SEAT_FB_OFF;
                }

            if ( get_fb_state() ) return VX_LOWER_SEAT;

            return VX_OFF_FB_ERR;

        case V_UPPER_SEAT:
            if ( get_manual_mode() )
                {
                return VX_UPPER_SEAT_MANUAL;
                }

            //Обратная связь отключена.
            if ( get_par( P_FB, 0 ) == FB_IS_AND_OFF )
                {
                return VX_UPPER_SEAT_FB_OFF;
                }

            if ( get_fb_state() ) return VX_UPPER_SEAT;

            return VX_OFF_FB_ERR;
        default:
            break;
        }

    return valve::get_state();
    }
//-----------------------------------------------------------------------------
float valve_iolink_mix_proof::get_value()
    {
    return 0.1f * in_info->pos;
    }
//-----------------------------------------------------------------------------
int valve_iolink_mix_proof::get_off_fb_value()
    {
    return out_info->sv1 == false && in_info->main && in_info->st;
    }
//-----------------------------------------------------------------------------
int valve_iolink_mix_proof::get_on_fb_value()
    {
    return out_info->sv1 == true && in_info->de_en && in_info->st;
    }
//-----------------------------------------------------------------------------
void valve_iolink_mix_proof::direct_on()
    {
    if ( false == in_info->main )
        {
        start_switch_time = get_millisec();
        }

    out_info->sv1 = true;
    out_info->sv2 = false;
    out_info->sv3 = false;
    }
//-----------------------------------------------------------------------------
void valve_iolink_mix_proof::direct_off()
    {
    if ( out_info->sv1 || out_info->sv2 || out_info->sv3 )
        {
        start_switch_time = get_millisec();
        }

    out_info->sv1 = false;
    out_info->sv2 = false;
    out_info->sv3 = false;
    }
//-----------------------------------------------------------------------------
int valve_iolink_mix_proof::set_cmd( const char *prop, u_int idx, double val )
    {
    if (G_DEBUG)
        {
        sprintf( G_LOG->msg,
            "%s\t valve_iolink_mix_proof::set_cmd() - prop = %s, idx = %d, val = %f",
            get_name(), prop, idx, val);
        G_LOG->write_log(i_log::P_DEBUG);
        }

    switch ( prop[ 0 ] )
        {
        case 'B': //BLINK
            {
            val > 0 ? out_info->wink = true : out_info->wink = false;
            blink = out_info->wink;
            break;
            }

        default:
            valve::set_cmd( prop, idx, val );
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void valve_iolink_mix_proof::direct_set_state( int new_state )
    {
    switch ( new_state )
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

        case V_UPPER_SEAT:
            {
            direct_off();

            out_info->sv2 = true;
            break;
            }

        case V_LOWER_SEAT:
            {
            direct_off();

            out_info->sv3 = true;
            break;
            }

        default:
            direct_on();
            break;
        }
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iolink_shut_off_sorio::valve_iolink_shut_off_sorio( const char* dev_name ) :
    valve( true, true, dev_name, DT_V, V_IOLINK_DO1_DI2 )
    {
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_iolink_shut_off_sorio::get_valve_state()
    {
#ifdef DEBUG_NO_IO_MODULES
    return (VALVE_STATE)digital_io_device::get_state();
#else

    if ( in_info->de_en ) return V_OFF;
    if ( in_info->main ) return V_ON;

    return V_OFF;
#endif // DEBUG_NO_IO_MODULES
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_sorio::evaluate_io()
    {
    out_info = (out_data_swapped*)get_AO_write_data( 0 );

    char* data = (char*)get_AI_data( 0 );
    char* buff = (char*)in_info;

    const int SIZE = 4;
    std::copy( data, data + SIZE, buff );
    std::swap( buff[ 0 ], buff[ 1 ] );
    std::swap( buff[ 2 ], buff[ 3 ] );

//#define DEBUG_IOLINK_
#ifdef DEBUG_IOLINK_
    char* tmp = (char*)in_info;

    sprintf( G_LOG->msg, "%x %x %x %x\n",
        tmp[ 0 ], tmp[ 1 ], tmp[ 2 ], tmp[ 3 ] );
    G_LOG->write_log( i_log::P_WARNING );

    sprintf( G_LOG->msg,
        "de_en %u, main %u, usl %u, lsp %u, pos %.1f\n",
        in_info->de_en, in_info->main, in_info->usl,
        in_info->lsp, 0.1 * in_info->pos );
    G_LOG->write_log( i_log::P_NOTICE );
#endif
    }
//-----------------------------------------------------------------------------
int valve_iolink_shut_off_sorio::save_device_ex( char* buff )
    {
    bool cs = out_info->sv1;
    int err = in_info->err;

    int res = sprintf( buff, "BLINK=%d, CS=%d, ERR=%d, ", blink, cs, err );
    res += sprintf( buff + res, "V=%.1f, ", get_value() );

    return res;
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES
bool valve_iolink_shut_off_sorio::get_fb_state()
    {
    u_long dt = get_delta_millisec( start_switch_time );
    if ( dt < get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    if ( out_info->sv1 == false && in_info->de_en && in_info->sv1 ) return true;
    if ( out_info->sv1 == true && in_info->main && in_info->sv1 ) return true;

    return false;
    }
//-----------------------------------------------------------------------------
float valve_iolink_shut_off_sorio::get_value()
    {
    return 0.1f * in_info->pos;
    }
//-----------------------------------------------------------------------------
int valve_iolink_shut_off_sorio::get_off_fb_value()
    {
    return !in_info->sv1;
    }
//-----------------------------------------------------------------------------
int valve_iolink_shut_off_sorio::get_on_fb_value()
    {
    return in_info->sv1;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_sorio::direct_on()
    {
    if ( false == in_info->main )
        {
        start_switch_time = get_millisec();
        }

    out_info->sv1 = true;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_sorio::direct_off()
    {
    if ( out_info->sv1 )
        {
        start_switch_time = get_millisec();
        }

    out_info->sv1 = false;
    }
//-----------------------------------------------------------------------------
int valve_iolink_shut_off_sorio::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_DEBUG )
        {
        sprintf( G_LOG->msg,
            "%s\t valve_iolink_mix_proof::set_cmd() - prop = %s, idx = %d, val = %f",
            get_name(), prop, idx, val );
        G_LOG->write_log( i_log::P_DEBUG );
        }

    switch ( prop[ 0 ] )
        {
        case 'B': //BLINK
            {
            val > 0 ? out_info->wink = true : out_info->wink = false;
            blink = out_info->wink;
            break;
            }

        default:
            valve::set_cmd( prop, idx, val );
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_sorio::direct_set_state( int new_state )
    {
    switch ( new_state )
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

       default:
            direct_on();
            break;
        }
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iolink_shut_off_thinktop::valve_iolink_shut_off_thinktop( const char* dev_name ) :
    valve( true, true, dev_name, DT_V, V_IOLINK_DO1_DI2 )
    {
    in_info->err = 0;
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_iolink_shut_off_thinktop::get_valve_state()
    {
#ifdef DEBUG_NO_IO_MODULES
    return (VALVE_STATE)digital_io_device::get_state();
#else

    if (in_info->de_en) return V_OFF;
    if (in_info->main) return V_ON;

    return V_OFF;
#endif // DEBUG_NO_IO_MODULES
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_thinktop::evaluate_io()
    {
    out_info = (out_data_swapped*)get_AO_write_data(
        static_cast<u_int>( CONSTANTS::C_AI_INDEX ) );
    if ( extra_offset < 0 )
        {
        out_info = (out_data_swapped*)( (char*)out_info + extra_offset );
        }

    char* data = (char*)get_AI_data(
        static_cast<u_int>( CONSTANTS::C_AI_INDEX ) );
    char* buff = (char*)in_info;

    const int SIZE = 4;
    std::copy( data, data + SIZE, buff );
    std::swap( buff[ 0 ], buff[ 1 ] );
    std::swap( buff[ 2 ], buff[ 3 ] );

    //#define DEBUG_IOLINK_
#ifdef DEBUG_IOLINK_
    char* tmp = (char*)in_info;

    sprintf( G_LOG->msg, "%x %x %x %x\n",
        tmp[ 0 ], tmp[ 1 ], tmp[ 2 ], tmp[ 3 ] );
    G_LOG->write_log( i_log::P_WARNING );

    sprintf( G_LOG->msg,
        "de_en %u, main %u, usl %u, lsp %u, pos %.1f\n",
        in_info->de_en, in_info->main, in_info->usl,
        in_info->lsp, 0.1 * in_info->pos );
    G_LOG->write_log( i_log::P_NOTICE );
#endif
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_thinktop::set_rt_par( u_int idx, float value )
    {
    switch ( idx )
        {
        case 1:
            extra_offset = (int)value;
            break;

        default:
            valve::set_rt_par( idx, value );
            break;
        }
    }
//-----------------------------------------------------------------------------
int valve_iolink_shut_off_thinktop::save_device_ex( char* buff )
    {
    bool cs = out_info->sv1;
    int err = in_info->err;

    int res = sprintf( buff, "BLINK=%d, CS=%d, ERR=%d, ", blink, cs, err );
    res += sprintf( buff + res, "V=%.1f, ", get_value() );

    return res;
    }
//-----------------------------------------------------------------------------
valve_iolink_shut_off_thinktop::~valve_iolink_shut_off_thinktop()
    {
    delete in_info;
    in_info = nullptr;
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES
bool valve_iolink_shut_off_thinktop::get_fb_state()
    {
    if ( get_AI_IOLINK_state( static_cast<u_int>( CONSTANTS::C_AI_INDEX ) ) !=
        io_device::IOLINKSTATE::OK )
        {
        return false;
        }

    if ( in_info->err ) return false;

    if (get_delta_millisec( start_switch_time ) <
        get_par( valve::P_ON_TIME, 0 ))
        {
        return true;
        }

    if (out_info->sv1 == false && in_info->de_en && in_info->st) return true;
    if (out_info->sv1 == true && in_info->main && in_info->st) return true;

    return false;
    }
//-----------------------------------------------------------------------------
float valve_iolink_shut_off_thinktop::get_value()
    {
    return 0.1f * in_info->pos;
    }
//-----------------------------------------------------------------------------
int valve_iolink_shut_off_thinktop::get_off_fb_value()
    {
    return out_info->sv1 == false && in_info->main && in_info->st;
    }
//-----------------------------------------------------------------------------
int valve_iolink_shut_off_thinktop::get_on_fb_value()
    {
    return out_info->sv1 == true && in_info->de_en && in_info->st;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_thinktop::direct_on()
    {
    if (false == in_info->main)
        {
        start_switch_time = get_millisec();
        }

    out_info->sv1 = true;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_thinktop::direct_off()
    {
    if (out_info->sv1)
        {
        start_switch_time = get_millisec();
        }

    out_info->sv1 = false;
    }
//-----------------------------------------------------------------------------
int valve_iolink_shut_off_thinktop::set_cmd( const char* prop, u_int idx, double val )
    {
    if (G_DEBUG)
        {
        sprintf( G_LOG->msg,
            "%s\t valve_iolink_mix_proof::set_cmd() - prop = %s, idx = %d, val = %f",
            get_name(), prop, idx, val );
        G_LOG->write_log( i_log::P_DEBUG );
        }

    switch (prop[ 0 ])
        {
        case 'B': //BLINK
        {
        val > 0 ? out_info->wink = true : out_info->wink = false;
        blink = out_info->wink;
        break;
        }

        default:
            valve::set_cmd( prop, idx, val );
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void valve_iolink_shut_off_thinktop::direct_set_state( int new_state )
    {
    switch (new_state)
        {
        case V_OFF:
            direct_off();
            break;

        case V_ON:
            direct_on();
            break;

        default:
            direct_on();
            break;
        }
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iolink_vtug::valve_iolink_vtug( const char* dev_name,
    device::DEVICE_SUB_TYPE sub_type ) : valve( dev_name, DT_V, sub_type ),
    vtug_number( 0 )
    {
    }
//-----------------------------------------------------------------------------
valve_iolink_vtug::valve_iolink_vtug( bool is_on_fb, bool is_off_fb,
    const char* dev_name, device::DEVICE_SUB_TYPE sub_type ) :
    valve( is_on_fb, is_off_fb, dev_name, DT_V, sub_type ),
    vtug_number( 0 )
    {
    }
//-----------------------------------------------------------------------------
void valve_iolink_vtug::set_rt_par( u_int idx, float value )
    {
    switch ( idx )
        {
        case 1:
            vtug_number = (u_int)value;
            break;

        case 2:
            vtug_io_size = (u_int)value;
            break;

        default:
            valve::set_rt_par( idx, value );
            break;
        }
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES
void valve_iolink_vtug::direct_on()
    {
    char* data = (char*)get_AO_write_data( AO_INDEX );
    char read_state = get_state_data( data );

    if ( 0 == read_state )
        {
        start_switch_time = get_millisec();
        }

    if ( !data || !vtug_number )
        {
        return;
        }

    u_int offset = ( vtug_number - 1 ) / 8;
    data[ offset ] |= 1 << ( ( vtug_number - 1 ) % 8 );
    }
//-----------------------------------------------------------------------------
void valve_iolink_vtug::direct_off()
    {
    char* data = (char*)get_AO_write_data( AO_INDEX );
    char read_state = get_state_data( data );

    if ( 1 == read_state )
        {
        start_switch_time = get_millisec();
        }

    if ( !data || !vtug_number )
        {
        return;
        }

    u_int offset = ( vtug_number - 1 ) / 8;
    data[ offset ] &= ~( 1 << ( ( vtug_number - 1 ) % 8 ) );
    }
//-----------------------------------------------------------------------------
int valve_iolink_vtug::get_state()
    {
    if ( get_AO_IOLINK_state( 0 ) != io_device::IOLINKSTATE::OK )
        {
        return -1;
        }
    else
        {
        return valve::get_state();
        }
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
char valve_iolink_vtug::get_state_data( char* data )
    {
    if ( !data || !vtug_number )
        {
        return 0;
        }

    u_int offset = ( vtug_number - 1 ) / 8;
    char state = data[ offset ];
    state >>= ( vtug_number - 1 ) % 8;
    state &= 1;

    return state;
    }
//-----------------------------------------------------------------------------
valve::VALVE_STATE valve_iolink_vtug::get_valve_state()
    {
#ifdef DEBUG_NO_IO_MODULES
    return (VALVE_STATE)digital_io_device::get_state();
#else
    char* data = (char*)get_AO_read_data( AO_INDEX );
    char state = get_state_data( data );

    return (VALVE_STATE)state;
#endif // DEBUG_NO_IO_MODULES
    }
//-----------------------------------------------------------------------------
/// @brief Получение состояния обратной связи.
bool valve_iolink_vtug::get_fb_state()
    {
    return true;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iolink_vtug_on::valve_iolink_vtug_on(const char* dev_name) :
    valve_iolink_vtug( true, false, dev_name, V_IOLINK_VTUG_DO1_FB_ON)
    {
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES
bool valve_iolink_vtug_on::get_fb_state()
    {
    char* data = (char*)get_AO_read_data(AO_INDEX);
    int o = (int)get_state_data(data);
    int i = get_DI(DI_INDEX);

    if (o == i)
        {
        start_switch_time = get_millisec();
        return true;
        }

    if (get_delta_millisec(start_switch_time) < get_par(valve::P_ON_TIME, 0))
        {
        return true;
        }

    return false;
    }
//-----------------------------------------------------------------------------
int valve_iolink_vtug_on::get_on_fb_value()
    {
    return get_DI(DI_INDEX);
    }
//-----------------------------------------------------------------------------
inline int valve_iolink_vtug_on::get_off_fb_value()
    {
    return false;
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iolink_vtug_off::valve_iolink_vtug_off(const char* dev_name) :
    valve_iolink_vtug(false, true, dev_name, V_IOLINK_VTUG_DO1_FB_OFF)
    {
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES
bool valve_iolink_vtug_off::get_fb_state()
    {
    char* data = (char*)get_AO_read_data(AO_INDEX);
    int o = (int)get_state_data(data);
    int i = get_DI(DI_INDEX);

    if (o != i)
        {
        start_switch_time = get_millisec();
        return true;
        }

    if (get_delta_millisec(start_switch_time) < get_par(valve::P_ON_TIME, 0))
        {
        return true;
        }

    return false;
    }
//-----------------------------------------------------------------------------
int valve_iolink_vtug_off::get_on_fb_value()
    {
    return false;
    }
//-----------------------------------------------------------------------------
inline int valve_iolink_vtug_off::get_off_fb_value()
    {
    return get_DI(DI_INDEX);
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
analog_valve_iolink::analog_valve_iolink( const char* dev_name ) : AO1(
    dev_name, DT_VC, DST_VC_IOLINK, 0 )
    {
    }

//-----------------------------------------------------------------------------
analog_valve_iolink::~analog_valve_iolink()
    {
    delete in_info;
    in_info = nullptr;
    }

//-----------------------------------------------------------------------------
void analog_valve_iolink::evaluate_io()
    {
    out_info = (out_data*)get_AO_write_data( AO_INDEX );

    char* data = (char*)get_AI_data( AO_INDEX );
    char* buff = (char*)in_info;

    const int SIZE = 10;
    std::copy( data, data + SIZE, buff );

#ifdef DEBUG_VC_IOLINK
    //Variants of float representation.
    int a1 = ( buff[ 0 ] << 24 ) + ( buff[ 1 ] << 16 ) + ( buff[ 2 ] << 8 ) + buff[ 3 ];
    int a2 = ( buff[ 1 ] << 24 ) + ( buff[ 0 ] << 16 ) + ( buff[ 3 ] << 8 ) + buff[ 2 ];
    int a3 = ( buff[ 2 ] << 24 ) + ( buff[ 3 ] << 16 ) + ( buff[ 1 ] << 8 ) + buff[ 0 ];
    int a4 = ( buff[ 3 ] << 24 ) + ( buff[ 2 ] << 16 ) + ( buff[ 0 ] << 8 ) + buff[ 1 ];
    float* f1 = (float*)&a1;
    float* f2 = (float*)&a2;
    float* f3 = (float*)&a3;
    float* f4 = (float*)&a4;
    sprintf( G_LOG->msg, "WARNING %s %f %f %f %f", get_name(),
        *f1, *f2, *f3, *f4 );
    G_LOG->write_log( i_log::P_WARNING );
#endif

    //Reverse byte order to get correct float.
    std::swap( buff[ 3 ], buff[ 0 ] );
    std::swap( buff[ 1 ], buff[ 2 ] );
    std::swap( buff[ 7 ], buff[ 4 ] );
    std::swap( buff[ 5 ], buff[ 6 ] );

#ifdef DEBUG_VC_IOLINK
    sprintf( G_LOG->msg,
        "closed %u, opened %u, status %u, NAMUR state %u, used setpoint %.3f, valve position %.3f\n",
        in_info->closed, in_info->opened, in_info->status, in_info->namur_state,
        in_info->setpoint, in_info->position );
    G_LOG->write_log( i_log::P_NOTICE );
#endif
    }
//-----------------------------------------------------------------------------
float analog_valve_iolink::get_min_value()
    {
    return static_cast<float>( CONSTANTS::FULL_CLOSED );
    }
//-----------------------------------------------------------------------------
float analog_valve_iolink::get_max_value()
    {
    return static_cast<float>( CONSTANTS::FULL_OPENED );
    }
//-----------------------------------------------------------------------------
int analog_valve_iolink::save_device_ex( char* buff )
    {
    int res = sprintf( buff, "NAMUR_ST=%u, ", in_info->namur_state );
    res += sprintf( buff + res, "OPENED=%u, ", in_info->opened );
    res += sprintf( buff + res, "CLOSED=%u, ", in_info->closed );

    res += sprintf( buff + res, "BLINK=%d, ", blink );
    return res;
    }
#ifndef DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
void analog_valve_iolink::direct_on()
    {
    direct_set_value( static_cast<float>( CONSTANTS::FULL_OPENED ) );
    }
//-----------------------------------------------------------------------------
void analog_valve_iolink::direct_off()
    {
    direct_set_value( static_cast<float>( CONSTANTS::FULL_CLOSED ) );
    }
//-----------------------------------------------------------------------------
void analog_valve_iolink::direct_set_value( float new_value )
    {
    out_info->position = new_value;

    char *buff = (char*) &out_info->position;
    std::swap( buff[ 3 ], buff[ 0 ] );//Reverse byte order to get correct float.
    std::swap( buff[ 1 ], buff[ 2 ] );
    }
//-----------------------------------------------------------------------------
float analog_valve_iolink::get_value()
    {
    return in_info->position;
    }
//-----------------------------------------------------------------------------
int analog_valve_iolink::get_state()
    {
    return in_info->status;
    }
//-----------------------------------------------------------------------------
inline int analog_valve_iolink::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( G_DEBUG )
        {
        sprintf( G_LOG->msg,
            "%s\t analog_valve_iolink::set_cmd() - prop = %s, idx = %d, val = %f",
            get_name(), prop, idx, val );
        G_LOG->write_log( i_log::P_DEBUG );
        }

    switch ( prop[ 0 ] )
        {
        case 'B': //BLINK
            {
            val > 0 ? out_info->wink = true : out_info->wink = false;
            blink = out_info->wink;
            break;
            }

        default:
            AO1::set_cmd( prop, idx, val );
            break;
        }

    return 0;
    }
#endif
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES
void DI1::direct_on()
    {
    }
//-----------------------------------------------------------------------------
void DI1::direct_off()
    {
    }
//-----------------------------------------------------------------------------
int DI1::get_state()
    {
    u_int_4 dt = ( u_int_4 ) get_par( P_DT, 0 );

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
#else
int DI1::get_state()
    {
    u_int_4 dt = ( u_int_4 ) get_par( P_DT, 0 );

    if ( dt > 0 )
        {
        if ( current_state != digital_io_device::get_state() )
            {
            if ( get_delta_millisec( time ) > dt  )
                {
                current_state = digital_io_device::get_state();
                time = get_millisec();
                }
            }
        else
            {
            time = get_millisec();
            }
        }
    else current_state = digital_io_device::get_state();

    return current_state;
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_iolink_vtug_DO2::valve_iolink_vtug_DO2( const char* dev_name ) :
    valve_iolink_vtug( true, false, dev_name, V_IOLINK_VTUG_DO1_DI2 )
    {
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES
bool valve_iolink_vtug_DO2::get_fb_state()
    {
    char* data = (char*)get_AO_read_data( AO_INDEX );
    int o = (int)get_state_data( data );

    int i1 = get_DI( DI_INDEX_ON );
    int i2 = get_DI( DI_INDEX_OFF );
    if ( o == 1 && i1 == 1 && i2 == 0 )
        {
        start_switch_time = get_millisec();
        return true;
        }

    if ( o == 0 && i2 == 1 && i1 == 0 )
        {
        start_switch_time = get_millisec();
        return true;
        }

    if ( get_delta_millisec( start_switch_time ) < get_par( valve::P_ON_TIME, 0 ) )
        {
        return true;
        }

    return false;
    }
//-----------------------------------------------------------------------------
int valve_iolink_vtug_DO2::get_on_fb_value()
    {
    return get_DI( DI_INDEX_ON );
    }
//-----------------------------------------------------------------------------
inline int valve_iolink_vtug_DO2::get_off_fb_value()
    {
    return get_DI( DI_INDEX_OFF );
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AI1::AI1( const char *dev_name, device::DEVICE_TYPE type,
    device::DEVICE_SUB_TYPE sub_type, u_int par_cnt ) : analog_io_device( dev_name, type, sub_type,
        par_cnt + ADDITIONAL_PARAM_COUNT )
#ifdef DEBUG_NO_IO_MODULES
        ,st( 1 )
#endif
    {
    set_par_name( P_ZERO_ADJUST_COEFF, 0, "P_CZ" );
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_IO_MODULES

float AI1::get_value()
    {
    return get_par( P_ZERO_ADJUST_COEFF, 0 ) + analog_io_device::get_value();
    }

#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES

float AI1::get_value()
    {
    return get_par( P_ZERO_ADJUST_COEFF, 0 ) +
        get_AI( C_AI_INDEX, get_min_val(), get_max_val() );
    }
//-----------------------------------------------------------------------------
void AI1::direct_set_value( float new_value )
    {
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
temperature_e_analog::temperature_e_analog( const char* dev_name ) :
    AI1( dev_name, DT_TE, DST_TE_ANALOG, LAST_PARAM_IDX - 1 )
    {
    start_param_idx = AI1::get_params_count();
    set_par_name( P_ERR_T, start_param_idx, "P_ERR_T" );
    set_par_name( P_MIN_V, start_param_idx, "P_MIN_V" );
    set_par_name( P_MAX_V, start_param_idx, "P_MAX_V" );
    }
//-----------------------------------------------------------------------------
float temperature_e_analog::get_value()
    {
#ifdef DEBUG_NO_IO_MODULES
    return analog_io_device::get_value();
#else
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
#endif
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES
int temperature_e_analog::get_state()
    {
    float v = get_AI( C_AI_INDEX, 0, 0 );
    if ( v < 0 )
        {
        return -1;
        }

    return 0;
    }
#endif
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
temperature_e_iolink::temperature_e_iolink( const char *dev_name ):
    AI1(dev_name, DT_TE, DST_TE_IOLINK, ADDITIONAL_PARAM_COUNT), info(new TE_data)
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
#ifndef DEBUG_NO_IO_MODULES
float temperature_e_iolink::get_value()
    {
    if (get_AI_IOLINK_state(C_AI_INDEX) != io_device::IOLINKSTATE::OK)
        {
        return get_par(P_ERR_T, start_param_idx);
        }
    else
        {
        char* data = (char*)get_AI_data(C_AI_INDEX);

        int16_t tmp = data[1] + 256 * data[0];
        memcpy(info, &tmp, 2);

        return get_par(P_ZERO_ADJUST_COEFF, 0) + 0.1f * info->v;
        }
    }
#else
//-----------------------------------------------------------------------------
float temperature_e_iolink::get_value()
   {
   return analog_io_device::get_value();
   }
#endif
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
    }

virtual_wages::virtual_wages( const char* dev_name ) :
    device( dev_name, device::DT_WT, device::DST_WT_VIRT, 0 ),
    value( 0 ), state( 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
wages_RS232::wages_RS232( const char* dev_name ) :
    analog_io_device( dev_name, device::DT_WT, device::DST_WT_RS232,
    static_cast<int>( CONSTANTS::LAST_PARAM_IDX ) - 1 ), state( 1 )
    {
    set_par_name( static_cast<int>( CONSTANTS::P_CZ ), 0, "P_CZ" );
    }

float wages_RS232::get_value_from_wages()
    {
    //Получение массива данных (1). Если данные пустые, вернуть старое
    //значение (2). Если 1 - буфер не пустой, переключиться в режим считывания
    //данных, вернуть старое значение (3). После переключения в режим
    //считывания данных получаем данные и обрабатываем (4)
    //Если данные корректные, в 4м бите символ "+" (ASCII - 43), иначе -
    //ошибка (5).

    char* data = (char*)get_AI_data(
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
    int* out =
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
#ifdef DEBUG_NO_IO_MODULES
    value = analog_io_device::get_value();
#else
    value = get_value_from_wages();
#endif
    }

#ifndef DEBUG_NO_IO_MODULES
void wages_RS232::direct_set_value( float new_value )
    {
    }
#endif // DEBUG_NO_IO_MODULES

void wages_RS232::tare()
    {
    //Этот метод нужен для тарировки весов (когда текущий вес устанавливается 
    //в качестве нулевого).
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
wages::wages( const char *dev_name ) : analog_io_device(
    dev_name, DT_WT, DST_NONE, ADDITIONAL_PARAM_COUNT )
    {
    set_par_name( P_NOMINAL_W,  0, "P_NOMINAL_W" );
    set_par_name( P_RKP, 0, "P_RKP");
    set_par_name( P_C0, 0, "P_CZ" );
    set_par_name( P_DT, 0, "P_DT");
    weight = 0;
    filter_time = get_millisec();
    }
//-----------------------------------------------------------------------------
void wages::tare()
    {
    set_par(P_C0, 0, -weight);
    return;
    }

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
#ifdef DEBUG_NO_IO_MODULES

float wages::get_value()
    {
    return weight + get_par(P_C0, 0);
    }
//-----------------------------------------------------------------------------
void wages::direct_set_value( float new_value )
    {
    weight = new_value;
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES

float wages::get_value()
    {
    return get_weight();
    }

void wages::direct_set_state( int new_state )
    {
    switch ( new_state )
        {
        case S_TARE:
            tare();
            break;
        }
    }

#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES

float AO1::get_value()
    {
    return get_AO( AO_INDEX, get_min_value(), get_max_value() );
    }
//-----------------------------------------------------------------------------
void AO1::direct_set_value( float new_value )
    {
    set_AO( AO_INDEX, new_value, get_min_value(), get_max_value() );
    }
#endif // DEBUG_NO_IO_MODULES
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
#ifndef DEBUG_NO_IO_MODULES

void valve_DO1::direct_on()
    {
    set_DO( DO_INDEX, 1 );
    }
//-----------------------------------------------------------------------------
void valve_DO1::direct_off()
    {
    set_DO( DO_INDEX, 0 );
    }

#endif // DEBUG_NO_IO_MODULES
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
float i_motor::get_amperage() const
    {
    return 0.0f;
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
    i_motor( dev_name, device::DST_M_VIRT, 0 ),
    value( 0 ),
    state( 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float motor::get_value()
    {
#ifdef DEBUG_NO_IO_MODULES
    return freq;
#else
    if ( sub_type == device::DST_M_FREQ || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        return get_AO( AO_INDEX, C_MIN_VALUE, C_MAX_VALUE );
        }

    return 0;
#endif // DEBUG_NO_IO_MODULES
    }
//-----------------------------------------------------------------------------
void motor::direct_set_value( float value )
    {
#ifdef DEBUG_NO_IO_MODULES
    freq = value;
#else
    if ( sub_type == device::DST_M_FREQ || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        set_AO( AO_INDEX, value, C_MIN_VALUE, C_MAX_VALUE );
        }
#endif // DEBUG_NO_IO_MODULES
    }
//-----------------------------------------------------------------------------
void motor::direct_set_state( int new_state )
    {
#ifdef DEBUG_NO_IO_MODULES
    if ( -1 == new_state )
        {
        state = ( char ) -1;
        return;
        }
#endif // DEBUG_NO_IO_MODULES

    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ )
        {
        if ( new_state == 2 )
            {
#ifdef DEBUG_NO_IO_MODULES
            state = 2;
#else
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
#endif // DEBUG_NO_IO_MODULES

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
#ifdef DEBUG_NO_IO_MODULES
            state = 2;
#else
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
#endif // DEBUG_NO_IO_MODULES

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
#ifdef DEBUG_NO_IO_MODULES
    return state;
#else
    int o = get_DO( DO_INDEX );

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
#endif // DEBUG_NO_IO_MODULES
    }
//-----------------------------------------------------------------------------
void motor::direct_on()
    {
#ifdef DEBUG_NO_IO_MODULES
    state = 1;
#else
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
#endif // DEBUG_NO_IO_MODULES
    }
//-----------------------------------------------------------------------------
void motor::direct_off()
    {
#ifdef DEBUG_NO_IO_MODULES
    state = 0;
#else
    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_millisec();
        set_DO( DO_INDEX, 0 );
        }

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
#endif // DEBUG_NO_IO_MODULES

    direct_set_value( 0 );
    }
//-----------------------------------------------------------------------------
int motor::save_device_ex( char *buff )
    {
    int res = 0;
#ifdef DEBUG_NO_IO_MODULES
    res = sprintf( buff, "R=0, " );
#else
    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ ||
        sub_type == device::DST_M_REV_2 || sub_type == device::DST_M_REV_FREQ_2 ||
        sub_type == device::M_REV_2_ERROR ||
        sub_type == device::DST_M_REV_FREQ_2_ERROR )
        {
        if ( sub_type == device::M_REV_2_ERROR ||
            sub_type == device::DST_M_REV_FREQ_2_ERROR )
            {
            res = sprintf( buff, "R=%d, ERR=%d, ",
                get_DO( DO_INDEX_REVERSE ), get_DI( DI_INDEX_ERROR ) );
            }
        else
            {
            res = sprintf( buff, "R=%d,", get_DO( DO_INDEX_REVERSE ) );
            }
        }
#endif //DEBUG_NO_IO_MODULES
    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool level_s::is_active()
    {
    switch ( sub_type )
        {
        case DST_LS_MIN:
            return get_state() == 0 ? 0 : 1;

        case DST_LS_MAX:
            return get_state() == 0 ? 1 : 0;

        default:
            return get_state() == 0 ? 0 : 1;
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
    current_state( sub_type == device::LS_IOLINK_MAX ? 1 : 0 ), time( 0 ),
    n_article( ARTICLE::DEFAULT ),
    v( 0 ), st( 0 )
    {
    set_par_name( P_DT, 0, "P_DT" );
    set_par_name( P_ERR, 0, "P_ERR" );
    }

void level_s_iolink::evaluate_io()
    {
    char* data = (char*)get_AI_data( C_AI_INDEX );

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

#ifndef DEBUG_NO_IO_MODULES
float level_s_iolink::get_value()
    {
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
	io_device::IOLINKSTATE devstate = get_AI_IOLINK_state(C_AI_INDEX);
	if (devstate != io_device::IOLINKSTATE::OK)
		{
		return sub_type == device::LS_IOLINK_MAX ? 1 : 0;
		}

    u_int_4 dt = (u_int_4)get_par( P_DT, 0 );
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
#endif

bool level_s_iolink::is_active()
    {
    return get_state();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
level_e_iolink::level_e_iolink( const char *dev_name ) :
    level( dev_name, DST_LT_IOLINK, LAST_PARAM_IDX - 1 ),
    n_article( pressure_e_iolink::ARTICLE::DEFAULT ), v( 0 ), st( 0 ),
    PT_extra( 0 )
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
#ifndef DEBUG_NO_IO_MODULES
    v = this->v;
#else
    v = get_value();
    v = v / 100 * get_par(P_MAX_P, start_param_idx);
#endif

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
#ifndef DEBUG_NO_IO_MODULES
float level_e_iolink::get_value()
    {
    if (get_AI_IOLINK_state(C_AI_INDEX) != io_device::IOLINKSTATE::OK)
        {
        return get_par( level::P_ERR, level::start_param_idx );
        }
    else
        {
        return v / get_par( P_MAX_P, start_param_idx ) * 100;
        }
    }
//-----------------------------------------------------------------------------
int level_e_iolink::get_state()
    {
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
#endif
//-----------------------------------------------------------------------------
void level_e_iolink::set_article( const char* new_article )
    {
    device::set_article( new_article );
    pressure_e_iolink::read_article( new_article, n_article, this );
    }
//-----------------------------------------------------------------------------
void level_e_iolink::evaluate_io()
    {
    char* data = (char*)get_AI_data( C_AI_INDEX );
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
    analog_io_device( dev_name, DT_PT, DST_PT_IOLINK, LAST_PARAM_IDX - 1 ),
    n_article( ARTICLE::DEFAULT ), v( 0 ), st( 0 )
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
    ARTICLE& n_article, device* dev )
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

    //Debug print.
    //G_LOG->debug("%s v=%f, st=%d, %d %d %d %d", name, v, st,
    //    data[ 0 ], data[ 1 ], data[ 2 ], data[ 3 ] );
    }
//-----------------------------------------------------------------------------
void pressure_e_iolink::evaluate_io()
    {
    evaluate_io( get_name(), (char*)get_AI_data( C_AI_INDEX ), n_article, v, st );
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES

float pressure_e_iolink::get_value()
    {
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

#endif
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
circuit_breaker::circuit_breaker( const char* dev_name ):analog_io_device(
    dev_name, DT_F, DST_F, 0), is_read_OK( false ), v( 0 ), st( 0 ),
    err( 0 ), m( 0 ), in_info{}, out_info( new F_data_out() )
    {
    }
//-----------------------------------------------------------------------------
int circuit_breaker::save_device_ex( char *buff )
    {
    int res = sprintf( buff, "ERR=%d,M=%d, ", err, m );

    res += sprintf( buff + res, "NOMINAL_CURRENT_CH={%d,%d,%d,%d}, ",
        in_info.nominal_current_ch1, in_info.nominal_current_ch2,
        in_info.nominal_current_ch3, in_info.nominal_current_ch4 );
    res += sprintf( buff + res, "LOAD_CURRENT_CH={%.1f,%.1f,%.1f,%.1f}, ",
        .1f * in_info.load_current_ch1, .1f * in_info.load_current_ch2,
        .1f * in_info.load_current_ch3, .1f * in_info.load_current_ch4 );

    res += sprintf( buff + res, "ST_CH={%d,%d,%d,%d}, ",
        in_info.st_ch1, in_info.st_ch2,
        in_info.st_ch3, in_info.st_ch4 );
    res += sprintf( buff + res, "ERR_CH={%d,%d,%d,%d}, ",
        in_info.err_ch1, in_info.err_ch2,
        in_info.err_ch3, in_info.err_ch4 );
    return res;
    }
//-----------------------------------------------------------------------------
int circuit_breaker::set_cmd( const char *prop, u_int idx, double val )
    {
    if (G_DEBUG)
        {
        sprintf( G_LOG->msg,
            "%s\t circuit_breaker::set_cmd() - prop = %s, idx = %d, val = %f",
            get_name(), prop, idx, val);
        G_LOG->write_log(i_log::P_DEBUG);
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
#ifdef DEBUG_NO_IO_MODULES
                in_info.st_ch1 = val;
#endif
                return 0;
            case 2:
                out_info->switch_ch2 = val;
#ifdef DEBUG_NO_IO_MODULES
                in_info.st_ch2 = val;
#endif
                return 0;
            case 3:
                out_info->switch_ch3 = val;
#ifdef DEBUG_NO_IO_MODULES
                in_info.st_ch3 = val;
#endif
                return 0;
            case 4:
                out_info->switch_ch4 = val;
#ifdef DEBUG_NO_IO_MODULES
                in_info.st_ch4 = val;
#endif
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

#ifdef DEBUG_NO_IO_MODULES
    in_info.st_ch1 = true;
    in_info.st_ch2 = true;
    in_info.st_ch3 = true;
    in_info.st_ch4 = true;
#endif
    }
//-----------------------------------------------------------------------------
void circuit_breaker::direct_off()
    {
    out_info->valid_flag = true;
    out_info->switch_ch1 = false;
    out_info->switch_ch2 = false;
    out_info->switch_ch3 = false;
    out_info->switch_ch4 = false;

#ifdef DEBUG_NO_IO_MODULES
    in_info.st_ch1 = false;
    in_info.st_ch2 = false;
    in_info.st_ch3 = false;
    in_info.st_ch4 = false;
#endif
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES
float circuit_breaker::get_value()
    {
    return v;
    }
//-----------------------------------------------------------------------------
int circuit_breaker::get_state()
    {
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
#endif
//-----------------------------------------------------------------------------
void circuit_breaker::evaluate_io()
    {
    out_info = ( F_data_out* ) get_AO_write_data( 0 );

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

    char* data = (char*)get_AI_data(C_AI_INDEX);
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
concentration_e_iolink::concentration_e_iolink( const char* dev_name ) :
    analog_io_device( dev_name,
    DT_QT, DST_QT_IOLINK, LAST_PARAM_IDX - 1 ),
    info( new QT_data )
    {
    set_par_name( P_ERR, 0, "P_ERR" );
    };
//-----------------------------------------------------------------------------
concentration_e_iolink::~concentration_e_iolink()
    {
    delete info;
    info = 0;
    }
//-----------------------------------------------------------------------------
int concentration_e_iolink::save_device_ex( char *buff )
    {
    int res = sprintf( buff, "T=%.1f, ", get_temperature() );

    return res;
    }
//-----------------------------------------------------------------------------
float concentration_e_iolink::get_temperature() const
    {
    return 0.1f * info->temperature;
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_IO_MODULES
float concentration_e_iolink::get_value()
    {
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
#endif
//-----------------------------------------------------------------------------
void concentration_e_iolink::evaluate_io()
    {
    char* data = (char*)get_AI_data(0);

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
void analog_io_device::direct_set_state( int new_state )
    {
    direct_set_value( ( float ) new_state );
    }
//-----------------------------------------------------------------------------
int analog_io_device::get_state()
    {
    return ( int ) get_value();
    }
//-----------------------------------------------------------------------------
void analog_io_device::print() const
    {
    device::print();
    //io_device::print();
    }
//-----------------------------------------------------------------------------
void analog_io_device::direct_on()
    {
    }
//-----------------------------------------------------------------------------
void analog_io_device::direct_off()
    {
    direct_set_value( 0 );
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_IO_MODULES

float analog_io_device::get_value()
    {
    return value;
    }
//-----------------------------------------------------------------------------
void analog_io_device::direct_set_value( float new_value )
    {
    value = new_value;
    }
#endif // DEBUG_NO_IO_MODULES
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
timer::timer(): last_time( 0 ),
    work_time( 0 ),
    state( STATE::S_STOP ),
    countdown_time( 0 )
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
timer_manager::timer_manager( u_int timers_count ) : timers_cnt( timers_count ),
    timers( 0 )
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
//-----------------------------------------------------------------------------
device_manager* G_DEVICE_MANAGER()
    {
    return device_manager::get_instance();
    }
//-----------------------------------------------------------------------------
i_DO_device* V( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "V%d", dev_n );

    return G_DEVICE_MANAGER()->get_V( name );
    }

valve* V( const char *dev_name )
    {
    return ( valve* ) G_DEVICE_MANAGER()->get_V( dev_name );
    }
//-----------------------------------------------------------------------------
i_AO_device* VC( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_VC( dev_name );
    }
//-----------------------------------------------------------------------------
i_motor* M( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "M%d", dev_n );

    return G_DEVICE_MANAGER()->get_M( name );
    }

i_motor* M( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_M( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* LS( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "LS%d", dev_n );

    return G_DEVICE_MANAGER()->get_LS( name );
    }

i_DI_device* LS( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_LS( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* FS( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "FS%d", dev_n );

    return G_DEVICE_MANAGER()->get_FS( name );
    }

i_DI_device* FS( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_FS( dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* AI( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_AI( dev_name );
    }
//-----------------------------------------------------------------------------
i_AO_device* AO( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "AO%d", dev_n );

    return G_DEVICE_MANAGER()->get_AO( name );
    }

i_AO_device* AO( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_AO( dev_name );
    }
//-----------------------------------------------------------------------------
i_counter* FQT( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "FQT%d", dev_n );

    return G_DEVICE_MANAGER()->get_FQT( name );
    }

i_counter* FQT( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_FQT( dev_name );
    }

virtual_counter* virtual_FQT( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_virtual_FQT( dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* TE( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "TE%d", dev_n );

    return G_DEVICE_MANAGER()->get_TE( name );
    }

i_AI_device* TE( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_TE( dev_name );
    }
//-----------------------------------------------------------------------------
level* LT( const char *dev_name )
    {
    return ( level* ) G_DEVICE_MANAGER()->get_LT( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* GS( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_GS( dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* HA( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_HA( dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_device* HL( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_HL( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* SB( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_SB( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* DI( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_DI( dev_name );
    }

i_DI_device* DI( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "DI%d", dev_n );
    return G_DEVICE_MANAGER()->get_DI( name );
    }

//-----------------------------------------------------------------------------
i_DO_device* DO( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_DO( dev_name );
    }

i_DO_device* DO( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "DO%d", dev_n );
    return G_DEVICE_MANAGER()->get_DO( name );
    }
//-----------------------------------------------------------------------------
i_AI_device* QT( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "QT%d", dev_n );

    return G_DEVICE_MANAGER()->get_QT( name );
    }

i_AI_device* QT( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_QT( dev_name );
    }
//-----------------------------------------------------------------------------
i_AI_device* PT( u_int dev_n )
    {
    static char name[device::C_MAX_NAME] = "";
    snprintf( name, device::C_MAX_NAME, "PT%d", dev_n );

    return G_DEVICE_MANAGER()->get_PT( name );
    }

i_AI_device* PT( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_PT( dev_name );
    }
//-----------------------------------------------------------------------------
wages* WT( u_int dev_n )
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf( name, device::C_MAX_NAME, "WT%d", dev_n );

    return G_DEVICE_MANAGER()->get_WT( name );
    }

wages* WT( const char *dev_name )
    {
    return G_DEVICE_MANAGER()->get_WT( dev_name );
    }
//-----------------------------------------------------------------------------
i_DO_AO_device* F(u_int dev_n)
    {
    static char name[ device::C_MAX_NAME ] = "";
    snprintf(name, sizeof(name), "F%d", dev_n);

    return G_DEVICE_MANAGER()->get_F(name);
    }

i_DO_AO_device* F(const char* dev_name)
    {
    return G_DEVICE_MANAGER()->get_F(dev_name);
    }
//-----------------------------------------------------------------------------
PID* C( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_C( dev_name );
    }
//-----------------------------------------------------------------------------
signal_column* HLA( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_HLA( dev_name );
    }
//-----------------------------------------------------------------------------
camera* CAM( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_CAM( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* PDS( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_PDS( dev_name );
    }
//-----------------------------------------------------------------------------
i_DI_device* TS( const char* dev_name )
    {
    return G_DEVICE_MANAGER()->get_TS( dev_name );
    }
//-----------------------------------------------------------------------------
dev_stub* STUB()
    {
    return G_DEVICE_MANAGER()->get_stub();
    }
//-----------------------------------------------------------------------------
device* DEVICE( int s_number )
    {
    return G_DEVICE_MANAGER()->get_device( s_number );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_AS::valve_AS( const char *dev_name, DEVICE_SUB_TYPE sub_type ):
    valve( true, true, dev_name, DT_V, sub_type ),
    AS_number( 0 )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_AS_mix_proof::valve_AS_mix_proof( const char *dev_name ):
    valve_AS( dev_name, DST_V_AS_MIXPROOF )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_AS_DO1_DI2::valve_AS_DO1_DI2( const char *dev_name ):
    valve_AS( dev_name, DST_V_AS_DO1_DI2 ),
    start_err_time( get_millisec() )
    {
    }
//---------------------------------------------------------------------------
void valve_AS_DO1_DI2::direct_set_state(int new_state)
    {
    switch ( new_state )
        {
        case (int) VALVE_STATE::V_UPPER_SEAT:
        case (int) VALVE_STATE::V_LOWER_SEAT:
            //Ничего не делаем, так как нет седел.
            break;

        default:
            valve_AS::direct_set_state( new_state );
            break;
        }
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
    value = 0;
    state = 0;
    level_logic_invert = false;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
virtual_counter::virtual_counter( const char *dev_name ) :
    device( dev_name, DT_FQT, DST_FQT_VIRT, 0 ),
    last_read_value( 0 ),
    abs_last_read_value( 0 ),
    state( STATES::S_WORK ),
    flow_value( 0 ),
    value( 0 ),
    abs_value( 0 ),
    is_first_read( true )
    {
    }
//-----------------------------------------------------------------------------
float virtual_counter::get_value()
    {
    return (float)get_quantity();
    }
//-----------------------------------------------------------------------------
void virtual_counter::direct_set_value( float new_value )
    {
    value = (u_int)new_value;
    }
//-----------------------------------------------------------------------------
int virtual_counter::get_state()
    {
    return (int) state;
    }
//-----------------------------------------------------------------------------
void virtual_counter::direct_on()
    {
    start();
    }

void  virtual_counter::direct_off()
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
        }
    }
//-----------------------------------------------------------------------------
void virtual_counter::pause()
    {
    state = STATES::S_PAUSE;
    }
//-----------------------------------------------------------------------------
void virtual_counter::start()
    {
    if ( STATES::S_PAUSE == state )
        {
        state = STATES::S_WORK;
        }
    }
//-----------------------------------------------------------------------------
void virtual_counter::reset()
    {
    value = 0;
    }
//-----------------------------------------------------------------------------
u_int virtual_counter::get_quantity()
    {
    return value;
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
int virtual_counter::set_cmd( const char *prop, u_int idx, double val )
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
void virtual_counter::set( u_int value, u_int abs_value, float flow )
    {
    this->value = value;
    this->abs_value = abs_value;

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
            value += read_value - last_read_value;
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
int virtual_counter::save_device_ex( char *buff )
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
motor_altivar::motor_altivar( const char* dev_name,
    device::DEVICE_SUB_TYPE sub_type, u_int par_cnt ) :
    i_motor( dev_name, sub_type, par_cnt + ADDITIONAL_PARAM_COUNT ),
    io_device( dev_name ),
    start_switch_time( get_millisec() ),
    atv( nullptr )
    {
    set_par_name( P_ON_TIME, 0, "P_ON_TIME" );
    }

int motor_altivar::save_device_ex(char * buff)
    {
    int res = 0;
#ifdef DEBUG_NO_IO_MODULES
    res = sprintf( buff, "R=%d, FRQ=%.1f, RPM=%d, EST=%d, AMP=%.1f, MAX_FRQ=0.0, ",
        reverse, freq, rpm, est, amperage );
#else
    res = sprintf(buff, "R=%d, FRQ=%.1f, RPM=%d, EST=%d, AMP=%.1f, MAX_FRQ=%.1f, ",
        atv->reverse, atv->frq_value, atv->rpm_value, atv->remote_state, atv->amperage, atv->frq_max);
#endif //DEBUG_NO_IO_MODULES
    return res;
    }

float motor_altivar::get_value()
    {
#ifdef DEBUG_NO_IO_MODULES
    return freq;
#else
    return atv->get_output_in_percent();
#endif // DEBUG_NO_IO_MODULES
    }

void motor_altivar::direct_set_value(float value)
    {
#ifdef DEBUG_NO_IO_MODULES
    freq = value;
#else
    atv->set_output_in_percent( value );
#endif // DEBUG_NO_IO_MODULES
    }

void motor_altivar::direct_set_state(int new_state)
    {
#ifdef DEBUG_NO_IO_MODULES
    if (-1 == new_state)
        {
        state = (char)-1;
        return;
        }
#endif // DEBUG_NO_IO_MODULES


    if (new_state == 2)
        {
#ifdef DEBUG_NO_IO_MODULES
        state = 2;
#else
        atv->cmd = 2;
        atv->reverse = 1;
#endif // DEBUG_NO_IO_MODULES
        return;
        }

    if (new_state == 100)
        {
#ifndef DEBUG_NO_IO_MODULES
        atv->Disable();
        return;
#endif
        }

    if (new_state == 101)
        {
#ifndef DEBUG_NO_IO_MODULES
        atv->Enable();
        return;
#endif
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
#ifdef DEBUG_NO_IO_MODULES
    return state;
#else
    return atv->state;
#endif // DEBUG_NO_IO_MODULES
    }

void motor_altivar::direct_on()
    {
#ifdef DEBUG_NO_IO_MODULES
    state = 1;
#else
    atv->cmd = 1;
    atv->reverse = 0;
#endif // DEBUG_NO_IO_MODULES
    }

void motor_altivar::direct_off()
    {
#ifdef DEBUG_NO_IO_MODULES
    state = 0;
    freq = 0;
#else
    if (atv->state < 0)
        {
            atv->cmd = 4; //2 bit - fault reset
        }
    else
        {
        atv->cmd = 0;
        }
    atv->reverse = 0;
#endif // DEBUG_NO_IO_MODULES
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
        std::string nodeip = std::string(value);
        nodeip.append(":");
        nodeip.append(std::to_string(port));
        nodeip.append(" ");
        nodeip.append(std::to_string(timeout));
        if (!atv)
            {
            atv = G_ALTIVAR_MANAGER()->get_node(nodeip.c_str());
            if (!atv)
                {
                G_ALTIVAR_MANAGER()->add_node(value, port, timeout, article);
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
#ifdef DEBUG_NO_IO_MODULES
    return amperage;
#else
    return atv->amperage;
#endif
    }

#ifdef DEBUG_NO_IO_MODULES
int motor_altivar::set_cmd( const char* prop, u_int idx, double val )
    {
    printf( "motor_altivar::set_cmd() - prop = %s, idx = %d, val = %f\n",
        prop, idx, val );

    if ( strcmp( prop, "R" ) == 0 )
        {
        reverse = static_cast<int>( val );
        }
    else if ( strcmp( prop, "FRQ" ) == 0 )
        {
        freq = static_cast<float>( val );
        }
    else if ( strcmp( prop, "RPM" ) == 0 )
        {
        rpm = static_cast<int>( val );
        }
    else if ( strcmp( prop, "EST" ) == 0 )
        {
        est = static_cast<int>( val );
        }
    else if ( strcmp( prop, "AMP" ) == 0 )
        {
        amperage = static_cast<float>( val );
        }
    else
        {
        return device::set_cmd( prop, idx, val );
        }

    return 0;
    }
#endif // DEBUG_NO_IO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float motor_altivar_linear::get_linear_speed() const
    {
    float d = get_par( P_SHAFT_DIAMETER, start_param_idx );
    float n = get_par( P_TRANSFER_RATIO, start_param_idx );
    float v = .0f;

    if ( 0 != d && 0 != n )
        {
#ifdef DEBUG_NO_IO_MODULES
        v = ( rpm * (float)M_PI * d ) / ( n * SEC_IN_MIN );
#else
        v = ( atv->rpm_value * (float)M_PI * d ) / ( n * SEC_IN_MIN );
#endif
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
