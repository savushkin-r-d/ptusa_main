#if !defined WIN_OS
#error You must define OS!
#endif // !defined WIN_OS

#include <string.h>
#include <stdlib.h>

#include "prj_mngr.h"
#include "wago.h"
#include "PAC_dev.h"
#include "param_ex.h"

#include "lua_manager.h"

#ifdef WIN_OS
#include "w_mem.h"
#endif // WIN_OS

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
    if ( instance.is_null() )
        {
        instance = new project_manager();

#ifdef WIN_OS
        instance->cfg_file = new data_file();
#endif // WIN_OS        
        }

    return instance;
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
int project_manager::lua_load_configuration()
    {
#ifdef DEBUG
    Print( "\nproject_manager - read configuration...\n" );

    //-File editor_version.
    int file_version = 
        lua_manager::get_instance()->int_no_param_exec_lua_method( "system",
        "get_file_version", "lua_load_configuration" );

    Print( "file_version %d\n", file_version );
#endif // DEBUG

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

#ifdef DEBUG
    wago_manager::get_instance()->print();
#endif // DEBUG
    
    //-Devices data.    
    lua_manager::get_instance()->void_exec_lua_method( "system", 
        "init_devices", "lua_load_configuration" );

    //G_DEVICE_CMMCTR->add_device( 
    //  device_manager::get_instance()->get_communication_device() );    

#ifdef DEBUG
    Print( "Reading configuration completed.\n\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
