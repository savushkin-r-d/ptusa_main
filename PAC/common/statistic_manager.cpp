#include "statistic_manager.h"

auto_smart_ptr < statistic_manager > statistic_manager::instance;
//-----------------------------------------------------------------------------
device_with_statistic::device_with_statistic( device *dev,
	int device_resource ) : dev( dev ), prev_device_state( dev->get_state() ),
	device_resource( device_resource )
	{
	state_change_count =
		static_cast<int>( par[ device_with_statistic::STATE_CHANGE_INDEX ] );
	working_time = static_cast<unsigned long>
		( par[ device_with_statistic::WORKING_TIME_INDEX ] );
	}
//-----------------------------------------------------------------------------
int device_with_statistic::get_cur_device_stat() const
	{
	return state_change_count;
	}
//-----------------------------------------------------------------------------
int device_with_statistic::get_device_working_time_sec() const
	{
	return (int)working_time;
	}
//-----------------------------------------------------------------------------
int device_with_statistic::get_device_working_time_h() const
	{
	return get_device_working_time_sec() / 3600;
	}
//-----------------------------------------------------------------------------
float device_with_statistic::get_cur_device_wear() const
	{
	return ( (float)state_change_count / (float)device_resource ) * 100;
	}
//-----------------------------------------------------------------------------
void device_with_statistic::evaluate_collecting()
	{
	if( prev_device_state != dev->get_state() )
		{
		if( dev->get_state() < 1 ) // Устройство выключено или в ошибке
			{
			prev_device_state = dev->get_state();

			working_time += get_sec() - start_time;
			par.save( device_with_statistic::WORKING_TIME_INDEX,
				(float)working_time );
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
				(float)state_change_count );
			}
		}
	}
//-----------------------------------------------------------------------------
int device_with_statistic::save_common_stat( char *buff)
	{
	const char *nm = dev->get_name();
	return (int)fmt::format_to_n( buff, MAX_COPY_SIZE,
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
	auto cmd = std::string( prop );
	if( cmd == "SC" )
		{
		state_change_count = (int)val;
		par.save( device_with_statistic::STATE_CHANGE_INDEX,
			(float)state_change_count );
		}
	else if( cmd == "RS" )
		{
		device_resource = (int)val;
		}
	else if( cmd == "WT" )
		{
		working_time = (int)val * 3600;
		par.save( device_with_statistic::WORKING_TIME_INDEX,
			(float)working_time );
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
device_with_statistic* statistic_manager::add_new_dev_with_stat( device *dev,
	int device_resource )
	{
	objs_with_stat.emplace_back( std::unique_ptr< i_statistic_collecting > {
		new device_with_statistic( dev, device_resource ) } );
	return static_cast< device_with_statistic* >
		( objs_with_stat[ objs_with_stat.size() - 1 ].get() );
	}
//-----------------------------------------------------------------------------
void statistic_manager::evaluate() const
	{
	for( const auto &dev : objs_with_stat )
		{
		dev->evaluate_collecting();
		}
	}
//-----------------------------------------------------------------------------
int statistic_manager::save_device( char *buff )
	{
	int res = 0;
	for( const auto &dev : objs_with_stat )
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