#include "tech_def_tests.h"
#include "lua_manager.h"

using namespace ::testing;

TEST( tech_object, lua_get_run_step_after_pause )
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

TEST( tech_object, evaluate )
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


    //Корректный переход от выполнения к паузе и опять к выполнению.
    const unsigned int STEP_N1 = 1;
    const unsigned int STEP_N2 = 2;
    operation_1->add_step( "Init", 2, -1 );
    operation_1->add_step( "Process #1", 3, -1 );
    operation_1->add_step( "Process #2", 2, -1 );

    tank->set_mode( OPER_N1, operation::RUN );
    EXPECT_EQ( operation::RUN, tank->get_mode( OPER_N1 ) );
    EXPECT_EQ( STEP_N1, operation_1->active_step() );
    operation_1->to_next_step();                    //Переход к следующему шагу.
    EXPECT_EQ( STEP_N2, operation_1->active_step() );
    tank->set_mode( OPER_N1, operation::PAUSE );    //Пауза.
    tank->set_mode( OPER_N1, operation::RUN );      //Возобновление.
    //После возобновления после паузы должен быть активен шаг, который был
    //до паузы.
    EXPECT_EQ( STEP_N2, operation_1->active_step() );

    tank->set_mode( OPER_N1, operation::IDLE );
    EXPECT_EQ( operation::IDLE, tank->get_mode( OPER_N1 ) );


    G_LUA_MANAGER->free_Lua();
    test_params_manager::removeObject();
    }

TEST( tech_object, lua_check_function )
    {
    lua_State* L = lua_open();
    ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );
    G_LUA_MANAGER->set_Lua( L );


    tech_object tank1( "TANK", 1, 1, "TANK1", 10, 1, 10, 10, 10, 10 );

    //В Lua нет ни объекта танка, ни функций проверки. Вызов должен отработать
    //корректно и вернуть 0.
    auto res = tank1.lua_check_function( "no_function", "test call", 1, true );
    ASSERT_EQ( 0, res );

    G_LUA_MANAGER->free_Lua();
    }

TEST( tech_object, save )
    {
	lua_State* L = lua_open();
	ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );
	G_LUA_MANAGER->set_Lua( L );

	//Tank with no operations.
    tech_object tank0( "TANK", 1, 1, "TANK0", 0, 1, 10, 10, 10, 10 );
	const auto BUFF_SIZE = 1000;
	char buff[ BUFF_SIZE ];
	tank0.save_device( buff );
	auto REF_STR0 = R"(t.TANK0 = t.TANK0 or {}
t.TANK0=
	{
	CMD=0,
	LASTRECNMR=0,
	LASTRECNAME='Не выбран',
	ST=
		{
		
		},
	MODES=
		{
		
		},
	OPERATIONS=
		{
		
		},
	AVAILABILITY=
		{
		
		},
	ACTIVE_OPERATION_OR_IDLE_TIME=0,
	ACTIVE_STEP_TIME=0,
	MODES_TIME=
		{
		
		},
	MODES_STEPS=
		{
		
		},
	MODES_RUN_STEPS=
		{
		
		},
	S_PAR_F=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		},
	S_PAR_UI=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		},
	RT_PAR_F=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		},
	RT_PAR_UI=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		},
	}
)";
	EXPECT_STREQ( REF_STR0, buff );

	tech_object tank1( "TANK", 1, 1, "TANK1", 2, 1, 10, 10, 10, 10 );
    tank1.get_modes_manager()->add_operation( "Test operation" );
    tank1.save_device( buff );
    auto REF_STR1 = R"(t.TANK1 = t.TANK1 or {}
t.TANK1=
	{
	CMD=0,
	LASTRECNMR=0,
	LASTRECNAME='Не выбран',
	ST=
		{
		0, 
		},
	MODES=
		{
		0, 0, 
		},
	OPERATIONS=
		{
		0, 0, 
		},
	AVAILABILITY=
		{
		1, 1, 
		},
	ACTIVE_OPERATION_OR_IDLE_TIME=0,
	ACTIVE_STEP_TIME=0,
	MODES_TIME=
		{
		0, 0, 
		},
	MODES_STEPS=
		{
		0, 0, 
		},
	MODES_RUN_STEPS=
		{
		-1, -1, 
		},
	S_PAR_F=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		},
	S_PAR_UI=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		},
	RT_PAR_F=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		},
	RT_PAR_UI=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		},
	}
)";
	EXPECT_STREQ( REF_STR1, buff );


    //Время активной операции и её главного шага.
    const unsigned int OPER_N1 = 1;
    auto operation_1 = ( *tank1.get_modes_manager() )[ OPER_N1 ];

    operation_1->add_step( "Init", 2, -1 );
    operation_1->add_step( "Process #1", 3, -1 );
    operation_1->add_step( "Process #2", 2, -1 );

    tank1.set_mode( OPER_N1, operation::RUN );
    tank1.evaluate();
    sleep_ms( 1000 );
    tank1.save_device( buff );

    auto REF_STR2 = R"(t.TANK1 = t.TANK1 or {}
t.TANK1=
	{
	CMD=0,
	LASTRECNMR=0,
	LASTRECNAME='Не выбран',
	ST=
		{
		1, 
		},
	MODES=
		{
		1, 0, 
		},
	OPERATIONS=
		{
		1, 0, 
		},
	AVAILABILITY=
		{
		1, 1, 
		},
	ACTIVE_OPERATION_OR_IDLE_TIME=1,
	ACTIVE_STEP_TIME=1,
	MODES_TIME=
		{
		1, 0, 
		},
	MODES_STEPS=
		{
		1, 0, 
		},
	MODES_RUN_STEPS=
		{
		-1, -1, 
		},
	STEPS1=
		{
		1, 0, 0, 
		},
	RUN_STEPS1=
		{
		1, 0, 0, 
		},
	S_PAR_F=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		},
	S_PAR_UI=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		},
	RT_PAR_F=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		},
	RT_PAR_UI=
		{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		},
	}
)";
	EXPECT_STREQ( REF_STR2, buff );

	G_LUA_MANAGER->free_Lua();
    }

TEST( tech_object_manager, save_params_as_Lua_str )
    {
	lua_State* L = lua_open();
	ASSERT_EQ( 1, tolua_PAC_dev_open( L ) );
	G_LUA_MANAGER->set_Lua( L );


    tech_object tank1( "TANK", 1, 1, "TANK1", 2, 1, 10, 10, 10, 10 );
    tank1.get_modes_manager()->add_operation( "Test operation" );

    G_TECH_OBJECT_MNGR()->add_tech_object( &tank1 );

    const auto BUFF_SIZE = 1000;
    char buff[ BUFF_SIZE ];
    G_TECH_OBJECT_MNGR()->save_params_as_Lua_str( buff );
    auto REF_STR1 = R"(params{ object = 'TANK1', param_name = 'par_float', par_id = 1,
values=
	{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	} }
params{ object = 'TANK1', param_name = 'rt_par_float', par_id = 2,
values=
	{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	} }
params{ object = 'TANK1', param_name = 'par_uint', par_id = 3,
values=
	{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	} }
params{ object = 'TANK1', param_name = 'rt_par_uint', par_id = 4,
values=
	{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	} }
)";
	EXPECT_STREQ( REF_STR1, buff );

	G_LUA_MANAGER->free_Lua();
    }
