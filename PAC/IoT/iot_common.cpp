#include "iot_common.h"
#include "iot_altivar.h"

i_iot_node::i_iot_node()
{
}

i_iot_node::~i_iot_node()
{
}

void IOT_INIT()
{
}

void IOT_EVALUATE()
{
#ifdef WIN_OS
	G_ALTIVAR_MANAGER()->evaluate();
#endif // WIN_OS
}

void IOT_FINAL()
{
	((auto_smart_ptr< altivar_manager >)G_ALTIVAR_MANAGER()).free();
}
