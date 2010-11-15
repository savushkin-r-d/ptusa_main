#include <string.h>
#include <stdlib.h>

#include "prj_mngr.h"
#include "wago.h"
#include "PAC_dev.h"
#include "param_ex.h"

#include "lua_manager.h"

auto_smart_ptr < project_manager > project_manager::instance;
//-----------------------------------------------------------------------------
int project_manager::proc_main_params( int argc, char *argv[] )
    {
    for ( int i = 1; i < argc; i++ )
        {
        if ( strcmp( argv[ i ], "rcrc" ) == 0 )
            {
#ifdef DEBUG
            Print( "Resetting CRC ...\n" );
#endif
            params_manager::get_instance()->reset_CRC();
#ifdef DEBUG
            Print( "Resetting CRC completed.\n" );
#endif
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
project_manager* project_manager::get_instance()
    {
    return instance;
    }
//-----------------------------------------------------------------------------
void project_manager::set_instance( project_manager* new_instance )
    {
    instance = new_instance;
    }
//-----------------------------------------------------------------------------
project_manager::~project_manager()
    {
    if ( cfg_file )
        {
        delete cfg_file;
        cfg_file = 0;
        }
    }
//-----------------------------------------------------------------------------
void project_manager::free_instance()
    {
    instance.free();
    }
//-----------------------------------------------------------------------------
int project_manager::lua_load_configuration( const char *file_name )
    {
#ifdef DEBUG
    Print( "\nRead configuration...\n" );
#endif // DEBUG

    //-File editor_version.
    int file_version = 
        lua_manager::get_instance()->int_no_param_exec_lua_method( "system",
        "get_file_version", "lua_load_configuration" );
    
    //-Editor editor_version.
    const int CURRENT_EDITOR_VERSION = 12;
    int editor_version =  
        lua_manager::get_instance()->int_no_param_exec_lua_method( "system",
        "get_editor_version", "lua_load_configuration" );
        
    if ( editor_version < CURRENT_EDITOR_VERSION )
        {
#ifdef DEBUG
        Print( "project_manager:load_configuration(...) - not correct editor editor_version - %d, must be %d!\n",
            editor_version, CURRENT_EDITOR_VERSION );
#endif // DEBUG
        exit( 1 );
        }
    
    //-Wago data.
    lua_manager::get_instance()->void_exec_lua_method( "system", 
        "init_wago", "lua_load_configuration" );

    wago_manager::get_instance()->print();

    //-Devices data.    
    lua_manager::get_instance()->void_exec_lua_method( "system", 
        "init_devices", "lua_load_configuration" );

    G_DEVICE_CMMCTR->add_device( device_manager::get_instance()->get_device() );
    G_DEVICE_CMMCTR->print();

#ifdef DEBUG
    Print( "Reading configuration completed.\n\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
