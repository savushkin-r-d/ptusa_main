#include "iot_altivar.h"

altivar_manager::~altivar_manager()
{
}

altivar_manager* altivar_manager::get_instance()
{
	if (instance.is_null())
	{
		instance = new altivar_manager();
	}
	return instance;
}

altivar_manager::altivar_manager()
{
}

auto_smart_ptr< altivar_manager > altivar_manager::instance;

altivar_manager * G_ALTIVAR_MANAGER()
{
	return altivar_manager::get_instance();
}

