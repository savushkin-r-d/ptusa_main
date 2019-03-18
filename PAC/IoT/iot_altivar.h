#pragma once
#include "smart_ptr.h"
#ifndef iot_altivar_h__
#define iot_altivar_h__

class altivar_manager
{
public:

	virtual ~altivar_manager();

	/// @brief Получение единственного экземпляра класса.
	static altivar_manager* get_instance();
protected:
	altivar_manager();

	static auto_smart_ptr< altivar_manager > instance;
};

altivar_manager* G_ALTIVAR_MANAGER();


#endif // iot_altivar_h__