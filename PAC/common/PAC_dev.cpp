#include "PAC_dev.h"

device_manager* device_manager::instance;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
device::device() : number( 0 ),
type( DEVICE_TYPE( 0 ) ),
sub_type( DEVICE_SUB_TYPE( 0 ) )

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
    load_table_from_string( cfg_file->fget_line(), DO_channels );
    load_table_from_string( cfg_file->fget_line(), AI_channels );
    load_table_from_string( cfg_file->fget_line(), AO_channels );

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
        DO_channels.tables && DO_channels.offsets )
        {
        return wago_manager::get_instance()->get_DO( DO_channels.tables[ index ],
            DO_channels.offsets[ index ] );
        }

#ifdef DEBUG
    Print( "wago_device->get_DO(...) - error!\n" );
#endif // DEBUG

    return 1;
    }
//-----------------------------------------------------------------------------
int wago_device::set_DO( u_int index, char value )
    {
    if ( index < DO_channels.count && 
        DO_channels.tables && DO_channels.offsets )
        {
        return wago_manager::get_instance()->set_DO( DO_channels.tables[ index ],
            DO_channels.offsets[ index ], value );
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
        DI_channels.tables && DI_channels.offsets )
        {
        return wago_manager::get_instance()->get_DI( DI_channels.tables[ index ],
            DI_channels.offsets[ index ] );
        }

#ifdef DEBUG
    Print( "wago_device->get_DI(...) - error!\n" );
#endif // DEBUG

    return 1;
    }
//-----------------------------------------------------------------------------
int wago_device::set_DI( u_int index, char value )
    {
    if ( index < DI_channels.count && 
        DI_channels.tables && DI_channels.offsets )
        {
        return wago_manager::get_instance()->set_DI( DI_channels.tables[ index ],
            DI_channels.offsets[ index ], value );
        }

#ifdef DEBUG
    Print( "wago_device->set_DI(...) - error!\n" );
#endif // DEBUG

    return 1;
    }
//-----------------------------------------------------------------------------
float wago_device::get_AO( u_int index )
    {
    if ( index < AO_channels.count && 
        AO_channels.tables && AO_channels.offsets )
        {
        return wago_manager::get_instance()->get_AO( AO_channels.tables[ index ],
            AO_channels.offsets[ index ] );
        }

#ifdef DEBUG
    Print( "wago_device->get_AO(...) - error!\n" );
#endif // DEBUG

    return 1;
    }
//-----------------------------------------------------------------------------
int wago_device::set_AO( u_int index, float value )
    {
    if ( index < AO_channels.count && 
        AO_channels.tables && AO_channels.offsets )
        {
        return wago_manager::get_instance()->set_AO( AO_channels.tables[ index ],
            AO_channels.offsets[ index ], value );
        }

#ifdef DEBUG
    Print( "wago_device->set_AO(...) - error!\n" );
#endif // DEBUG

    return 1;
    }
//-----------------------------------------------------------------------------
float wago_device::get_AI( u_int index )
    {
    if ( index < AI_channels.count && 
        AI_channels.tables && AI_channels.offsets )
        {
        return wago_manager::get_instance()->get_AI( AI_channels.tables[ index ],
            AI_channels.offsets[ index ] );
        }

#ifdef DEBUG
    Print( "wago_device->get_AI(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_device::set_AI( u_int index, float value )
    {
    if ( index < AI_channels.count && 
        AI_channels.tables && AI_channels.offsets )
        {
        return wago_manager::get_instance()->set_AI( AI_channels.tables[ index ],
            AI_channels.offsets[ index ], value );
        }

#ifdef DEBUG
    Print( "wago_device->set_AI(...) - error!\n" );
#endif // DEBUG

    return 1;
    }
//-----------------------------------------------------------------------------
void wago_device::print() const
    {
    print_table( "DI", DI_channels );
    Print( "; " );
    print_table( "DO", DO_channels );
    Print( "; " );
    print_table( "AI", AI_channels );
    Print( "; " );
    print_table( "AO", AO_channels );
    Print( ".\n" );
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
int DO_1::set_state( int new_state )
    {
    return digital_device::set_state( new_state );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
i_DO_device* device_manager::get_V( int number )
    {
    int res = get_device( device::DT_V, number );
    if ( -1 == res )
        {
#ifdef DEBUG
        Print( "V[ %d ] not found!\n", number );
#endif // DEBUG
        return &stub;
        } 
    
    switch (  project_devices[ res ]->get_type() )
        {
        case device::DT_V:     
            switch ( project_devices[ res ]->get_sub_type() )
                {
                case device::DST_V_DO_1:
                    return ( DO_1* ) project_devices[ res ];
                    
                case device::DST_V_DO_2:
                    return ( DO_2* ) project_devices[ res ];
                    
                case device::DST_V_DO_1_DI_1:
                    return ( DO_1_DI_1* ) project_devices[ res ];
                    
                case device::DST_V_DO_1_DI_2:
                    return ( DO_1_DI_2* ) project_devices[ res ];
                    
                case device::DST_V_DO_2_DI_2:
                    return ( DO_2_DI_2* ) project_devices[ res ];
                    
                case device::DST_V_MIXPROOF:
                    return ( mix_proof* ) project_devices[ res ];
                    
                default:
#ifdef DEBUG
                    Print( "device_manager::get_V(...) - unknown V device subtype %d!\n",
                        project_devices[ res ]->get_sub_type() );
#endif // DEBUG
                    return &stub;
                }
            break;

        default:
#ifdef DEBUG
            Print( "device_manager::get_V(...) - unknown device type %d!\n",
                project_devices[ res ]->get_sub_type() );
#endif // DEBUG
            return &stub;
        }
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
int device_manager::get_device( device::DEVICE_TYPE dev_type, u_int dev_number )
    {
    int l = dev_types_ranges[ dev_type ].start_pos;
    int u = dev_types_ranges[ dev_type ].end_pos;

    while ( l <= u ) 
        {
        int i = ( l + u ) / 2;

        if ( dev_number == project_devices[ i ]->get_n() ) return i;
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
        for ( int i = 0; i < devices_count; i++ )
            {
            int dev_type = 0;
            int dev_sub_type = 0;
            sscanf( cfg_file->pfget_line(), "%d %d", &dev_type, &dev_sub_type );

            switch ( dev_type )
                {
                case device::DT_V:     
                    switch ( dev_sub_type )
                        {
                        case device::DST_V_DO_1:
                            project_devices[ i ] = new DO_1();
                            break;

                        case device::DST_V_DO_2:
                            project_devices[ i ] = new DO_2();
                            break;

                        case device::DST_V_DO_1_DI_1:
                            project_devices[ i ] = new DO_1_DI_1();
                            break;

                        case device::DST_V_DO_1_DI_2:
                            project_devices[ i ] = new DO_1_DI_2();
                            break;

                        case device::DST_V_DO_2_DI_2:
                            project_devices[ i ] = new DO_2_DI_2();
                            break;

                        case device::DST_V_MIXPROOF:
                            project_devices[ i ] = new mix_proof();
                            break;

                        default:
#ifdef DEBUG
                            Print( "Unknown V device subtype %d!\n", dev_sub_type );
#endif // DEBUG
                            project_devices[ i ] = new dev_stub();
                            break;
                        }
                    break;

                case device::DT_N:
                    project_devices[ i ] = new DO_1_DI_1();
                    break;

                case device::DT_M:
                    project_devices[ i ] = new DO_1_DI_1();
                    break;

                case device::DT_LS:
                    project_devices[ i ] = new DI();
                    break;

                case device::DT_TE:
                    project_devices[ i ] = new AI();
                    break;

                case device::DT_FE:
                    project_devices[ i ] = new AI();
                    break;

                case device::DT_FS:
                    project_devices[ i ] = new dev_stub();
                    project_devices[ i ] = new DI();
                    break;

                case device::DT_CTR:
                    project_devices[ i ] = new counter();
                    break;

                case device::DT_AO:
                    project_devices[ i ] = new AO();
                    break;

                case device::DT_LE:
                    project_devices[ i ] = new AI();
                    break;

                case device::DT_FB:
                    project_devices[ i ] = new DI();
                    break;

                case device::DT_UPR:
                    project_devices[ i ] = new DO_1();
                    break;

                case device::DT_QE:
                    project_devices[ i ] = new AI();
                    break;

                case device::DT_AI:
                    project_devices[ i ] = new AI();
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
int DO_2::set_state( int new_state )
    {
    return digital_device::set_state( new_state );
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
int DO_1_DI_1::set_state( int new_state )
    {
    return digital_device::set_state( new_state );
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
int DO_1_DI_2::set_state( int new_state )
    {
    return digital_device::set_state( new_state );
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
int DO_2_DI_2::set_state( int new_state )
    {
    return digital_device::set_state( new_state );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int mix_proof::get_state()
    {
    int o = get_DO( DO_INDEX );            
    int i0 = get_DI( DI_INDEX_U );
    int i1 = get_DI( DI_INDEX_L );

    if ( ( o == 0 && i0 == 1 && i1 == 0 ) ||
        ( o == 1 && i1 == 1 && i0 == 0 ) )
        {
        switch_time = get_ms();
        if ( o == 0 && get_DO( DO_INDEX_U ) == 1 ) return 2;
        if ( o == 0 && get_DO( DO_INDEX_L ) == 1 ) return 3;
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
void mix_proof::on()
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
void mix_proof::open_upper_seat()
    {
    off();
    set_DO( DO_INDEX_U, 1 );
    }
//-----------------------------------------------------------------------------
void mix_proof::open_low_seat()
    {
    off();
    set_DO( DO_INDEX_L, 1 );
    }
//-----------------------------------------------------------------------------
void mix_proof::off()
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
int mix_proof::set_state( int new_state )
    {
    switch ( new_state )
        {
        case 0:
            off();
            break;

        case 1:
            on();
            break;

        case 2:
            open_upper_seat();
            break;

        case 3:
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
int DI::get_state()
    {
    return get_DI( DI_INDEX );
    }
//-----------------------------------------------------------------------------
void DI::on()
    {
    set_DI( DI_INDEX, 1 );
    }
//-----------------------------------------------------------------------------
void DI::off()
    {
    set_DI( DI_INDEX, 0 );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
