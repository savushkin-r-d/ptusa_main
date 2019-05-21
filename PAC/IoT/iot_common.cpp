#include "iot_common.h"
#include "iot_altivar.h"

int i_iot_node::set_state(int value, int index)
{
	return 0;
}

int i_iot_node::get_state(int index)
{
	return 0;
}

int i_iot_node::set_value(float value, int index)
{
	return 0;
}

float i_iot_node::get_value(int index)
{
	return 0.0f;
}

unsigned long i_iot_node::set_counter(unsigned long value, int index)
{
	return 0;
}

int i_iot_node::get_counter(int index)
{
	return 0;
}

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

#ifdef LINUX_OS
    G_ALTIVAR_MANAGER()->evaluate();
#endif
}

void IOT_FINAL()
{
    ((auto_smart_ptr< altivar_manager >)G_ALTIVAR_MANAGER()).free();
}
