#include "PAC_dev.h"
#include "tech_def.h"

#include "errors.h"

auto_smart_ptr < device_manager > device_manager::instance;

const char device::DEV_NAMES[][ 5 ] = { "V", "N", "M", "LS", "TE", "FE", "FS",
    "CTR", "AO", "LE", "FB", "UPR", "QE", "AI" };

const char device::DEV_TYPES[] =        { 1,    1,   1,   1,    4,    4,    1,
    2,      4,    4,    1,    1,     4,    4 };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void device::print() const
    {
#ifdef DEBUG    
    switch ( type )
        {
    case DT_V:
        Print( "V   " );
        break;

    case DT_N:
        Print( "N   " );
        break;

    case DT_M:
        Print( "M   " );
        break;

    case DT_LS:
        Print( "LS  " );
        break;

    case DT_TE:
        Print( "TE  " );
        break;

    case DT_FE:
        Print( "FE  " );
        break;

    case DT_FS:
        Print( "FS  " );
        break;

    case DT_CTR:
        Print( "CTR " );
        break;

    case DT_AO:
        Print( "AO  " );
        break;

    case DT_LE:
        Print( "LE  " );
        break;

    case DT_FB:
        Print( "FB  " );
        break;

    case DT_UPR:
        Print( "UPR " );
        break;

    case DT_QE:
        Print( "QE  " );
        break;

    case DT_AI:
        Print( "AI  " );
        break;

    default:
        Print( "Unknown " );
        break;
        }
    Print( "%5lu\t", ( u_long ) number );

#endif // DEBUG
    }
//-----------------------------------------------------------------------------
const char * device::get_name() const
    {
    return "";
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
    sprintf( buff, "%s[%d]={ST=%d, V=%.2f },\n",
        prefix, get_n(),  get_state(), get_value() );

    return strlen( buff );
    }
//-----------------------------------------------------------------------------
int device::set_cmd( const char *prop, u_int idx, char *val )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int device::set_cmd( const char *prop, u_int idx, double val )
    {
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

    default:
#ifdef DEBUG
        Print( "Error device::set_cmd() - prop = %s, val = %f\n", 
            prop, val );
#endif // DEBUG
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

int DO_1::get_state_now()
    {
    return get_DO( DO_INDEX );
    }
//-----------------------------------------------------------------------------
void DO_1::direct_on()
    {
    set_DO( DO_INDEX, 1 );
    }
//-----------------------------------------------------------------------------
void DO_1::direct_off()
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
device* device_manager::get_device( device::DEVICE_TYPE dev_type,
    u_int dev_number )
    {
    int dev_n = get_device_n( dev_type, dev_number );

    if ( dev_n >= 0 )
        {
        return project_devices[ dev_n ];
        }
    else
        {
#ifdef DEBUG
        switch ( dev_type )
            {
        case device::DT_V:
            Print( "V   " );
            break;

        case device::DT_N:
            Print( "N   " );
            break;

        case device::DT_M:
            Print( "M   " );
            break;

        case device::DT_LS:
            Print( "LS  " );
            break;

        case device::DT_TE:
            Print( "TE  " );
            break;

        case device::DT_FE:
            Print( "FE  " );
            break;

        case device::DT_FS:
            Print( "FS  " );
            break;

        case device::DT_CTR:
            Print( "CTR " );
            break;

        case device::DT_AO:
            Print( "AO  " );
            break;

        case device::DT_LE:
            Print( "LE  " );
            break;

        case device::DT_FB:
            Print( "FB  " );
            break;

        case device::DT_UPR:
            Print( "UPR " );
            break;

        case device::DT_QE:
            Print( "QE  " );
            break;

        case device::DT_AI:
            Print( "AI  " );
            break;

        default:
            Print( "Unknown " );
            break;
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
    Print( "\nProject devices:\n" );
    for ( u_int i = 0; i < project_devices.size(); i++ )
        {
        Print( "    %3i. ", i + 1 );
        project_devices[ i ]->print();
        }
    Print( "\n" );
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

    //if ( devices )
    //    {
    //    for ( int i = 0; i < device::C_DEVICE_TYPE_CNT; i++ )
    //        {
    //        if ( devices->sub_dev[ i ] )
    //            {
    //            delete devices->sub_dev[ i ];
    //            devices->sub_dev[ i ] = 0;
    //            }
    //        }

    //    delete devices;
        //devices = 0;
        //}
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_N( int number )
    {
    return get_device( device::DT_N, number );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_M( int number )
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
i_counter* device_manager::get_CTR( int number )
    {
    int res = get_device_n( device::DT_CTR, number );

    if ( res >= 0 ) return ( counter* ) project_devices.at( res );

    return &stub;
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_TE( int number )
    {
    return get_device( device::DT_TE, number );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_FE( int number )
    {
    return get_device( device::DT_FE, number );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_LE( int number )
    {
    return get_device( device::DT_LE, number );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_FB( int number )
    {
    return get_device( device::DT_FB, number );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_UPR( int number )
    {
    return get_device( device::DT_UPR, number );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_QE( int number )
    {
    return get_device( device::DT_QE, number );
    }
//-----------------------------------------------------------------------------
wago_device* device_manager::add_device( int dev_type, int dev_sub_type,
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
        case device::DST_V_DO_1:
            new_device      = new valve_DO_1( number );
            new_wago_device = ( valve_DO_1* ) new_device;
            break;

        case device::DST_V_DO_2:
            new_device      = new valve_DO_2( number );
            new_wago_device = ( valve_DO_2* ) new_device;
            break;

        case device::DST_V_DO_1_DI_1:
            new_device      = new valve_DO_1_DI_1( number );
            new_wago_device = ( valve_DO_1_DI_1* ) new_device;
            break;

        case device::DST_V_DO_1_DI_2:
            new_device      = new valve_DO_1_DI_2( number );
            new_wago_device = ( valve_DO_1_DI_2* ) new_device;
            break;

        case device::DST_V_DO_2_DI_2:
            new_device      = new valve_DO_2_DI_2( number );
            new_wago_device = ( valve_DO_2_DI_2* ) new_device;
            break;

        case device::DST_V_MIXPROOF:
            new_device      = new valve_mix_proof( number );
            new_wago_device = ( valve_mix_proof* ) new_device;
            break;

        case device::DST_V_AS_MIXPROOF:
            new_device      = new valve_AS_mix_proof( number );
            new_wago_device = ( valve_AS_mix_proof* ) new_device;
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

    case device::DT_N:
        new_device      = new pump( number );
        new_wago_device = ( pump* ) new_device;
        break;

    case device::DT_M:
        new_device      = new mixer( number );
        new_wago_device = ( mixer* ) new_device;
        break;

    case device::DT_LS:
        new_device      = new level_s( number );
        new_wago_device = ( level_s* ) new_device;
        break;

    case device::DT_TE:
        new_device      = new temperature_e( number );
        new_wago_device = ( temperature_e* ) new_device;
        break;

    case device::DT_FE:
        new_device      = new flow_e( number );
        new_wago_device = ( flow_e* ) new_device;
        break;

    case device::DT_FS:                    
        new_device      = new flow_s( number );
        new_wago_device = ( flow_s* ) new_device;
        break;

    case device::DT_CTR:
        new_device      = new counter( number );
        new_wago_device = ( counter* ) new_device;
        break;

    case device::DT_AO:
        new_device      = new AO_0_100( number );
        new_wago_device = ( AO_0_100* ) new_device;
        break;

    case device::DT_LE:
        new_device      = new level_e( number );
        new_wago_device = ( level_e* ) new_device;
        break;

    case device::DT_FB:
        new_device      = new feedback( number );
        new_wago_device = ( feedback* ) new_device;
        break;

    case device::DT_UPR:
        new_device      = new control_s( number );
        new_wago_device = ( control_s* ) new_device;
        break;

    case device::DT_QE:
        new_device      = new concentration_e( number );
        new_wago_device = ( concentration_e* ) new_device;
        break;

    case device::DT_AI:
        new_device      = new analog_input_4_20( number );
        new_wago_device = ( analog_input_4_20* ) new_device;
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
//-----------------------------------------------------------------------------

void i_counter::restart()
    {
    reset();
    start();
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
i_DI_device::i_DI_device() :last_check_time( get_millisec() ),
    state( 0 ),
    dt( 0 )
    {
    }
//-----------------------------------------------------------------------------
void i_DI_device::set_change_time( u_int time )
    {
    dt = time;
    }
//-----------------------------------------------------------------------------
void i_DI_device::set_state( int new_state )
    {
    state = new_state;
    last_check_time = get_millisec();
    }
//-----------------------------------------------------------------------------
int i_DI_device::get_state()
    {
    if ( dt > 0 )
        {
        if ( state != get_state_now() )
            {
            if ( get_delta_millisec( last_check_time ) > dt  )
                {
                state = get_state_now();
                }
            }
        else
            {
            last_check_time = get_millisec();
            }
        }
    else state = get_state_now();

    return state;
    }
//-----------------------------------------------------------------------------
bool i_DI_device::is_active()
    {
    return get_state() == 0 ? 0 : 1;
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
int dev_stub::get_state_now()
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
    Print( "virtual device\n" );
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
int counter::get_state_now()
    {
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
    wago_device::print();
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
        last_read_value = ( u_int ) get_AI( AI_INDEX );
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
        u_int current = ( u_int ) get_AI( AI_INDEX );

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
//-----------------------------------------------------------------------------
float digital_device::get_value()
    {
    return ( float ) get_state_now();
    }
//-----------------------------------------------------------------------------
void digital_device::direct_set_value( float new_value )
    {
    direct_set_state( ( int ) new_value );
    }
//-----------------------------------------------------------------------------
void digital_device::direct_set_state( int new_state )
    {
    if ( new_state )
        {
        if ( -1 == new_state ) state = ( char ) -1;
        else direct_on();
        }
    else direct_off();
    }
//-----------------------------------------------------------------------------
void digital_device::print() const
    {
    device::print();
    wago_device::print();
    }
//-----------------------------------------------------------------------------
int digital_device::get_state()
    {
    return i_DI_device::get_state();
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_WAGO_MODULES

int digital_device::get_state_now()
    {
    return state;
    }
//-----------------------------------------------------------------------------
void digital_device::direct_on()
    {
    state = 1;
    }
//-----------------------------------------------------------------------------
void digital_device::direct_off()
    {
    state = 0;
    }
#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

int DO_2::get_state_now()
    {
    int b1 = get_DO( DO_INDEX_1 );
    int b2 = get_DO( DO_INDEX_2 );
    if ( b1 == b2 ) return -1;
    return b2;
    }
//-----------------------------------------------------------------------------
void DO_2::direct_on()
    {
    set_DO( DO_INDEX_1, 0 );
    set_DO( DO_INDEX_2, 1 );
    }
//-----------------------------------------------------------------------------
void DO_2::direct_off()
    {
    set_DO( DO_INDEX_1, 1 );
    set_DO( DO_INDEX_2, 0 );
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

int DO_1_DI_1::get_state_now()
    {
    int o = get_DO( DO_INDEX );
    int i = get_DI( DI_INDEX );

    if ( o == i )
        {
        start_switch_time = get_sec();
        return i;
        }   

    if ( get_sec() - start_switch_time > C_SWITCH_TIME )
        {
        return -1;
        }
    else
        {
        return i;
        }
    }
//-----------------------------------------------------------------------------
void DO_1_DI_1::direct_on()
    {
    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_sec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void DO_1_DI_1::direct_off()
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

int DO_1_DI_2::get_state_now()
    {
    int o = get_DO( DO_INDEX );
    int i0 = get_DI( DI_INDEX_1 );
    int i1 = get_DI( DI_INDEX_2 );

    if ( ( o == 0 && i0 == 1 && i1 == 0 ) ||
        ( o == 1 && i1 == 1 && i0 ==0 ) )
        {
        start_switch_time = get_sec();
        return o;
        }

    if ( get_sec() - start_switch_time > C_SWITCH_TIME )
        {
        return -1;
        }
    else
        {
        return o;
        }
    }
//-----------------------------------------------------------------------------
void DO_1_DI_2::direct_on()
    {
    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        start_switch_time = get_sec();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void DO_1_DI_2::direct_off()
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

int DO_2_DI_2::get_state_now()
    {
    int o0 = get_DO( DO_INDEX_1 );
    int o1 = get_DO( DO_INDEX_2 );
    int i0 = get_DI( DI_INDEX_1 );
    int i1 = get_DI( DI_INDEX_2 );

    if ( ( o1 == i1 ) && ( o0 == i0 ) )
        {
        start_switch_time = get_sec();
        return o1;
        };

    if ( get_sec() - start_switch_time > C_SWITCH_TIME )
        {
        return -1;
        }
    else
        {
        return o1;
        }
    }
//-----------------------------------------------------------------------------
void DO_2_DI_2::direct_on()
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
void DO_2_DI_2::direct_off()
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
    direct_set_state( ST_UPPER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::open_low_seat()
    {
    direct_set_state( ST_LOW_SEAT );
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

int valve_mix_proof::get_state_now()
    {
    int o = get_DO( DO_INDEX );            
    int i0 = get_DI( DI_INDEX_U );
    int i1 = get_DI( DI_INDEX_L );

    if ( ( o == 0 && i0 == 1 && i1 == 0 ) ||
        ( o == 1 && i1 == 1 && i0 == 0 ) )
        {
        start_switch_time = get_sec();
        if ( o == 0 && get_DO( DO_INDEX_U ) == 1 ) return ST_UPPER_SEAT;
        if ( o == 0 && get_DO( DO_INDEX_L ) == 1 ) return ST_LOW_SEAT;
        return o;
        }

    if ( get_sec() - start_switch_time > C_SWITCH_TIME )
        {
        return -1;
        }
    else
        {
        return o;
        }
    }
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
void valve_mix_proof::direct_set_state( int new_state )
    {
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

    case ST_LOW_SEAT:
        direct_off();
        set_DO( DO_INDEX_L, 1 );
        break;

    default:
        direct_on();
        break;
        }    
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

int DI_1::get_state_now()
    {
    return get_DI( DI_INDEX );
    }
//-----------------------------------------------------------------------------
void DI_1::direct_on()
    {
    }
//-----------------------------------------------------------------------------
void DI_1::direct_off()
    {
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

float AI_1::get_value()
    {
    return get_AI( AI_INDEX, get_min_val(), get_max_val() );
    }
//-----------------------------------------------------------------------------
void AI_1::direct_set_value( float new_value )
    {    
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

float AO_1::get_value()
    {
    return get_AO( AO_INDEX, get_min_val(), get_max_val() );
    }
//-----------------------------------------------------------------------------
void AO_1::direct_set_value( float new_value )
    {
    set_AO( AO_INDEX, new_value, get_min_val(), get_max_val() );
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
float flow_e::get_max_val()
    {
    return get_par( C_MAX_PAR_NUMBER );
    }
//-----------------------------------------------------------------------------
float flow_e::get_min_val()
    {
    return get_par( C_MIN_PAR_NUMBER );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool level_s::is_active()
    {
    if ( get_par( C_ACTIVE_STATE ) == 0 )
        {
        return get_state() == 0 ? 1 : 0;
        }
    else
        {
        return get_state() == 0 ? 0 : 1;
        }
    }
//-----------------------------------------------------------------------------
level_s::level_s( u_int number, u_int dt /*= 1000 */ ) : DI_1( number, DT_LS, DST_NONE )
    {
    set_change_time( dt );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float concentration_e::get_max_val()
    {
    return get_par( C_MAX_PAR_NUMBER );
    }
//-----------------------------------------------------------------------------
float concentration_e::get_min_val()
    {
    return get_par( C_MIN_PAR_NUMBER );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float analog_input_4_20::get_max_val()
    {
    return 20;
    }
//-----------------------------------------------------------------------------
float analog_input_4_20::get_min_val()
    {
    return 4;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void analog_device::direct_set_state( int new_state )
    {
    direct_set_value( ( float ) new_state );
    }
//-----------------------------------------------------------------------------
int analog_device::get_state_now()
    {
    return ( int ) get_value();
    }
//-----------------------------------------------------------------------------
void analog_device::print() const
    {
    device::print();
    wago_device::print();
    }
//-----------------------------------------------------------------------------
void analog_device::direct_on()
    {
    }
//-----------------------------------------------------------------------------
void analog_device::direct_off()
    {
    direct_set_value( 0 );
    }
//-----------------------------------------------------------------------------
#ifdef DEBUG_NO_WAGO_MODULES

float analog_device::get_value()
    {
    return value;
    }
//-----------------------------------------------------------------------------
void analog_device::direct_set_value( float new_value )
    {
    value = new_value;    
    }

#endif // DEBUG_NO_WAGO_MODULES
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float AO_0_100::get_max_val()
    {
    return C_AO_MIN_VALUE;
    }
//-----------------------------------------------------------------------------
float AO_0_100::get_min_val()
    {
    return C_AO_MAX_VALUE;
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
    Print( " %lu\n", work_time + get_delta_millisec( last_time ) );

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
timer& timer_manager::operator[]( unsigned int index )
    {
    if ( index < timers_cnt )
        {
        return timers[ index ];
        }
#ifdef DEBUG
    else
        {
        Print( "timer_manager[] - error: index[ %u ] > count [ %u ]\n",
            index, timers_cnt );
        }
#endif // DEBUG

    return stub;
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
i_DO_device* N( int number )
    {
    return G_DEVICE_MANAGER()->get_N( number );
    }
//-----------------------------------------------------------------------------
i_DO_device* M( int number )
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
i_counter* CTR( int number )
    {
    return G_DEVICE_MANAGER()->get_CTR( number );
    }
//-----------------------------------------------------------------------------
i_AI_device* TE( int number )
    {
    return G_DEVICE_MANAGER()->get_TE( number );
    }
//-----------------------------------------------------------------------------
i_AI_device* FE( int number )
    {
    return G_DEVICE_MANAGER()->get_FE( number );
    }
//-----------------------------------------------------------------------------
i_AI_device* LE( int number )
    {
    return G_DEVICE_MANAGER()->get_LE( number );
    }
//-----------------------------------------------------------------------------
i_DI_device* FB( int number )
    {
    return G_DEVICE_MANAGER()->get_FB( number );
    }
//-----------------------------------------------------------------------------
i_DO_device* UPR( int number )
    {
    return G_DEVICE_MANAGER()->get_UPR( number );
    }
//-----------------------------------------------------------------------------
i_AI_device* QE( int number )
    {
    return G_DEVICE_MANAGER()->get_QE( number );
    }
//-----------------------------------------------------------------------------
dev_stub* STUB()
    {
    return G_DEVICE_MANAGER()->get_stub();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
valve_AS_mix_proof::valve_AS_mix_proof( u_int number ) : digital_device( number,
    DT_V, DST_V_AS_MIXPROOF )
    {
    }
//-----------------------------------------------------------------------------
void valve_AS_mix_proof::open_upper_seat()
    {
    direct_set_state( ST_UPPER_SEAT );
    }
//-----------------------------------------------------------------------------
void valve_AS_mix_proof::open_low_seat()
    {
    direct_set_state( ST_LOW_SEAT );
    }
//-----------------------------------------------------------------------------
#ifndef DEBUG_NO_WAGO_MODULES

int  valve_AS_mix_proof::get_state_now()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void valve_AS_mix_proof::direct_on()
    {
    }
//-----------------------------------------------------------------------------
void valve_AS_mix_proof::direct_off()
    {
    }
//-----------------------------------------------------------------------------
void valve_AS_mix_proof::direct_set_state( int new_state )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif // DEBUG_NO_WAGO_MODULES
