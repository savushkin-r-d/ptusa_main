#if !defined WIN_OS && !defined LINUX_OS
#error You must define OS!
#endif

#include <string.h>
#include <stdlib.h>

#include <cxxopts.hpp>

#include "prj_mngr.h"
#include "bus_coupler_io.h"
#include "PAC_dev.h"
#include "param_ex.h"

#include "lua_manager.h"

#include "tech_def.h"

#include "log.h"

#include "g_errors.h"

#ifdef WIN_OS
#include "w_mem.h"
#endif

#ifdef LINUX_OS
#include "l_mem.h"
#endif

extern bool G_NO_IO_NODES;
extern bool G_READ_ONLY_IO_NODES;

auto_smart_ptr < project_manager > project_manager::instance;
//-----------------------------------------------------------------------------
int project_manager::proc_main_params( int argc, const char* argv[] )
    {
    if ( !argc || !argv || !argv[ 0 ] ) return 2;

    //-Работа с параметрами командной строки.
    cxxopts::Options options( argv[ 0 ], "Main control program" );

    options.add_options()
        ( "s,script", "The script file to execute", cxxopts::value<std::string>()->default_value( "main.plua" ) )
        ( "d,debug", "Enable debugging", cxxopts::value<bool>()->default_value( "false" ) )
#if defined WIN_OS
        ( "no_io_nodes", "No communicate with I\\O nodes", cxxopts::value<bool>()->default_value( "true" ) )
        ( "read_only_io_nodes", "Read only from I\\O nodes", cxxopts::value<bool>()->default_value( "true" ) )
#else
        ( "no_io_nodes", "No communicate with I\\O nodes", cxxopts::value<bool>()->default_value( "false" ) )
        ( "read_only_io_nodes", "Read only from I\\O nodes", cxxopts::value<bool>()->default_value( "false" ) )
#endif // defined WIN_OS        
        ( "p,port", "Param port", cxxopts::value<int>()->default_value( "10000" ) )
        ( "h,help", "Print help info" )
        ( "r,rcrc", "Reset params" )
        ( "sys_path", "Sys path", cxxopts::value<std::string>() )
        ( "path", "Path", cxxopts::value<std::string>() )
        ( "extra_paths", "Extra paths", cxxopts::value<std::string>() )
        ( "sleep_time_ms", "Sleep time, ms", cxxopts::value<unsigned int>()->default_value( "2" ) );

    options.positional_help( "<script>" );
    options.parse_positional( { "script" } );
    options.show_positional_help();
    options.allow_unrecognised_options(); //Unrecognised arguments are allowed.
    auto result = options.parse( argc, argv );

    if ( result.count( "help" ) || argc < 2 )
        {
        fmt::print( options.help() );
        return 1;
        }

    if ( result[ "debug" ].as<bool>() )
        {
        G_DEBUG = 1;
        fmt::print( "DEBUG ON.\n" );
        }

    if ( result.count( "rcrc" ) )
        {
        if ( G_DEBUG )
            {
            fmt::print( "Resetting params (command line parameter \"rcrc\").\n" );
            }
        params_manager::get_instance()->reset_params_size();
        }

    if ( result.count( "port" ) )
        {
        int p = result[ "port" ].as<int>();
        if ( p > 0 )
            {
            tcp_communicator::set_port( p, p + 502 );
            G_LOG->notice( "New tcp_communicator ports: %d [modbus %d].", p, p + 502 );
            }
        }

    if ( result.count( "sys_path" ) )
        {
        auto& sys_path_str = result[ "sys_path" ].as<std::string>();
        init_sys_path( sys_path_str.c_str() );
        }
    if ( result.count( "path" ) )
        {
        auto& path_str = result[ "path" ].as<std::string>();
        init_path( path_str.c_str() );
        }
    if ( result.count( "extra_paths" ) )
        {
        auto& extra_paths_str = result[ "extra_paths" ].as<std::string>();
        init_extra_paths( extra_paths_str.c_str() );
        }
    main_script = result[ "script" ].as<std::string>();
    sleep_time_ms = result[ "sleep_time_ms" ].as<unsigned int>();

    // Отключить/включить обмен с модулями ввода/вывода.
    if ( result[ "no_io_nodes" ].as<bool>() )
        {
        G_NO_IO_NODES = true;
        }
    else
        {
        G_NO_IO_NODES = false;
        }
    // Только чтение/запись+чтение данных с модулей ввода/вывода.
    if ( result[ "read_only_io_nodes" ].as<bool>() )
        {
        G_READ_ONLY_IO_NODES = true;
        }
    else
        {
        G_READ_ONLY_IO_NODES = false;
        }

    if ( G_NO_IO_NODES )
        G_LOG->warning( "Bus couplers are disabled." );
    else
        {
        G_LOG->warning( "Bus couplers are enabled." );
        if ( G_READ_ONLY_IO_NODES )
            G_LOG->warning( "Bus couplers are read only." );           
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
        cfg_file = nullptr;
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
int project_manager::init_extra_paths( const char* paths )
    {
    if ( paths )
        {
        this->extra_paths = paths;
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
    auto res = lua_manager::get_instance()->void_exec_lua_method( "system",
        "create_io", "project_manager::lua_load_configuration()" );

    if ( res ) return 1;

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
    res = lua_manager::get_instance()->void_exec_lua_method( "system",
        "create_devices", "project_manager::lua_load_configuration()" );

    if ( res ) return 1;

    //-Devices properties.
    res = lua_manager::get_instance()->void_exec_lua_method( "system",
        "init_devices_properties", "project_manager::lua_load_configuration()" );

    if ( res ) return 1;

    if ( G_DEBUG )
        {
        printf( "Oк.\n" );
        }

    if ( G_DEBUG )
        {
        G_DEVICE_MANAGER()->print();
        printf( "\n" );
        }

    res = lua_manager::get_instance()->int_exec_lua_method( "",
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
        printf( "Получение конфигурации Profibus DP slave...\n" );
        }

    lua_manager::get_instance()->void_exec_lua_method( "system",
        "init_profibus", "project_manager::lua_load_configuration()" );
    if ( G_DEBUG )
        {
        printf( "Oк.\n" );
        }

    if ( G_DEBUG )
        {
        printf( "Project manager - processing configuration completed.\n" );
        printf( "\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------