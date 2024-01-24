#include "prj_mngr_tests.h"
#include "lua_manager.h"

extern const char* FILES[ FILE_CNT ];

using namespace ::testing;

TEST( project_manager, lua_load_configuration )
    {
    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );


    // Should fail without correct data (no system Lua-scripts).
    auto res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );
   
    // Should not fail - correct data. System scripts (Lua) return 0 -
    // just everthing is OK.
    auto system_Lua_script = R"(
system =
    {
    create_io = function( self )
        return 0  
    end,

    create_devices = function( self )
        return 0
    end,

    init_devices_properties = function( self )
        return 0
    end,

    init_devices_params = function( self )
        return 0
    end
    }

init_tech_objects = function( self )
    return 0
end

object_manager =
    {
    --Получение количества пользовательских технологических объектов.
    get_objects_count = function( self )
        return 0
    end
    }
)";
    lua_manager::use_print_stack_traceback();
    EXPECT_EQ( 0, luaL_dostring( L, system_Lua_script ) );
    res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 0, res );


    G_LUA_MANAGER->free_Lua();    
    }
