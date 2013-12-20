#include <stdlib.h>
#include <vector>
#include <stdio.h>

#include "tech_def.h"

#include "lua_manager.h"

#include "errors.h"
//-----------------------------------------------------------------------------
auto_smart_ptr < tech_object_manager > tech_object_manager::instance;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
tech_object::tech_object( const char* new_name, u_int number, u_int type,
    const char *name_Lua,
    u_int modes_count,
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
        type( type ),
        cmd( 0 ),
        modes_count( modes_count ),
        modes_time( run_time_params_u_int_4( modes_count, "MODES_TIME" ) ),

        modes_manager( 0 )
    {
    u_int state_size_in_int4 = modes_count / 32; // Размер состояния в double word.
    if ( modes_count % 32 > 0 ) state_size_in_int4++;
    for( u_int i = 0; i < state_size_in_int4; i++ )
        {
        state.push_back( 0 );
        }

    strlcpy( name, new_name, C_MAX_NAME_LENGTH );
    strlcpy( this->name_Lua, name_Lua, C_MAX_NAME_LENGTH );

    modes_manager = new mode_manager( modes_count );
    modes_manager->set_param( &par_uint );
    }
//-----------------------------------------------------------------------------
tech_object::~tech_object()
    {
    for ( u_int i = 0; i < errors.size(); i++ )
        {
        delete errors[ i ];
        errors[ i ] = 0;
        }
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

    Print( "%sStart \'%.40s\' [%2u] set mode = %2u --> %s.\n",
        white_spaces, name, number, mode,
        newm == 0 ? "OFF" : " ON" );

    white_spaces[ idx++ ] = ' ';
    white_spaces[ idx++ ] = ' ';
    white_spaces[ idx++ ] = ' ';
    white_spaces[ idx++ ] = 0;
#endif

    if ( newm != 0 ) newm = 1;
    if ( mode > modes_count ) 
        {
        res = 3;
        }
    if ( 0 == mode ) 
        {
        res = 4;
        }

    if ( 0 == res )
        {
        if ( get_mode( mode ) == newm ) res = 1;
        else
            {
            if ( newm == 0 ) // Off mode.
                {

                if ( ( res = lua_check_off_mode( mode ) ) == 0 ) // Check if possible.
                    {
                    int idx = mode - 1;
                    state[ idx / 32 ] = state[ idx / 32 ] & ~( 1UL << idx % 32 );
                    lua_final_mode( mode );
                    }
                else
                    {
                    res += 100;
                    }
                }
            else
                {
                //Проверка режима на проверку ОС устройств.
                const int ERR_STR_SIZE = 41;
                char res_str[ ERR_STR_SIZE ] = "обр. связь ";
                                
                int res = ( *modes_manager )[ mode ]->check_devices(
                    res_str + strlen( res_str ), ERR_STR_SIZE - strlen( res_str ) );
                if ( res && is_check_mode( mode ) == 1 )
                    {
                    set_err_msg( res_str, mode );
                    res = mode + 1000;
                    }
                else
                    {
                    bool is_dev_err = res ? true : false;
                    //Проверка режима на проверку ОС устройств.

                    if ( ( res = lua_check_on_mode( mode ) ) == 0 ) // Check if possible.
                        {
                        lua_init_mode( mode );
                        int idx = mode - 1;
                        state[ idx / 32 ] = state[ idx / 32 ] | 1UL << idx % 32;

                        //Проверка режима на проверку ОС устройств.
                        if ( is_dev_err )
                            {
                            set_err_msg( res_str, mode, 0, ERR_ON_WITH_ERRORS );
                            }
                        //Проверка режима на проверку ОС устройств.
                        }
                    else
                        {
                        res += 100;
                        }
                    }
                }
            }
        }
#ifdef DEBUG
    idx -= 4;
    white_spaces[ idx ] = 0;

    Print( "%sEnd \'%.40s\' [%2u] set mode = %2u --> %s, res = %d",
        white_spaces, name, number, mode,
        newm == 0 ? "OFF" : " ON", res );

    switch ( res )
        {
        case 1:
            Print( " (is already %s).\n", newm == 0 ? "OFF" : " ON" );
            break;

        case 3:
            Print( " (mode %d > modes count %d).\n",  mode, modes_count );
            break;

        case 4:
            Print( " (no zero (0) mode).\n" );
            break;

         default:
             if ( res > 100 )
                {
                Print( " (can't on).\n" );
                break;
                }

            Print( ".\n" );
            break;
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
    if ( mode > modes_count || 0 == mode ) return 0;

    int idx = mode - 1;
    return ( int )( ( u_int_4 ) ( state.at( idx / 32 ) >> idx % 32 ) & 1 );
    }
//-----------------------------------------------------------------------------
int tech_object::check_on_mode( u_int mode, char* reason )
    {
    if ( mode > modes_count || 0 == mode ) return 0;
        
    return (*modes_manager)[ mode ]->check_on( reason );
    }
//-----------------------------------------------------------------------------
void tech_object::init_mode( u_int mode )
    {
    if ( mode > modes_count || 0 == mode ) return;

    (*modes_manager)[ mode ]->init();
    }
//-----------------------------------------------------------------------------
int tech_object::evaluate()
    {
    for ( u_int i = 0; i < modes_count; i++ )
        {
        int idx = i + 1;
        modes_time[ idx ] =( *modes_manager )[ idx ]->evaluation_time() / 1000;

        if ( get_mode( idx ) == 1 )
        	{
            ( *modes_manager )[ idx ]->evaluate();
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
    if ( mode > modes_count || 0 == mode ) return 1;
        
    ( *modes_manager )[ mode ]->final();

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::lua_exec_cmd( u_int cmd )
    {
    tech_object::exec_cmd( cmd );

    return lua_manager::get_instance()->int_exec_lua_method( name_Lua, "exec_cmd",
        cmd, "int tech_object::lua_exec_cmd( u_int cmd )" );
    }
//-----------------------------------------------------------------------------
int tech_object::lua_check_on_mode( u_int mode )
    {
    char err_msg[ 200 ] = "";
        
    if ( int res = tech_object::check_on_mode( mode, err_msg ) )
        {
        set_err_msg( err_msg, mode );

        return 1000 + res;
        }

    return lua_manager::get_instance()->int_exec_lua_method( name_Lua,
        "check_on_mode", mode, "int tech_object::lua_check_on_mode( u_int mode )" );
    }
//-----------------------------------------------------------------------------
void tech_object::lua_init_mode( u_int mode )
    {
    tech_object::init_mode( mode );

    lua_manager::get_instance()->int_exec_lua_method( name_Lua,
        "init_mode", mode, "void tech_object::lua_init_mode( u_int mode )" );
    }
//-----------------------------------------------------------------------------
int tech_object::lua_check_off_mode( u_int mode )
    {
    tech_object::check_off_mode( mode );

    return lua_manager::get_instance()->int_exec_lua_method( name_Lua,
        "check_off_mode", mode, "int tech_object::lua_check_off_mode( u_int mode )" );
    }
//-----------------------------------------------------------------------------
int  tech_object::lua_final_mode( u_int mode )
    {
    tech_object::final_mode( mode );

    lua_manager::get_instance()->int_exec_lua_method( name_Lua,
        "final_mode", mode, "int tech_object::lua_final_mode( u_int mode )" );
    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::lua_init_params()
    {
    tech_object::init_params();

    //Проверка на наличии функции инициализации параметров uint.
    lua_getfield( lua_manager::get_instance()->get_Lua(), LUA_GLOBALSINDEX,
        name_Lua );
    lua_getfield( lua_manager::get_instance()->get_Lua(), -1, "init_params_uint" );
    lua_remove( lua_manager::get_instance()->get_Lua(), -2 );

    if ( lua_isfunction( lua_manager::get_instance()->get_Lua(), -1 ) )
        {
        lua_manager::get_instance()->int_exec_lua_method( name_Lua,
            "init_params_uint", 0, "int tech_object::lua_init_params()" );
        }
    //Удаляем "init_params_uint" со стека.
    lua_remove( lua_manager::get_instance()->get_Lua(), -1 );

    //Проверка на наличии функции инициализации параметров float.
    lua_getfield( lua_manager::get_instance()->get_Lua(), LUA_GLOBALSINDEX,
        name_Lua );
    lua_getfield( lua_manager::get_instance()->get_Lua(), -1, "init_params_float" );
    lua_remove( lua_manager::get_instance()->get_Lua(), -2 );

    if ( lua_isfunction( lua_manager::get_instance()->get_Lua(), -1 ) )
        {
        lua_manager::get_instance()->int_exec_lua_method( name_Lua,
            "init_params_float", 0, "int tech_object::lua_init_params()" );
        }
    //Удаляем "init_params_float" со стека.
    lua_remove( lua_manager::get_instance()->get_Lua(), -1 );

    //Проверка на наличии функции инициализации рабочих параметров uint.
    lua_getfield( lua_manager::get_instance()->get_Lua(), LUA_GLOBALSINDEX,
        name_Lua );
    lua_getfield( lua_manager::get_instance()->get_Lua(), -1, 
        "init_rt_params_uint" );
    lua_remove( lua_manager::get_instance()->get_Lua(), -2 );

    if ( lua_isfunction( lua_manager::get_instance()->get_Lua(), -1 ) )
        {
        lua_manager::get_instance()->int_exec_lua_method( name_Lua,
            "init_rt_params_uint", 0, "int tech_object::lua_init_params()" );
        }
    //Удаляем "init_params_uint" со стека.
    lua_remove( lua_manager::get_instance()->get_Lua(), -1 );

    //Проверка на наличии функции инициализации рабочих параметров float.
    lua_getfield( lua_manager::get_instance()->get_Lua(), LUA_GLOBALSINDEX,
        name_Lua );
    lua_getfield( lua_manager::get_instance()->get_Lua(), -1, 
        "init_rt_params_float" );
    lua_remove( lua_manager::get_instance()->get_Lua(), -2 );

    if ( lua_isfunction( lua_manager::get_instance()->get_Lua(), -1 ) )
        {
        lua_manager::get_instance()->int_exec_lua_method( name_Lua,
            "init_rt_params_float", 0, "int tech_object::lua_init_params()" );
        }
    //Удаляем "init_params_float" со стека.
    lua_remove( lua_manager::get_instance()->get_Lua(), -1 );

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::lua_init_runtime_params()
    {
    tech_object::init_runtime_params();

    return lua_manager::get_instance()->int_exec_lua_method( name_Lua,
        "init_runtime_params", 0, "int tech_object::lua_init_runtime_params()" );
    }
//-----------------------------------------------------------------------------
int tech_object::save_device( char *buff )
    {
    sprintf( buff, "t.%s = t.%s or {}\nt.%s=\n\t{\n",
        name_Lua, name_Lua,
        name_Lua );

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

    //Время простоя.
    char up_time_str [ 50 ];
    u_int_4 up_hours;
    u_int_4 up_mins;
    u_int_4 up_secs;

    up_secs = modes_manager->get_idle_time() / 1000;

    up_hours = up_secs / ( 60 * 60 );
    up_mins = up_secs / 60 % 60 ;
    up_secs %= 60;

    snprintf( up_time_str, 50, "\tIDLE_TIME = \'%02lu:%02lu:%02lu\',\n",
        ( u_long ) up_hours, ( u_long ) up_mins, ( u_long ) up_secs );
    sprintf( buff + answer_size, "%s", up_time_str );
    answer_size += strlen( buff + answer_size );

    //Время режимов.
    sprintf( buff + answer_size, "\tMODES_TIME=\n\t\t{\n\t\t" );
    answer_size += strlen( buff + answer_size );

    for ( u_int i = 1; i <= modes_time.get_count(); i++ )
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
            (*modes_manager)[ i + 1 ]->active_step());
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
        par_float.save( idx, ( float ) val );
        return 0;
        }

    if ( strcmp( prop, "RT_PAR_F" ) == 0 )
        {
        rt_par_float[ idx ] = ( float ) val;
        return 0;
        }

    if ( strcmp( prop, "S_PAR_UI" ) == 0 )
        {
        par_uint.save( idx, ( u_int_4 ) val );
        return 0;
        }

    if ( strcmp( prop, "RT_PAR_UI" ) == 0 )
        {
        rt_par_uint[ idx ] = ( u_int_4 ) val;
        return 0;
        }

#ifdef DEBUG
        Print( "Eror tech_object::set_cmd(...), prop = \"%s\", idx = %u, val = %f\n",
            prop, idx, val );
#endif // DEBUG

    return 1;
    }
//-----------------------------------------------------------------------------
int tech_object::save_params_as_Lua_str( char* str )
    {
    sprintf( str, "params{ object = \'%s\', param_name = \'%s\', "
        "par_id = %d,\n",
        name_Lua, "par_float", ID_PAR_FLOAT );
    par_float.save_device_ex( str + strlen( str ), "", "values"  );
    sprintf( str + strlen( str ) - 2, "%s", " }\n" );

    sprintf( str + strlen( str ), "params{ object = \'%s\', param_name = \'%s\', "
        "par_id = %d,\n",
        name_Lua, "rt_par_float", ID_RT_PAR_FLOAT );
    rt_par_float.save_device_ex( str + strlen( str ), "", "values"  );
    sprintf( str + strlen( str ) - 2, "%s", " }\n" );

    sprintf( str + strlen( str ), "params{ object = \'%s\', param_name = \'%s\', "
        "par_id = %d,\n",
        name_Lua, "par_uint", ID_PAR_UINT );
    par_uint.save_device_ex( str + strlen( str ), "", "values"  );
    sprintf( str + strlen( str ) - 2, "%s", " }\n" );

    sprintf( str + strlen( str ), "params{ object = \'%s\', param_name = \'%s\', "
        "par_id = %d,\n",
        name_Lua, "rt_par_uint", ID_RT_PAR_UINT );
    rt_par_uint.save_device_ex( str + strlen( str ), "", "values"  );
    sprintf( str + strlen( str ) - 2, "%s", " }\n" );

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::set_param( int par_id, int index, double value )
    {
    switch ( par_id )
        {
    case ID_PAR_FLOAT:
        par_float.save( index, ( float ) value );
        break;

    case ID_RT_PAR_FLOAT:
        rt_par_float[ index ] = ( float ) value;
        break;

    case ID_PAR_UINT:
        par_uint.save( index, ( u_int ) value );
        break;

    case ID_RT_PAR_UINT:
        rt_par_uint[ index ] = ( u_int ) value;
        break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::is_check_mode( int mode ) const
    {
    int res = lua_manager::get_instance()->int_exec_lua_method( name_Lua,
        "is_check_mode", mode, "int tech_object::is_check_mode( u_int mode )" );

    if ( res >= 0 )
        {
        return res;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::set_err_msg( const char *err_msg, int mode, int new_mode,
                             ERR_MSG_TYPES type /*= ERR_CANT_ON */ )
    {
    err_info *new_err = new err_info;
    static int error_number = 0;

    error_number++;
    new_err->n = error_number;
    new_err->type = type;
    
    switch ( type )
        {
        case ERR_CANT_ON:
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%.40s %d\' - не включен режим %.1d \'%.40s\' - %.60s.",
                name, number, mode, ( *modes_manager )[ mode ]->get_name(), err_msg );
            break;

        case ERR_ON_WITH_ERRORS:
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%.40s %d\' - включен с ошибкой режим %.1d \'%.40s\' - %.50s.",
                name, number, mode, ( *modes_manager )[ mode ]->get_name(), err_msg );
            break;

        case ERR_OFF:
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%.40s %d\' - отключен режим %.1d \'%.40s\' - %.50s.",
                name, number, mode, ( *modes_manager )[ mode ]->get_name(), err_msg );
            break;

        case ERR_OFF_AND_ON:
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%.40s %d\' - переход от %.1d \'%.40s\' к %.1d \'%.40s\'.",
                name, number, mode, ( *modes_manager )[ mode ]->get_name(),
                new_mode, ( *modes_manager )[ new_mode ]->get_name() );

            if ( strcmp( err_msg, "" ) != 0 )
                {
                snprintf( new_err->msg + strlen( new_err->msg ) - 1,
                    sizeof( new_err->msg ) - strlen( new_err->msg ) - 1,
                    " - %.50s.", err_msg );
                }
            break;

        case ERR_DURING_WORK:
            if ( mode > 0 )
                {
                snprintf( new_err->msg, sizeof( new_err->msg ),
                    "\'%.40s %d\' - режим %.1d \'%.40s\' - %.50s.",
                    name, number, mode, ( *modes_manager )[ mode ]->get_name(), err_msg );
                }
            else
                {
                snprintf( new_err->msg, sizeof( new_err->msg ),
                    "\'%.40s %d\' - %.50s.",
                    name, number, err_msg );
                }

            break;

        case ERR_ALARM:
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%.40s %d\' - %.60s.", name, number, err_msg );
            break;

        default:
#ifdef DEBUG
            Print( "Error tech_object::set_err_msg(...) - unknown error type!\n" );
            debug_break;
#endif // DEBUG
            snprintf( new_err->msg, sizeof( new_err->msg ),
                "\'%.40s\' - режим %.1d \'%.40s\' - %.50s.",
                name, mode, ( *modes_manager )[ mode ]->get_name(), err_msg );
            break;
        }

#ifdef DEBUG
    Print( "err_msg -> %s\n", err_msg );
    Print( "err_str -> %s\n", new_err->msg );
#endif // DEBUG

    if ( errors.size() < E_MAX_ERRORS_SIZE )
        {
        errors.push_back( new_err );
        }
#ifdef DEBUG
    else
        {
        Print( "Error: max errors count (%d) is reached.",
            E_MAX_ERRORS_SIZE );
        }
#endif // DEBUG

    return 0;
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

    static char has_Lua_eval = 0;
    if ( has_Lua_eval == 0 )
        {
        lua_getfield( lua_manager::get_instance()->get_Lua(), LUA_GLOBALSINDEX,
            "eval" );

        if ( lua_isfunction( lua_manager::get_instance()->get_Lua(), -1 ) )
            {
            has_Lua_eval = 2;
            }
        else
            {
            has_Lua_eval = 1;
            }
        }

    if ( has_Lua_eval == 2 )
        {
        lua_getfield( lua_manager::get_instance()->get_Lua(), LUA_GLOBALSINDEX,
            "eval" );

        lua_call( lua_manager::get_instance()->get_Lua(), 0, 0 );
        }
    }
//-----------------------------------------------------------------------------
int tech_object_manager::init_objects()
    {
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

    G_DEV_ERRORS_MANAGER->add_error( new tech_dev_error( new_tech_object ) );
    }
//-----------------------------------------------------------------------------
int tech_object_manager::save_params_as_Lua_str( char* str )
    {
    str[ 0 ] = 0;

    for ( u_int i = 0; i < tech_objects.size(); i++ )
        {
        tech_objects[ i ]->save_params_as_Lua_str( str + strlen( str ) );
        }
    return 0;
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
