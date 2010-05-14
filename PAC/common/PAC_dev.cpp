#include "PAC_dev.h"

device_manager* device_manager::instance;
char wago_device::debug_mode;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
device::device() : number( 0 ),
    type( DT_NONE ),
    sub_type( DST_NONE )
    { 
    }
//-----------------------------------------------------------------------------
int device::save_device( char *buff )
    {
    ( ( u_int_4* ) buff )[ 0 ] = number;
    return sizeof( u_int_4 );
    }
//-----------------------------------------------------------------------------
void device::print() const
    {
#ifdef DEBUG    
    switch ( type )
        {
        case DT_V:
            Print( "V  " );
            break;

        case DT_N:
            Print( "N  " );
            break;

        case DT_M:
            Print( "M  " );
            break;

        case DT_LS:
            Print( "LS " );
            break;

        case DT_TE:
            Print( "TE " );
            break;

        case DT_FE:
            Print( "FE " );
            break;

        case DT_FS:
            Print( "FS " );
            break;

        case DT_CTR:
            Print( "CTR" );
            break;

        case DT_AO:
            Print( "AO " );
            break;

        case DT_LE:
            Print( "LE " );
            break;

        case DT_FB:
            Print( "FB " );
            break;

        case DT_UPR:
            Print( "UPR" );
            break;

        case DT_QE:
            Print( "QE " );
            break;

        case DT_AI:
            Print( "AI " );
            break;

        default:
            Print( "Uknown" );
            break;
        }
    Print( "%5lu\t", ( u_long ) number );

#endif // DEBUG
    }
//-----------------------------------------------------------------------------
int device::load( file *cfg_file )
    {    
    sscanf( cfg_file->fget_line(), "%u %u %u", ( u_int* ) &type,
        ( u_int* ) &sub_type, &number );

    return 0;
    }
//-----------------------------------------------------------------------------
u_int_4 device::get_n() const
    {
    return number;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int char_state_device::save_changed_state( char *buff )
    {
    if ( prev_state != get_state() )
        {
        return save_state( buff );
        }
    return 0;
    }
//-----------------------------------------------------------------------------
int char_state_device::save_state( char *buff )
    {
    buff[ 0 ] = get_state();
    prev_state = get_state();
    return sizeof( char );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int u_int_4_state_device::save_changed_state( char *buff )
    {
    if ( prev_state != get_u_int_4_state() )
        {
        return save_state( buff );
        }
    return 0;
    }
//-----------------------------------------------------------------------------
int u_int_4_state_device::save_state( char *buff )
    {
    *( ( u_int_4* ) buff ) = ( u_int_4 ) get_u_int_4_state();
    prev_state = ( u_int_4 ) get_u_int_4_state();
    return sizeof( u_int_4 );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int wago_device::load( file *cfg_file )
    {           
    load_table_from_string( cfg_file->fget_line(), DI_channels );
    for ( u_int i = 0; i < DI_channels.count; i++ )
        {
        DI_channels.char_read_values[ i ] = wago_manager::get_instance()->
             get_DI_read_data( DI_channels.tables[ i ], DI_channels.offsets[ i ] );
        }
    load_table_from_string( cfg_file->fget_line(), DO_channels );
    for ( u_int i = 0; i < DO_channels.count; i++ )
        {        
        DO_channels.char_read_values[ i ] = wago_manager::get_instance()->
            get_DO_read_data( DO_channels.tables[ i ], DO_channels.offsets[ i ] );
        DO_channels.char_write_values[ i ] = wago_manager::get_instance()->
            get_DO_write_data( DO_channels.tables[ i ], DO_channels.offsets[ i ] );
        }
    load_table_from_string( cfg_file->fget_line(), AI_channels );
    for ( u_int i = 0; i < AI_channels.count; i++ )
        {
        AI_channels.int_read_values[ i ] = wago_manager::get_instance()->
            get_AI_read_data( AI_channels.tables[ i ], AI_channels.offsets[ i ] );
        }
    load_table_from_string( cfg_file->fget_line(), AO_channels );
    for ( u_int i = 0; i < AO_channels.count; i++ )
        {
        AO_channels.int_read_values[ i ] = wago_manager::get_instance()->
            get_AO_read_data( AO_channels.tables[ i ], AO_channels.offsets[ i ] );
        AO_channels.int_write_values[ i ] = wago_manager::get_instance()->
            get_AO_write_data( AO_channels.tables[ i ], AO_channels.offsets[ i ] );
        }

    // 2 1.1 2.1
    // количество значение_№1 значение_№2 ...
    char *str = cfg_file->fget_line();
    int pos = sscanf( str, "%u", &params_count );

    if ( params_count > 0 )
        {        
        params = new float [ params_count ];
        for ( u_int i = 0; i < params_count; i++ )
            {
            pos += sscanf( str + pos, " %f", &params[ i ] );
            }
        }
    cfg_file->fget_line();

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_device::get_DO( u_int index )
    {
    if ( index < DO_channels.count &&
        DO_channels.char_read_values &&
        DO_channels.char_read_values[ index ] )
        {
        return *DO_channels.char_read_values[ index ];
        }

#ifdef DEBUG
    Print( "wago_device->get_DO(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_device::set_DO( u_int index, char value )
    {
    if ( index < DO_channels.count && 
        DO_channels.char_write_values &&
        DO_channels.char_write_values[ index ] )
        {
        *DO_channels.char_write_values[ index ] = value;
        if ( debug_mode )
            {
            *DO_channels.char_read_values[ index ] = value;
            }
        return 0;
        }

#ifdef DEBUG
    Print( "wago_device->set_DO(...) - error!\n" );
#endif // DEBUG

    return 1;
    }
//-----------------------------------------------------------------------------
int wago_device::get_DI( u_int index )
    {
    if ( index < DI_channels.count &&
        DI_channels.char_read_values &&
        DI_channels.char_read_values[ index ] )
        {
        return *DI_channels.char_read_values[ index ];
        }

#ifdef DEBUG
    Print( "wago_device->get_DI(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_device::set_DI( u_int index, char value )
    {
    if ( index < DI_channels.count &&
        DI_channels.char_read_values &&
        DI_channels.char_read_values[ index ] )
        {
        *DI_channels.char_read_values[ index ] = value;
        return 0;
        }

#ifdef DEBUG
    Print( "wago_device->set_DI(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
u_int wago_device::get_AO( u_int index )
    {
    if ( index < AO_channels.count &&
        AO_channels.char_read_values &&
        AO_channels.char_read_values[ index ] )
        {
        return *AO_channels.int_read_values[ index ];
        }

#ifdef DEBUG
    Print( "wago_device->get_AO(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_device::set_AO( u_int index, u_int value )
    {
    if ( index < AO_channels.count &&
        AO_channels.char_read_values &&
        AO_channels.char_read_values[ index ] )
        {
        *AO_channels.int_write_values[ index ] = value;
        if ( debug_mode )
            {
            *AO_channels.int_read_values[ index ] = value;
            }
        return 0;
        }

#ifdef DEBUG
    Print( "wago_device->set_AO(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
u_int wago_device::get_AI( u_int index )
    {
    if ( index < AI_channels.count &&
        AI_channels.char_read_values &&
        AI_channels.char_read_values[ index ] )
        {
        return *AI_channels.int_read_values[ index ];
        }

#ifdef DEBUG
    Print( "wago_device->get_AI(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_device::set_AI( u_int index, u_int value )
    {
    if ( index < AI_channels.count &&
        AI_channels.char_read_values &&
        AI_channels.char_read_values[ index ] )
        {
        *AI_channels.int_read_values[ index ] = value;
        return 0;
        }

#ifdef DEBUG
    Print( "wago_device->set_AI(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
void wago_device::print() const
    {
    print_table( "DI", DI_channels );    
    print_table( "DO", DO_channels );    
    print_table( "AI", AI_channels );    
    print_table( "AO", AO_channels );
    Print( "\n" );
    }
//-----------------------------------------------------------------------------
int wago_device::load_table_from_string( char *str, IO_channels &channels )
    {
    // 2 1 2 1 3
    // количество_DI_(например) номер_таблицы_DI_№1 смещение_в_пределах_таблицы_№1 номер_таблицы_DI_№2 ...
    u_int cnt;
    int pos = sscanf( str, "%d", &cnt );

    if ( cnt > 0 )
        {
        channels.count = cnt;

        channels.tables = new u_int[ cnt ];
        channels.offsets = new u_int[ cnt ];

        switch ( channels.type )
            {
            case IO_channels::CT_DI:
                channels.char_read_values = new u_char*[ cnt ];
                break;

            case IO_channels::CT_DO:
                channels.char_read_values = new u_char*[ cnt ];
                channels.char_write_values = new u_char*[ cnt ];
                break;

            case IO_channels::CT_AI:
                channels.int_read_values = new u_int*[ cnt ];
                break;

            case IO_channels::CT_AO:
                channels.int_read_values = new u_int*[ cnt ];
                channels.int_write_values = new u_int*[ cnt ];
                break;
            }

        for ( u_int i = 0; i < cnt; i++ )
            {
            pos += sscanf( str + pos, " %d %d", &channels.tables[ i ],
                &channels.offsets[ i ] );
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void wago_device::print_table( const char *str, 
                              const IO_channels &channels ) const
    {
    if ( channels.count )
        {
        Print( "%s:%d", str, channels.count );
        if ( channels.count )
            {
            Print( "[ " );
            for ( u_int i = 0; i < channels.count; i++ )
                {
                Print("%d:%2d", channels.tables[ i ],
                    channels.offsets[ i ] );
                if ( i < channels.count - 1 ) Print( "; " );
                }
            Print( " ]" );
            }
        Print( "; " );
        }
    }
//-----------------------------------------------------------------------------
float wago_device::get_par( u_int index )
    {
    if ( index < params_count && params )
        {
        return params[ index ];
        }

#ifdef DEBUG
    Print( "wago_device->get_par(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
wago_device::wago_device() :DI_channels( IO_channels::CT_DI ), 
DO_channels( IO_channels::CT_DO ),
AI_channels( IO_channels::CT_AI ),
AO_channels( IO_channels::CT_AO )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int DO_1::get_state()
    {
    return get_DO( DO_INDEX );
    }
//-----------------------------------------------------------------------------
void DO_1::on()
    {
    set_DO( DO_INDEX, 1 );
    }
//-----------------------------------------------------------------------------
void DO_1::off()
    {
    set_DO( DO_INDEX, 0 );
    }
//-----------------------------------------------------------------------------
//int DO_1::set_state( int new_state )
//    {
//    return digital_device::set_state( new_state );
//    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_V( int number )
    {
    return get_device( device::DT_V, number, "V" );
    }
//-----------------------------------------------------------------------------
device_manager* device_manager::get_instance()
    {
    return instance;
    }
//-----------------------------------------------------------------------------
void device_manager::set_instance( device_manager* new_instance )
    {
    instance = new_instance;
    }
//-----------------------------------------------------------------------------
int device_manager::get_device_n( device::DEVICE_TYPE dev_type, 
    u_int dev_number )
    {
    int l = dev_types_ranges[ dev_type ].start_pos;
    int u = dev_types_ranges[ dev_type ].end_pos;

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
        u_int dev_number, char const * dev_name )
    {
    int dev_n = get_device_n( dev_type, dev_number );

    if ( dev_n >= 0 )
        {
        return project_devices[ dev_n ];
        }
    else
        {
#ifdef DEBUG
    Print( "%s[ %d ] not found!\n", dev_name, dev_number );
#endif // DEBUG
        }

    return &stub;
    }
//-----------------------------------------------------------------------------
int device_manager::load_from_cfg_file( file *cfg_file )
    {
    cfg_file->fget_line();                      // Пропускаем заголовок.
    sscanf( cfg_file->fget_line(), "%d", &devices_count );
    cfg_file->fget_line();                      // Пропускаем пустую строку.

#ifdef DEBUG
    Print( "Total devices count %d.\n", devices_count );
#endif // DEBUG

    if ( devices_count )
        {
        char is_first_device[ device::C_DEVICE_TYPE_CNT ] = { 0 };

        project_devices = new device* [ devices_count ];
        project_valves = new valve* [ devices_count ];

        for ( int i = 0; i < devices_count; i++ )
            {
            int dev_type = 0;
            int dev_sub_type = 0;
            cfg_file->fget_line();              // Пропускаем комментарий.
            sscanf( cfg_file->pfget_line(), "%d %d", &dev_type, &dev_sub_type );
            
            switch ( dev_type )
                {                
                case device::DT_V:
                    {
                    switch ( dev_sub_type )
                        {
                        case device::DST_V_DO_1:
                            project_devices[ i ] = new valve_DO_1();
                            break;

                        case device::DST_V_DO_2:
                            project_devices[ i ] = new valve_DO_2();
                            break;

                        case device::DST_V_DO_1_DI_1:
                            project_devices[ i ] = new valve_DO_1_DI_1();
                            break;

                        case device::DST_V_DO_1_DI_2:
                            project_devices[ i ] = new valve_DO_1_DI_2();
                            break;

                        case device::DST_V_DO_2_DI_2:
                            project_devices[ i ] = new valve_DO_2_DI_2();
                            break;

                        case device::DST_V_MIXPROOF:
                            project_devices[ i ] = new valve_mix_proof();
                            break;

                        default:
#ifdef DEBUG
                            Print( "Unknown V device subtype %d!\n", dev_sub_type );
                            Getch();
#endif // DEBUG
                            project_devices[ i ] = new dev_stub();
                            break;
                        }
                    break;
                    }

                case device::DT_N:
                    project_devices[ i ] = new pump();
                    break;

                case device::DT_M:
                    project_devices[ i ] = new mixer();
                    break;

                case device::DT_LS:
                    project_devices[ i ] = new level_s();
                    break;

                case device::DT_TE:
                    project_devices[ i ] = new temperature_e();
                    break;

                case device::DT_FE:
                    project_devices[ i ] = new flow_e();
                    break;

                case device::DT_FS:                    
                    project_devices[ i ] = new flow_s();
                    break;

                case device::DT_CTR:
                    project_devices[ i ] = new counter();
                    break;

                case device::DT_AO:
                    project_devices[ i ] = new AO_0_100();
                    break;

                case device::DT_LE:
                    project_devices[ i ] = new level_e();
                    break;

                case device::DT_FB:
                    project_devices[ i ] = new feedback();
                    break;

                case device::DT_UPR:
                    project_devices[ i ] = new control_s();
                    break;

                case device::DT_QE:
                    project_devices[ i ] = new concentration_e();
                    break;

                case device::DT_AI:
                    project_devices[ i ] = new analog_input_4_20();
                    break;

                default:
#ifdef DEBUG
                    Print( "Unknown device type %d!\n", dev_type );
#endif // DEBUG
                    project_devices[ i ] = new dev_stub();
                    break;
                }

            if ( dev_type < device::C_DEVICE_TYPE_CNT )
                {
                if ( 0 == is_first_device[ dev_type ] )
                    {
                    dev_types_ranges[ dev_type ].start_pos = i;
                    is_first_device[ dev_type ] = 1;
                    }
                dev_types_ranges[ dev_type ].end_pos = i;
                }

            project_devices[ i ]->load( cfg_file );
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void device_manager::print() const
    {
    for ( int i = 0; i < devices_count; i++ )
        {
        Print( "    " );
        project_devices[ i ]->print();
        }
    Print( "\n" );
    }
//-----------------------------------------------------------------------------
device_manager::device_manager()
    {
    for ( int i = 0; i < device::C_DEVICE_TYPE_CNT; i++ )
        {
        dev_types_ranges[ i ].start_pos = 0;
        dev_types_ranges[ i ].end_pos = 0;
        }
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_N( int number )
    {
    return get_device( device::DT_N, number, "N" );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_M( int number )
    {
    return get_device( device::DT_M, number, "M" );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_LS( int number )
    {
    return get_device( device::DT_LS, number, "LS" );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_FS( int number )
    {
    return get_device( device::DT_FS, number, "FS" );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_AI( int number )
    {
    return get_device( device::DT_AI, number, "AI" );
    }
//-----------------------------------------------------------------------------
i_AO_device* device_manager::get_AO( int number )
    {
    return get_device( device::DT_AO, number, "AO" );
    }
//-----------------------------------------------------------------------------
i_counter* device_manager::get_CTR( int number )
    {
    int res = get_device_n( device::DT_CTR, number );

    if ( res >= 0 ) return ( i_counter* ) project_devices[ res ];

    return 0;
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_TE( int number )
    {
    return get_device( device::DT_TE, number, "TE" );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_FE( int number )
    {
    return get_device( device::DT_FE, number, "FE" );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_LE( int number )
    {
    return get_device( device::DT_LE, number, "LE" );
    }
//-----------------------------------------------------------------------------
i_DI_device* device_manager::get_FB( int number )
    {
    return get_device( device::DT_FB, number, "UPR" );
    }
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_UPR( int number )
    {
    return get_device( device::DT_UPR, number, "UPR" );
    }
//-----------------------------------------------------------------------------
i_AI_device* device_manager::get_QE( int number )
    {
    return get_device( device::DT_QE, number, "QE" );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float dev_stub::get_value()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int dev_stub::set_value( float new_value )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int dev_stub::get_state()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_stub::on()
    {
    }
//-----------------------------------------------------------------------------
void dev_stub::off()
    {
    }
//-----------------------------------------------------------------------------
int dev_stub::set_state( int new_state )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int dev_stub::parse_cmd( char *buff )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int dev_stub::load( file *cfg_file )
    {
    device::load( cfg_file );
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float counter::get_value()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int counter::set_value( float new_value )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int counter::get_state()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void counter::on()
    {
    }
//-----------------------------------------------------------------------------
void counter::off()
    {
    }
//-----------------------------------------------------------------------------
int counter::set_state( int new_state )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int counter::parse_cmd( char *buff )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int counter::load( file *cfg_file )
    {
    device::load( cfg_file );
    wago_device::load( cfg_file );

    return 0;
    }
//-----------------------------------------------------------------------------
void counter::print() const
    {
    device::print();
    wago_device::print();
    }
//-----------------------------------------------------------------------------
u_int_4 counter::get_u_int_4_state()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float digital_device::get_value()
    {
    return get_state();
    }
//-----------------------------------------------------------------------------
int digital_device::set_value( float new_value )
    {
    return set_state( ( int ) new_value );
    }
//-----------------------------------------------------------------------------
int digital_device::set_state( int new_state )
    {
    if ( new_state ) on();
    else off();

    return 0;
    }
//-----------------------------------------------------------------------------
int digital_device::parse_cmd( char *buff )
    {
    set_state( buff[ 0 ] );
    return sizeof( char );
    }
//-----------------------------------------------------------------------------
int digital_device::load( file *cfg_file )
    {
    device::load( cfg_file );
    wago_device::load( cfg_file );

    return 0;
    }
//-----------------------------------------------------------------------------
void digital_device::print() const
    {
    device::print();
    wago_device::print();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int DO_2::get_state()
    {
    int b1 = get_DO( DO_INDEX_1 );
    int b2 = get_DO( DO_INDEX_2 );
    if ( b1 == b2 ) return -1;
    return b2;
    }
//-----------------------------------------------------------------------------
void DO_2::on()
    {
    set_DO( DO_INDEX_1, 0 );
    set_DO( DO_INDEX_2, 1 );
    }
//-----------------------------------------------------------------------------
void DO_2::off()
    {
    set_DO( DO_INDEX_1, 1 );
    set_DO( DO_INDEX_2, 0 );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int DO_1_DI_1::get_state()
    {
    int o = get_DO( DO_INDEX );
    int i = get_DI( DI_INDEX );

    if ( get_par( PAR_FB_STATE ) == 0 )
        {
        if ( ( o == 0 && i == 1 ) || ( o == 1 && i == 0 ) )
            {
            switch_time = get_ms();
            return o;
            }
        }
    else
        {
        if ( o == i )
            {
            switch_time = get_ms();
            return i;
            }
        }

    if ( get_ms() - switch_time > C_SWITCH_TIME )
        {
        return -1;
        }
    else
        {
        if ( get_par( PAR_FB_STATE ) == 0 ) return !i;
        else return i;
        }
    }
//-----------------------------------------------------------------------------
void DO_1_DI_1::on()
    {
    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        switch_time = get_ms();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void DO_1_DI_1::off()
    {
    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        switch_time = get_ms();
        set_DO( DO_INDEX, 0 );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int DO_1_DI_2::get_state()
    {
    int o = get_DO( DO_INDEX );
    int i0 = get_DI( DI_INDEX_1 );
    int i1 = get_DI( DI_INDEX_2 );

    if ( ( o == 0 && i0 == 1 && i1 == 0 ) ||
        ( o == 1 && i1 == 1 && i0 ==0 ) )
        {
        switch_time = get_ms();
        return o;
        }

    if ( get_ms() - switch_time > C_SWITCH_TIME )
        {
        return -1;
        }
    else
        {
        return o;
        }
    }
//-----------------------------------------------------------------------------
void DO_1_DI_2::on()
    {
    int o = get_DO( DO_INDEX );
    if ( 0 == o )
        {
        switch_time = get_ms();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void DO_1_DI_2::off()
    {
    int o = get_DO( DO_INDEX );
    if ( o != 0 )
        {
        switch_time = get_ms();
        set_DO( DO_INDEX, 0 );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int DO_2_DI_2::get_state()
    {
    int o0 = get_DO( DO_INDEX_1 );
    int o1 = get_DO( DO_INDEX_2 );
    int i0 = get_DI( DI_INDEX_1 );
    int i1 = get_DI( DI_INDEX_2 );

    if ( ( o1 == i1 ) && ( o0 == i0 ) )
        {
        switch_time = get_ms();
        return o1;
        };

    if ( get_ms() - switch_time > C_SWITCH_TIME )
        {
        return -1;
        }
    else
        {
        return o1;
        }
    }
//-----------------------------------------------------------------------------
void DO_2_DI_2::on()
    {
    int o = get_DO( DO_INDEX_1 );
    if ( 0 == o )
        {
        switch_time = get_ms();
        set_DO( DO_INDEX_1, 1 );
        set_DO( DO_INDEX_2, 0 );
        }
    }
//-----------------------------------------------------------------------------
void DO_2_DI_2::off()
    {
    int o = get_DO( DO_INDEX_2 );
    if ( 0 == o )
        {
        switch_time = get_ms();
        set_DO( DO_INDEX_1, 0 );
        set_DO( DO_INDEX_2, 1 );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int valve_mix_proof::get_state()
    {
    int o = get_DO( DO_INDEX );            
    int i0 = get_DI( DI_INDEX_U );
    int i1 = get_DI( DI_INDEX_L );

    if ( ( o == 0 && i0 == 1 && i1 == 0 ) ||
        ( o == 1 && i1 == 1 && i0 == 0 ) )
        {
        switch_time = get_ms();
        if ( o == 0 && get_DO( DO_INDEX_U ) == 1 ) return ST_UPPER_SEAT;
        if ( o == 0 && get_DO( DO_INDEX_L ) == 1 ) return ST_LOW_SEAT;
        return o;
        }

    if ( get_ms() - switch_time > C_SWITCH_TIME )
        {
        return -1;
        }
    else
        {
        return o;
        }
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::on()
    {
    set_DO( DO_INDEX_U, 0 );
    set_DO( DO_INDEX_L, 0 );
    int o = get_DO( DO_INDEX );

    if ( 0 == o )
        {
        switch_time = get_ms();
        set_DO( DO_INDEX, 1 );
        }
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::open_upper_seat()
    {
    off();
    set_DO( DO_INDEX_U, 1 );
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::open_low_seat()
    {
    off();
    set_DO( DO_INDEX_L, 1 );
    }
//-----------------------------------------------------------------------------
void valve_mix_proof::off()
    {
    set_DO( DO_INDEX_U, 0 );
    set_DO( DO_INDEX_L, 0 );
    int o = get_DO( DO_INDEX );

    if ( o != 0 )
        {
        switch_time = get_ms();
        set_DO( DO_INDEX, 0 );
        }
    }
//-----------------------------------------------------------------------------
int valve_mix_proof::set_state( STATES new_state )
    {
    switch ( new_state )
        {
        case ST_CLOSE:
            off();
            break;

        case ST_OPEN:
            on();
            break;

        case ST_UPPER_SEAT:
            open_upper_seat();
            break;

        case ST_LOW_SEAT:
            open_low_seat();
            break;

        default:
            on();
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int DI_1::get_state()
    {
    return get_DI( DI_INDEX );
    }
//-----------------------------------------------------------------------------
void DI_1::on()
    {
    set_DI( DI_INDEX, 1 );
    }
//-----------------------------------------------------------------------------
void DI_1::off()
    {
    set_DI( DI_INDEX, 0 );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int float_state_device::save_changed_state( char *buff )
    {
    if ( prev_state != get_value() )
        {
        return save_state( buff );
        }
    return 0;
    }
//-----------------------------------------------------------------------------
int float_state_device::save_state( char *buff )
    {
    *( ( float* ) buff ) = get_value();
    prev_state = get_value();
    return sizeof( float );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float AI_1::get_value()
    {
    return get_AI( AI_INDEX ) / C_MAX_ANALOG_CHANNEL_VALUE *
        get_max_val();
    }
//-----------------------------------------------------------------------------
int AI_1::set_value( float new_value )
    {
    if ( new_value < get_min_val() )
        {
        new_value = get_min_val();
        }
    if ( new_value > get_max_val() )
        {
        new_value = get_max_val();
        }

    u_int value = ( u_int ) ( C_MAX_ANALOG_CHANNEL_VALUE * 
        new_value / get_max_val() );

    return set_AI( AI_INDEX, value );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float AO_1::get_value()
    {
    return get_AO( AO_INDEX ) / C_MAX_ANALOG_CHANNEL_VALUE * get_max_val();
    }
//-----------------------------------------------------------------------------
int AO_1::set_value( float new_value )
    {
    // Работаем в диапазоне 0-100.
    if ( new_value < get_min_val() )
        {
        new_value = get_min_val();
        }
    if ( new_value > get_max_val() )
        {
        new_value = get_max_val();
        }

    u_int_2 value = ( u_int_2 ) ( C_MAX_ANALOG_CHANNEL_VALUE * new_value /
        get_max_val() );

    return set_AO( AO_INDEX, value );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float temperature_e::get_max_val()
    {
    return C_MAX_ANALOG_CHANNEL_VALUE;
    }
//-----------------------------------------------------------------------------
float temperature_e::get_min_val()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
float temperature_e::get_value()
    {
    short int tmp = get_AI( AI_INDEX );
    float val = 0.1 * tmp;
    val = val >= -50 && val <= 150 ? val : -1000;

    return val;
    }
//-----------------------------------------------------------------------------
int temperature_e::set_value( float new_value )
    {
    return set_AI( AI_INDEX, ( int ) ( 10 * new_value ) );
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
