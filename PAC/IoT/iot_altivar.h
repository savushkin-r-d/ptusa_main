#pragma once
#ifndef iot_altivar_h__
#define iot_altivar_h__

#include "smart_ptr.h"
#include "iot_common.h"
#include "modbus_client.h"
#include <map>
#include <string>

class altivar_node : public i_iot_node
{
public:
	altivar_node(unsigned int id, char* ip, unsigned int port, unsigned long exchangetimeout);
	~altivar_node();
	void Evaluate();
	bool enabled;
	unsigned long queryinterval;

	enum CFG_STEP
	{
		CFG_STEP_END = 99,
	};

	enum RUN_STEP
	{
		RUN_STEP_CHECK_CONFIG = 0,
		RUN_STEP_CONFIG = 1,
		RUN_STEP_INIT_END = 98,
		RUN_STEP_END = 99,
	};

protected:
	modbus_client* mc;
	float fc_setpoint;
	float fc_value;
	bool configure;
	int querystep;
	int configurestep;
	unsigned long querytimer;
};

typedef std::map<std::string, altivar_node*> altivar_node_map;
typedef std::pair<std::string, altivar_node*> altivar_node_pair;

typedef std::map<unsigned int, altivar_node*> altivar_node_num_map;
typedef std::pair<unsigned int, altivar_node*> altivar_node_num_pair;

class altivar_manager
{
public:

	virtual ~altivar_manager();

	/// @brief Получение единственного экземпляра класса.
	static altivar_manager* get_instance();
	void add_node(unsigned int index, char* IP_address, char* name, unsigned int port, unsigned int timeout);
	void evaluate();
protected:
	altivar_manager();
	altivar_node_map nodes;
	altivar_node_num_map num_nodes;
	//altivar_node** nodes;
	//u_int nodes_count;

	static auto_smart_ptr< altivar_manager > instance;
};



altivar_manager* G_ALTIVAR_MANAGER();



#endif // iot_altivar_h__