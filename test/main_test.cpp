#include "includes.h"
#include "lua_manager_tests.h"

int G_DEBUG = 0;    //Вывод дополнительной отладочной информации.
int G_USE_LOG = 0;  //Вывод в системный лог (syslog).

int main(int argc, char* argv[])
{
    // Eternal hook for testing of LUA_MANAGER
    subhook_install(subhook_new((void *) lua_close, (void *) mock_lua_close, SUBHOOK_64BIT_OFFSET));
    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
