#include "life_device.h"
//-----------------------------------------------------------------------------
life_device::life_device( const char* name, device::DEVICE_SUB_TYPE sub_type ) :device( name,
    device::DEVICE_TYPE::DT_LIFE_DEVICE, sub_type,
    static_cast<u_int>( PARAM::PARAMS_COUNT ) - 1 )
    {
    set_par_name( static_cast<u_int>( PARAM::P_DT ), 0, "P_DT" );
    }
//-----------------------------------------------------------------------------
void life_device::evaluate_io()
    {
    if ( !dev )
        {
        device::set_state( 0 );
        return;
        }

    if ( get_sub_type() == device::DEVICE_SUB_TYPE::DST_LIFEBIT )
        {
        if ( auto st = dev->get_state(); prev_dev_state != st )
            {
            prev_dev_state = st;
            device::set_state( 1 );
            start_time = get_millisec();
            return;
            }
        }
    else  // DST_LIFECOUNTER
        {
        if ( auto v = dev->get_value(); prev_dev_value != v )
            {
            prev_dev_value = v;
            device::set_state( 1 );
            start_time = get_millisec();
            return;
            }
        }

    
    if ( auto now = get_millisec(), dt = now - start_time,
        set_dt = static_cast<unsigned long>(
        get_par( static_cast<u_int>( PARAM::P_DT ) ) ); 
        dt > set_dt )
        {
        start_time = now;
        device::set_state( -1 );
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

    if ( strcmp( field, "DEV" ) == 0 )    // Explicitly match "DEV".
        {
        dev = G_DEVICE_MANAGER()->get_device( value );
        }
    else
        {
        G_LOG->alert( "%s\t life_device::set_string_property() - "
            "Unknown field \"%s\"", get_name(), field );
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

    if ( strcmp( field, "DEV" ) == 0 )    // Explicitly match "DEV".
        {
        dev = value;
        }
    else
        {
        G_LOG->alert( "%s\t life_device::set_property() - "
            "Unknown field \"%s\"", get_name(), field );
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
