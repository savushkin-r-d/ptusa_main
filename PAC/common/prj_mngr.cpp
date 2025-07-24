#if !defined WIN_OS && !defined LINUX_OS
#error You must define OS!
#endif

#include <string.h>
#include <stdlib.h>
#include <filesystem>

#include <cxxopts.hpp>
#include <fmt/core.h>

#include "prj_mngr.h"
#include "bus_coupler_io.h"
#include "device/device.h"
#include "device/manager.h"
#include "param_ex.h"
#include "params_recipe_manager.h"
#include "OPCUAServer.h"

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
#ifdef OPCUA
        ("opc-r", "Start OPC UA server with program start (only read)" )
        ("opc-rw", "Start OPC UA server with program start (read-write)" )
#endif        
        ( "sys_path", "Sys path", cxxopts::value<std::string>()->default_value( "./sys" ) )
        ( "path", "Path", cxxopts::value<std::string>()->default_value( "." ) )
        ( "extra_paths", "Extra paths", cxxopts::value<std::string>()->default_value( "./dairy-sys" ) )
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

#ifdef OPCUA
    if ( result[ "opc-rw" ].as<bool>() )
    {
        G_PAC_INFO()->par.save( PAC_info::P_IS_OPC_UA_SERVER_CONTROL, 1 );
        G_LOG->warning( "OPC UA server is activated(read-write)." );
    } else if( result[ "opc-r" ].as<bool>())
        {
        G_PAC_INFO()->par.save( PAC_info::P_IS_OPC_UA_SERVER_CONTROL, 0 );
        G_PAC_INFO()->par.save( PAC_info::P_IS_OPC_UA_SERVER_ACTIVE, 1 );
        G_LOG->warning( "OPC UA server is activated(only read)." );
        }
#endif
    main_script = result[ "script" ].as<std::string>();
    sleep_time_ms = result[ "sleep_time_ms" ].as<unsigned int>();
    
    path = result[ "path" ].as<std::string>();
    sys_path = result[ "sys_path" ].as<std::string>();
    extra_paths = result[ "extra_paths" ].as<std::string>();

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

    G_DEVICE_CMMCTR->add_device( ParamsRecipeManager::getInstance());

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