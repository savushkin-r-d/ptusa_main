#include "includes.h"
#include "lua_manager_tests.h"

int G_DEBUG = 1;    //Вывод дополнительной отладочной информации.
int G_USE_LOG = 0;  //Вывод в системный лог (syslog).

bool G_NO_IO_NODES = true; // По умолчанию обмен с модулями отключен.
bool G_READ_ONLY_IO_NODES = false;

unsigned long subhook_for_get_delta_millisec(unsigned long)
{
    return DeltaMilliSecSubHooker::get_millisec();
}

int main(int argc, char* argv[])
{
#if defined WIN_OS
    setlocale( LC_ALL, "ru_RU.UTF-8" );
    setlocale( LC_NUMERIC, "C" );
#endif

    // Eternal hook for testing of LUA_MANAGER
    subhook_install(subhook_new((void *) lua_close, (void *) mock_lua_close, SUBHOOK_64BIT_OFFSET));
    ::testing::InitGoogleTest(&argc, argv);
	auto res = RUN_ALL_TESTS();
    return res;
}
