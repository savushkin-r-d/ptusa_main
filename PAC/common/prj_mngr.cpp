#if !defined WIN_OS && !defined LINUX_OS
#error You must define OS!
#endif

#include <string.h>
#include <stdlib.h>

#include "prj_mngr.h"
#include "bus_coupler_io.h"
#include "PAC_dev.h"
#include "param_ex.h"

#include "lua_manager.h"

#include "tech_def.h"

#include "rm_manager.h"
#include "log.h"

#ifdef WIN_OS
#include "w_mem.h"

#pragma comment(lib, "Shell32.lib")
#include <shellapi.h>
#endif

#ifdef LINUX_OS
#include "l_mem.h"
#endif

auto_smart_ptr < project_manager > project_manager::instance;
//-----------------------------------------------------------------------------
int project_manager::proc_main_params( int argc, const char *argv[] )
    {
#ifdef WIN32
    wchar_t** w_argv = CommandLineToArgvW( GetCommandLineW(), &argc );
#endif

    for ( int i = 1; i < argc; i++ )
        {
        if ( strcmp( argv[ i ], "debug" ) == 0 )
            {
            G_DEBUG = 1;
            printf( "DEBUG ON.\n" );
            }
        }

    for ( int i = 1; i < argc; i++ )
        {
        if ( strcmp( argv[ i ], "rcrc" ) == 0 )
            {
            if ( G_DEBUG )
                {
                printf( "Resetting params (command line parameter \"rcrc\").\n" );
                }            
            params_manager::get_instance()->reset_params_size();
            }
        }

    // port 10001
    for ( int i = 1; i < argc - 1; i++ )
        {
        if ( strcmp( argv[ i ], "port" ) == 0 )
            {
            int p = atoi( argv[ i + 1 ] );

            if ( p > 0 )
                {
                tcp_communicator::set_port( p, p + 502 );

                sprintf( G_LOG->msg, 
                    "New tcp_communicator ports: %d [modbus %d].", p, p + 502 );
                G_LOG->write_log( i_log::P_NOTICE );
                }
            }
        }

    // sys_path  "C:/system_scripts"
    for ( int i = 1; i < argc - 1; i++ )
        {
        if ( strcmp( argv[ i ], "sys_path" ) == 0 )
            {
            init_sys_path( argv[ i + 1 ] );
            }
        }

    // path  "C:/project folder"
    for ( int i = 1; i < argc - 1; i++ )
        {
        if ( strcmp( argv[ i ], "path" ) == 0 )
            {
#ifdef WIN32
            wchar_t* w_path = w_argv[ i + 1 ];
            int utf16len = wcslen( w_path );
            int utf8len = WideCharToMultiByte( CP_UTF8, 0, w_path, utf16len,
                NULL, 0, NULL, NULL );

            char* path = new char[ utf8len + 1 ];            
            memset( path, 0, utf8len + 1 );

            WideCharToMultiByte( CP_UTF8, 0, w_path, utf16len, path, utf8len, 0, 0 );

            init_path( path );
#else
            init_path( argv[ i + 1 ] );
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
int project_manager::init_path( const char* path )
    {
    if ( path )
        {
        this->path = path;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int project_manager::init_sys_path( const char* sys_path )
    {
    if ( sys_path )
        {
        this->sys_path = sys_path;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//Порядок загрузки:
//1.Модули.
//2.Устройства.
//3.Переменные для доступа к устройства из Lua (совпадают с именем устройства).
int project_manager::lua_load_configuration()
    {
    if ( G_DEBUG ) 
        {
        printf( "\nProject manager - processing configuration...\n" );
        }

    //-I/O modules data.
    lua_manager::get_instance()->void_exec_lua_method( "system",
        "create_io", "project_manager::lua_load_configuration()" );

    if ( G_DEBUG ) 
        {
        if ( G_USE_LOG )
            {
            io_manager::get_instance()->print_log();
            }
        else
            {
            io_manager::get_instance()->print();
            printf( "\n" );
            }
        }

    //-Devices data.
    lua_manager::get_instance()->void_exec_lua_method( "system",
        "create_devices", "project_manager::lua_load_configuration()" );

    if ( G_DEBUG ) 
        {
        printf( "Получение имен и комментария к устройствам из Lua...\n");
        }
    //Получение имен и комментария к устройствам из Lua.
    lua_manager::get_instance()->void_exec_lua_method( "system",
        "init_devices_names", "project_manager::lua_load_configuration()" );
    if ( G_DEBUG ) 
        {
        printf( "Oк.\n");
        }

    if ( G_DEBUG ) 
        {
        printf( "Name for devices to access from Lua...\n");
        }
    //-Name for devices to access from Lua.
    lua_manager::get_instance()->void_exec_lua_method( "system",
        "init_dev_names", "project_manager::lua_load_configuration()" );
    if ( G_DEBUG ) 
        {
        printf( "Oк.\n");
        }

    if ( G_DEBUG ) 
        {
        printf( "Devices properties...\n");
        }
    //-Devices properties.
    lua_manager::get_instance()->void_exec_lua_method( "system",
        "init_devices_properties", "project_manager::lua_load_configuration()" );
    if ( G_DEBUG ) 
        {
        printf( "Oк.\n");
        }

    if ( G_DEBUG ) 
        {
        G_DEVICE_MANAGER()->print();
        printf( "\n" );
        }

    int res = lua_manager::get_instance()->int_exec_lua_method( "",
        "init_tech_objects", 0, "project_manager::lua_load_configuration()" );
    if ( res )
        {
        printf( "Fatal error!\n" );
        return 1;
        }

    res = lua_manager::get_instance()->int_exec_lua_method( "object_manager",
        "get_objects_count", 0, "project_manager::lua_load_configuration()" );
    if ( res < 0 )
        {
        printf( "Fatal error!\n" );
        return 1;
        }

    int objects_count = res;
    for ( int i = 1; i <= objects_count; i++ )
        {
        void * res_object = lua_manager::get_instance()->user_object_exec_lua_method(
            "object_manager", "get_object", i,
            "project_manager::lua_load_configuration()" );

        if ( 0 == res_object )
            {
            printf( "Fatal error!\n" );
            return 1;
            }

        G_TECH_OBJECT_MNGR()->add_tech_object( ( tech_object * ) res_object );
        }

    if ( G_DEBUG ) 
        {
        G_TECH_OBJECT_MNGR()->print();
        printf( "\n" );
        }

    //-Добавление технологических объектов проекта.
    for ( u_int i = 0; i < G_TECH_OBJECT_MNGR()->get_count(); i++ )
        {
        G_DEVICE_CMMCTR->add_device( G_TECH_OBJECTS( i ) );
        }
    //-Добавление системных тегов контроллера.
    G_DEVICE_CMMCTR->add_device( PAC_info::get_instance() );

    G_DEVICE_CMMCTR->add_device( siren_lights_manager::get_instance() );

    if ( G_DEBUG ) 
        {
        printf( "Получение конфигурации Profibus DP slave...\n");
        }

    lua_manager::get_instance()->void_exec_lua_method( "system",
        "init_profibus", "project_manager::lua_load_configuration()" );
    if ( G_DEBUG ) 
        {
        printf( "Oк.\n");
        }


#ifdef RM_PAC
    // Добавление удаленных PAC.
    if ( G_DEBUG ) 
        {
        printf( "Remote PAC's...\n");
        }

    res = lua_manager::get_instance()->int_exec_lua_method( "system",
        "init_rm_PACs", 0, "project_manager::lua_load_configuration()" );
    if ( res < 0 )
        {
        printf( "Fatal error!\n" );
        return 1;
        }

    if ( G_DEBUG ) 
        {
        G_RM_MANAGER()->print();
        printf( "\n" );
        }

#endif // RM_PAC

    if ( G_DEBUG ) 
        {
        printf( "Project manager - processing configuration completed.\n" );
        printf( "\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
