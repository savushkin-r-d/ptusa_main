#include "main_cycle_test.h"
#include "lua_manager.h"
#include "tcp_cmctr.h"
#include "PAC_info.h"

using namespace ::testing;

extern bool G_NO_IO_NODES;
extern bool G_READ_ONLY_IO_NODES;

tm get_time_next_hour()
    {
    static struct tm timeInfo_;
#ifdef LINUX_OS
    auto t_ = time( nullptr );
    localtime_r( &t_, &timeInfo_ );
#else
    static time_t t_ = time( nullptr );
    localtime_s( &timeInfo_, &t_ );
#endif // LINUX_OS
    timeInfo_.tm_hour++;

    return timeInfo_;
    }

TEST( main_cycle, main_cycle )
    {
    auto L = lua_open();
    G_LUA_MANAGER->set_Lua( L );
    tcp_communicator::init_instance( "Тест", "Test" );

    G_NO_IO_NODES = false;
    G_READ_ONLY_IO_NODES = false;
    G_PAC_INFO()->par[ PAC_info::P_IS_OPC_UA_SERVER_ACTIVE ] = 1;

    main_cycle();
    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_time_next_hour ), SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );
    main_cycle();

    subhook_remove( get_time_hook );
    G_LUA_MANAGER->free_Lua();
    tcp_communicator::clear_instance();
    }
