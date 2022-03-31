#include "mock_params_manager.h"

auto_smart_ptr< mock_params_manager > mock_params_manager::instance;
params_manager* test_params_manager::prev_pointer = nullptr;
