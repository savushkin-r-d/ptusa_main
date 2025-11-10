#include "iot_common.h"
#include "iot_altivar.h"
#include "network_settings.h"





void IOT_INIT()
	{
	}

void IOT_EVALUATE()
	{
#ifdef WIN_OS
	G_ALTIVAR_MANAGER()->evaluate();
	G_NETWORK_SETTINGS_MANAGER()->evaluate();
#endif // WIN_OS

#ifdef LINUX_OS
	G_ALTIVAR_MANAGER()->evaluate();
	G_NETWORK_SETTINGS_MANAGER()->evaluate();
#endif
	}

void IOT_FINAL()
	{
	// Закрываем все открытые порты при завершении
	G_NETWORK_SETTINGS_MANAGER()->close_all_ports();
	}


