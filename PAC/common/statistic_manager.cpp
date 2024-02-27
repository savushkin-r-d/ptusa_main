#include "statistic_manager.h"

auto_smart_ptr < statistic_manager > statistic_manager::instance;
//-----------------------------------------------------------------------------
device_with_statistic::device_with_statistic( device * dev,
	int device_resource ) : par( saved_params_float( 2 ) ), working_time( 0 ),
	device_resource( device_resource ), dev( dev )
	{
	prev_device_state = dev->get_state();
	state_change_count = par[ device_with_statistic::STATE_CHANGE_INDEX ];
	working_time = par[ device_with_statistic::WORKING_TIME_INDEX ];
	}
//-----------------------------------------------------------------------------
int device_with_statistic::get_cur_device_stat()
	{
	return state_change_count;
	}
//-----------------------------------------------------------------------------
int device_with_statistic::get_device_working_time_sec()
	{
	return working_time;
	}
//-----------------------------------------------------------------------------
int device_with_statistic::get_device_working_time_h()
	{
	return get_device_working_time_sec() / 3600;
	}
//-----------------------------------------------------------------------------
float device_with_statistic::get_cur_device_wear()
	{
	return ( state_change_count / (float)device_resource ) * 100;
	}
//-----------------------------------------------------------------------------
void device_with_statistic::check_state_changes()
	{
	if( prev_device_state != dev->get_state() )
		{
		if( dev->get_state() < 1 ) // Устройство выключено или в ошибке
			{
			prev_device_state = dev->get_state();

			working_time += get_sec() - start_time;
			par.save( device_with_statistic::WORKING_TIME_INDEX, working_time );
			}
		else // Нормальная работа
			{
			if( prev_device_state == 0 ) //Засечь время включения
				{
				start_time = get_sec();
				}
			prev_device_state = dev->get_state();

			state_change_count++;
			par.save( device_with_statistic::STATE_CHANGE_INDEX,
				state_change_count );
			}
		}
	}

//-----------------------------------------------------------------------------
int device_with_statistic::save_common_stat( char *buff)
	{
	const char *nm = dev->get_name();
	return fmt::format_to_n( buff, MAX_COPY_SIZE,
		"t.{}_STAT={{}}\n" \
		"t.{}_STAT.SC={:d}\n" \
		"t.{}_STAT.RS={:d}\n" \
		"t.{}_STAT.WR={:.2f}\n" \
		"t.{}_STAT.WT={:d}\n",
		nm, nm, state_change_count, nm, device_resource,
		nm, get_cur_device_wear(), nm, get_device_working_time_h() ).size;
	}
//-----------------------------------------------------------------------------
const char *device_with_statistic::get_name()
	{
	return dev->get_name();
	}
//-----------------------------------------------------------------------------
int device_with_statistic::set_cmd( const char *prop, u_int idx, double val )
	{
	std::string cmd = std::string( prop );
	if( cmd == "SC" )
		{
		state_change_count = (int)val;
		par.save( device_with_statistic::STATE_CHANGE_INDEX,
			state_change_count );
		}
	else if( cmd == "RS" )
		{
		device_resource = (int)val;
		}
	else if( cmd == "WT" )
		{
		working_time = (int)val * 3600;
		par.save( device_with_statistic::WORKING_TIME_INDEX, working_time );
		}
	else
		{
		return 1;
		}

	return 0;
	}
//-----------------------------------------------------------------------------
int device_with_statistic::set_cmd( const char *prop, u_int idx, char *val )
	{
	return 0;
	}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
statistic_manager::statistic_manager()
	{
	G_DEVICE_CMMCTR->add_device( this );
	}
//-----------------------------------------------------------------------------
statistic_manager::~statistic_manager()
	{
	for( auto dev : devs_with_stat )
		{
		delete dev;
		dev = nullptr;
		}
	}
//-----------------------------------------------------------------------------
device_with_statistic* statistic_manager::add_new_dev_with_stat( device *dev,
	int device_resource )
	{
	auto new_dev = new device_with_statistic( dev, device_resource );
	devs_with_stat.push_back( new_dev );
	return new_dev;
	}
//-----------------------------------------------------------------------------
void statistic_manager::evaluate()
	{
	for( auto dev : devs_with_stat )
		{
		dev->check_state_changes();
		}
	}
//-----------------------------------------------------------------------------
int statistic_manager::save_device( char *buff )
	{
	int res = 0;
	for( auto dev : devs_with_stat )
		{
		res += dev->save_common_stat( buff + res );
		}
	return res;
	}
//-----------------------------------------------------------------------------
const char *statistic_manager::get_name_in_Lua() const
	{
	return "statistic_manager";
	}
//-----------------------------------------------------------------------------
statistic_manager* statistic_manager::get_instance()
	{
	if( instance.is_null() )
		{
		instance = new statistic_manager();
		}

	return instance;
	}
//-----------------------------------------------------------------------------
statistic_manager* G_STATISTIC_MANAGER()
	{
	return statistic_manager::get_instance();
	}