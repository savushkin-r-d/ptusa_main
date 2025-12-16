#include <fmt/core.h>

#include "node_dev.h"
#include "bus_coupler_io.h"
#include "log.h"

//-----------------------------------------------------------------------------
node_dev::node_dev( const char* name )
	: device( name, device::DEVICE_TYPE::DT_NODE,
        device::DEVICE_SUB_TYPE::DST_NODE, 0 )
	{    
	}
//-----------------------------------------------------------------------------
void node_dev::set_io_node( io_manager::io_node *io_node )
    {
    node = io_node;
    }
//-----------------------------------------------------------------------------
void node_dev::evaluate_io()
	{
	// Обновление состояния узла.
	if ( node )
		{
		// Обновление состояния на основе состояния узла.
		device::direct_set_state( node->get_display_state() );
		}
	else
		{
		device::direct_set_state( -2 );
		}
	}
//-----------------------------------------------------------------------------
int node_dev::save_device( char* buff, const char* prefix )
	{
    return device::save_device( buff, prefix );
	}
//-----------------------------------------------------------------------------
int node_dev::save_device_ex( char* buff )
    {
    auto res_n = fmt::format_to_n( buff, MAX_COPY_SIZE,
        "WEB = {}, STARTUP = {}, IP = '{}', ",
        web_value, startup_value, get_ip() );
    return res_n.size;
    }
//-----------------------------------------------------------------------------
const char* node_dev::get_name_in_Lua() const
	{
	return "node_dev";
	}
//-----------------------------------------------------------------------------
const char* node_dev::get_ip() const
	{
	if ( node )
		{
		return node->ip_address;
		}

	return "";
	}
//-----------------------------------------------------------------------------
