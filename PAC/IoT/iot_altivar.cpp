#include "iot_altivar.h"
#include "modbus_client.h"


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

void altivar_manager::add_node(const char* IP_address, unsigned int port, unsigned int timeout)
	{
	std::string nodeip = std::string(IP_address);
	nodeip.append(":");
	nodeip.append(std::to_string(port));
	nodeip.append(" ");
	nodeip.append(std::to_string(timeout));
	altivar_node* new_node = new altivar_node(SOCKID_ALTIVAR + index, IP_address, port, timeout);
	nodes.insert(altivar_node_pair(nodeip, new_node));
	num_nodes.insert(altivar_node_num_pair(index, new_node));
	index++;
	}

altivar_node * altivar_manager::get_node(const char * IP_address)
	{
	altivar_node_map::iterator it;
	it = nodes.find(IP_address);
	if (it != nodes.end())
		{
		return it->second;
		}
	else
		{
		return nullptr;
		}
	}

altivar_node* altivar_manager::get_node(unsigned int id)
	{
	altivar_node_num_map::iterator it;
	it = num_nodes.find(id);
	if (it != num_nodes.end())
		{
		return it->second;
		}
	else
		{
		return nullptr;
		}
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

unsigned int altivar_manager::index = 0;

altivar_manager * G_ALTIVAR_MANAGER()
	{
	return altivar_manager::get_instance();
	}


altivar_node::altivar_node(unsigned int id, const char* ip, unsigned int port, unsigned long exchangetimeout)
	{
	mc = new modbus_client(id, (char*)ip, port, exchangetimeout);
	strcpy(ip_address, ip);
	configure = true;
	querystep = RUN_STEP_CHECK_CONFIG;
	configurestep = 0;
	enabled = true;
	queryinterval = 200;
	querytimer = get_millisec();
	reverse = 0;
	modbustimeout = get_millisec();
	ismodbuserror = 0;
	cmd = 4; //Сброс ошибки при запуске.
	state = 0;
	fc_setpoint = 0;
	fc_value = 0;
	rpm_value = 0;
	rpm_setpoint = 0;
	}

altivar_node::~altivar_node()
	{
	delete mc;
	}

void altivar_node::Evaluate()
	{
#ifndef DEBUG_NO_IO_MODULES
	int commres;
	if (enabled)
		{
		switch (querystep)
			{
			case RUN_STEP_CHECK_CONFIG:
				if (configure)
					{
					configurestep = CFG_STEP_INIT_OUTPUTS;
					querystep = RUN_STEP_CONFIG;
					}
				else
					{
					querystep = RUN_STEP_INIT_IOSCANNER;
					}
				break;
			case RUN_STEP_CONFIG:
				switch (configurestep)
					{
					case CFG_STEP_INIT_OUTPUTS:
						mc->zero_output_buff();
						mc->set_station(0);
						mc->set_int2(0, 8501);	//CMD(Command word) --default
						mc->set_int2(1, 8502);	//LFR (Frequency setpoint)
						mc->set_int2(2, 0);
						mc->set_int2(3, 0);
						mc->set_int2(4, 0);
						mc->set_int2(5, 0);
						configurestep = CFG_STEP_SET_OUTPUTS;
						break;
					case CFG_STEP_SET_OUTPUTS:
						if (mc->async_write_multiply_registers(15421, 6)) //Задание ouputs для modbus-scanner
							{
							configurestep = CFG_STEP_INIT_INPUTS;
							}
						break;

					case CFG_STEP_INIT_INPUTS:
						mc->zero_output_buff();
						mc->set_station(0);
						mc->set_int2(0, 3201);	//ETA(Status word) --default
						mc->set_int2(1, 8604);	//RFRD (Output velocity) --default
						mc->set_int2(2, 3202);	//RFR (Output frequency)
						mc->set_int2(3, 0);
						mc->set_int2(4, 0);
						mc->set_int2(5, 0);
						configurestep = CFG_STEP_SET_INPUTS;
						break;
					case CFG_STEP_SET_INPUTS:
						if (mc->async_write_multiply_registers(15401, 6)) //Задание inputs для modbus-scanner
							{
							configurestep = CFG_STEP_INIT_IOSCANNER;
							}
						break;

					case CFG_STEP_INIT_IOSCANNER:
						mc->zero_output_buff();
						mc->set_station(0);
						mc->set_int2(0, 1);
						configurestep = CFG_STEP_SET_IOSCANNER;
						break;
					case CFG_STEP_SET_IOSCANNER:
						if (mc->async_write_multiply_registers(64239, 1)) //Включаем IO scanner
							{
							configurestep = CFG_STEP_INIT_IOPROFILE;
							}
						break;

					case CFG_STEP_INIT_IOPROFILE:
						mc->zero_output_buff();
						mc->set_station(0);
						mc->set_int2(0, 3);
						configurestep = CFG_STEP_SET_IOPROFILE;
						break;
					case CFG_STEP_SET_IOPROFILE:
						if (mc->async_write_multiply_registers(8401, 1)) //Задаем метод управления через IO-profile
							{
							configurestep = CFG_STEP_INIT_REF1;
							}
						break;

					case CFG_STEP_INIT_REF1:
						mc->zero_output_buff();
						mc->set_station(0);
						mc->set_int2(0, 169);
						configurestep = CFG_STEP_SET_REF1;
						break;
					case CFG_STEP_SET_REF1:
						if (mc->async_write_multiply_registers(8413, 1)) //Задаем канал управления частотой через modbus
							{
							configurestep = CFG_STEP_INIT_CMD1;
							}
						break;

					case CFG_STEP_INIT_CMD1:
						mc->zero_output_buff();
						mc->set_station(0);
						mc->set_int2(0, 30);
						configurestep = CFG_STEP_SET_CMD1;
						break;
					case CFG_STEP_SET_CMD1:
						if (mc->async_write_multiply_registers(8423, 1)) //Задаем канал команд через modbus
							{
							configurestep = CFG_STEP_INIT_FAULTRESET;
							}
						break;

					case CFG_STEP_INIT_FAULTRESET:
						mc->zero_output_buff();
						mc->set_station(0);
						mc->set_int2(0, 1);		//Включаем automatic restart
						mc->set_int2(1, 0);		//automatic restart timeout
						mc->set_int2(2, 210);	//Биндим сброс ошибки на 2 бит управляющего слова CMD.
						configurestep = CFG_STEP_SET_FAULTRESET;
						break;
					case CFG_STEP_SET_FAULTRESET:
						if (mc->async_write_multiply_registers(7122, 3)) //Задаем параметры fault reset.
							{
							configurestep = CFG_STEP_INIT_SAVESETTINGS;
							}
						break;

					case CFG_STEP_INIT_SAVESETTINGS:
						mc->zero_output_buff();
						mc->set_station(0);
						mc->set_int2(0, 1);
						configurestep = CFG_STEP_SET_SAVESETTINGS;
						break;
					case CFG_STEP_SET_SAVESETTINGS:
						if (mc->async_write_multiply_registers(8001, 1)) //Сохраняем настройки как Profile1
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
			case RUN_STEP_INIT_IOSCANNER:
				mc->zero_output_buff(15);
				mc->set_station(255);
				mc->set_int2(2, cmd);
				mc->set_int2(3, (int)(fc_setpoint * 10));
				mc->set_int2(4, 0);
				mc->set_int2(5, 0);
				mc->set_int2(6, 0);
				mc->set_int2(7, 0);
				querystep = RUN_STEP_QUERY_IOSCANNER;
				break;
			case RUN_STEP_QUERY_IOSCANNER:
				commres = mc->async_read_write_multiply_registers(1, 6, 1, 6);
				if (commres == 1)
					{
					remote_state = mc->get_int2(0);
					rpm_value = mc->get_int2(1);
					fc_value = mc->get_int2(2);
					int newstate = remote_state & 0x006F;
					switch (newstate)
						{
						case 0x0040:
							state = -3;
							break;
						case 0x0021:
							state = -2;
							break;
						case 0x0023:
							state = 0;
							break;
						case 0x0027:
							reverse ? state = 2 : state = 1;
							break;
						case 0x0007:
							state = -1;
							break;
						default:
							state = -1;
							break;
						}
					querystep = RUN_STEP_INIT_END;
					}
				if (commres == -1) //IO scanner not configured. Launch configuration.
					{
					configure = 1;
					querystep = RUN_STEP_CHECK_CONFIG;
					}
				break;
			case RUN_STEP_INIT_END:
				querytimer = get_millisec();
				querystep = RUN_STEP_END;
				break;
			case RUN_STEP_END:
				if (get_delta_millisec(querytimer) > queryinterval)
					{
					querystep = RUN_STEP_CHECK_CONFIG;
					}
				break;
			default:
				querystep = RUN_STEP_CHECK_CONFIG;
				break;
			}


		int ar = mc->get_connected_state();
		if (ar != tcp_client::ACS_CONNECTED && get_delta_millisec(modbustimeout) > 5000L && !ismodbuserror)
			{
			PAC_critical_errors_manager::get_instance()->set_global_error(
				PAC_critical_errors_manager::AC_NO_CONNECTION,
				PAC_critical_errors_manager::AS_FC_ALTIVAR,
				mc->get_id());
			ismodbuserror = 1;
			}
		if (ar == tcp_client::ACS_CONNECTED && ismodbuserror)
			{
			PAC_critical_errors_manager::get_instance()->reset_global_error(
				PAC_critical_errors_manager::AC_NO_CONNECTION,
				PAC_critical_errors_manager::AS_FC_ALTIVAR,
				mc->get_id());
			ismodbuserror = 0;
			}
		if (ar == tcp_client::ACS_CONNECTED)
			{
			modbustimeout = get_millisec();
			}
		}

#endif

	}

