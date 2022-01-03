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

	par_mock->init( 0 );
	par_mock->final_init( 0, 0, 0 );

	tech_object test_tank( "Танк1", 1, 1, "T", 10, 10, 10, 10, 10, 10 );
	DO1 test_DO( "test_DO1", device::DEVICE_TYPE::DT_DO, device::DEVICE_SUB_TYPE::DST_DO_VIRT );
	valve_mix_proof test_v_mix_proof( "test_V1" );

	test_tank.get_modes_manager()->add_operation( "Тестовая операция" );
	auto operation_mngr = test_tank.get_modes_manager();
	auto operation = operation_mngr[ 0 ][ 1 ];
	auto operation_state = operation[ 0 ][ 1 ];
	auto step = operation_state[ 0 ][ 1 ];

	auto action_upper = step[ 0 ][ step::ACTIONS::A_UPPER_SEATS_ON ];
	action_upper->add_dev( &test_DO, 0, valve::V_UPPER_SEAT );
	action_upper->add_dev( &test_v_mix_proof, 0, valve::V_UPPER_SEAT );
	action_upper->init();
	auto action_lower = step[ 0 ][ step::ACTIONS::A_LOWER_SEATS_ON ];
	action_lower->add_dev( &test_DO, 0, valve::V_LOWER_SEAT );
	action_lower->add_dev( &test_v_mix_proof, 0, valve::V_LOWER_SEAT );
	action_lower->init();

	action_upper->evaluate();
	action_upper->evaluate();
	action_lower->evaluate();
	action_lower->evaluate();
	action_upper->final();
	action_lower->final();


	test_project_manager::removeObject();
	test_params_manager::removeObject();
	delete prj_mock;
	}