#include "PAC_dev.h"

device_manager* device_manager::instance;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
device::device() : number( 0 )
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
    Print( "%5lu\n", ( unsigned long ) number );
#endif // DEBUG
    }
//-----------------------------------------------------------------------------
int device::load( char *stream )
    {
    number = *( ( u_int_4* ) stream );
    return sizeof( u_int_4 );
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
        buff[ 0 ] = get_state();
        prev_state = get_state();
        return sizeof( char );
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
int wago_device::load( char *stream )
    {
    //[ 0   ] - номер устройства.
    //[ 1   ] - 
    //[ 2   ] - 
    //[ 3   ] - 
    int pos = 0;

    //[ 4   ] - количество DI.
    //[ 5   ] - 
    //[ 6   ] - номер таблицы DI.
    //[ 7   ] - 
    //[ 8   ] - смещение в пределах таблицы.
    //[ ... ] -   
    //[ x1  ] - количество DO.
    //[ ... ] -   
    //[ x2  ] - количество AO.
    //[ ... ] -   
    //[ x3  ] - количество AI.
    //[ ... ] -            
    pos += load_table( stream + pos, DI_channels );
    pos += load_table( stream + pos, DI_channels );
    pos += load_table( stream + pos, DI_channels );
    pos += load_table( stream + pos, DI_channels );
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
int wago_device::load_table( char *stream, IO_channels &channels )
    {
    //[ 0   ] - количество DI (например).
    //[ 1   ] - 
    //[ 2   ] - номер таблицы DI №1.
    //[ 3   ] - 
    //[ 4   ] - смещение в пределах таблицы №1.
    //[ 5   ] -
    //[ 6   ] - номер таблицы DI №2.
    //[ 7   ] - 
    //[ 8   ] - смещение в пределах таблицы №2.
    //[ ... ] -  
    count = *( ( u_int_2 * ) stream );
    int pos = sizeof( u_int_2 );

    if ( count > 0 )
        {
        channels.count = count;

        channels.tables = new u_int_2[ count ];
        channels.offsets = new u_int_2[ count ];
        for ( u_int i = 0; i < count; i++ )
            {
            channels.tables[ i ] = *( ( u_int_2 * ) ( stream + pos ) );
            pos += sizeof( u_int_2 );

            channels.offsets[ i ] = *( ( u_int_2 * ) ( stream + pos ) );
            pos += sizeof( u_int_2 );
            }
        }

    return pos;
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
//-----------------------------------------------------------------------------
DO_device* device_manager::get_V( int number )
    {
    int res = get_device( device::DT_V, number );
    if ( -1 == res )
    	{
#ifdef DEBUG
        Print( "V[ %d ] not found!\n", number );
#endif // DEBUG
        return stub;
    	}

    return project_devices[ res ];    
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
int device_manager::get_device( device::DEVICE_TYPE dev_type, int dev_number )
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
