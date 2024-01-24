#include "prj_mngr_tests.h"
#include "lua_manager.h"

extern const char* FILES[ FILE_CNT ];

using namespace ::testing;

TEST( project_manager, lua_load_configuration )
    {
    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );


    // Should fail without correct data - no all system Lua-functions.
    auto res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );

    // Should fail without correct data - no create_devices() Lua-function.
    auto system_Lua_script_1 = R"(
system =
    {
    create_io = function( self )
        return 0  
    end
    }
)";
    EXPECT_EQ( 0, luaL_dostring( L, system_Lua_script_1 ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );


    // Should fail without correct data - no init_devices_properties() Lua-function.
    auto system_Lua_script_2 = R"(
system.create_devices = function( self )
        return 0
    end
)";
    EXPECT_EQ( 0, luaL_dostring( L, system_Lua_script_2 ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );

    // Should fail without correct data - no init_tech_objects() Lua-function.
    auto system_Lua_script_3 = R"(
system.init_devices_properties = function( self )
        return 0
    end
)";
    EXPECT_EQ( 0, luaL_dostring( L, system_Lua_script_3 ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );

    // Should fail without correct data - no get_objects_count() Lua-function.
    auto system_Lua_script_4 = R"(
init_tech_objects = function( self )
    return 0
end
)";

    EXPECT_EQ( 0, luaL_dostring( L, system_Lua_script_4 ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );

    // Should not fail - all system Lua-functions. They (Lua) return 0 -
    // just everthing is OK.
    auto system_Lua_script_5 = R"(
object_manager =
    {
    --Получение количества пользовательских технологических объектов.
    get_objects_count = function( self )
        return 0
    end
    }
)";
    EXPECT_EQ( 0, luaL_dostring( L, system_Lua_script_5 ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 0, res );


    G_LUA_MANAGER->free_Lua();    
    }
