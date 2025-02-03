#include "includes.h"
#include "lua_manager_tests.h"

int G_DEBUG = 1;    //Вывод дополнительной отладочной информации.
int G_USE_LOG = 0;  //Вывод в системный лог (syslog).

bool G_NO_IO_NODES = true; // По умолчанию обмен с модулями отключен.
bool G_READ_ONLY_IO_NODES = false;

const subhook_t GTESTS_DELTA_MILLISEC_SUBHOOK = subhook_new(
    reinterpret_cast<void*>( &get_delta_millisec ),
    reinterpret_cast<void*>( &subhook_for_get_delta_millisec ),
    SUBHOOK_64BIT_OFFSET );

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

    ::testing::InitGoogleTest(&argc, argv);
	auto res = RUN_ALL_TESTS();

    subhook_remove( GTESTS_DELTA_MILLISEC_SUBHOOK );
    subhook_free( GTESTS_DELTA_MILLISEC_SUBHOOK );

    return res;
}
