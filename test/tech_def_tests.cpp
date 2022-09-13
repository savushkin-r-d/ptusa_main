#include "tech_def_tests.h"
#include "lua_manager.h"

using namespace ::testing;

TEST( lua_get_run_step_after_pause, tech_object )
	{
	char* res = 0;
	mock_params_manager* par_mock = new mock_params_manager();
	test_params_manager::replaceEntity( par_mock );

	EXPECT_CALL( *par_mock, init( _ ) );
	EXPECT_CALL( *par_mock, final_init( _, _, _ ) );
	EXPECT_CALL( *par_mock, get_params_data( _, _ ) )
		.Times( AtLeast( 2 ) )
		.WillRepeatedly( Return( res ) );

	par_mock->init( 0 );
	par_mock->final_init( 0, 0, 0 );

	lua_State* L = lua_open();
	ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );
	G_LUA_MANAGER->set_Lua( L );

	ASSERT_EQ( 0,
		luaL_dostring( L, "o1=tech_object( \'O1\', 1, 1, \'o1\', 1, 1, 10, 10, 10, 10 )" ) );
	ASSERT_EQ( 0,
		luaL_dostring( L, "o1:get_modes_manager():add_operation(\'Test operation\')" ) );
	lua_getfield( L, LUA_GLOBALSINDEX, "o1" );
	auto tank = (tech_object*)tolua_tousertype( L, -1, 0 );
	ASSERT_NE( nullptr, tank );
	//Метода нет, должен быть возвращён 0.
	ASSERT_EQ( 0, tank->lua_get_run_step_after_pause( 1 ) );

	ASSERT_EQ( 0,
		luaL_dostring( L,
		"function o1:get_run_step_after_pause( m )\n"
		"    if m == 1 then return 1\n"
		"    else return 2 end\n"
		"end" ) );
	//Метод есть, должны быть возвращены определённые числа.
	ASSERT_EQ( 1, tank->lua_get_run_step_after_pause( 1 ) );
	ASSERT_EQ( 2, tank->lua_get_run_step_after_pause( 2 ) );

	G_LUA_MANAGER->free_Lua();
	test_params_manager::removeObject();
	}
