#include "iot_altivar.h"

altivar_manager::~altivar_manager()
{
	if (nodes.size() > 0)
	{
		altivar_node_map::iterator it;
		for (it = nodes.begin(); it != nodes.end(); it++)
		{
			delete it->second;
		}
	}
	
}

altivar_manager* altivar_manager::get_instance()
{
	if (instance.is_null())
	{
		instance = new altivar_manager();
	}
	return instance;
}

void altivar_manager::add_node(unsigned int index, char* IP_address, char* name, unsigned int port, unsigned int timeout)
{
	std::string nodeip = std::string(IP_address);
	nodeip.append(":");
	nodeip.append(std::to_string(port));
	nodeip.append(" ");
	nodeip.append(std::to_string(timeout));
	altivar_node* new_node = new altivar_node(SOCKID_ALTIVAR + index, IP_address, port, timeout);
	nodes.insert(altivar_node_pair(nodeip, new_node));
	num_nodes.insert(altivar_node_num_pair(index, new_node));
}

void altivar_manager::evaluate()
{
	if (nodes.size() > 0)
	{
		altivar_node_map::iterator it;
		for (it = nodes.begin(); it != nodes.end(); it++)
		{
			it->second->Evaluate();
		}
	}
}

altivar_manager::altivar_manager()
{
}

auto_smart_ptr< altivar_manager > altivar_manager::instance;

altivar_manager * G_ALTIVAR_MANAGER()
{
	return altivar_manager::get_instance();
}


altivar_node::altivar_node(unsigned int id, char* ip, unsigned int port, unsigned long exchangetimeout)
{
	mc = new modbus_client(id, ip, port, exchangetimeout);
	configure = true;
	querystep = RUN_STEP_CHECK_CONFIG;
	configurestep = 0;
	enabled = true;
	queryinterval = 200;
	querytimer = get_millisec();
}

altivar_node::~altivar_node()
{
	delete mc;
}

void altivar_node::Evaluate()
{
	if (enabled)
	{
		switch (querystep)
		{
		case RUN_STEP_CHECK_CONFIG:
			if (configure)
			{
				configurestep = 0;
				querystep = RUN_STEP_CONFIG;
			}
			else
			{
				querystep = RUN_STEP_INIT_END;
			}
			break;
		case RUN_STEP_CONFIG:
			switch (configurestep)
			{
			case 0:
				mc->zero_output_buff();
				mc->set_station(0);
				mc->set_int2(0, 8501);	//CMD(Command word) --default
				mc->set_int2(1, 8602);	//LFRT (Speed setpoint) --default
				mc->set_int2(2, 0);
				mc->set_int2(3, 0);
				mc->set_int2(4, 0);
				mc->set_int2(5, 0);
				configurestep = 1;
				break;
			case 1:
				if (mc->async_write_multiply_registers(15421, 6)) //Задание ouputs для modbus-scanner
				{
					configurestep = 2;
				}
				break;
			case 2:
				mc->zero_output_buff();
				mc->set_station(0);
				mc->set_int2(0, 3201);	//ETA(Status word) --default
				mc->set_int2(1, 8604);	//RFRD (Output velocity) --default
				mc->set_int2(2, 0);
				mc->set_int2(3, 0);
				mc->set_int2(4, 0);
				mc->set_int2(5, 0);
				configurestep = 3;
				break;
			case 3:
				if (mc->async_write_multiply_registers(15401, 6)) //Задание inputs для modbus-scanner
				{
					configurestep = CFG_STEP_END;
				}
				break;
			case CFG_STEP_END:
				configure = false;
				querystep = 2;
				break;
			default:
				configurestep = 0;
				break;
			}
			break;
		case 2:
			mc->set_station(255);
			querystep = RUN_STEP_INIT_END;
			break;
		case RUN_STEP_INIT_END:
			querytimer = get_millisec();
			querystep = RUN_STEP_END;
			break;
		case RUN_STEP_END:
			if (get_delta_millisec(querytimer) > queryinterval)
			{
				querystep = 0;
			}
			break;
		default:
			querystep = 0;
			break;
		}
	}
	
}
