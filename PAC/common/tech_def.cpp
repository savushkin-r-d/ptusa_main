#include <stdlib.h>
#include <vector>

#include "tech_def.h"
#include "sys.h"

#include <string>
#include <sstream>
#include <iostream>

extern lua_State * G_LUA;

lua_State *getthread (lua_State *L, int *arg)
{
  if (lua_isthread(L, 1)) {
    *arg = 1;
    return lua_tothread(L, 1);
  }
  else {
    *arg = 0;
    return L;
  }
}

  int error_trace( lua_State * L )
    {
    lua_Debug ar;
    int level = 1;

    int arg;
    lua_State *L1 = getthread( L, &arg);

    while( lua_getstack( L1, level, &ar ) )
        {
        lua_getinfo(L1, "Sln", &ar );
        Print( "source %s\n", ar.source);
        Print( "short_src %s\n", ar.short_src);
        Print( "linedefined %d\n", ar.linedefined);
        Print( "lastlinedefined %d\n", ar.lastlinedefined);
        Print( "what %s\n", ar.what);
        level++;
        }
    return 0;
    }
//-----------------------------------------------------------------------------
int add_file_and_line( const char *c_function_name, lua_State* L, int res )
    {
    Print( "C++ call - %s():\n", c_function_name );
    Print( "lua [file = %s]\n", "main.lua" );
	Print( "lua [ #line = %d]\n", res );
	Print( "lua [%s]\n", lua_tostring( G_LUA, -1 ) );
    lua_pop( G_LUA, 1 );


//   lua_Debug d;
//   lua_getstack(L, 1, &d);
//
//   std::string err = lua_tostring(L, -1);
//   lua_pop(L, 1);
//   std::cout << d.short_src << ":" << d.currentline;
//
//   if (d.name != 0)
//       {
//       std::cout << "(" << d.namewhat << " " << d.name << ")";
//       }
//   std::cout << " " << err;

    
//    lua_getinfo(L, "Snl", &ar);
//    Print( "source %s\n", ar.source);
//    Print( "short_src %s\n", ar.short_src);
//    Print( "linedefined %d\n", ar.linedefined);
//    Print( "lastlinedefined %d\n", ar.lastlinedefined);
//    Print( "what %s\n", ar.what);

    exit( 1 );
    }
//-----------------------------------------------------------------------------
auto_smart_ptr < tech_object_manager > tech_object_manager::instance;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
tech_object::tech_object( const char* new_name, u_int number, u_int modes_count,
    u_int timers_count,
    u_int par_float_count, u_int runtime_par_float_count,
    u_int par_uint_count,
    u_int runtime_par_uint_count) :        
        par_float( saved_params_float( par_float_count ) ),
        rt_par_float( run_time_params_float( runtime_par_float_count ) ),
        par_uint( saved_params_u_int_4( par_uint_count ) ),
        rt_par_uint( run_time_params_u_int_4( runtime_par_uint_count ) ),
        number( number ),
        modes_count( modes_count ),
        timers( timers_count ),
        mode_time( run_time_params_u_int_4( modes_count, "MODE_TIME" ) )
    {
    u_int state_size_in_int4 = modes_count / 32; // Размер состояния в double word.
    if ( modes_count % 32 > 0 ) state_size_in_int4++;
    for( u_int i = 0; i < state_size_in_int4; i++ )
        {
        state.push_back( 0 );
        }
    for( u_int i = 0; i < modes_count; i++ )
        {
        mode_start_time.push_back( 0 );
        mode_start_time.at( i ) = get_sec();
        }
    
    com_dev = new complex_device( number, new_name, 8,
            i_complex_device::COMPLEX_DEV );

    com_dev->sub_dev[ 0 ] = new single_state( "SINGLE_STATE", number,
            &state.front(), this, single_state::T_TECH_OBJECT, modes_count );
    com_dev->sub_dev[ 1 ] = new complex_state( "STATE", number,
            &state.front(), this, single_state::T_TECH_OBJECT, state_size_in_int4 );
    com_dev->sub_dev[ 2 ] = new complex_state( "CMD", number,
            &cmd, this,single_state::T_TECH_OBJECT );
    com_dev->sub_dev[ 3 ] = &par_float;
    com_dev->sub_dev[ 4 ] = &rt_par_float;
    com_dev->sub_dev[ 5 ] = &par_uint;
    com_dev->sub_dev[ 6 ] = &rt_par_uint;
    com_dev->sub_dev[ 7 ] = &mode_time;

    strncpy( name, new_name, C_MAX_NAME_LENGTH );
    snprintf( name + strlen( name ), C_MAX_NAME_LENGTH, "%d", number );
    }
//-----------------------------------------------------------------------------
tech_object::~tech_object()
    {
    delete com_dev->sub_dev[ 0 ];
    com_dev->sub_dev[ 0 ] = 0;
    delete com_dev->sub_dev[ 1 ];
    com_dev->sub_dev[ 1 ] = 0;
    delete com_dev->sub_dev[ 2 ];
    com_dev->sub_dev[ 2 ] = 0;

    delete com_dev;

    mode_start_time.clear();
    }
//-----------------------------------------------------------------------------
int tech_object::init_params()
    {
    par_float.reset_to_0();
    par_uint.reset_to_0();

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::init_runtime_params()
    {
    rt_par_float.reset_to_0();
    rt_par_uint.reset_to_0();

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::set_mode( u_int mode, int newm )
    {
    int res = 0;

#ifdef DEBUG
    static char   white_spaces[ 256 ] = "";
    static u_char idx = 0;

    Print( "%sStart %s[ %2u ] set mode = %2u --> %s.\n",
        white_spaces, com_dev->get_name(), number, mode, 
        newm == 0 ? "OFF" : " ON" );

    white_spaces[ idx++ ] = ' ';
    white_spaces[ idx++ ] = ' ';
    white_spaces[ idx++ ] = ' ';
    white_spaces[ idx++ ] = 0;
#endif

    if ( newm != 0 ) newm = 1;
    if ( mode > modes_count - 1 ) res = 3;
    else
        {
        if ( get_mode( mode ) == newm ) res = 1;
        else
            {
            if ( newm == 0 ) // Off mode.
                {

                if ( ( res = lua_check_off_mode( mode ) ) == 0 ) // Check if possible.
                    {
                    state[ mode / 32 ] = state[ mode / 32 ] & ~( 1UL << mode % 32 );
                    lua_final_mode( mode );
                    res = mode + 2000;
#ifdef USE_COMB
                g_greb->close_path( paths[ mode ].in_x, paths[ mode ].in_y,
                    paths[ mode ].out_x, paths[ mode ].out_y );
#endif // USE_COMB
                    }
                }
            else
                {
                if ( ( res = lua_check_on_mode( mode ) ) == 0 ) // Check if possible.
                    {
                    lua_init_mode( mode );
                    state[ mode / 32 ] = state[ mode / 32 ] | 1UL << mode % 32;
                    res = mode + 1000;
#ifdef USE_COMB
                    g_greb->open_path( paths[ mode ].in_x, paths[ mode ].in_y,
                        paths[ mode ].out_x, paths[ mode ].out_y, comb_path::OT_COMB,
                        this, mode );
#endif // USE_COMB
                    }
                }
            }
        }
#ifdef DEBUG
    idx -= 4;
    white_spaces[ idx ] = 0;
    
    Print( "%sEnd   %s[ %2u ] set mode = %2u --> %s, res = %d. \n",
        white_spaces, com_dev->get_name(), number, mode,
        newm == 0 ? "OFF" : " ON", res );

    for ( u_int i = 0; i < state.size(); i++ )
        {
        Print( "%sstate[ %d ] = %u (",
            white_spaces, i, state[ i ] );
        print_binary( state[ i ] );
        Print( ")\n" );
        }
    Print( "\n" );
#endif
  
    return res;
    }
//-----------------------------------------------------------------------------
int tech_object::get_mode( u_int mode )
    {
    if ( mode >= modes_count ) return 0;

    return ( int )( ( u_int_4 ) ( state.at( mode / 32 ) >> mode % 32 ) & 1 );
    }
//-----------------------------------------------------------------------------
int tech_object::check_on_mode( u_int mode )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
void tech_object::init_mode( u_int mode )
    {
    if ( mode < modes_count )
        {
        mode_start_time.at( mode ) = get_sec();
        }
    }
//-----------------------------------------------------------------------------
int tech_object::evaluate()
    {
    for ( u_int i = 0; i < modes_count; i++ )
        {
        mode_time[ i ] = get_sec() - mode_start_time.at( i );        
        }
    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::check_off_mode( u_int mode )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::final_mode( u_int mode )
    {
    if ( mode < modes_count )
        {
        mode_start_time.at( mode ) = get_sec();
        }
    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::exec_lua_chunk( int cmd, const char *function_name ) const
    {
    lua_getfield( G_LUA, LUA_GLOBALSINDEX, name );
    lua_getfield( G_LUA, -1, function_name );
    lua_remove( G_LUA, -2 );
    lua_getfield( G_LUA, LUA_GLOBALSINDEX, name );
    lua_pushnumber( G_LUA, cmd );
    
    int res = lua_pcall( G_LUA, 2, 1, 0 );
    if ( res )
        {
        add_file_and_line( function_name, G_LUA, res );
        return -1;
        }

    res = tolua_tonumber( G_LUA, -1, NULL );
    lua_pop( G_LUA, 1 );
    return res;
    }
//-----------------------------------------------------------------------------
int tech_object::lua_exec_cmd( u_int cmd )
    {
    return exec_lua_chunk( cmd, "exec_cmd" );
    }
//-----------------------------------------------------------------------------
int  tech_object::lua_check_on_mode( u_int mode )
    {
    return exec_lua_chunk( mode, "check_on_mode" );
    }
//-----------------------------------------------------------------------------
void tech_object::lua_init_mode( u_int mode )
    {
    exec_lua_chunk( mode, "init_mode" );
    }
//-----------------------------------------------------------------------------
int  tech_object::lua_evaluate()
    {
    return exec_lua_chunk( 0, "evaluate" );
    }
//-----------------------------------------------------------------------------
int  tech_object::lua_check_off_mode( u_int mode )
    {
    return exec_lua_chunk( mode, "check_off_mode" );
    }
//-----------------------------------------------------------------------------
int  tech_object::lua_final_mode( u_int mode )
    {
    return exec_lua_chunk( mode, "final_mode" );
    }
//-----------------------------------------------------------------------------
int  tech_object::lua_init_params()
    {
    return exec_lua_chunk( 0, "init_params" );
    }
//-----------------------------------------------------------------------------
int  tech_object::lua_init_runtime_params()
    {
    return exec_lua_chunk( 0, "init_runtime_params" );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int tech_object_manager::init_params()
    {
    for( u_int i = 0; i < tech_objects.size( ); i++ )
        {
        tech_objects[ i ]->lua_init_params( );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object_manager::init_runtime_params()
    {
    for ( u_int i = 0; i < tech_objects.size(); i++ )
        {
        tech_objects[ i ]->lua_init_runtime_params();
        }

    return 0;
    }
//-----------------------------------------------------------------------------
tech_object_manager* tech_object_manager::get_instance()
    {
    return instance;
    }
//-----------------------------------------------------------------------------
int tech_object_manager::set_instance( tech_object_manager* new_instance )
    {
    instance = new_instance;
    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object_manager::get_object_with_active_mode( u_int mode,
    u_int start_idx, u_int end_idx )
    {
    for ( u_int i = start_idx; 
        i <= end_idx && i < tech_objects.size(); i++ )
        {
        if ( tech_objects.at( i )->get_mode( mode ) ) return i;
        }

    return -1;
    }
//-----------------------------------------------------------------------------
tech_object* tech_object_manager::get_tech_objects( u_int idx )
    {
    return tech_objects.at( idx );
    }
//-----------------------------------------------------------------------------
u_int tech_object_manager::get_count() const
    {
    return tech_objects.size();
    }
//-----------------------------------------------------------------------------
void tech_object_manager::evaluate()
    {
    for ( u_int i = 0; i < tech_objects.size(); i++ )
        {
        tech_objects.at( i )->lua_evaluate();
        }
    }
//-----------------------------------------------------------------------------
int tech_object_manager::init_objects()
    {
    lua_getfield( G_LUA, LUA_GLOBALSINDEX, "object_manager" );
    lua_getfield( G_LUA, -1, "get_objects_count" );
    lua_remove( G_LUA, -2 );
    lua_getfield( G_LUA, LUA_GLOBALSINDEX, "object_manager" );
    int res = lua_pcall( G_LUA, 1, 1, 0 );
    if ( res )
        {
        add_file_and_line( "object_manager:get_objects_count", G_LUA, res );
        return -1;
        }

    int objects_count = tolua_tonumber( G_LUA, -1, NULL );
    for ( int i = 1; i <= objects_count; i++ )
        {
        lua_getfield( G_LUA, LUA_GLOBALSINDEX, "object_manager" );
        lua_getfield( G_LUA, -1, "get_object" );
        lua_remove( G_LUA, -2 );
        lua_getfield( G_LUA, LUA_GLOBALSINDEX, "object_manager" );
        lua_pushinteger( G_LUA, i );
        int res = lua_pcall( G_LUA, 2, 1, 0 );
        if ( res )
            {
            add_file_and_line( "object_manager:get_object", G_LUA, res );
            return -1;
            }

        tech_object *lua_object = ( tech_object* ) tolua_tousertype( G_LUA, -1, NULL );
        add_tech_object( lua_object );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
tech_object_manager::~tech_object_manager()
    {
// Удаляются в Lua.
//    for ( u_int i = 0; i < tech_objects.size(); i++ )
//        {
//        delete tech_objects[ i ];
//        }

    }
//-----------------------------------------------------------------------------
void tech_object_manager::add_tech_object( tech_object* new_tech_object )
    {
    tech_objects.push_back( new_tech_object );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
tech_object_manager* G_TECH_OBJECT_MNGR()
    {
    return tech_object_manager::get_instance();
    }
//-----------------------------------------------------------------------------
tech_object* G_TECH_OBJECTS( u_int idx )
    {
    return tech_object_manager::get_instance()->get_tech_objects( idx );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
