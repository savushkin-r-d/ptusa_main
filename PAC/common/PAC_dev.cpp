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
    if ( prev_state != get_state() )
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

    return pos;
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

    return 1;
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
            Print("%d:%d", channels.tables[ i ],
                channels.offsets[ i ] );
            if ( i < channels.count - 1 ) Print( "; " );
            }
        Print( " ]" );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float DO_1::get_value()
    {
    return get_state();
    }
//-----------------------------------------------------------------------------
int DO_1::set_value( float new_value )
    {
    return set_state( ( int ) new_value );
    }
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
    return set_DO( DO_INDEX, new_state );
    }
//-----------------------------------------------------------------------------
int DO_1::parse_cmd( char *buff )
    {
    set_state( buff[ 0 ] );
    return sizeof( char );
    }
//-----------------------------------------------------------------------------
int DO_1::load( file *cfg_file )
    {
    device::load( cfg_file );
    wago_device::load( cfg_file );

    return 0;
    }
//-----------------------------------------------------------------------------
void DO_1::print() const
    {
    device::print();
    wago_device::print();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DO_device* device_manager::get_V( int number )
    {
    int res = get_device( device::DT_V, number );
    if ( -1 == res )
    	{
#ifdef DEBUG
        Print( "V[ %d ] not found!\n", number );
#endif // DEBUG
        return &stub;
    	}

    return ( DO_device* ) project_devices[ res ];
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
        project_devices = new device* [ devices_count ];
        for ( int i = 0; i < devices_count; i++ )
            {
            int dev_type = 0;
            int dev_sub_type = 0;
            sscanf( cfg_file->pfget_line(), "%d %d", &dev_type, &dev_sub_type );
            
            switch ( dev_type )
                {
                case device::DT_V:
                    switch ( dev_type )
                        {
                        case device::DST_V_1DO:
                            project_devices[ i ] = new DO_1();
                            project_devices[ i ]->load( cfg_file );

                            break;                    
                        }
                    break;

                case device::DT_CTR:
                    project_devices[ i ] = new counter();
                    project_devices[ i ]->load( cfg_file );

                    break;
                }

             project_devices[ i ]->print();

            }
        }


     
    return 0;
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
    char_state_device::load( cfg_file );
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