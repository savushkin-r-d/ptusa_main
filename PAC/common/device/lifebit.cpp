#include "lifebit.h"
//-----------------------------------------------------------------------------
lifebit::lifebit( const char* name ) :device( name,
    device::DEVICE_TYPE::DT_LIFE_DEVICE,
    device::DEVICE_SUB_TYPE::DST_LIFEBIT,
    static_cast<u_int>( PARAM::PARAMS_COUNT ) - 1 )
    {
    set_par_name( static_cast<u_int>( PARAM::DT ), 0, "DT" );
    }
//-----------------------------------------------------------------------------
void lifebit::evaluate_io()
    {
    if ( !di_device )
        {
        return;
        }

    if ( auto st = di_device->get_state(); prev_di_state != st )
        {
        prev_di_state = st;
        device::set_state( 1 );
        start_time = get_millisec();        
        return;
        }

    unsigned long now = get_millisec();
    unsigned long dt = now - start_time;
    if ( dt > get_par( static_cast<u_int>( PARAM::DT ) ) )
        {
        start_time = now;
        device::set_state( 0 );
        return;
        }
    }
//-----------------------------------------------------------------------------
void lifebit::set_string_property( const char* field, const char* value )
    {
    if ( !field ) return;

    switch ( field[ 0 ] )
        {
        //DI
        case 'D':
            di_device = G_DEVICE_MANAGER()->get_device( value );
            break;

        default:
            device::set_string_property( field, value );
            break;
        }
    }
//-----------------------------------------------------------------------------
void lifebit::set_property( const char* field, device* value )
    {
    if ( !field ) return;

    switch ( field[ 0 ] )
        {
        //DI
        case 'D':
            di_device = value;
            break;

        default:
            device::set_property( field, value );
            break;
        }
    }
//-----------------------------------------------------------------------------
int lifebit::save_device( char* buff )
    {
    auto answer_size = device::save_device( buff, "\t" );

    return answer_size;
    }
//-----------------------------------------------------------------------------
const char* lifebit::get_name_in_Lua() const
    {
    return get_name();
    }   
//-----------------------------------------------------------------------------
