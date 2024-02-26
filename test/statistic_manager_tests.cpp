#include "statistic_manager_tests.h"

using namespace ::testing;

TEST( device_with_statistic, all_get_statistics_methods_test )
	{
	device* dev = new DO_signal( "test_dev" );
	auto dev_w_st = new device_with_statistic( dev, 10 );

	/// Начальное состояние
	EXPECT_EQ( dev_w_st->get_cur_device_stat(), 0 );
	EXPECT_NEAR( dev_w_st->get_cur_device_wear(), 0.0f, 0.01f );
	EXPECT_EQ( dev_w_st->get_device_working_time_sec(), 0 );

	///Включение
	dev->set_state( 1 );
	dev_w_st->check_state_changes();

	EXPECT_EQ( dev_w_st->get_cur_device_stat(), 1 );
	EXPECT_NEAR( dev_w_st->get_cur_device_wear(), 10.0f, 0.01f );
	EXPECT_EQ( dev_w_st->get_device_working_time_sec(), 0 );

	sleep_ms( 1100 );
	///Выключение 
	dev->set_state( 0 );
	dev_w_st->check_state_changes();

	EXPECT_EQ( dev_w_st->get_cur_device_stat(), 1 );
	EXPECT_NEAR( dev_w_st->get_cur_device_wear(), 10.0f, 0.01f );
	EXPECT_GT( dev_w_st->get_device_working_time_sec(), 0 );
	
	///Включение
	dev->set_state( 1 );
	dev_w_st->check_state_changes();

	EXPECT_EQ( dev_w_st->get_cur_device_stat(), 2 );
	EXPECT_NEAR( dev_w_st->get_cur_device_wear(), 20.0f, 0.01f );
	EXPECT_GT( dev_w_st->get_device_working_time_sec(), 0 );

	sleep_ms( 1100 );
	///Переключение состояния 
	dev->set_state( 2 );
	dev_w_st->check_state_changes();

	EXPECT_EQ( dev_w_st->get_cur_device_stat(), 3 );
	EXPECT_NEAR( dev_w_st->get_cur_device_wear(), 30.0f, 0.01f );
	EXPECT_GT( dev_w_st->get_device_working_time_sec(), 0 );
	dev->set_state( 0 );
	dev_w_st->check_state_changes();

	EXPECT_EQ( dev_w_st->get_cur_device_stat(), 3 );
	EXPECT_NEAR( dev_w_st->get_cur_device_wear(), 30.0f, 0.01f );
	EXPECT_GT( dev_w_st->get_device_working_time_sec(), 1 );
	EXPECT_EQ( dev_w_st->get_device_working_time_h(), 0 );

	delete dev;
	delete dev_w_st;
	}

TEST( device_with_statistic, save_common_stat )
	{
	device *dev = new DO_signal( "test_dev" );
	auto dev_w_st = new device_with_statistic( dev, 10 );

	char buf[ MAX_COPY_SIZE ];

	/// Начальное состояние
	int res = dev_w_st->save_common_stat( buf );
	buf[ res ] = '\0';
	
	EXPECT_EQ( res, 96 );
	std::string act_str = "t.test_dev.STAT_CH = 0\nt.test_dev.STAT_RS = 10\n" \
		"t.test_dev.STAT_WR = 0.00\nt.test_dev.STAT_WT = 0\n";
	EXPECT_STREQ( act_str.c_str(), buf );

	///Включение
	dev->set_state( 1 );
	dev_w_st->check_state_changes();

	res = dev_w_st->save_common_stat( buf );
	buf[ res ] = '\0';

	EXPECT_EQ( res, 97 );
	act_str = "t.test_dev.STAT_CH = 1\nt.test_dev.STAT_RS = 10\n" \
		"t.test_dev.STAT_WR = 10.00\nt.test_dev.STAT_WT = 0\n";
	EXPECT_STREQ( act_str.c_str(), buf );

	///Выключение
	dev->set_state( 0 );
	dev_w_st->check_state_changes();

	res = dev_w_st->save_common_stat( buf );
	buf[ res ] = '\0';

	EXPECT_EQ( res, 97 );
	act_str = "t.test_dev.STAT_CH = 1\nt.test_dev.STAT_RS = 10\n" \
		"t.test_dev.STAT_WR = 10.00\nt.test_dev.STAT_WT = 0\n";
	EXPECT_STREQ( act_str.c_str(), buf );

	delete dev;
	delete dev_w_st;
	}

TEST( statistic_manager, manager_common_test )
	{
	EXPECT_NE( G_STATISTIC_MANAGER(), nullptr );

	device *dev1 = new DO_signal( "test_DO_dev" );
	device *dev2 = new DI_signal( "test_DI_dev" );

	G_STATISTIC_MANAGER()->add_new_dev_with_stat( dev1, 10 );
	G_STATISTIC_MANAGER()->add_new_dev_with_stat( dev2, 10 );

	char buf[ MAX_COPY_SIZE ];

	int res = G_STATISTIC_MANAGER()->save_device( buf );
	buf[ res ] = '\0';

	EXPECT_EQ( res, 216 );

	std::string act_str =
		"t.test_DO_dev.STAT_CH = 0\nt.test_DO_dev.STAT_RS = 10\n" \
		"t.test_DO_dev.STAT_WR = 0.00\nt.test_DO_dev.STAT_WT = 0\n" \
		"t.test_DI_dev.STAT_CH = 0\nt.test_DI_dev.STAT_RS = 10\n" \
		"t.test_DI_dev.STAT_WR = 0.00\nt.test_DI_dev.STAT_WT = 0\n";
	EXPECT_STREQ( act_str.c_str(), buf );

	dev1->set_state( 1 );
	G_STATISTIC_MANAGER()->evaluate();

	res = G_STATISTIC_MANAGER()->save_device( buf );
	buf[ res ] = '\0';

	EXPECT_EQ( res, 217 );

	act_str =
		"t.test_DO_dev.STAT_CH = 1\nt.test_DO_dev.STAT_RS = 10\n" \
		"t.test_DO_dev.STAT_WR = 10.00\nt.test_DO_dev.STAT_WT = 0\n" \
		"t.test_DI_dev.STAT_CH = 0\nt.test_DI_dev.STAT_RS = 10\n" \
		"t.test_DI_dev.STAT_WR = 0.00\nt.test_DI_dev.STAT_WT = 0\n";
	EXPECT_STREQ( act_str.c_str(), buf );

	EXPECT_STREQ( G_STATISTIC_MANAGER()->get_name_in_Lua(), "statistic_manager" );
	}