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
	const unsigned int OPER_N1 = 1;
	const unsigned int OPER_N2 = 2;
	const unsigned int STEP_N1 = 1;
	const unsigned int STEP_N2 = 2;
	//Метода нет, должен быть возвращён 0.
	ASSERT_EQ( 0, tank->lua_get_run_step_after_pause( OPER_N1 ) );

	ASSERT_EQ( 0,
		luaL_dostring( L,
		"function o1:get_run_step_after_pause( m )\n"
		"    if m == 1 then return 2\n"
		"    else return 1 end\n"
		"end" ) );
	//Метод есть, должны быть возвращены определённые числа.
	ASSERT_EQ( STEP_N2, tank->lua_get_run_step_after_pause( OPER_N1 ) );
	ASSERT_EQ( STEP_N1, tank->lua_get_run_step_after_pause( OPER_N2 ) );
	
    ( *tank->get_modes_manager() )[ OPER_N1 ]->add_step( "Test step #1", -1, -1 );
	( *tank->get_modes_manager() )[ OPER_N1 ]->add_step( "Test step #2", -1, -1 );

	tank->set_mode( OPER_N1, operation::RUN );
	//После старта операции должен быть активным шаг 1.
	ASSERT_EQ( STEP_N1, ( *tank->get_modes_manager() )[ OPER_N1 ]->active_step() );
	tank->set_mode( OPER_N1, operation::PAUSE );
	tank->set_mode( OPER_N1, operation::RUN );
	//При возобновления операции из паузы должен быть активным шаг 2.
	ASSERT_EQ( STEP_N2, ( *tank->get_modes_manager() )[ OPER_N1 ]->active_step() );

	G_LUA_MANAGER->free_Lua();
	test_params_manager::removeObject();
	}

TEST( evaluate, tech_object )
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

	ASSERT_EQ( 0,
		luaL_dostring( L,
		"function o1:evaluate()\n"
		"end" ) );

	tank->evaluate();
	const unsigned int OPER_N1 = 1;	
	auto operation_1 = ( *tank->get_modes_manager() )[ OPER_N1 ];
	//Операция должна быть в состоянии бездействия.
	ASSERT_EQ( operation::IDLE, operation_1->get_state() );

	tank->set_mode( OPER_N1, operation::RUN );
	tank->evaluate();
	//Операция должна быть в состоянии выполнения.
	ASSERT_EQ( operation::RUN, operation_1->get_state() );

	tank->set_mode( OPER_N1, operation::IDLE );
	tank->evaluate();
	//Операция должна быть в состоянии бездействия.
	ASSERT_EQ( operation::IDLE, operation_1->get_state() );

	G_LUA_MANAGER->free_Lua();
	test_params_manager::removeObject();
	}
