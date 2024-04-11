#include "PAC_dev_lua_tests.h"

using namespace ::testing;

TEST( toLuapp, tolua_PAC_dev_open )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    EXPECT_EQ( 0,
        luaL_dostring( L, "o1=tech_object( \'O1\', 1, 1, \'O1\', 1, 1, 10, 10, 10, 10 )" ) );
    EXPECT_EQ( 0,
        luaL_dostring( L, "o1:get_modes_manager():add_operation(\'Test operation\')" ) );

    //u_int operation_state::steps_count() const
    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ][ operation.RUN ]:steps_count()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_EQ( 0, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );

    EXPECT_EQ( 0,
        luaL_dostring( L, "o1:get_modes_manager()[ 1 ]:add_step(\'Test step\', -1, -1, -1 )" ) );
    
    // int enable_step_by_signal::set_bool_property( const char* name, bool value )
    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ][ 1 ][ 1 ][ step.A_ENABLE_STEP_BY_SIGNAL ]:"
        "set_bool_property( 'should_turn_off', false )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_EQ( 0, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );

    //int step::get_tag() const
    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ][ 1 ][ 1 ]:get_tag()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_EQ( -1, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );

    //int step::set_tag( int new_tag )
    EXPECT_EQ( 0,
        luaL_dostring( L, "o1:get_modes_manager()[ 1 ][ 1 ][ 1 ]:set_tag( 20 )" ) );
    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ][ 1 ][ 1 ]:get_tag()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_EQ( 20, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );

    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ][ operation.RUN ]:steps_count()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_EQ( 1, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );    

    //int jump_if_action::set_int_property( const char* name, unsigned int idx, int value );
    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ][ 1 ][ 1 ][ step.A_JUMP_IF ]:"
        "set_int_property( 'next_step_n', 0, 1 )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_EQ( 0, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );
    EXPECT_EQ( 0,
        luaL_dostring( L, "res=o1:get_modes_manager()[ 1 ][ 1 ][ 1 ][ step.A_JUMP_IF ]:"
        "set_int_property( 'no_such_property', 0, 1 )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    EXPECT_EQ( 1, tolua_tonumber( L, -1, 0 ) );
    lua_remove( L, -1 );

    
    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_PDS00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );
    ASSERT_EQ( 1, luaL_dostring( L, "res = PDS()" ) );  //Некорректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "res = PDS( \'PDS1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    auto PDS1 = static_cast<diff_pressure*>( tolua_touserdata( L, -1, 0 ) );
    EXPECT_NE( nullptr, PDS1 );
    lua_remove( L, -1 );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_TS00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );
    ASSERT_EQ( 1, luaL_dostring( L, "res = TS()" ) );   //Некорректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "res = TS( \'TS1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    auto TS1 = static_cast<temperature_signal*>( tolua_touserdata( L, -1, 0 ) );
    EXPECT_NE( nullptr, TS1 );
    lua_remove( L, -1 );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_C00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );
    ASSERT_EQ( 1, luaL_dostring( L, "res = C()" ) );   //Некорректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "res = C( \'TRC1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    auto TRC1 = static_cast<i_DO_AO_device*>( tolua_touserdata( L, -1, 0 ) );
    lua_remove( L, -1 );
    EXPECT_NE( nullptr, TRC1 );
    EXPECT_EQ( STUB(), dynamic_cast<dev_stub*>( TRC1 ) );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_device_param_emulator00 )
    {
    lua_State* L = lua_open();    
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L, 
        "G_DEVICE_MANAGER():add_io_device( 4, 1, \'TE1\', \'Test device\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L,
        "TE1 = G_DEVICE_MANAGER():get_device( 4, \'TE1\')" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "TE1" );
    auto TE1 = static_cast<device*>( tolua_touserdata( L, -1, 0 ) );
    EXPECT_NE( nullptr, TE1 );
    lua_remove( L, -1 );    

    const int BUFF_SIZE = 200;
    char buff[ BUFF_SIZE ] = { 0 };
    TE1->save_device( buff, "" );
    EXPECT_STREQ(
        "TE1={M=0, ST=1, V=0, E=0, M_EXP=20.0, S_DEV=2.0, P_CZ=0, P_ERR_T=0},\n",
        buff );

    //Некорректные параметры при вызове метода.
    ASSERT_EQ( 1, luaL_dostring( L, "TE1:param_emulator( 50 )" ) );

    ASSERT_EQ( 0, luaL_dostring( L, "TE1:param_emulator( 50, 5 )" ) );
    TE1->save_device( buff, "" );
    EXPECT_STREQ(
        "TE1={M=0, ST=1, V=0, E=0, M_EXP=50.0, S_DEV=5.0, P_CZ=0, P_ERR_T=0},\n",
        buff );

    lua_close( L );
    }

TEST( toLuapp, tolua_PAC_dev_i_wages_get_state00 )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );

    ASSERT_EQ( 0, luaL_dostring( L,
        "G_DEVICE_MANAGER():add_io_device( "
        "device.DT_WT, device.DST_WT_ETH, \'WT1\', \'Test wages\', \'\' )" ) );
    ASSERT_EQ( 0, luaL_dostring( L, "WT1 = WT( \'WT1\' )" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "WT1" );
    auto WT1 = static_cast<i_wages*>( tolua_touserdata( L, -1, 0 ) );
    EXPECT_NE( nullptr, WT1 );
    lua_remove( L, -1 );

    auto ip = "0.0.0.0";
    auto field = "IP";
    dynamic_cast<wages_eth*>( WT1 )->set_string_property( field, ip );

    auto st = WT1->get_state();
    //Некорректный вызов - отсуствует self.
    ASSERT_NE( 0, luaL_dostring( L, "res = WT1.get_state()" ) );    
    //Корректный вызов.
    ASSERT_EQ( 0, luaL_dostring( L, "res = WT1:get_state()" ) );
    lua_getfield( L, LUA_GLOBALSINDEX, "res" );
    auto lua_st = tolua_tonumber( L, -1, 0 );
    lua_pop( L, 1 );
    ASSERT_EQ( st, lua_st );

    lua_close( L );
    }
