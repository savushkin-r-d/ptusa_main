#if !defined WIN_OS && !defined LINUX_OS && !defined MINIOS7
#error You must define OS!
#endif 

#include <string.h>
#include <stdlib.h>

#include "prj_mngr.h"
#include "wago.h"
#include "PAC_dev.h"
#include "param_ex.h"

#include "lua_manager.h"

#include "tech_def.h"

#ifdef WIN_OS
#include "w_mem.h"
#endif

#ifdef LINUX_OS
#include "l_mem.h"
#endif

#ifdef MINIOS7
#include "mos7_mem.h"
#endif

auto_smart_ptr < project_manager > project_manager::instance;
//-----------------------------------------------------------------------------
int project_manager::proc_main_params( int argc, char *argv[] )
    {
    for ( int i = 1; i < argc; i++ )
        {
        if ( strcmp( argv[ i ], "rcrc" ) == 0 )
            {
#ifdef DEBUG
            Print( "Resetting CRC ... " );
#endif
            params_manager::get_instance()->reset_CRC();
#ifdef DEBUG
            Print( "completed.\n" );
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

#ifdef LINUX_OS
        instance->cfg_file = new data_file();
#endif // LINUX_OS
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
//Порядок загрузки:
//1.Модули Wago.
//2.Устройства Wago.
//3.Переменные для доступа к устройства из Lua (совпадают с именем устройства).
int project_manager::lua_load_configuration()
    {
#ifdef DEBUG
    Print( "\nProject manager - processing configuration...\n" );

    //-File editor_version.
    const char* file_date = 
        lua_manager::get_instance()->char_no_param_exec_lua_method( "system",
        "get_file_date", "project_manager::lua_load_configuration()" );

    Print( "--Files date: \"%s\"\n", file_date );
#endif // DEBUG
   
    //-Wago modules data.
    lua_manager::get_instance()->void_exec_lua_method( "system", 
        "create_wago", "project_manager::lua_load_configuration()" );

#ifdef DEBUG
    wago_manager::get_instance()->print();
#endif // DEBUG

    //-Devices data.    
    lua_manager::get_instance()->void_exec_lua_method( "system", 
        "create_devices", "project_manager::lua_load_configuration()" );
    
    //-Name for devices.    
    lua_manager::get_instance()->void_exec_lua_method( "system", 
        "init_dev_names", "project_manager::lua_load_configuration()" ); 

    //-Devices properties.    
    lua_manager::get_instance()->void_exec_lua_method( "system", 
        "init_devices_properties", "project_manager::lua_load_configuration()" );

#ifdef DEBUG    
    G_DEVICE_MANAGER()->print();        
#endif // DEBUG

    int res = lua_manager::get_instance()->int_exec_lua_method( "",
        "init_tech_objects", 0, "project_manager::lua_load_configuration()" );
    if ( res )
        {
        Print( "Fatal error!\n" );
        exit( 1 );
        }

    res = lua_manager::get_instance()->int_exec_lua_method( "object_manager",
        "get_objects_count", 0, "project_manager::lua_load_configuration()" );
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
            "project_manager::lua_load_configuration()" );

        if ( 0 == res_object )
            {
            Print( "Fatal error!\n" );
            exit( 1 );
            }

        G_TECH_OBJECT_MNGR()->add_tech_object( ( tech_object * ) res_object );
        }

    //-Добавление технологических объектов проекта.
    for ( u_int i = 0; i < G_TECH_OBJECT_MNGR()->get_count(); i++ )
        {
        G_DEVICE_CMMCTR->add_device( G_TECH_OBJECTS( i ) );        
        }
    //-Добавление системных тегов контроллера.
    G_DEVICE_CMMCTR->add_device( PAC_info::get_instance() );

#ifdef DEBUG
    Print( "Project manager - processing configuration completed.\n\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
