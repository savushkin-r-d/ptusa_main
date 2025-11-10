$#include "iot_altivar.h"
$#include "network_settings.h"

$#include "device/device.h"
$#include "tech_def.h"

$#ifdef WIN_OS
$#pragma warning(disable: 4800) //Warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
$#endif // WIN_OS

class altivar_manager
{
public:
	void add_node(const char* IP_address, unsigned int port, unsigned int timeout, const char* article);
};

altivar_manager* G_ALTIVAR_MANAGER();

class network_settings_manager
{
public:
	void add_port_forward_rule(unsigned int command_id, const char* device_name,
	                          unsigned int internal_port, unsigned int external_port,
	                          const char* device_ip);
	bool process_command(unsigned int command_id, bool open = true);
	void close_all_ports();
};

network_settings_manager* G_NETWORK_SETTINGS_MANAGER();