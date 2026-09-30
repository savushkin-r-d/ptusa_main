#include "includes.h"
#include "tech_def.h"
#include "lua_manager.h"
#include "g_errors.h"

namespace
{
class reload_manager : public tech_object_manager {};

class hot_reload_test : public ::testing::Test
    {
    protected:
        void SetUp() override
            {
            L = lua_open();
            luaL_openlibs( L );
            tolua_PAC_dev_open( L );
            G_LUA_MANAGER->set_Lua( L );
            G_ERRORS_MANAGER->clear();
            }

        void TearDown() override
            {
            G_ERRORS_MANAGER->clear();
            G_DEVICE_CMMCTR->clear_devices();
            G_LUA_MANAGER->free_Lua();
            }

        lua_State* L{};
    };
}

TEST_F( hot_reload_test, missing_object )
    {
    reload_manager manager;
    EXPECT_EQ( -1, manager.reload_object( 1 ) );
    }

TEST_F( hot_reload_test, failed_reload_keeps_object_and_lua_stack )
    {
    reload_manager manager;
    tech_object old_object( "Old", 1, 1, "OBJECT1", 0, 0, 0, 0, 0, 0 );
    manager.add_tech_object( &old_object );
    const char* scripts[] =
        {
        "reload_tech_object = nil",
        "function reload_tech_object(n) error('reload failed') end",
        "function reload_tech_object(n) return nil end",
        "function reload_tech_object(n) return {} end",
        "function reload_tech_object(n) return io.stdout end",
        };
    for ( auto script : scripts )
        {
        ASSERT_EQ( 0, luaL_dostring( L, script ) );
        const int top = lua_gettop( L );
        EXPECT_EQ( -3, manager.reload_object( 1 ) ) << script;
        EXPECT_EQ( &old_object, manager.get_tech_objects( 0 ) );
        EXPECT_EQ( top, lua_gettop( L ) ) << script;
        lua_settop( L, top );
        }
    }

TEST_F( hot_reload_test, running_paused_and_stopping_objects_are_rejected )
    {
    reload_manager manager;
    tech_object old_object( "Old", 1, 1, "OBJECT1", 1, 0, 0, 0, 0, 0 );
    old_object.get_modes_manager()->add_operation( "Operation" );
    manager.add_tech_object( &old_object );
    ASSERT_EQ( 0, luaL_dostring( L,
        "OBJECT1 = setmetatable({}, {__index = function() "
        "return function() return 0 end end})\n"
        "reload_called = false\n"
        "function reload_tech_object() reload_called = true end" ) );
    for ( const auto state : { operation::RUN, operation::PAUSE, operation::STOP } )
        {
        ASSERT_EQ( 0, old_object.set_mode( 1, state ) );
        EXPECT_EQ( -2, manager.reload_object( 1 ) );
        EXPECT_EQ( &old_object, manager.get_tech_objects( 0 ) );
        }
    ASSERT_EQ( 0, luaL_dostring( L, "assert(not reload_called)" ) );
    }

TEST_F( hot_reload_test, repeated_reload_preserves_serial_number )
    {
    reload_manager manager;
    tech_object old_object( "Old", 1, 1, "OBJECT1", 0, 0, 0, 0, 0, 0 );
    tech_object new_object( "New", 1, 1, "OBJECT1", 0, 0, 0, 0, 0, 0 );
    tech_object next_object( "Next", 1, 1, "OBJECT1", 0, 0, 0, 0, 0, 0 );
    manager.add_tech_object( &old_object );
    old_object.set_serial_idx( 42 );
    tolua_pushusertype( L, &new_object, "tech_object" );
    lua_setglobal( L, "replacement" );
    ASSERT_EQ( 0, luaL_dostring( L,
        "function reload_tech_object(n) assert(n == 42); return replacement end" ) );
    EXPECT_EQ( 0, manager.reload_object( 42 ) );
    EXPECT_EQ( &new_object, manager.get_tech_objects( 0 ) );
    EXPECT_EQ( 42u, new_object.get_serial_idx() );
    tolua_pushusertype( L, &next_object, "tech_object" );
    lua_setglobal( L, "replacement" );
    EXPECT_EQ( 0, manager.reload_object( 42 ) );
    EXPECT_EQ( &next_object, manager.get_tech_objects( 0 ) );
    }

TEST_F( hot_reload_test, replacing_errors_notifies_even_when_count_is_unchanged )
    {
    tech_object old_object( "Old", 1, 1, "OBJECT1", 0, 0, 0, 0, 0, 0 );
    tech_object new_object( "New", 1, 1, "OBJECT1", 0, 0, 0, 0, 0, 0 );
    G_ERRORS_MANAGER->add_error( new tech_obj_error( &old_object ) );
    G_ERRORS_MANAGER->evaluate();
    const auto previous_id = G_ERRORS_MANAGER->get_errors_id();
    EXPECT_EQ( 0, G_ERRORS_MANAGER->update_tech_object( &old_object, &new_object ) );
    G_ERRORS_MANAGER->evaluate();
    EXPECT_NE( previous_id, G_ERRORS_MANAGER->get_errors_id() );
    }
