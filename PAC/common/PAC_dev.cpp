#include "PAC_dev.h"
#include "tech_def.h"

#include "errors.h"
#include "lua_manager.h"

auto_smart_ptr < device_manager > device_manager::instance;

const char device::DEV_NAMES[][ 5 ] =
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
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void par_device::save_device ( char *str )
    {
    str[ 0 ] = 0;

    for ( u_int i = 1; i <= par.get_count(); i++ )
        {
        if ( par_name[ i - 1 ] )
            {
            sprintf( str + strlen( str ), "%s=", par_name[ i - 1 ] );

            float val =  par[ i ];
            if ( 0. == val )
                {
                sprintf( str + strlen( str ), "0, " );
                }
            else
                {
                double tmp;
                if ( modf( val, &tmp ) == 0 )
                    {
                    sprintf( str + strlen( str ), "%d, ", ( int ) val );
                    }
                else
                    {
                    sprintf( str + strlen( str ), "%.2f, ", val );
                    }
                }
            }
        }
    }
//-----------------------------------------------------------------------------
int par_device::set_cmd( const char *name, double val )
    {
    for ( u_int i = 0; i < par.get_count(); i++ )
        {
        if ( strcmp( par_name[ i ], name ) == 0 )
            {
            par[ i ] = ( float ) val;
#ifdef DEBUG
            Print( "par_device::set_cmd() - name = %s, val = %f.\n",
                name, val );
#endif // DEBUG
            return 0;
            }
        }

#ifdef DEBUG
    Print( "par_device::set_cmd() - name = %s wasn't found.\n",
        name );
#endif // DEBUG
    return 1;
    }
//-----------------------------------------------------------------------------
void par_device::set_par( u_int idx, u_int offset, float value )
    {
    par[ offset + idx ] = value;
    }
//-----------------------------------------------------------------------------
par_device::par_device ( u_int par_cnt ) : par ( saved_params_float ( par_cnt ) ),
    par_name ( 0 )
    {
    if ( par_cnt )
        {
        par_name = new char*[ par_cnt ];
        for ( u_int i = 0; i < par_cnt; i++ )
            {
            par_name[ i ] = 0;
            }
        }
    }
//-----------------------------------------------------------------------------
par_device::~par_device()
    {
    for (u_int i = 0; i < par.get_count(); i++)
        {
        delete par_name[ i ];
        par_name[ i ] = 0;
        }

    delete par_name;
    par_name = 0;
    }
//-----------------------------------------------------------------------------
float par_device::get_par( u_int idx, u_int offset )
    {
    return par[ offset + idx ];
    }
//-----------------------------------------------------------------------------
void par_device::set_par_name( u_int idx, u_int offset, const char* name )
    {
    if ( offset + idx <= par.get_count() && ( offset + idx > 0 ) )
        {
        if ( strlen( name ) > C_MAX_PAR_NAME_LENGTH )
            {
#ifdef DEBUG
            Print( "Error par_device::set_par_name( u_int idx, u_int offset, const char* name ) - "
                "name length (%d) > param C_MAX_PAR_NAME_LENGTH (%d).",
                strlen( name ), C_MAX_PAR_NAME_LENGTH );
#endif // DEBUG

            return;
            }

        if ( 0 == par_name[ offset + idx - 1 ] )
            {
            par_name[ offset + idx - 1 ] = new char[ strlen( name ) + 1 ];
            strcpy( par_name[ offset + idx - 1 ], name );
            }
#ifdef DEBUG
        else
            {
            Print( "Error par_device::set_par_name (u_int idx, u_int offset, const char* name) - "
                "param (%d %d) already has name (%s).",
                offset, idx, par_name[ offset + idx - 1 ] );
            }
#endif // DEBUG

        }
#ifdef DEBUG
    else
        {
        Print( "Error par_device::set_par_name (u_int idx, u_int offset, const char* name) - "
            "offset (%d) + idx (%d) > param count ( %d ).",
            offset, idx, par.get_count() );
        }
#endif // DEBUG
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void device::set_name( const char *new_name, const char *new_description )
    {
    name = new_name;
    description = new_description;
    }
//-----------------------------------------------------------------------------
void device::print() const
    {
#ifdef DEBUG
    if ( ( CONSTANTS ) type <= C_DEVICE_TYPE_CNT )
        {
        Print( "%s", DEV_NAMES[ type ] );
        }
    else
        {
        Print( "Unknown " );
        }
    Print( "%5lu\t", ( u_long ) number );

#endif // DEBUG
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
int device::save_device( char *buff, const char *prefix )
    {
    sprintf( buff, "%s%s={M=%d, ",
        prefix,  get_name(), is_manual_mode );

    if ( type != DT_AO &&
        type != DT_TE )
        {
        sprintf( buff + strlen( buff ), "ST=%d, ", get_state() );
        }

    if ( type != DT_V &&

        type != DT_LS &&
        type != DT_FS &&
        type != DT_GS &&

        type != DT_HA &&
        type != DT_HL &&
        type != DT_SB &&

        type != DT_DI &&
        type != DT_DO )
        {
        if ( get_value() == 0 )
            {
            sprintf( buff + strlen( buff ), "V=0, " );
            }
        else
            {
            sprintf( buff + strlen( buff ), "V=%.2f, ", get_value() );
            }
        }

    save_device_ex( buff + strlen( buff ) );

    par_device::save_device( buff + strlen( buff ) );
    buff[ strlen( buff ) - 2 ] = 0; //Убираем лишнюю последнюю запятую и пробел.

    sprintf( buff + strlen( buff ), "},\n" );

    return strlen( buff );
    }
//-----------------------------------------------------------------------------
int device::set_cmd( const char *prop, u_int idx, char *val )
    {
#ifdef DEBUG
    Print( "device::set_cmd() - prop = %s, idx = %d, val = %s\n",
        prop, idx, val );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int device::set_cmd( const char *prop, u_int idx, double val )
    {
#ifdef DEBUG
    Print( "device::set_cmd() - prop = %s, idx = %d, val = %f\n",
        prop, idx, val );
#endif // DEBUG

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

#ifdef DEBUG
        default:
            Print( "Error device::set_cmd() - prop = %s, val = %f\n",
                prop, val );
#endif // DEBUG
        }

    return 0;
    }
//-----------------------------------------------------------------------------
device::device( int number, DEVICE_TYPE type, DEVICE_SUB_TYPE sub_type,
               u_int par_cnt ) :
par_device( par_cnt ),
    number( number ),
    type( type ),
    sub_type( sub_type ),
    is_manual_mode( false )
    {
    }
//-----------------------------------------------------------------------------
device::~device()
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

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

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_V( int number )
    {
    return get_device( device::DT_V, number );
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
int device_manager::get_device_n( device::DEVICE_TYPE dev_type,
                                 u_int dev_number )
    {
    int l = dev_types_ranges[ dev_type ].start_pos;
    int u = dev_types_ranges[ dev_type ].end_pos;

    if ( -1 == l ) return -1; // Нет устройств.

    while ( l <= u )
        {
        int i = ( l + u ) / 2;

        if ( dev_number == project_devices[ i ]->get_n() )
            {
            return i;
            }

        if ( dev_number > project_devices[ i ]->get_n() )
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
device* device_manager::get_device( int dev_type,
                                   u_int dev_number )
    {
    int dev_n = get_device_n( ( device::DEVICE_TYPE ) dev_type, dev_number );

    if ( dev_n >= 0 )
        {
        try
            {
            return project_devices[ dev_n ];
            }
        catch (...)
            {
            }
        }
    else
        {
#ifdef DEBUG

        if ( dev_type <= device::C_DEVICE_TYPE_CNT )
            {
            Print( "%s ", device::DEV_NAMES[ dev_type ] );
            }
        else
            {
            Print( "Unknown" );
            }

        Print( "%5lu\t", ( u_long ) dev_number );
        Print( "not found!\n" );
#endif // DEBUG
        }

    return &stub;
    }
//-----------------------------------------------------------------------------
void device_manager::print() const
    {
    Print( "Device manager [%d]:\n", project_devices.size() );
    for ( u_int i = 0; i < project_devices.size(); i++ )
        {
        Print( "    %3i. ", i + 1 );        
        Print( "%s\t%s", 
            project_devices[ i ]->get_name(), project_devices[ i ]->get_description() );
        Print( "\n" );
        }
    }
//-----------------------------------------------------------------------------
device_manager::device_manager(): project_devices( 0 )
    {
    for ( int i = 0; i < device::C_DEVICE_TYPE_CNT; i++ )
        {
        dev_types_ranges[ i ].start_pos = -1;
        dev_types_ranges[ i ].end_pos = -1;
        }

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
i_AO_device* device_manager::get_VC( int number )
    {
    return get_device( device::DT_VC, number );
    }
//-----------------------------------------------------------------------------
i_DO_AO_device* device_manager::get_M( int number )
    {
    return get_device( device::DT_M, number );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_LS( int number )
    {
    return get_device( device::DT_LS, number );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_FS( int number )
    {
    return get_device( device::DT_FS, number );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_GS( int number )
    {
    return get_device( device::DT_GS, number );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_AI( int number )
    {
    return get_device( device::DT_AI, number );
    }
//-----------------------------------------------------------------------------
i_AO_device* device_manager::get_AO( int number )
    {
    return get_device( device::DT_AO, number );
    }
//-----------------------------------------------------------------------------
i_counter* device_manager::get_FQT( int number )
    {
    int res = get_device_n( device::DT_FQT, number );

    if ( res >= 0 ) return ( counter* ) project_devices.at( res );

    return &stub;
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_TE( int number )
    {
    return get_device( device::DT_TE, number );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_LT( int number )
    {
    return get_device( device::DT_LT, number );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_DI( int number )
    {
    return get_device( device::DT_DI, number );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_SB( int number )
    {
    return get_device( device::DT_SB, number );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_DO( int number )
    {
    return get_device( device::DT_DO, number );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_HA( int number )
    {
    return get_device( device::DT_HA, number );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_HL( int number )
    {
    return get_device( device::DT_HL, number );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_QT( int number )
    {
    return get_device( device::DT_QT, number );
    }
//-----------------------------------------------------------------------------
wago_device* device_manager::add_wago_device( int dev_type, int dev_sub_type,
                                             u_int number, char * descr )
    {
    static char is_first_device[ device::C_DEVICE_TYPE_CNT ] = { 0 };

    device* new_device = 0;
    wago_device* new_wago_device = 0;

    switch ( dev_type )
        {
        case device::DT_V:
            {
            switch ( dev_sub_type )
                {
                case device::DST_V_DO1:
                    new_device      = new valve_DO1( number );
                    new_wago_device = ( valve_DO1* ) new_device;
                    break;

                case device::DST_V_DO2:
                    new_device      = new valve_DO2( number );
                    new_wago_device = ( valve_DO2* ) new_device;
                    break;

                case device::DST_V_DO1_DI1_FB_OFF:
                    new_device      = new valve_DO1_DI1_off( number );
                    new_wago_device = ( valve_DO1_DI1_off* ) new_device;
                    break;

                case device::DST_V_DO1_DI1_FB_ON:
                    new_device      = new valve_DO1_DI1_on( number );
                    new_wago_device = ( valve_DO1_DI1_on* ) new_device;
                    break;

                case device::DST_V_DO1_DI2:
                    new_device      = new valve_DO1_DI2( number );
                    new_wago_device = ( valve_DO1_DI2* ) new_device;
                    break;

                case device::DST_V_DO2_DI2:
                    new_device      = new valve_DO2_DI2( number );
                    new_wago_device = ( valve_DO2_DI2* ) new_device;
                    break;

                case device::DST_V_MIXPROOF:
                    new_device      = new valve_mix_proof( number );
                    new_wago_device = ( valve_mix_proof* ) new_device;
                    break;

                default:
#ifdef DEBUG
                    Print( "Unknown V device subtype %d!\n", dev_sub_type );
                    get_char();
#endif // DEBUG
                    new_device      = new dev_stub();
                    break;
                }
            break;
            }

        case device::DT_VC:
            new_device      = new analog_valve( number );
            new_wago_device = ( analog_valve* ) new_device;
            break;

        case device::DT_M:
            new_device      = new motor( number,
                ( device::DEVICE_SUB_TYPE ) dev_sub_type );
            new_wago_device = ( motor* ) new_device;
            break;

        case device::DT_LS:
            new_device      = new level_s( number,
                ( device::DEVICE_SUB_TYPE ) dev_sub_type );
            new_wago_device = ( level_s* ) new_device;
            break;

        case device::DT_TE:
            new_device      = new temperature_e( number );
            new_wago_device = ( temperature_e* ) new_device;
            break;

        case device::DT_FS:
            new_device      = new flow_s( number );
            new_wago_device = ( flow_s* ) new_device;
            break;

        case device::DT_FQT:
            new_device      = new counter( number );
            new_wago_device = ( counter* ) new_device;
            break;

        case device::DT_AO:
            new_device      = new analog_output( number );
            new_wago_device = ( analog_output* ) new_device;
            break;

        case device::DT_LT:
            new_device      = new level_e( number );
            new_wago_device = ( level_e* ) new_device;
            break;

        case device::DT_DI:
            new_device      = new DI_signal( number );
            new_wago_device = ( DI_signal* ) new_device;
            break;

        case device::DT_DO:
            new_device      = new DO_signal( number );
            new_wago_device = ( DO_signal* ) new_device;
            break;

        case device::DT_QT:
            new_device      = new concentration_e( number );
            new_wago_device = ( concentration_e* ) new_device;
            break;

        case device::DT_AI:
            new_device      = new analog_input( number );
            new_wago_device = ( analog_input* ) new_device;
            break;

        case device::DT_HA:
            new_device      = new siren( number );
            new_wago_device = ( siren* ) new_device;
            break;

        case device::DT_HL:
            new_device      = new lamp( number );
            new_wago_device = ( lamp* ) new_device;
            break;

        case device::DT_SB:
            new_device      = new button( number );
            new_wago_device = ( button* ) new_device;
            break;

        case device::DT_GS:
            new_device      = new state_s( number );
            new_wago_device = ( state_s* ) new_device;
            break;

        default:
#ifdef DEBUG
            Print( "Unknown device type %d!\n", dev_type );
#endif // DEBUG
            new_device = new dev_stub();
            break;
        }

    // Ошибки.
    G_DEV_ERRORS_MANAGER->add_error( new simple_error( new_device ) );

    u_int new_dev_index = project_devices.size();
    project_devices.push_back( new_device );

    if ( dev_type < device::C_DEVICE_TYPE_CNT )
        {
        if ( 0 == is_first_device[ dev_type ] )
            {
            dev_types_ranges[ dev_type ].start_pos = new_dev_index;
            is_first_device[ dev_type ] = 1;
            }
        dev_types_ranges[ dev_type ].end_pos = new_dev_index;
        }

    return new_wago_device;
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
    sprintf( buff, "t=\n" );
    int answer_size = strlen( buff );
    sprintf( buff + answer_size, "\t{\n" );
    answer_size += strlen( buff + answer_size );

    for ( u_int i = 0; i < project_devices.size(); i++)
        {
        project_devices[ i ]->save_device( buff + answer_size, "\t" );
        answer_size += strlen( buff + answer_size );
        }

    sprintf( buff + answer_size, "\t}\n" );
    answer_size += strlen( buff + answer_size );

    return answer_size;
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
float dev_stub::get_value()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::direct_set_value( float new_value )
    {
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
u_int_4 dev_stub::get_n() const
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::print() const
    {
#ifdef DEBUG
    Print( "virtual device" );
#endif // DEBUG
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
//-----------------------------------------------------------------------------
float counter::get_value()
    {
    return ( float ) get_quantity();
    }
//-----------------------------------------------------------------------------
void counter::direct_set_value( float new_value )
    {
    value = ( u_int ) new_value;
    }
//-----------------------------------------------------------------------------
int counter::get_state()
    {
    if ( motors.size() > 0 )
        {
        char           is_pump_working         = 0;
        static u_int_4 start_pump_working_time = 0;
        static u_int_4 counter_prev_value      = 0;

        for ( u_int i = 0; i < motors.size(); i++ )
            {
            if ( motors[ i ]->get_state() == 1 )
                {
                is_pump_working = 1;
                if ( 0 == start_pump_working_time )
                    {
                    start_pump_working_time = get_millisec();
                    counter_prev_value      = get_quantity();
                    }
                }
            }

        if ( 0 == is_pump_working )
            {
            // Насос не работает.
            start_pump_working_time = 0;
            if ( state != S_PAUSE )
                {
                state = S_STOP;
                }
            }
        else
            {
            // Насос работает.
            if ( state == S_PAUSE )
                {
                start_pump_working_time = get_millisec();
                }
            else          // Работа.
                {
                state = S_WORK;

                if ( get_delta_millisec( start_pump_working_time ) > get_par( P_DT, 0 ) )
                    {
                    // Проверяем счетчик на ошибку - он должен изменить свои показания.
                    if ( get_quantity() == counter_prev_value )
                        {
                        state = S_ERROR;
                        }
                    else
                        {
                        start_pump_working_time = get_millisec();
                        counter_prev_value      = get_quantity();
                        }
                    }
                }
            }
        }// if ( motors.size() > 0

    return state;
    }
//-----------------------------------------------------------------------------
void counter::direct_on()
    {
    start();
    }
//-----------------------------------------------------------------------------
void counter::direct_off()
    {
    reset();
    }
//-----------------------------------------------------------------------------
void counter::direct_set_state( int new_state )
    {
    switch ( new_state )
        {
        case S_STOP:
            reset();
            break;

        case S_WORK:
            start();
            break;

        case S_PAUSE:
            pause();
            break;
        }
    }
//-----------------------------------------------------------------------------
void counter::print() const
    {
    device::print();
    //wago_device::print();
    }
//-----------------------------------------------------------------------------
void counter::pause()
    {
    get_quantity(); // Пересчитываем значение счетчика.

    state = S_PAUSE;
    }
//-----------------------------------------------------------------------------
void counter::start()
    {
    if ( S_STOP == state || S_PAUSE == state )
        {
        if ( S_STOP == state )
            {
            value = 0;
            }

        state = S_WORK;
        last_read_value = ( u_int ) get_AI( AI_Q_INDEX );
        }
    }
//-----------------------------------------------------------------------------
void counter::reset()
    {
    state = S_STOP;
    value = 0;
    }
//-----------------------------------------------------------------------------
u_int counter::get_quantity()
    {
    if ( S_WORK == state )
        {
        u_int delta;
        u_int current = ( u_int ) get_AI( AI_Q_INDEX );

        if ( current < last_read_value )
            {
            delta = MAX_VAL - last_read_value + current;
            }
        else
            {
            delta = current - last_read_value;
            }
        if ( delta > 0 )
            {
            value += delta;
            last_read_value = current;
            }
        }

    return value;
    }
//-----------------------------------------------------------------------------
float counter::get_flow()
    {
    return get_par( P_CZ, 0 ) +
#ifdef DEBUG_NO_WAGO_MODULES
        flow_value;
#else
        get_AI( AI_FLOW_INDEX, get_par( P_MIN_FLOW, 0 ), get_par( P_MAX_FLOW, 0 ) );
#endif // NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void counter::set_property( const char* field, device* dev )
    {
    if ( field && field[ 0 ] == 'M' ) //Связанные насосы.
        {
        motors.push_back( dev );
        }
    else
        {
        device::set_property( field, dev );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float digital_wago_device::get_value()
    {
    return ( float ) get_state();
    }
//-----------------------------------------------------------------------------
void digital_wago_device::direct_set_value( float new_value )
    {
    direct_set_state( ( int ) new_value );
    }
//-----------------------------------------------------------------------------
void digital_wago_device::direct_set_state( int new_state )
    {
#ifdef DEBUG_NO_WAGO_MODULES
    state = ( char ) new_state;    
#else
    if ( new_state )
        {
        direct_on();
        }
    else direct_off();
#endif //DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void digital_wago_device::print() const
    {
    device::print();
    //wago_device::print();
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_WAGO_MODULES
int digital_wago_device::get_state()
    {
    return state;
    }
//-----------------------------------------------------------------------------
void digital_wago_device::direct_on()
    {
    state = 1;
    }
//-----------------------------------------------------------------------------
void digital_wago_device::direct_off()
    {
    state = 0;
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
digital_wago_device::digital_wago_device( int number, device::DEVICE_TYPE type,
                                         device::DEVICE_SUB_TYPE sub_type, u_int par_cnt ) :
device( number, type, sub_type, par_cnt )
#ifdef DEBUG_NO_WAGO_MODULES
    , state( 0 )
#endif // DEBUG_NO_WAGO_MODULES
    {
    }
//-----------------------------------------------------------------------------
digital_wago_device::~digital_wago_device()
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

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

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve::valve( bool is_on_fb, bool is_off_fb, int number,
             device::DEVICE_TYPE type, device::DEVICE_SUB_TYPE sub_type ) :
digital_wago_device( number, type, sub_type, ADDITIONAL_PARAMS_COUNT ),
    is_on_fb( is_on_fb ),
    is_off_fb( is_off_fb ),
    on_fb( true ),
    off_fb( true )
    {
    set_par_name( P_ON_TIME, 0, "P_ON_TIME" );
    set_par_name( P_FB,  0, "P_FB" );    
    }
//-----------------------------------------------------------------------------
void valve::save_device_ex( char *buff )
    {
    if ( is_on_fb )
        {
        sprintf( buff, "FB_ON_ST=%d, ", get_on_fb_value() );
        }

    if ( is_off_fb )
        {
        sprintf( buff, "FB_OFF_ST=%d, ", get_off_fb_value() );
        }
    }
//-----------------------------------------------------------------------------
int valve::get_state()
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
            if ( is_off_fb || is_on_fb )
                {
                //Обратная связь отключена.
                if ( get_par( P_FB, 0 ) == FB_IS_AND_OFF )
                    {
                    if ( get_manual_mode() )
                        {
                        return VX_ON_FB_OFF_MANUAL;
                        }
                    else
                        {
                        return VX_ON_FB_OFF;
                        }
                    }
                }

            //Обратная связь не отключена.
            if ( get_manual_mode() )
                {
                if ( get_fb_state() == true )
                    {
                    return VX_ON_FB_OK_MANUAL;
                    }
                else
                    {
                    return VX_ON_FB_ERR_MANUAL;
                    }
                }// if ( get_manual_mode() )
            else
                {
                if ( get_fb_state() == true )
                    {
                    return VX_ON_FB_OK;
                    }
                else
                    {
                    return VX_ON_FB_ERR;
                    }
                }
            break;

        case V_OFF:
            if ( is_off_fb || is_on_fb )
                {
                //Обратная связь отключена.
                if ( get_par( P_FB, 0 ) == FB_IS_AND_OFF )
                    {
                    if ( get_manual_mode() )
                        {
                        return VX_OFF_FB_OFF_MANUAL;
                        }
                    else
                        {
                        return VX_OFF_FB_OFF;
                        }
                    }
                }

            //Обратная связь не отключена.
            if ( get_manual_mode() )
                {
                if ( get_fb_state() )
                    {
                    return VX_OFF_FB_OK_MANUAL;
                    }
                else
                    {
                    return VX_OFF_FB_ERR_MANUAL;
                    }
                }// if ( get_manual_mode() )
            else
                {
                if ( get_fb_state() == true )
                    {
                    return VX_OFF_FB_OK;
                    }
                else
                    {
                    return VX_OFF_FB_ERR;
                    }
                }
            break;
        }

    return VX_UNKNOWN;
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_WAGO_MODULES
int valve::set_cmd( const char *prop, u_int idx, double val )
    {
#ifdef DEBUG
    Print( "valve::set_cmd() - prop = %s, idx = %d, val = %f\n",
        prop, idx, val );
#endif // DEBUG

    switch ( prop[ 0 ] )
        {
        case 'F':
            if ( strcmp( prop, "FB_ON_ST" ) == 0 )
                {
                on_fb = val == .0;
                }
            else
                {
                off_fb = val == .0;
                }
            break;

        default:
            device::set_cmd( prop, idx, val );
        }

    return 0;
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_DO1_DI1_off::valve_DO1_DI1_off( int number ) : valve( false, true,
                                                           number, DT_V, DST_V_DO1_DI1_FB_OFF ),
                                                           start_switch_time( 0 )
    {
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES
void valve_DO1_DI1_off::direct_on()
    {
    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_sec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI1_off::direct_off()
    {
    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_sec();
        set_DO( DO_INDEX, 0 );
        }
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES
void valve_DO1_DI1_on::direct_on()
    {
    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_sec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI1_on::direct_off()
    {
    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_sec();
        set_DO( DO_INDEX, 0 );
        }
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

void valve_DO1_DI2::direct_on()
    {
    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_sec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_DO1_DI2::direct_off()
    {
    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_sec();
        set_DO( DO_INDEX, 0 );
        }
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

void valve_DO2_DI2::direct_on()
    {
    int o = get_DO( DO_INDEX_1 );
    if ( 0 == o )
        {
        start_switch_time = get_sec();
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
        start_switch_time = get_sec();
        set_DO( DO_INDEX_1, 0 );
        set_DO( DO_INDEX_2, 1 );
        }
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void valve_mix_proof::open_upper_seat()
    {
    direct_set_state( i_mix_proof::ST_UPPER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::open_lower_seat()
    {
    direct_set_state( i_mix_proof::ST_LOWER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::direct_set_state( int new_state )
    {
#ifdef DEBUG_NO_WAGO_MODULES
    state = ( char ) new_state;  
#else
    switch ( new_state )
        {
        case ST_CLOSE:
            direct_off();
            break;

        case ST_OPEN:
            direct_on();
            break;

        case ST_UPPER_SEAT:
            direct_off();
            set_DO( DO_INDEX_U, 1 );
            break;

        case ST_LOWER_SEAT:
            direct_off();
            set_DO( DO_INDEX_L, 1 );
            break;

        default:
            direct_on();
            break;
        }
#endif //DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

void valve_mix_proof::direct_on()
    {
    set_DO( DO_INDEX_U, 0 );
    set_DO( DO_INDEX_L, 0 );
    int o = get_DO( DO_INDEX );

    if ( 0 == o )
        {
        start_switch_time = get_sec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::direct_off()
    {
    set_DO( DO_INDEX_U, 0 );
    set_DO( DO_INDEX_L, 0 );
    int o = get_DO( DO_INDEX );

    if ( o != 0 )
        {
        start_switch_time = get_sec();
        set_DO( DO_INDEX, 0 );
        }
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

void DI1::direct_on()
    {
    }
//-----------------------------------------------------------------------------
void DI1::direct_off()
    {
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AI1::AI1( u_int number, device::DEVICE_TYPE type,
         device::DEVICE_SUB_TYPE sub_type, u_int par_cnt,
         u_int *start_par_idx ) : analog_wago_device( number, type, sub_type,
         par_cnt + ADDITIONAL_PARAM_COUNT )
    {
    if ( start_par_idx )
        {
        *start_par_idx = ADDITIONAL_PARAM_COUNT;
        }

    set_par_name( P_ZERO_ADJUST_COEFF,  0, "P_CZ" );
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_WAGO_MODULES

float AI1::get_value()
    {
    return get_par( P_ZERO_ADJUST_COEFF, 0 ) + analog_wago_device::get_value();
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

float AI1::get_value()
    {
    return get_par( P_ZERO_ADJUST_COEFF, 0 ) + get_AI( C_AI_INDEX, get_min_val(), get_max_val() );
    }
//-----------------------------------------------------------------------------
void AI1::direct_set_value( float new_value )
    {
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

float AO1::get_value()
    {
    return get_AO( AO_INDEX, get_min_value(), get_max_value() );
    }
//-----------------------------------------------------------------------------
void AO1::direct_set_value( float new_value )
    {
    set_AO( AO_INDEX, get_min_value(), get_max_value() );
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float temperature_e::get_max_val()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
float temperature_e::get_min_val()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float level_e::get_max_val()
    {
    return 100;
    }
//-----------------------------------------------------------------------------
float level_e::get_min_val()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

void valve_DO1::direct_on()
    {
    set_DO( DO_INDEX, 1 );
    }
//-----------------------------------------------------------------------------
void valve_DO1::direct_off()
    {
    set_DO( DO_INDEX, 0 );
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float motor::get_value()
    {
#ifdef DEBUG_NO_WAGO_MODULES
    return freq;
#else
    if ( sub_type == device::DST_M_FREQ || sub_type == device::DST_M_REV_FREQ )
        {
        return get_AO( AO_INDEX, C_MIN_VALUE, C_MAX_VALUE );
        }

    return 0;    
#endif // DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void motor::direct_set_value( float value )
    {
#ifdef DEBUG_NO_WAGO_MODULES
    freq = value;
#else
    if ( sub_type == device::DST_M_FREQ || sub_type == device::DST_M_REV_FREQ )
        {
        set_AO( AO_INDEX, value, C_MIN_VALUE, C_MAX_VALUE );
        }    
#endif // DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void motor::direct_set_state( int new_state )
    {
    if ( new_state )
        {
#ifdef DEBUG_NO_WAGO_MODULES
        if ( -1 == new_state )
            {
            state = ( char ) -1;
            return;
            }
#endif // DEBUG_NO_WAGO_MODULES

        if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ )
            {
            if ( new_state == 2 )
                {
#ifdef DEBUG_NO_WAGO_MODULES
                state = 2;
#else
                // Отключение прямого пуска.
                int o = get_DO( DO_INDEX );
                if ( 1 == o )
                    {
                    start_switch_time = get_sec();
                    set_DO( DO_INDEX, 0 );
                    }

                // Включение реверса.
                o = get_DO( DO_INDEX_REVERSE );
                if ( 0 == o )
                    {
                    start_switch_time = get_sec();
                    set_DO( DO_INDEX_REVERSE, 1 );
                    }
#endif // DEBUG_NO_WAGO_MODULES

                return;
                }
            }

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
#ifdef DEBUG_NO_WAGO_MODULES
    return state;
#else
    int o = get_DO( DO_INDEX );
    int i = get_DI( DI_INDEX );

    if ( o == i )
        {
        start_switch_time = get_sec();
        return i;
        }

    if ( get_sec() - start_switch_time > P_ON_TIME )
        {
        return -1;
        }
    else
        {
        return i;
        }
#endif // DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void motor::direct_on()
    {
#ifdef DEBUG_NO_WAGO_MODULES
    state = 1;
#else
    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_sec();
        set_DO( DO_INDEX, 1 );
        }
#endif // DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void motor::direct_off()
    {
#ifdef DEBUG_NO_WAGO_MODULES
    state = 0;
#else
    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        start_switch_time = get_sec();
        set_DO( DO_INDEX, 0 );
        }

    if ( sub_type == device::DST_M_REV || sub_type == device::DST_M_REV_FREQ )
        {
        // Отключение реверса.
        o = get_DO( DO_INDEX_REVERSE );
        if ( o != 0 )
            {
            start_switch_time = get_sec();
            set_DO( DO_INDEX_REVERSE, 0 );
            }
        }    
#endif // DEBUG_NO_WAGO_MODULES

    direct_set_value( 0 );
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool level_s::is_active()
    {
    switch ( sub_type )
        {
        case DST_LS_MIN:
            return get_state() == 0 ? 1 : 0;
            break;

        case DST_LS_MAX:
            return get_state() == 0 ? 0 : 1;
            break;

        default:
            return get_state() == 0 ? 0 : 1;
        }
    }
//-----------------------------------------------------------------------------
level_s::level_s( u_int number, device::DEVICE_SUB_TYPE sub_type ):
    DI1( number, DT_LS, sub_type, ADDITIONAL_PARAMS_COUNT )
    {
    set_par_name( P_DT,  0, "P_DT" );
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
void analog_wago_device::direct_set_state( int new_state )
    {
    direct_set_value( ( float ) new_state );
    }
//-----------------------------------------------------------------------------
int analog_wago_device::get_state()
    {
    return ( int ) get_value();
    }
//-----------------------------------------------------------------------------
void analog_wago_device::print() const
    {
    device::print();
    //wago_device::print();
    }
//-----------------------------------------------------------------------------
void analog_wago_device::direct_on()
    {
    }
//-----------------------------------------------------------------------------
void analog_wago_device::direct_off()
    {
    direct_set_value( 0 );
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_WAGO_MODULES

float analog_wago_device::get_value()
    {
    return value;
    }
//-----------------------------------------------------------------------------
void analog_wago_device::direct_set_value( float new_value )
    {
    value = new_value;
    }
#endif // DEBUG_NO_WAGO_MODULES
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
    state( S_STOP ),
    countdown_time( 0 )
    {
    }
//-----------------------------------------------------------------------------
void timer::start()
    {
    if ( S_STOP == state )
        {
        work_time = 0;
        }

    if ( S_PAUSE == state || S_STOP == state )
        {
        state = S_WORK;
        last_time = get_millisec();
        }
    }
//-----------------------------------------------------------------------------
void timer::reset()
    {
    state = S_STOP;
    work_time = 0;
    }
//-----------------------------------------------------------------------------
void timer::pause()
    {
    if ( S_WORK == state )
        {
        work_time += get_delta_millisec( last_time );
        }
    state = S_STOP;
    }
//-----------------------------------------------------------------------------
bool timer::is_time_up() const
    {
    if ( state != S_STOP )
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
    return work_time + get_delta_millisec( last_time );
    }
//-----------------------------------------------------------------------------
void timer::set_countdown_time( u_long new_countdown_time )
    {
#ifdef DEBUG
    if ( 0 == new_countdown_time )
        {
        Print( "Error void timer::set_countdown_time( u_long time ), time = %lu!\n",
            new_countdown_time );
        }
#endif

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
        timers     = 0;
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
#ifdef DEBUG
    else
        {
        Print( "timer_manager[] - error: index[ %u ] > count [ %u ]\n",
            index, timers_cnt );
        }
#endif // DEBUG

    return &stub;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
device_manager* G_DEVICE_MANAGER()
    {
    return device_manager::get_instance();
    }
//-----------------------------------------------------------------------------
i_DO_device* V( int number )
    {
    return G_DEVICE_MANAGER()->get_V( number );
    }
//-----------------------------------------------------------------------------
i_AO_device* VC( int number )
    {
    return G_DEVICE_MANAGER()->get_VC( number );
    }
//-----------------------------------------------------------------------------
i_DO_AO_device* M( int number )
    {
    return G_DEVICE_MANAGER()->get_M( number );
    }
//-----------------------------------------------------------------------------
i_DI_device* LS( int number )
    {
    return G_DEVICE_MANAGER()->get_LS( number );
    }
//-----------------------------------------------------------------------------
i_DI_device* FS( int number )
    {
    return G_DEVICE_MANAGER()->get_FS( number );
    }
//-----------------------------------------------------------------------------
i_AI_device* AI( int number )
    {
    return G_DEVICE_MANAGER()->get_AI( number );
    }
//-----------------------------------------------------------------------------
i_AO_device* AO( int number )
    {
    return G_DEVICE_MANAGER()->get_AO( number );
    }
//-----------------------------------------------------------------------------
i_counter* FQT( int number )
    {
    return G_DEVICE_MANAGER()->get_FQT( number );
    }
//-----------------------------------------------------------------------------
i_AI_device* TE( int number )
    {
    return G_DEVICE_MANAGER()->get_TE( number );
    }
//-----------------------------------------------------------------------------
i_AI_device* LT( int number )
    {
    return G_DEVICE_MANAGER()->get_LT( number );
    }
//-----------------------------------------------------------------------------
i_DI_device* GS( int number )
    {
    return G_DEVICE_MANAGER()->get_GS( number );
    }
//-----------------------------------------------------------------------------
i_DO_device* HA( int number )
    {
    return G_DEVICE_MANAGER()->get_HA( number );
    }
//-----------------------------------------------------------------------------
i_DO_device* HL( int number )
    {
    return G_DEVICE_MANAGER()->get_HL( number );
    }
//-----------------------------------------------------------------------------
i_DI_device* SB( int number )
    {
    return G_DEVICE_MANAGER()->get_SB( number );
    }
//-----------------------------------------------------------------------------
i_DI_device* DI( int number )
    {
    return G_DEVICE_MANAGER()->get_DI( number );
    }
//-----------------------------------------------------------------------------
i_DO_device* DO( int number )
    {
    return G_DEVICE_MANAGER()->get_DO( number );
    }
//-----------------------------------------------------------------------------
i_AI_device* QT( int number )
    {
    return G_DEVICE_MANAGER()->get_QT( number );
    }
//-----------------------------------------------------------------------------
dev_stub* STUB()
    {
    return G_DEVICE_MANAGER()->get_stub();
    }
//-----------------------------------------------------------------------------
device* DEVICE( int type, int number )
    {
    return G_DEVICE_MANAGER()->get_device( ( device::DEVICE_TYPE ) type, number );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_AS_mix_proof::valve_AS_mix_proof( u_int number ) : device( number,
                                                                DT_V, DST_V_AS_MIXPROOF, 0 )
    {
    }
//-----------------------------------------------------------------------------
void valve_AS_mix_proof::open_upper_seat()
    {
    direct_set_state( ST_UPPER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_AS_mix_proof::open_lower_seat()
    {
    direct_set_state( ST_LOWER_SEAT );
    }
//-----------------------------------------------------------------------------
int valve_AS_mix_proof::get_state()
    {
#ifdef DEBUG_NO_WAGO_MODULES
    return state;
#else
    return 0;
#endif // DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void valve_AS_mix_proof::direct_on()
    {
#ifdef DEBUG_NO_WAGO_MODULES
    state = ST_OPEN;
#else
#endif // DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void valve_AS_mix_proof::direct_off()
    {
#ifdef DEBUG_NO_WAGO_MODULES
    state = ST_CLOSE;
#else
#endif // DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void valve_AS_mix_proof::direct_set_state( int new_state )
    {
#ifdef DEBUG_NO_WAGO_MODULES
    state = ( STATES ) new_state;
#else
#endif // DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
float valve_AS_mix_proof::get_value()
    {
#ifdef DEBUG_NO_WAGO_MODULES
    return ( float ) state;
#else
    return 0;
#endif // DEBUG_NO_WAGO_MODULES
    }
//-----------------------------------------------------------------------------
void valve_AS_mix_proof::direct_set_value( float new_value )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
