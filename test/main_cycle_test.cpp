#include "main_cycle_test.h"
#include "lua_manager.h"
#include "tcp_cmctr.h"

using namespace ::testing;

TEST( main_cycle, main_cycle )
    {
    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );
    tcp_communicator::init_instance( "Тест", "Test" );

    main_cycle();

    G_LUA_MANAGER->free_Lua();
    tcp_communicator::clear_instance();
    }
