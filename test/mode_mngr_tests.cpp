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
	const int FLIP_INTERVAL_MS = 1000;
	const int FLIP_DURATION_MS = 100;
	par[ PAC_info::P_MIX_FLIP_PERIOD ] = FLIP_INTERVAL_MS / MSEC_IN_SEC;
	par[ PAC_info::P_MIX_FLIP_UPPER_TIME ] = FLIP_DURATION_MS;
	par[ PAC_info::P_MIX_FLIP_LOWER_TIME ] = FLIP_DURATION_MS;

	tech_object test_tank( "Танк1", 1, 1, "T", 10, 10, 10, 10, 10, 10 );
	DO1 test_DO( "test_DO1", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO_VIRT );
	valve_mix_proof test_v_mix_proof( "test_V1" );

	test_tank.get_modes_manager()->add_operation( "Тестовая операция" );
	auto operation_mngr = test_tank.get_modes_manager();
	auto operation = operation_mngr[ 0 ][ 1 ];
	auto operation_state = operation[ 0 ][ 1 ];
	auto step = operation_state[ 0 ][ 1 ];

	auto action = step[ 0 ][ step::ACTIONS::A_UPPER_SEATS_ON ];
	action->add_dev( &test_DO, 0, valve::V_UPPER_SEAT );
	action->add_dev( &test_v_mix_proof, 0, valve::V_UPPER_SEAT );
	action->add_dev( &test_DO, 0, valve::V_LOWER_SEAT );
	action->add_dev( &test_v_mix_proof, 0, valve::V_LOWER_SEAT );

	action->init();

	action->evaluate();  // Wait
	EXPECT_EQ( false, test_DO.is_active() );
	EXPECT_EQ( true, test_v_mix_proof.is_closed() );

	sleep_ms( FLIP_INTERVAL_MS / 2 );
	action->evaluate();
	EXPECT_EQ( false, test_DO.is_active() );
	EXPECT_EQ( true, test_v_mix_proof.is_opened() );

	action->evaluate(); // Lower seats
	EXPECT_EQ( true, test_DO.is_active() );
	EXPECT_EQ( true, test_v_mix_proof.is_opened() );
	sleep_ms( FLIP_DURATION_MS );
	action->evaluate();
	EXPECT_EQ( false, test_DO.is_active() );
	EXPECT_EQ( true, test_v_mix_proof.is_closed() );

	action->evaluate();  // Wait
	EXPECT_EQ( false, test_DO.is_active() );
	sleep_ms( FLIP_INTERVAL_MS / 2 );
	action->evaluate();
	EXPECT_EQ( false, test_DO.is_active() );
	EXPECT_EQ( true, test_v_mix_proof.is_closed() );

	action->evaluate(); // Upper seats
	EXPECT_EQ( true, test_DO.is_active() );
	EXPECT_EQ( true, test_v_mix_proof.is_opened() );
	sleep_ms( FLIP_DURATION_MS );
	action->evaluate();
	EXPECT_EQ( false, test_DO.is_active() );
	EXPECT_EQ( true, test_v_mix_proof.is_closed() );

	action->final();

	test_params_manager::removeObject();
	}
