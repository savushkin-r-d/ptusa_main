#include <fmt/core.h>

#include "node_dev.h"
#include "bus_coupler_io.h"
#include "log.h"

//-----------------------------------------------------------------------------
node_dev::node_dev( const char* name, device::DEVICE_SUB_TYPE sub_type )
	: device( name, device::DEVICE_TYPE::DT_NODE, sub_type,
		static_cast<u_int>( PARAM::PARAMS_COUNT ) - 1 )
	{
	set_par_name( static_cast<u_int>( PARAM::P_NODE_IDX ), 0, "P_NODE_IDX" );
	}
//-----------------------------------------------------------------------------
void node_dev::evaluate_io()
	{
	// Обновление состояния узла.
	int node_idx = static_cast<int>(
		get_par( static_cast<u_int>( PARAM::P_NODE_IDX ) ) );

	if ( node_idx >= 0 &&
		node_idx < static_cast<int>( G_IO_MANAGER()->get_nodes_count() ) )
		{
		io_manager::io_node* node = G_IO_MANAGER()->get_node( node_idx );
		if ( node )
			{
			// Обновление состояния на основе состояния узла.
			device::set_state( node->get_display_state() );
			st_value = node->is_active ? 1 : 0;
			}
		else
			{
			device::set_state( -1 );
			st_value = 0;
			}
		}
	else
		{
		device::set_state( -1 );
		st_value = 0;
		}
	}
//-----------------------------------------------------------------------------
void node_dev::set_string_property( const char* field, const char* value )
	{
	if ( G_DEBUG )
		{
		G_LOG->debug( "%s\t node_dev::set_string_property() - "
			"field = %s, val = \"%s\"",
			get_name(), field, value );
		}

	if ( !field ) return;

	// В данной реализации нет строковых свойств.
	G_LOG->alert( "%s\t node_dev::set_string_property() - "
		"Unknown field \"%s\"", get_name(), field );
	}
//-----------------------------------------------------------------------------
int node_dev::save_device( char* buff )
	{
	int res = 0;
	res = device::save_device( buff, "\t" );

	auto res_n = fmt::format_to_n( buff + res, MAX_COPY_SIZE - res,
		"ST = {}, WEB = {}, STARTUP = {}, CMD = {},\n",
		st_value, web_value, startup_value, cmd_value );
	res += res_n.size;

	return res;
	}
//-----------------------------------------------------------------------------
const char* node_dev::get_name_in_Lua() const
	{
	return "node_dev";
	}
//-----------------------------------------------------------------------------
const char* node_dev::get_ip() const
	{
	int node_idx = static_cast<int>(
		get_par( static_cast<u_int>( PARAM::P_NODE_IDX ) ) );

	if ( node_idx >= 0 &&
		node_idx < static_cast<int>( G_IO_MANAGER()->get_nodes_count() ) )
		{
		io_manager::io_node* node = G_IO_MANAGER()->get_node( node_idx );
		if ( node )
			{
			return node->ip_address;
			}
		}

	return "";
	}
//-----------------------------------------------------------------------------
int node_dev::get_node_state() const
	{
	return st_value;
	}
//-----------------------------------------------------------------------------
void node_dev::set_property_value( PROPERTIES prop, int value )
	{
	switch ( prop )
		{
		case PROPERTIES::ST:
			st_value = value;
			break;

		case PROPERTIES::WEB:
			web_value = value;
			break;

		case PROPERTIES::STARTUP:
			startup_value = value;
			break;

		case PROPERTIES::CMD:
			cmd_value = value;
			break;

		default:
			if ( G_DEBUG )
				{
				G_LOG->debug( "%s\t node_dev::set_property_value() - "
					"Unknown property %d",
					get_name(), static_cast<int>( prop ) );
				}
			break;
		}
	}
//-----------------------------------------------------------------------------
int node_dev::get_property_value( PROPERTIES prop ) const
	{
	switch ( prop )
		{
		case PROPERTIES::ST:
			return st_value;

		case PROPERTIES::WEB:
			return web_value;

		case PROPERTIES::STARTUP:
			return startup_value;

		case PROPERTIES::CMD:
			return cmd_value;

		default:
			if ( G_DEBUG )
				{
				G_LOG->debug( "%s\t node_dev::get_property_value() - "
					"Unknown property %d",
					get_name(), static_cast<int>( prop ) );
				}
			return 0;
		}
	}
//-----------------------------------------------------------------------------
