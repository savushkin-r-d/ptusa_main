#include "statistic_manager.h"

auto_smart_ptr < statistic_manager > statistic_manager::instance;
//-----------------------------------------------------------------------------
device_with_statistic::device_with_statistic( device * dev,
	int device_resource ) : par( saved_params_float( 2 ) ), working_time( 0 ),
	device_resource( device_resource ), dev( dev )
	{
	prev_device_state = dev->get_state();
	cur_stat = par[ device_with_statistic::CUR_STAT_INDEX ];
	working_time = par[ device_with_statistic::WORKING_TIME_INDEX ];
	}
//-----------------------------------------------------------------------------
int device_with_statistic::get_cur_device_stat()
	{
	return cur_stat;
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
	return ( cur_stat / (float)device_resource ) * 100;
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

			cur_stat++;
			par.save( device_with_statistic::CUR_STAT_INDEX, cur_stat );
			}
		}
	}

//-----------------------------------------------------------------------------
int device_with_statistic::save_common_stat( char *buff )
	{
	return fmt::format_to_n( buff, MAX_COPY_SIZE,
		"{}={{STAT_CH={:d}, STAT_RS={:d}, STAT_WR={:.2f}, STAT_WT={:d}}},",
		dev->get_name(), cur_stat, device_resource, get_cur_device_wear(),
		get_device_working_time_h() ).size;
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
void statistic_manager::add_new_dev_with_stat( device *dev,
	int device_resource )
	{
	auto new_dev = new device_with_statistic( dev, device_resource );
	devs_with_stat.push_back( new_dev );
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
	int res = snprintf( buff, MAX_COPY_SIZE,"t.%s = t.%s or {}\nt.%s=\n\t{\n",
		get_name_in_Lua(), get_name_in_Lua(), get_name_in_Lua() );

	for( auto dev : devs_with_stat )
		{
		res += dev->save_common_stat( buff + res );
		}

	res += snprintf( buff + res, MAX_COPY_SIZE, "\t}\n" );
	return res;
	}
//-----------------------------------------------------------------------------
const char *statistic_manager::get_name_in_Lua() const
	{
	return "Statistic_manager";
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