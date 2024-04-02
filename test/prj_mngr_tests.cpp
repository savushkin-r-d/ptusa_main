#include <array>

#include "prj_mngr_tests.h"
#include "lua_manager.h"

extern const char* FILES[ FILE_CNT ];

using namespace ::testing;

extern bool G_NO_IO_MODULES;

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

    auto help = R"(Main control program
Usage:
  ptusa_main.exe [OPTION...] <script>

  -s, --script arg         The script file to execute (default: main.plua)
  -d, --debug              Enable debugging
  -p, --port arg           Param port (default: 10000)
  -h, --help               Print help info
  -r, --rcrc               Reset params
      --sys_path arg       Sys path
      --path arg           Path
      --extra_paths arg    Extra paths
      --sleep_time_ms arg  Sleep time, ms (default: 2)
)";

    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, help );

    std::array<const char*, 4> argv_ex = { "ptusa_main.exe", "--debug",
        "--rcrc", "main.plua" };
    testing::internal::CaptureStdout();
    res = G_PROJECT_MANAGER->proc_main_params( argv_ex.size(), argv_ex.data() );
    ASSERT_EQ( 0, res );
    auto debug = R"(DEBUG ON.
Resetting params (command line parameter "rcrc").
)";
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, debug );

    std::array<const char*, 12> argv_path{ "ptusa_main.exe", "--port", "20000",
        "--sys_path", "./sys/", "--path", "./", "--extra_paths", "./dairy_sys/",
        "--sleep_time_ms", "5", "main.plua" };

    res = G_PROJECT_MANAGER->proc_main_params( argv_path.size(), argv_path.data() );
    ASSERT_EQ( 0, res );

    G_LUA_MANAGER->free_Lua();
    }

TEST( project_manager, proc_main_params )
    {
    // Empty list.
    EXPECT_EQ( 0, G_PROJECT_MANAGER->proc_main_params( 0, nullptr ) );

    const char* argv[] = { "", "no_io_modules" };
    G_NO_IO_MODULES = false;
    EXPECT_EQ( 0, G_PROJECT_MANAGER->proc_main_params( 2, argv ) );
    EXPECT_TRUE( G_NO_IO_MODULES );
    }
