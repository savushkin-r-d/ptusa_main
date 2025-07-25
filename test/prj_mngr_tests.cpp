#include <array>
#include <iomanip>

#include "prj_mngr_tests.h"
#include "lua_manager.h"
#include "dtime.h"

extern const char* FILES[ FILE_CNT ];

using namespace ::testing;

TEST( project_manager, lua_load_configuration )
    {
    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );


    // Should fail without correct data - no all required Lua-functions.
    auto res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );

    // Should fail without correct data - no create_devices() Lua-function.
    auto system_Lua_script_create_io = R"(
system =
    {
    create_io = function( self )
        return 0  
    end
    }
)";
    EXPECT_EQ( 0, luaL_dostring( L, system_Lua_script_create_io ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );


    // Should fail without correct data - no init_devices_properties() Lua-function.
    auto system_Lua_script_create_devices = R"(
system.create_devices = function( self )
        return 0
    end
)";
    EXPECT_EQ( 0, luaL_dostring( L, system_Lua_script_create_devices ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );

    // Should fail without correct data - no init_tech_objects() Lua-function.
    auto system_Lua_script_init_devices_properties = R"(
system.init_devices_properties = function( self )
        return 0
    end
)";
    EXPECT_EQ( 0, luaL_dostring( L, system_Lua_script_init_devices_properties ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );

    // Should fail without correct data - no get_objects_count() Lua-function.
    auto system_Lua_script_init_tech_objects = R"(
init_tech_objects = function( self )
    return 0
end
)";

    EXPECT_EQ( 0, luaL_dostring( L, system_Lua_script_init_tech_objects ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );

    // Should not fail - all required Lua-functions. They (Lua) return 0 -
    // just everthing is OK.
    auto object_manager_Lua_script_get_objects_count = R"(
object_manager =
    {
    --Получение количества пользовательских технологических объектов.
    get_objects_count = function( self )
        return 0
    end
    }
)";
    EXPECT_EQ( 0, luaL_dostring( L, object_manager_Lua_script_get_objects_count ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 0, res );


    G_LUA_MANAGER->free_Lua();
    }

TEST( project_manager, proc_main_params )
    {
    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );

    auto res = G_PROJECT_MANAGER->proc_main_params( 1, nullptr );
    ASSERT_EQ( 2, res );

    const char* empty_first_param[] = { "" };
    res = G_PROJECT_MANAGER->proc_main_params( 0, empty_first_param );
    ASSERT_EQ( 2, res );

    const char* null_first_param[] = { nullptr };
    res = G_PROJECT_MANAGER->proc_main_params( 1, null_first_param );
    ASSERT_EQ( 2, res );

    char argv0[] = "ptusa_main.exe";
    char argv1[] = "--help";
    const char* argv[] = { argv0, argv1 };
        
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( 2, argv );
    ASSERT_EQ( 1, res );

    auto help = 
#if defined WIN_OS
        R"(Main control program
Usage:
  ptusa_main.exe [OPTION...] <script>

  -s, --script arg          The script file to execute (default: main.plua)
  -d, --debug               Enable debugging
      --no_io_nodes         No communicate with I\O nodes (default: true)
      --read_only_io_nodes  Read only from I\O nodes (default: true)
  -p, --port arg            Param port (default: 10000)
  -h, --help                Print help info
  -r, --rcrc                Reset params
      --opc-r               Start OPC UA server with program start (only 
                            read)
      --opc-rw              Start OPC UA server with program start 
                            (read-write)
      --sys_path arg        Sys path (default: ./sys)
      --path arg            Path (default: .)
      --extra_paths arg     Extra paths (default: ./dairy-sys)
      --sleep_time_ms arg   Sleep time, ms (default: 2)
)";
#else
            R"(Main control program
Usage:
  ptusa_main.exe [OPTION...] <script>

  -s, --script arg          The script file to execute (default: main.plua)
  -d, --debug               Enable debugging
      --no_io_nodes         No communicate with I\O nodes
      --read_only_io_nodes  Read only from I\O nodes
  -p, --port arg            Param port (default: 10000)
  -h, --help                Print help info
  -r, --rcrc                Reset params
      --opc-r               Start OPC UA server with program start (only 
                            read)
      --opc-rw              Start OPC UA server with program start 
                            (read-write)
      --sys_path arg        Sys path (default: ./sys)
      --path arg            Path (default: .)
      --extra_paths arg     Extra paths (default: ./dairy-sys)
      --sleep_time_ms arg   Sleep time, ms (default: 2)
)";
#endif // defined WIN_OS

    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, help );


    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_fixed_time ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );


    // Отключаем работу с модулями ввода/вывода, сбрасываем параметры,
    // запускаем в отладочном режиме.
    std::array<const char*, 4> argv_ex = { "ptusa_main.exe", "--debug",
        "--rcrc", "main.plua" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    ASSERT_EQ( 0, res );
    std::string debug = R"(DEBUG ON.
Resetting params (command line parameter "rcrc").
)";
    std::tm tm = get_time();
    std::stringstream tmp;
    tmp << std::put_time( &tm, "%Y-%m-%d %H.%M.%S " );
#if defined WIN_OS
    debug += tmp.str() + "WARNING(4) -> Bus couplers are disabled.\n";
#else
    debug += tmp.str() + "\x1B[33mWARNING(4) -> Bus couplers are enabled.\n\x1B[0m";
#endif
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, debug );

    // Включаем OPC UA.
    argv_ex = { "ptusa_main.exe", "main.plua", "--opc-r", "" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    ASSERT_EQ( 0, res );
        
#if defined WIN_OS
    debug = tmp.str() + "WARNING(4) -> OPC UA server is activated (only read).\n";
    debug += tmp.str() + "WARNING(4) -> Bus couplers are disabled.\n";
#else
    debug = tmp.str() + "\x1B[33mWARNING(4) -> OPC UA server is activated (only read).\n\x1B[0m";
    debug += tmp.str() + "\x1B[33mWARNING(4) -> Bus couplers are enabled.\n\x1B[0m";
#endif
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, debug );

    // Включаем работу с модулями ввода/вывода.
    argv_ex = { "ptusa_main.exe", "main.plua", "--no_io_nodes=false", "" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    ASSERT_EQ( 0, res );

#if defined WIN_OS
    debug = tmp.str() + "WARNING(4) -> Bus couplers are enabled.\n";
    debug += tmp.str() + "WARNING(4) -> Bus couplers are read only.\n";
#else
    debug = tmp.str() + "\x1B[33mWARNING(4) -> Bus couplers are enabled.\n\x1B[0m";
#endif
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, debug );

    // Включаем работу с модулями ввода/вывода, включаем только чтение.
    argv_ex = { "ptusa_main.exe", "main.plua", "--no_io_nodes=false",
        "--read_only_io_nodes" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    ASSERT_EQ( 0, res );

#if defined WIN_OS
    debug = tmp.str() + "WARNING(4) -> Bus couplers are enabled.\n";
    debug += tmp.str() + "WARNING(4) -> Bus couplers are read only.\n";
#else
    debug = tmp.str() + "\x1B[33mWARNING(4) -> Bus couplers are enabled.\n\x1B[0m";
    debug += tmp.str() + "\x1B[33mWARNING(4) -> Bus couplers are read only.\n\x1B[0m";
#endif
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, debug );

    // Включаем работу с модулями ввода/вывода, отключаем только чтение.
    argv_ex = { "ptusa_main.exe", "main.plua", "--no_io_nodes=false",
        "--read_only_io_nodes=false" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    ASSERT_EQ( 0, res );

#if defined WIN_OS
    debug = tmp.str() + "WARNING(4) -> Bus couplers are enabled.\n";
#else
    debug = tmp.str() + "\x1B[33mWARNING(4) -> Bus couplers are enabled.\n\x1B[0m";
#endif
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, debug );

    std::array<const char*, 14> argv_path{ "ptusa_main.exe", "--port", "20000",
        "--sys_path", "./sys/", "--path", "./", "--extra_paths", "./dairy_sys/",
        "--sleep_time_ms", "5", "--no_io_nodes", "--read_only_io_nodes", "main.plua" };
    res = G_PROJECT_MANAGER->proc_main_params( argv_path.size(), argv_path.data() );
    ASSERT_EQ( 0, res );


    subhook_remove( get_time_hook );
    subhook_free( get_time_hook );
    G_LUA_MANAGER->free_Lua();
    }
