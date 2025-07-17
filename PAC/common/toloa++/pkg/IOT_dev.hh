$#include "iot_altivar.h"

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