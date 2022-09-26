#include "mode_mngr_tests.h"

using namespace ::testing;

/*
	TEST METHOD DEFENITION:
	void evaluate()
*/

TEST( open_seat_action, evaluate )
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

	saved_params_u_int_4& par = PAC_info::get_instance()->par;
	const int FLIP_INTERVAL_MS = 5;
	const int FLIP_DURATION_MS = 2;
	par[ PAC_info::P_MIX_FLIP_UPPER_TIME ] = FLIP_DURATION_MS;
	par[ PAC_info::P_MIX_FLIP_LOWER_TIME ] = FLIP_DURATION_MS;

	tech_object test_tank( "Танк1", 1, 1, "T", 10, 10, 10, 10, 10, 10 );
	DO1 test_DO1( "test_DO1", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO_VIRT );
	DO1 test_DO2( "test_DO2", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO_VIRT );

	test_tank.get_modes_manager()->add_operation( "Тестовая операция" );
	auto operation_mngr = test_tank.get_modes_manager();
	auto operation = operation_mngr[ 0 ][ 1 ];
	operation->add_step( "Тестовый шаг", -1, -1 );
	auto operation_state = operation[ 0 ][ 1 ];
	auto step = operation_state[ 0 ][ 1 ];

	auto action = step[ 0 ][ step::ACTIONS::A_UPPER_SEATS_ON ];
	action->add_dev( &test_DO1, 0, valve::V_UPPER_SEAT );
	action->add_dev( &test_DO2, 0, valve::V_LOWER_SEAT );

	action->init();
	reinterpret_cast<open_seat_action*>( action )->set_wait_time( FLIP_INTERVAL_MS );

	action->evaluate();  // Wait
	EXPECT_EQ( false, test_DO1.is_active() );
	EXPECT_EQ( false, test_DO2.is_active() );

	sleep_ms( FLIP_INTERVAL_MS + 1 );
	action->evaluate();
	EXPECT_EQ( false, test_DO1.is_active() );
	EXPECT_EQ( false, test_DO2.is_active() );

	action->evaluate(); // Upper seats
	EXPECT_EQ( true, test_DO1.is_active() );
	EXPECT_EQ( false, test_DO2.is_active() );
	sleep_ms( FLIP_DURATION_MS + 1 );
	action->evaluate();
	EXPECT_EQ( false, test_DO1.is_active() );
	EXPECT_EQ( false, test_DO2.is_active() );

	action->evaluate();  // Wait
	EXPECT_EQ( false, test_DO1.is_active() );
	EXPECT_EQ( false, test_DO2.is_active() );
	sleep_ms( FLIP_INTERVAL_MS + 1 );
	action->evaluate();
	EXPECT_EQ( false, test_DO1.is_active() );
	EXPECT_EQ( false, test_DO2.is_active() );

	action->evaluate(); // Lower seats
	EXPECT_EQ( false, test_DO1.is_active() );
	EXPECT_EQ( true, test_DO2.is_active() );
	sleep_ms( FLIP_DURATION_MS + 1 );
	action->evaluate();
	EXPECT_EQ( false, test_DO1.is_active() );
	EXPECT_EQ( false, test_DO2.is_active() );

	action->finalize();

	test_params_manager::removeObject();
	}

/*
	TEST METHOD DEFENITION:
	void evaluate()
*/

TEST( operation, evaluate )
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

	tech_object test_tank( "Танк1", 1, 1, "T", 10, 10, 10, 10, 10, 10 );
	auto test_op = test_tank.get_modes_manager()->add_operation( "Test operation" );

	test_op->print( "" );

	test_op->add_step( "Init", 2, -1 );
	test_op->add_step( "Process #1", 3, -1 );
	test_op->add_step( "Process #2", 2, -1 );
	
	test_op->add_step( "Safe stop #1", 2, -1, operation::PAUSE );
	test_op->add_step( "Safe stop #2", 3, -1, operation::PAUSE );
	test_op->add_step( "Idle", -1, -1, operation::PAUSE );
	
	EXPECT_EQ( operation::IDLE, test_op->get_state() );
	test_op->evaluate();
	EXPECT_EQ( operation::IDLE, test_op->get_state() );
	test_op->pause();
	EXPECT_EQ( operation::IDLE, test_op->get_state() );
	test_op->stop();
	EXPECT_EQ( operation::IDLE, test_op->get_state() );
	test_op->final();
	EXPECT_EQ( operation::IDLE, test_op->get_state() );
		
	test_op->start();
	EXPECT_EQ( operation::RUN, test_op->get_state() );
	test_op->evaluate();
	test_op->pause();
	EXPECT_EQ( operation::PAUSE, test_op->get_state() );
	test_op->evaluate();
	test_op->start();
	EXPECT_EQ( operation::RUN, test_op->get_state() );
	test_op->evaluate();
	test_op->stop();
	EXPECT_EQ( operation::STOP, test_op->get_state() );
	test_op->evaluate();
	test_op->final();
	EXPECT_EQ( operation::IDLE, test_op->get_state() );

	test_op->start();
	EXPECT_EQ( operation::RUN, test_op->get_state() );
	test_op->evaluate();
	test_op->pause();
	EXPECT_EQ( operation::PAUSE, test_op->get_state() );
	test_op->evaluate();
	test_op->stop();
	EXPECT_EQ( operation::STOP, test_op->get_state() );
	test_op->evaluate();
	test_op->final();
	EXPECT_EQ( operation::IDLE, test_op->get_state() );


	//Корректный переход от выполнения к паузе и опять к выполнению.
	test_op->start();
	test_op->evaluate();
	test_op->to_next_step();
	test_op->evaluate();
	test_op->to_next_step();
	test_op->evaluate();
	EXPECT_EQ( 3, test_op->active_step() );
	EXPECT_EQ( 3, test_op->get_run_active_step() );

	test_op->pause();
	EXPECT_EQ( 1, test_op->active_step() );
	test_op->to_next_step();
	EXPECT_EQ( 2, test_op->active_step() );
	test_op->evaluate();
	EXPECT_EQ( 2, test_op->active_step() );

	test_op->start();
	EXPECT_EQ( 3, test_op->active_step() );
	test_op->evaluate();
	EXPECT_EQ( 3, test_op->active_step() );


	test_params_manager::removeObject();
	}

/*
	TEST METHOD DEFENITION:
	void check()
*/

TEST( AI_AO_action, check )
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

	tech_object test_tank( "Танк1", 1, 1, "T", 10, 10, 10, 10, 10, 10 );
	analog_output test_AO( "test_AO1" );
	analog_input test_AI( "test_AI1" );

	test_tank.get_modes_manager()->add_operation( "Тестовая операция" );
	auto operation_mngr = test_tank.get_modes_manager();
    auto operation = ( *operation_mngr )[ 1 ];
    auto operation_state = ( *operation )[ 1 ];
	auto step = ( *operation_state )[ 1 ];

	auto action = ( *step )[ step::ACTIONS::A_AI_AO ];	
	char msg[ 1024 ];

	EXPECT_EQ( 0, action->check( msg ) );

	action->add_dev( &test_AO );
	action->add_dev( &test_AI );
	EXPECT_NE( 0, action->check( msg ) );

	action->clear_dev();
	action->add_dev( &test_AI );
	action->add_dev( &test_AO );
	EXPECT_EQ( 0, action->check( msg ) );

	test_params_manager::removeObject();
	}

/*
	TEST METHOD DEFENITION:
	void check()
*/

TEST( DI_DO_action, check )
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

	tech_object test_tank( "Танк1", 1, 1, "T", 10, 10, 10, 10, 10, 10 );
	DO1 test_DO( "test_DO1", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO_VIRT );
	DI_signal test_DI( "test_DI1" );

	test_tank.get_modes_manager()->add_operation( "Тестовая операция" );
	auto operation_mngr = test_tank.get_modes_manager();
	auto operation = ( *operation_mngr )[ 1 ];
	auto operation_state = ( *operation )[ 1 ];
	auto step = ( *operation_state )[ 1 ];

	auto action = ( *step )[ step::ACTIONS::A_DI_DO ];
	char msg[ 1024 ];

	EXPECT_EQ( 0, action->check( msg ) );

	action->add_dev( &test_DO );
	action->add_dev( &test_DI );
	EXPECT_NE( 0, action->check( msg ) );

	action->clear_dev();
	action->add_dev( &test_DI );
	action->add_dev( &test_DO );
	EXPECT_EQ( 0, action->check( msg ) );

	test_params_manager::removeObject();
	}


TEST( checked_devices_action, finalize )
	{
	DO1 test_DO( "test_DO1", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO_VIRT );
	auto action = checked_devices_action();
	action.add_dev( &test_DO );
	
	test_DO.on();
	action.init();
	action.evaluate();
	EXPECT_EQ( 1, test_DO.get_state() );
	
	action.finalize();
	EXPECT_EQ( 1, test_DO.get_state() );
	}

TEST( delay_on_action, evaluate )
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

	tech_object test_tank( "Танк1", 1, 1, "T", 10, 10, 10, 10, 10, 10 );
	DO1 test_DO( "test_DO1", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO_VIRT );

	test_tank.get_modes_manager()->add_operation( "Тестовая операция" );
	auto operation_mngr = test_tank.get_modes_manager();
	auto operation = ( *operation_mngr )[ 1 ];
	auto operation_state = ( *operation )[ 1 ];
	auto step = ( *operation_state )[ -1 ];

	auto action = ( *step )[ step::ACTIONS::A_DELAY_ON ];

	action->add_dev( &test_DO );
	const int DELAY_TIME_MS = 10;
	const int DELAY_PARAM_IDX = 2;
	test_tank.par_float[ DELAY_PARAM_IDX ] = DELAY_TIME_MS;
	action->set_param_idx( 0, DELAY_PARAM_IDX );
	action->init();

	action->evaluate();
	EXPECT_EQ( 0, test_DO.get_state() );

	sleep_ms( DELAY_TIME_MS + 1 );
	action->evaluate();
	EXPECT_EQ( 1, test_DO.get_state() );

	action->finalize();
	EXPECT_EQ( 0, test_DO.get_state() );

	test_params_manager::removeObject();
	}

TEST( delay_off_action, evaluate )
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

	tech_object test_tank( "Танк1", 1, 1, "T", 10, 10, 10, 10, 10, 10 );
	DO1 test_DO( "test_DO1", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO_VIRT );

	test_tank.get_modes_manager()->add_operation( "Тестовая операция" );
	auto operation_mngr = test_tank.get_modes_manager();
	auto operation = ( *operation_mngr )[ 1 ];
	auto operation_state = ( *operation )[ 1 ];
	auto step = ( *operation_state )[ -1 ];

	auto action = ( *step )[ step::ACTIONS::A_DELAY_OFF ];

	action->add_dev( &test_DO );
	const int DELAY_TIME_MS = 10;
	const int DELAY_PARAM_IDX = 2;
	test_tank.par_float[ DELAY_PARAM_IDX ] = DELAY_TIME_MS;
	action->set_param_idx( 0, DELAY_PARAM_IDX );
	action->init();

	action->evaluate();
	EXPECT_EQ( 1, test_DO.get_state() );

	sleep_ms( DELAY_TIME_MS + 1 );
	action->evaluate();
	EXPECT_EQ( 0, test_DO.get_state() );

	action->finalize();
	EXPECT_EQ( 0, test_DO.get_state() );

	test_params_manager::removeObject();
	}

TEST( required_DI_action, finalize )
	{
	DI1 test_DI( "test_DI1", device::DEVICE_TYPE::DT_DI,
		device::DEVICE_SUB_TYPE::DST_DI_VIRT, 0 );
	auto action = checked_devices_action();
	action.add_dev( &test_DI );

	test_DI.on();
	action.init();
	action.evaluate();
	EXPECT_EQ( 1, test_DI.get_state() );

	action.finalize();
	EXPECT_EQ( 1, test_DI.get_state() );
	}

TEST( DI_DO_action, finalize )
	{
	DO1 test_DO( "test_DO1", device::DEVICE_TYPE::DT_DO,
		device::DEVICE_SUB_TYPE::DST_DO_VIRT );
	DI1 test_DI( "test_DI1", device::DEVICE_TYPE::DT_DI,
		device::DEVICE_SUB_TYPE::DST_DI_VIRT, 0 );
	auto action = DI_DO_action();
	action.add_dev( &test_DI, action::MAIN_GROUP, 1 );
	action.add_dev( &test_DO, action::MAIN_GROUP, 1 );

	test_DI.on();
	action.init();
	action.evaluate();
	EXPECT_EQ( 1, test_DI.get_state() );
	EXPECT_EQ( 1, test_DO.get_state() );

	action.finalize();
	EXPECT_EQ( 1, test_DI.get_state() );
	EXPECT_EQ( 0, test_DO.get_state() );
	}

TEST( AI_AO_action, finalize )
	{
	DO1 test_AO( "test_AO1", device::DEVICE_TYPE::DT_AO,
		device::DEVICE_SUB_TYPE::DST_AO_VIRT );
	DI1 test_AI( "test_AI1", device::DEVICE_TYPE::DT_AI,
		device::DEVICE_SUB_TYPE::DST_AI_VIRT, 0 );
	auto action = AI_AO_action();
	action.add_dev( &test_AI, action::MAIN_GROUP, 1 );
	action.add_dev( &test_AO, action::MAIN_GROUP, 1 );

	const int VALUE = 50;
	test_AI.set_value( VALUE );
	action.init();
	action.evaluate();
	EXPECT_EQ( VALUE, test_AI.get_value() );
	EXPECT_EQ( VALUE, test_AO.get_value() );

	action.finalize();
	EXPECT_EQ( VALUE, test_AI.get_value() );
	EXPECT_EQ( 0, test_AO.get_value() );
	}

TEST( wash_action, finalize )
	{
	DO1 test_DO( "test_DO1", device::DEVICE_TYPE::DT_DO,
		device::DEVICE_SUB_TYPE::DST_DO_VIRT );
	DI1 test_DI( "test_DI1", device::DEVICE_TYPE::DT_DI,
		device::DEVICE_SUB_TYPE::DST_DI_VIRT, 0 );

	virtual_motor test_M1( "M1" );
	virtual_motor test_M2( "M2" );

	auto action = wash_action();
	action.add_dev( &test_DI, action::MAIN_GROUP, 0 );
	action.add_dev( &test_DO, action::MAIN_GROUP, 1 );
	action.add_dev( &test_M1, action::MAIN_GROUP, 2 );
	action.add_dev( &test_M2, action::MAIN_GROUP, 3 );

	test_DI.on();
	action.init();
	action.evaluate();
	EXPECT_EQ( 1, test_DI.get_state() );
	EXPECT_EQ( 1, test_DO.get_state() );
	EXPECT_EQ( 1, test_M1.get_state() );
	EXPECT_EQ( 2, test_M2.get_state() );

	action.finalize();
	EXPECT_EQ( 1, test_DI.get_state() );
	EXPECT_EQ( 0, test_DO.get_state() );
	EXPECT_EQ( 0, test_M1.get_state() );
	EXPECT_EQ( 0, test_M2.get_state() );
	}


/*
	TEST METHOD DEFENITION: 
    should_turn_off()
*/

TEST( enable_step_by_signal, should_turn_off )
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

	tech_object test_tank( "Танк1", 1, 1, "T", 10, 10, 10, 10, 10, 10 );

	test_tank.get_modes_manager()->add_operation( "Тестовая операция" );
	auto operation_mngr = test_tank.get_modes_manager();
	auto operation = ( *operation_mngr )[ 1 ];
	auto operation_state = ( *operation )[ 1 ];
	auto step = ( *operation_state )[ -1 ];

	auto action = reinterpret_cast<enable_step_by_signal*>
		( ( *step )[ step::ACTIONS::A_ENABLE_STEP_BY_SIGNAL ] );

	EXPECT_EQ( true, action->should_turn_off() );	//Empty device list.

	DI1 test_DI( "test_DI1", device::DEVICE_TYPE::DT_DI,
		device::DEVICE_SUB_TYPE::DST_DI_VIRT, 0 );
	action->add_dev( &test_DI );
	EXPECT_EQ( true, action->should_turn_off() );	//Default flag value.

	action->set_bool_property( "should_turn_off", false );
	EXPECT_EQ( false, action->should_turn_off() );	//Flag was set to false.

	action->set_bool_property( "should_turn_off", true );
	EXPECT_EQ( true, action->should_turn_off() );	//Flag was set to true.

	test_params_manager::removeObject();
    }

TEST( to_step_if_devices_in_specific_state_action, is_goto_next_step )
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

	tech_object test_tank( "Танк1", 1, 1, "T", 10, 10, 10, 10, 10, 10 );

	test_tank.get_modes_manager()->add_operation( "Тестовая операция" );
	auto operation_mngr = test_tank.get_modes_manager();
	auto operation = ( *operation_mngr )[ 1 ];
	operation->add_step( "Тестовый шаг 1", -1, -1 );
	operation->add_step( "Тестовый шаг 2", -1, -1 );
	auto operation_state = operation[ 0 ][ 1 ];
	auto step = operation_state[ 0 ][ 1 ];

	operation->start();
	operation->evaluate();

	auto action = reinterpret_cast<to_step_if_devices_in_specific_state_action*>
		( ( *step )[ step::ACTIONS::A_TO_STEP_IF ] );

	int next_step = 0;
	auto is_goto_next_step = action->is_goto_next_step( next_step );
	EXPECT_EQ( false, is_goto_next_step );			//Empty action.
	EXPECT_EQ( -1, next_step );

	DI1 test_DI_one( "test_DI1", device::DEVICE_TYPE::DT_DI,
		device::DEVICE_SUB_TYPE::DST_DI_VIRT, 0 );
	action->add_dev( &test_DI_one, 0, 0 );
	DI1 test_DI_two( "test_DI2", device::DEVICE_TYPE::DT_DI,
		device::DEVICE_SUB_TYPE::DST_DI_VIRT, 0 );
	action->add_dev( &test_DI_two, 0, 1 );
	DI1 test_DI_three( "test_DI3", device::DEVICE_TYPE::DT_DI,
		device::DEVICE_SUB_TYPE::DST_DI_VIRT, 0 );
	action->add_dev( &test_DI_three, 1, 0 );
	const int SET_NEXT_STEP = 2;
	EXPECT_EQ( 1, action->set_int_property( "no_exist", 0, SET_NEXT_STEP ) );
	EXPECT_EQ( 0, action->set_int_property( "next_step_n", 0, SET_NEXT_STEP ) );	

	EXPECT_EQ( -1, action->get_int_property( "no_exist", 0 ) );
	next_step = action->get_int_property( "next_step_n", 0 );
	EXPECT_EQ( SET_NEXT_STEP, next_step );

	//По умолчанию все сигналы неактивны, к новому шагу не должно быть
	//перехода.
	is_goto_next_step = action->is_goto_next_step( next_step );
	EXPECT_EQ( false, is_goto_next_step );
	EXPECT_EQ( SET_NEXT_STEP, next_step );

	//Устанавливаем сигналы, к новому шагу не должно быть перехода.
	test_DI_one.on();
	test_DI_two.on();
	test_DI_three.off();
	is_goto_next_step = action->is_goto_next_step( next_step );
	EXPECT_EQ( false, is_goto_next_step );
	EXPECT_EQ( SET_NEXT_STEP, next_step );

	//Устанавливаем сигналы, к новому шагу должен быть переход.
	test_DI_one.on();
	test_DI_two.off();
	test_DI_three.off();
	is_goto_next_step = action->is_goto_next_step( next_step );
	EXPECT_EQ( true, is_goto_next_step );
	EXPECT_EQ( SET_NEXT_STEP, next_step );

	//Устанавливаем сигналы, к новому шагу должен быть переход.
	test_DI_one.off();
	test_DI_two.off();
	test_DI_three.on();
	is_goto_next_step = action->is_goto_next_step( next_step );
	EXPECT_EQ( true, is_goto_next_step );
	EXPECT_EQ( SET_NEXT_STEP, next_step );

	//Выполняем операцию, должен осуществиться переход к новому шагу.
	operation->evaluate();
	EXPECT_EQ( SET_NEXT_STEP, operation->get_run_active_step() );	

	test_params_manager::removeObject();
	}
