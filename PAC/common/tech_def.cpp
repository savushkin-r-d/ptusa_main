#include <stdlib.h>
#include <vector>
#include <stdio.h>

#include "tech_def.h"

#include "lua_manager.h"
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
        timers( timers_count ),
        number( number ),
        cmd( 0 ),
        modes_count( modes_count ),        
        modes_time( run_time_params_u_int_4( modes_count + 1, "MODES_TIME" ) ),

        modes_manager( 0 )
    {
    u_int state_size_in_int4 = modes_count / 32; // Размер состояния в double word.
    if ( modes_count % 32 > 0 ) state_size_in_int4++;
    for( u_int i = 0; i < state_size_in_int4; i++ )
        {
        state.push_back( 0 );
        }
    
    strncpy( name, new_name, C_MAX_NAME_LENGTH );
    strncpy( object_name, new_name, C_MAX_NAME_LENGTH );
    
    snprintf( name + strlen( name ), C_MAX_NAME_LENGTH, "%d", number );

    modes_manager = new mode_manager( modes_count );    
    modes_manager->set_param( &par_uint );
    }
//-----------------------------------------------------------------------------
tech_object::~tech_object()
    {
    //delete modes_manager;
    //modes_manager = 0;
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
        white_spaces, get_object_name(), number, mode, 
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
    
    Print( "%sEnd   %s[ %2u ] set mode = %2u --> %s, res = %d",
        white_spaces, get_object_name(), number, mode,
        newm == 0 ? "OFF" : " ON", res );

    if ( 1 == res )
    	{
        Print( "( is already %s).\n", newm == 0 ? "OFF" : " ON" );
    	}
    else
        {
        Print( ".\n" );
        }

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
    return modes_manager->check_on_mode( mode ) ? 0 : 1;
    }
//-----------------------------------------------------------------------------
void tech_object::init_mode( u_int mode )
    {
    modes_manager->init( mode );
    }
//-----------------------------------------------------------------------------
int tech_object::evaluate()
    {
    modes_time[ 0 ] = modes_manager->get_idle_time() / 1000;

    for ( u_int i = 0; i < modes_count; i++ )
        {
        modes_time[ i + 1 ] = modes_manager->get_mode_evaluation_time( i ) / 1000;

        if ( get_mode( i ) == 1 )
        	{
            modes_manager->evaluate( i );
        	}
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
        modes_manager->final( mode );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::lua_exec_cmd( u_int cmd )
    {
    tech_object::exec_cmd( cmd );

    return lua_manager::get_instance()->int_exec_lua_method( name, "exec_cmd",
        cmd, "int tech_object::lua_exec_cmd( u_int cmd )" );
    }
//-----------------------------------------------------------------------------
int tech_object::lua_check_on_mode( u_int mode )
    {
    if ( int res = tech_object::check_on_mode( mode ) ) return 1000 + res;

    return lua_manager::get_instance()->int_exec_lua_method( name, 
        "check_on_mode", mode, "int tech_object::lua_check_on_mode( u_int mode )" );
    }
//-----------------------------------------------------------------------------
void tech_object::lua_init_mode( u_int mode )
    {
    tech_object::init_mode( mode );

    lua_manager::get_instance()->int_exec_lua_method( name,
        "init_mode", mode, "void tech_object::lua_init_mode( u_int mode )" );
    }
//-----------------------------------------------------------------------------
int tech_object::lua_evaluate()
    {
    //return lua_manager::get_instance()->void_exec_lua_method( name,
    //    "evaluate", "int tech_object::lua_evaluate()" );

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::lua_check_off_mode( u_int mode )
    {
    tech_object::check_off_mode( mode );

    return lua_manager::get_instance()->int_exec_lua_method( name,
        "check_off_mode", mode, "int tech_object::lua_check_off_mode( u_int mode )" );    
    }
//-----------------------------------------------------------------------------
int  tech_object::lua_final_mode( u_int mode )
    {
    tech_object::final_mode( mode );

    lua_manager::get_instance()->int_exec_lua_method( name,
        "final_mode", mode, "int tech_object::lua_final_mode( u_int mode )" );
    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::lua_init_params()
    {
    tech_object::init_params();

    return lua_manager::get_instance()->int_exec_lua_method( name,
        "init_params", 0, "int tech_object::lua_init_params()" );
    }
//-----------------------------------------------------------------------------
int tech_object::lua_init_runtime_params()
    {
    tech_object::init_runtime_params();

    return lua_manager::get_instance()->int_exec_lua_method( name,
        "init_runtime_params", 0, "int tech_object::lua_init_runtime_params()" );
    }
//-----------------------------------------------------------------------------
int tech_object::save_device( char *buff )
    {
    sprintf( buff, "t.%s = t.%s or {}\nt.%s[%d]=\n\t{\n",
        get_object_name(), get_object_name(), 
        get_object_name(), get_number() );

    int answer_size = strlen( buff );

    //Состояние и команда.
    sprintf( buff + answer_size, "\tCMD=%lu,\n",
        ( u_long ) cmd );
    answer_size += strlen( buff + answer_size );

    sprintf( buff + answer_size, "\tST=\n\t\t{\n\t\t" );
    answer_size += strlen( buff + answer_size );
    for ( u_int i = 0; i < state.size(); i++ )
        {
        sprintf( buff + answer_size, "%lu, ", ( long int ) state[ i ] );
        answer_size += strlen( buff + answer_size );
        }
    sprintf( buff + answer_size, "\n\t\t},\n" );
    answer_size += strlen( buff + answer_size );

    //Режимы.
    sprintf( buff + answer_size, "\tMODES=\n\t\t{\n\t\t" );
    answer_size += strlen( buff + answer_size );
    for ( u_int i = 0; i < modes_count; i++ )
        {
        sprintf( buff + answer_size, "%d, ",
            get_mode( i ) ? 1 : 0 );
        answer_size += strlen( buff + answer_size );
        }
    sprintf( buff + answer_size, "\n\t\t},\n" );
    answer_size += strlen( buff + answer_size );
        
    //Время режимов.
    sprintf( buff + answer_size, "\tMODES_TIME=\n\t\t{\n\t\t" );
    answer_size += strlen( buff + answer_size );

    char up_time_str [ 50 ];
    u_int_4 up_hours;
    u_int_4 up_mins;
    u_int_4 up_secs;

    for ( u_int i = 1; i < modes_time.get_count(); i++ )
        {
        up_secs = modes_time[ i ];

        up_hours = up_secs / ( 60 * 60 );
        up_mins = up_secs / 60 % 60 ;
        up_secs %= 60;

        if ( up_hours )
            {
            snprintf( up_time_str, 50, "\'%02lu:%02lu:%02lu\', ",
                ( u_long ) up_hours, ( u_long ) up_mins, ( u_long ) up_secs );
            }
        else
            {
            if ( up_mins )
                {
                snprintf( up_time_str, 50, "\'   %02lu:%02lu\', ",
                    ( u_long ) up_mins, ( u_long ) up_secs );
                }
            else
                {
                snprintf( up_time_str, 50, "\'      %02lu\', ",
                    ( u_long ) up_secs );
                }
            }

        sprintf( buff + answer_size, "%s", up_time_str );
        answer_size += strlen( buff + answer_size );
        }
    sprintf( buff + answer_size, "\n\t\t},\n" );
    answer_size += strlen( buff + answer_size );
        
    //Шаги.
    sprintf( buff + answer_size, "\tMODES_STEPS=\n\t\t{\n\t\t" );
    answer_size += strlen( buff + answer_size );
    for ( u_int i = 0; i < modes_count; i++ )
        {
        sprintf( buff + answer_size, "%d, ",
            modes_manager->get_active_step( i ) );
        answer_size += strlen( buff + answer_size );
        }
    sprintf( buff + answer_size, "\n\t\t},\n" );
    answer_size += strlen( buff + answer_size );
    
    //Параметры.
    answer_size += par_float.save_device( buff + answer_size, "\t" );
    answer_size += par_uint.save_device( buff + answer_size, "\t" );
    answer_size += rt_par_float.save_device( buff + answer_size, "\t" );
    answer_size += rt_par_uint.save_device( buff + answer_size, "\t" );

    sprintf( buff + answer_size, "\t}\n" );
    answer_size += strlen( buff + answer_size );


    return answer_size;
    }
//-----------------------------------------------------------------------------
int tech_object::set_cmd( const char *prop, u_int idx, double val )
    {
    if ( strcmp( prop, "CMD" ) == 0 )
        {
#ifdef DEBUG      
        Print( "tech_object::set_cmd() - prop = \"%s\", val = %f\n", 
            prop, val );      
#endif // DEBUG

        if ( 0. == val )
            {
            cmd = 0;
            return 0;
            }

        u_int mode     = ( int ) val;
        char new_state = 0;

        if ( mode >= 1000 && mode < 2000 )      // On mode.
            {
            mode = mode - 1000;
            new_state = 1;
            }
        else
            {
            if ( mode >= 2000 && mode < 3000 )  // Off mode.
                {
                mode = mode - 2000;
                new_state = 0;
                }
            else
                {
#ifdef DEBUG
                Print( "Error complex_state::parse_cmd - new_mode = %lu\n",
                    ( unsigned long int ) mode );
#endif // DEBUG
                return 1;
                }
            }

        if ( mode > get_modes_count() )
            {
            // Command.
            cmd = lua_exec_cmd( mode );
            }
        else
            {
            // On/off mode.
            int res = set_mode( mode, new_state );
            if ( 0 == res )
                {
                cmd = ( int ) val;  // Ok.
                }
            else
                {
                cmd = res;          // Ошибка.
                }                    
            }  

        return 0;
        }

    if ( strcmp( prop, "S_PAR_F" ) == 0 )
        {
        par_float[ idx - 1 ] = ( float ) val;
        return 0;
        }

    if ( strcmp( prop, "RT_PAR_F" ) == 0 )
        {
        rt_par_float[ idx - 1 ] = ( float ) val;
        return 0;
        }

    if ( strcmp( prop, "S_PAR_UI" ) == 0 )
        {
        par_uint[ idx - 1 ] = ( u_int_4 ) val;
        return 0;
        }

    if ( strcmp( prop, "RT_PAR_UI" ) == 0 )
        {
        rt_par_uint[ idx - 1 ] = ( u_int_4 ) val;
        return 0;
        }

#ifdef DEBUG
        Print( "Eror tech_object::set_cmd(...), prop = \"%s\", idx = %u, val = %f\n",
            prop, idx, val );
#endif // DEBUG

    return 1;
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
    if ( instance.is_null() )
        {
        instance = new tech_object_manager();
        }

    return instance;
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
    try
        {
        return tech_objects.at( idx );
        }
    catch (...)
        {
#ifdef PAC_PC
        debug_break;
#endif // PAC_PC

        return 0;
        }   
    
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
        tech_objects.at( i )->evaluate();
        }

    static int call_count = 0;
    ++call_count;

    if ( call_count > 2 )
        {
        lua_getfield( lua_manager::get_instance()->get_Lua(), LUA_GLOBALSINDEX,
            "eval" );

        if ( lua_isfunction( lua_manager::get_instance()->get_Lua(), -1 ) )
            {
            lua_call( lua_manager::get_instance()->get_Lua(), 0, 0 );
            call_count = 0;
            }
        }
    
    }
//-----------------------------------------------------------------------------
int tech_object_manager::init_objects()
    {
    int res = lua_manager::get_instance()->int_exec_lua_method( "object_manager",
        "get_objects_count", 0, "int tech_object_manager::init_objects()" );
    if ( res < 0 )
        {
        Print( "Fatal error!\n" );
        exit( 1 );
        }

    int objects_count = res;
    for ( int i = 1; i <= objects_count; i++ )
        {
        void * res_object = lua_manager::get_instance()->user_object_exec_lua_method(
             "object_manager", "get_object", i,
             "int tech_object_manager::init_objects()" );

        if ( 0 == res_object )
            {
            Print( "Fatal error!\n" );
            exit( 1 );
            }

        add_tech_object( ( tech_object * ) res_object );
        }
    
    for ( u_int i = 0; i < get_count(); i++ )
        {
        G_TECH_OBJECTS( i )->lua_init_runtime_params();

        G_DEVICE_CMMCTR->add_device( G_TECH_OBJECTS( i ) );        
        }

    res = lua_manager::get_instance()->int_exec_lua_method( "system",
        "init_tech_dev_modes", 0, "int tech_object_manager::init_objects()" );
    if ( res < 0 )
        {
        Print( "Fatal error!\n" );
        exit( 1 );
        }

    //-Вызов пользовательской функции инициализации.
    lua_getfield( lua_manager::get_instance()->get_Lua(), LUA_GLOBALSINDEX,
        "init" );

    if ( lua_isfunction( lua_manager::get_instance()->get_Lua(), -1 ) )
        {
        lua_call( lua_manager::get_instance()->get_Lua(), 0, 0 );        
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
