#include "life_device.h"
//-----------------------------------------------------------------------------
life_device::life_device( const char* name, life_device::TYPE type ) :device( name,
    device::DEVICE_TYPE::DT_LIFE_DEVICE,
    device::DEVICE_SUB_TYPE::DST_LIFEBIT,
    static_cast<u_int>( PARAM::PARAMS_COUNT ) - 1 ),
    dev_type( type )
    {
    set_par_name( static_cast<u_int>( PARAM::DT ), 0, "DT" );
    }
//-----------------------------------------------------------------------------
void life_device::evaluate_io()
    {
    if ( !dev )
        {
        return;
        }

    if ( dev_type == TYPE::T_BIT )
        {
        if ( auto st = dev->get_state(); prev_dev_state != st )
            {
            prev_dev_state = st;
            device::set_state( 1 );
            start_time = get_millisec();
            return;
            }
        }
    else  // TYPE::T_COUNTER
        {
        if ( auto v = dev->get_value(); prev_dev_value != v )
            {
            prev_dev_value = v;
            device::set_state( 1 );
            start_time = get_millisec();
            return;
            }
        }

    unsigned long now = get_millisec();
    unsigned long dt = now - start_time;
    auto set_dt = static_cast<u_long>( get_par( static_cast<u_int>( PARAM::DT ) ) );
    if ( dt > set_dt )
        {
        start_time = now;
        device::set_state( 0 );
        return;
        }
    }
//-----------------------------------------------------------------------------
void life_device::set_string_property( const char* field, const char* value )
    {
    if ( G_DEBUG )
        {
        G_LOG->debug( "%s\t life_device::set_string_property() - "
            "field = %s, val = \"%s\"",
            get_name(), field, value );
        }

    if ( !field ) return;

    if ( field[ 0 ] == 'D' )    // "DEV".
        {
        dev = G_DEVICE_MANAGER()->get_device( value );
        }
    }
//-----------------------------------------------------------------------------
void life_device::set_property( const char* field, device* value )
    {
    if ( G_DEBUG )
        {
        G_LOG->debug( "%s\t life_device::set_property() - "
            "field = %s, val = \"%s\"",
            get_name(), field, value ? value->get_name() : "nullptr" );
        }

    if ( !field ) return;

    if ( field[ 0 ] == 'D' )    // "DEV".
        {
        dev = value;
        }
    }
//-----------------------------------------------------------------------------
int life_device::save_device( char* buff )
    {
    auto answer_size = device::save_device( buff, "" );

    return answer_size;
    }
//-----------------------------------------------------------------------------
const char* life_device::get_name_in_Lua() const
    {
    return get_name();
    }   
//-----------------------------------------------------------------------------
