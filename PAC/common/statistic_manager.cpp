#include "statistic_manager.h"

auto_smart_ptr < statistic_manager > statistic_manager::instance;

device_with_statistic::device_with_statistic() :
	par( new saved_params_float( 3 ) ), cur_stat(0), working_time(0), device_resource(0), dev(nullptr)
{

}

device_with_statistic::device_with_statistic( device * dev, int device_resource )
	: par( new saved_params_float( 3 ) ), cur_stat( 0 ), working_time( 0 ), device_resource( device_resource ), dev( dev )
{
	prev_device_state = dev->get_state();
}

int device_with_statistic::get_cur_device_stat()
{
	return cur_stat;
}

int device_with_statistic::get_device_working_time()
{
	return working_time;
}

float device_with_statistic::get_cur_device_wear()
{
	return (float)(cur_stat / device_resource) * 100;
}

void device_with_statistic::check_state_changes()
{
	if( prev_device_state != dev->get_state() && prev_device_state == 0)
	{
		prev_device_state = dev->get_state();
		cur_stat++;
	}
	else
	{
		prev_device_state = dev->get_state();
	}
}

statistic_manager::statistic_manager()
{
}

statistic_manager::~statistic_manager()
{
	for( auto dev : devs_with_stat )
	{
		delete dev;
		dev = nullptr;
	}
}

void statistic_manager::add_dev( device *dev, int device_resource )
{
	auto new_dev = new device_with_statistic( dev, device_resource );
	devs_with_stat.push_back(new_dev);
}

void statistic_manager::evaluate()
{
	for( auto dev : devs_with_stat )
	{
		dev->check_state_changes();
	}
}

statistic_manager *statistic_manager::get_instance()
{
	if( instance.is_null() )
	{
		instance = new statistic_manager();
	}
	return instance;
}