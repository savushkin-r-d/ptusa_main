#include "life_device.h"
//-----------------------------------------------------------------------------
watchdog::watchdog( const char* name, device::DEVICE_SUB_TYPE sub_type ) :device( name,
    device::DEVICE_TYPE::DT_WATCHDOG, sub_type,
    static_cast<u_int>( PARAM::PARAMS_COUNT ) - 1 )
    {
    set_par_name( static_cast<u_int>( PARAM::P_DT ), 0, "P_DT" );
    }
//-----------------------------------------------------------------------------
void watchdog::evaluate_io()
    {
    if ( !DI_dev && !AI_dev )
        {
        device::set_state( 0 );
        return;
        }

    if ( DI_dev )
        {
        if ( auto st = DI_dev->get_state(); prev_dev_state != st )
            {
            prev_dev_state = st;
            device::set_state( 1 );
            start_in_check_time = get_millisec();
            }
        }
    if ( AI_dev )
        {
        if ( auto v = AI_dev->get_value(); prev_dev_value != v )
            {
            prev_dev_value = v;
            device::set_state( 1 );
            start_in_check_time = get_millisec();
            }
        }
    
    if ( auto now = get_millisec(), dt = now - start_in_check_time,
        set_dt = static_cast<unsigned long>(
        get_par( static_cast<u_int>( PARAM::P_DT ) ) ); 
        dt > set_dt )
        {
        start_in_check_time = now;
        device::set_state( -1 );
        return;
        }

    if ( auto now = get_millisec(), dt = now - start_out_check_time,
        set_dt = static_cast<unsigned long>(
        get_par( static_cast<u_int>( PARAM::P_DT ) ) );
        dt > set_dt )
        {
        start_out_check_time = now;
        
        if ( DO_dev ) DO_dev->set_state( !DO_dev->is_active() );

        if ( AO_dev ) AO_dev->set_value( AO_dev->get_value() + 1 );
        if ( AO_dev->get_value() > MAX_OUT_VALUE ) AO_dev->set_value( 0 );
        }
    }
//-----------------------------------------------------------------------------
void watchdog::set_string_property( const char* field, const char* value )
    {
    if ( G_DEBUG )
        {
        G_LOG->debug( "%s\t watchdog::set_string_property() - "
            "field = %s, val = \"%s\"",
            get_name(), field, value );
        }

    if ( !field ) return;

    if ( strcmp( field, "DI_dev" ) == 0 )
        {
        DI_dev = G_DEVICE_MANAGER()->get_device( value );
        }
    else if ( strcmp( field, "AI_dev" ) == 0 )
        {
        AI_dev = G_DEVICE_MANAGER()->get_device( value );
        }
    else if ( strcmp( field, "DO_dev" ) == 0 )
        {
        DO_dev = G_DEVICE_MANAGER()->get_device( value );
        }
    else if ( strcmp( field, "AO_dev" ) == 0 )
        {
        AO_dev = G_DEVICE_MANAGER()->get_device( value );
        }
    else
        {
        G_LOG->alert( "%s\t watchdog::set_string_property() - "
            "Unknown field \"%s\"", get_name(), field );
        }
    }
//-----------------------------------------------------------------------------
void watchdog::set_property( const char* field, device* value )
    {
    if ( G_DEBUG )
        {
        G_LOG->debug( "%s\t watchdog::set_property() - "
            "field = %s, val = \"%s\"",
            get_name(), field, value ? value->get_name() : "nullptr" );
        }

    if ( !field ) return;

    if ( strcmp( field, "DI_dev" ) == 0 )
        {
        DI_dev = value;
        }
    else if ( strcmp( field, "AI_dev" ) == 0 )
        {
        AI_dev = value;
        }
    else if ( strcmp( field, "DO_dev" ) == 0 )
        {
        DO_dev = value;
        }
    else if ( strcmp( field, "AO_dev" ) == 0 )
        {
        AO_dev = value;
        }
    else
        {
        G_LOG->alert( "%s\t watchdog::set_property() - "
            "Unknown field \"%s\"", get_name(), field );
        }
    }
//-----------------------------------------------------------------------------
int watchdog::save_device( char* buff )
    {
    auto answer_size = device::save_device( buff, "" );

    return answer_size;
    }
//-----------------------------------------------------------------------------
const char* watchdog::get_name_in_Lua() const
    {
    return get_name();
    }
//-----------------------------------------------------------------------------
