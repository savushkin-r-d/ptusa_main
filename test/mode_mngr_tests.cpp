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

	action->final();

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


TEST( checked_devices_action, final )
	{
	DO1 test_DO( "test_DO1", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO_VIRT );
	auto action = checked_devices_action();
	action.add_dev( &test_DO );
	
	test_DO.on();
	action.init();
	action.evaluate();
	EXPECT_EQ( 1, test_DO.get_state() );
	
	action.final();
	EXPECT_EQ( 1, test_DO.get_state() );
	}

TEST( on_action, evaluate )
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

	auto action = ( *step )[ step::ACTIONS::A_ON ];

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

	action->final();
	EXPECT_EQ( 0, test_DO.get_state() );

	test_params_manager::removeObject();
	}
