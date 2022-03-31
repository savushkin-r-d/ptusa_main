#pragma once
#include "includes.h"
#include "param_ex.h"

class mock_params_manager : public params_manager
{
    public:
	MOCK_METHOD(int, init, (unsigned int project_id));
    MOCK_METHOD(void, final_init, 
        (int auto_init_params, int auto_init_work_params, void(*custom_init_params_function)()));
    MOCK_METHOD(char*, get_params_data, (int size, int &start_pos));

	static mock_params_manager* get_instance()
		{
		if ( instance.is_null() )
			{
			instance = new mock_params_manager();
			}
		return instance;
		}

	private:
		static auto_smart_ptr< mock_params_manager > instance;
};

class test_params_manager 
{
    public:
	static void replaceEntity(mock_params_manager* p)
	{
		params_manager::is_init = 1;
		prev_pointer = params_manager::instance;
		params_manager::instance.replace_without_free( p );
	}

	static void removeObject()
	{
		params_manager::instance = prev_pointer;
	}

private:
	static params_manager* prev_pointer;
};