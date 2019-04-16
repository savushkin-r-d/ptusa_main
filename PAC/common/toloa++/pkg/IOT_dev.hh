$#include "iot_altivar.h"

$#include "PAC_dev.h"
$#include "tech_def.h"

$#ifdef WIN_OS
$#pragma warning(disable: 4800) //Warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
$#endif // WIN_OS

class altivar_manager
{
public:
	void add_node(unsigned int index, char* IP_address, char* name, unsigned int port, unsigned int timeout);
};

altivar_manager* G_ALTIVAR_MANAGER();