#include "prj_mngr_tests.h"
#include "lua_manager.h"

extern const char* FILES[ FILE_CNT ];

using namespace ::testing;

TEST( project_manager, lua_load_configuration )
    {
    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );


    // Should fail without correct data.
    auto res = G_PROJECT_MANAGER->lua_load_configuration();
    ASSERT_EQ( 1, res );

   
    G_LUA_MANAGER->free_Lua();    
    }
