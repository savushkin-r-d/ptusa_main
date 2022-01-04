#include "mode_mngr_tests.h"

using namespace ::testing;

/*
	TEST METHOD DEFENITION:
	void init()
*/

TEST( open_seat_action, init )
	{
	char* res = 0;
	mock_project_manager* prj_mock = new mock_project_manager();
	mock_params_manager* par_mock = new mock_params_manager();
	test_params_manager::replaceEntity( par_mock );

	EXPECT_CALL( *par_mock, init( _ ) );
	EXPECT_CALL( *par_mock, final_init( _, _, _ ) );

	par_mock->init( 0 );
	par_mock->final_init( 0, 0, 0 );

	saved_params_u_int_4& par = PAC_info::get_instance()->par;
	par[ PAC_info::P_MIX_FLIP_PERIOD ] = 1;
	par[ PAC_info::P_MIX_FLIP_UPPER_TIME ] = 100;
	par[ PAC_info::P_MIX_FLIP_LOWER_TIME ] = 100;

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
	sleep_ms( 500 );
	action->evaluate();

	action->evaluate(); // Lower seats
	sleep_ms( 100 );
	action->evaluate();

	action->evaluate();  // Wait
	sleep_ms( 500 );
	action->evaluate();

	action->evaluate(); // Upper seats
	sleep_ms( 100 );
	action->evaluate();

	action->final();

	test_project_manager::removeObject();
	test_params_manager::removeObject();
	delete prj_mock;
	}