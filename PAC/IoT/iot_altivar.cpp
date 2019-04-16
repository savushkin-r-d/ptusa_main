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
}

altivar_node::~altivar_node()
{
	delete mc;
}

void altivar_node::Evaluate()
{
	if (mc->async_read_holding_registers(0, 1) == 1)
	{
		//printf("\n\rMB read");
	}
}
