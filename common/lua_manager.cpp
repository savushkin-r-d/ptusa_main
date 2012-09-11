#ifndef __BORLANDC__
#include <cstdlib>
#endif

#ifdef WIN_OS
#include <Windows.h>
#endif // OS_WIN

#if defined MINIOS7
#include <alloc.h>
#endif

#include "lua_manager.h"

#include "prj_mngr.h"
#include "PAC_dev.h"

//-----------------------------------------------------------------------------
auto_smart_ptr< lua_manager > lua_manager::instance;
//-----------------------------------------------------------------------------
lua_manager* lua_manager::get_instance()
    {
    static int is_init = 0;
    if ( 0 == is_init )
        {
        instance = new lua_manager();
        is_init = 1;
        }

    return instance;
    }
//-----------------------------------------------------------------------------
int check_file( const char* file_name, char* err_str )
    {
    strcpy( err_str, "" );

    FILE *f = fopen( file_name, "r");
    if ( 0 == f )
        {
        sprintf( err_str, "File \"%s\" not found!", file_name );
        return -1;
        }

    char str[ 100 ] = "";
    fgets( str, sizeof( str ), f );   

    int version = 0;
    sscanf( str, "--version = %d", &version );

    return version;
    }
//-----------------------------------------------------------------------------
const int SYS_FILE_CNT = 3;
const int FILE_CNT     = 6;
//-----------------------------------------------------------------------------
#ifdef PAC_PC
const char* SYS_PATH = "..\\..\\system scripts\\";
#endif // PAC_PC

const char *FILES[ FILE_CNT ] = 
    {    
    "sys.wago.lua",
    "sys.devices.lua",
    "sys.objects.lua",

    "main.wago.lua",
    "main.devices.lua",
    "main.objects.lua"
    };
//-----------------------------------------------------------------------------
const int FILES_VERSION[ FILE_CNT ] = 
    {    
    1, //"sys.wago.plua",
    1, //"sys.devices.lua",
    1, //"sys.tech_objects.plua",

    1, //"main.wago.plua",
    1, //"main.devices.plua",
    1, //"main.objects.plua",
    };
//-----------------------------------------------------------------------------
//I
//Загружаем последовательно Lua-скрипты. Их структура такова, что все необходимые
//действия реализованы в виде функций, которые потом в определенном порядке 
//будут выполнятся.
//Скрипты разделены на системные (общие
//для всех проектов) и проекта (описание отдельного проекта). Системные скрипты
//имеют префикс sys и хранятся в папке "system scripts" на PC. Список скриптов:
// 1. sys.wago.plua - функции получения описания Wago и устройств с привязкой к 
// каналам ввода\вывода. Они вызываются исполняющей программой (С++).
// 2. sys.devices.plua - функции получения описания технологических устройств
// (клапана, насосы и т.д.). Они вызываются исполняющей программой 
// (С++).
// 3. sys.objects.plua - функции получения описания технологических 
// объектов (режимы, параметры и т.д.). Они вызываются исполняющей программой 
// (С++).
//Пользовательские скрипты имеют префикс main и хранятся в папке конкретного
//проекта. Список скриптов:
// 1. main.wago.plua - описание Wago и устройств проекта с привязкой к каналам 
// ввода\вывода. 
// 2. main.devices.plua - буквенно-цифровое описание технологических устройств
// (клапана, насосы и т.д.) проекта для более удобного использования (S1V52  
// вместо V(1251)).
// 2. main.objects.plua - описание технологических объектов (режимы, 
// параметры и т.д.) проекта.
// 3. main.plua - пользовательская логика работы проекта.
//Для каждого файла проверяется его наличие и версия (первая строка файла должна
//быть такой: "--version = 1"). Если файла нет или он имеет не поддерживаемую
//версию, то выдается соответствующее сообщение и работа приложения завершается.
//II
//Экспортируем в Lua классы и функции из C++.

int lua_manager::init( lua_State* lua_state, char* script_name )
    {
#if defined DEBUG
    Print( "Init Lua.\n" );
#endif

    if ( 0 == lua_state )
        {
        //Инициализация Lua.
        L = lua_open();   // Create Lua context.

        if ( NULL == L )
            {
            Print( "Error creating Lua context.\n" );
            return 1;
            }
        is_free_lua = 1;

        luaL_openlibs( L );    // Open standard libraries.

        }
    else
        {
        L = lua_state;
        is_free_lua = 0;
        }
        
    //I
    //Проверка наличия и версии скриптов.
    char err_str[ 100 ] = "";

    for ( int i = 0; i < FILE_CNT; i++ )
        {
#ifdef PAC_PC
        char path[ 100 ];
        if ( i < SYS_FILE_CNT )
            {
            snprintf( path, sizeof( path ), "%s%s", SYS_PATH, FILES[ i ] );
            }
        else
            {
            snprintf( path, sizeof( path ), "%s", FILES[ i ] );
            }

        int res = check_file( path, err_str );
#else
        int res = check_file( FILES[ i ], err_str );
#endif // PAC_PC

        if ( res == -1 )
            {
            Print( err_str );
            return 1;
            }

        if ( FILES_VERSION[ i ] != res )
            {
            snprintf( err_str, sizeof( err_str ), "File \"%s\" has version %d, must be %d!\n",
                FILES[ i ], res, FILES_VERSION[ i ] );
            Print( err_str );
            return 1;
            }
        }

    //-Выполнение системных скриптов sys.lua. 
    for ( int i = 0; i < FILE_CNT; i++ )
        {

#ifdef PAC_PC
        char path[ 100 ] = "";
        if ( i < SYS_FILE_CNT )
            {
            snprintf( path, sizeof( path ), "%s%s", SYS_PATH, FILES[ i ] );
            }
        else
            {
            snprintf( path, sizeof( path ), "%s", FILES[ i ] );
            }
        
        if ( luaL_dofile( L, path ) != 0 )
#else
        if ( luaL_dofile( L, FILES[ i ] ) != 0 )
#endif // PAC_PC
            {
#ifdef DEBUG
            Print( "Load Lua script \"%s\" error!\n", 
                FILES[ i ] );
            Print( "\t%s\n", lua_tostring( L, -1 ) );
#endif // DEBUG
            lua_pop( L, 1 );

            return 1;
            }
        }

    //II
    //Экспортируем в Lua необходимые объекты.
    tolua_PAC_dev_open( L );

    //III
    //Выполняем процедуры инициализации.
    G_PROJECT_MANAGER->lua_load_configuration();

    if ( 0 == lua_state )      
        {
        if( luaL_loadfile( L, script_name ) != 0 )
            {
            Print( "Load Lua main script \"%s\" error!\n", script_name );
            Print( "\t%s\n", lua_tostring( L, -1 ) );

            lua_pop( L, 1 );
            return 1;
            }
        //-Инициализация Lua.--!>

        int i_line = lua_pcall( L, 0, LUA_MULTRET, 0 );
        if ( i_line != 0 )
            {
            Print( "Evaluate Lua script error!\n" );
            Print( "\t%s\n", lua_tostring( L, -1 ) );

            lua_pop( L, 1 );            
            return 1;
            } 
        }       

    return 0;
    }
//-----------------------------------------------------------------------------
lua_manager::~lua_manager()
    {
    if ( 1 == is_free_lua )
        {
        if ( L )
            {
            lua_close( L );
            L = NULL;
            }
        }    
    }
//-----------------------------------------------------------------------------
int lua_manager::void_exec_lua_method( const char *object_name,
    const char *function_name, const char *c_function_name ) const
    {
    int res = 0;
    if ( 0 == exec_lua_method( object_name, function_name, 0, 0, 0 ) )
        {
        }
    else
        {
        res = 1;
#ifdef DEBUG
        Print( "Error during C++ call - \"%s\"\n", c_function_name );
#endif // DEBUG
        }

    return res;
    }
//-----------------------------------------------------------------------------
int lua_manager::int_exec_lua_method( const char *object_name,
    const char *function_name, int param, const char *c_function_name ) const
    {
    int res = 0;
    if ( 0 == exec_lua_method( object_name, function_name, param ) )
        {
        res = ( int ) tolua_tonumber( L, -1, 0 );
        lua_remove( L, -1 );
        }
    else
        {
        res = 1;
#ifdef DEBUG
        Print( "Error during C++ call - \"%s\"\n", c_function_name );
#endif // DEBUG
        }

    return res;
    }
//-----------------------------------------------------------------------------
void* lua_manager::user_object_exec_lua_method( const char *object_name,
    const char *function_name, int param, const char *c_function_name ) const
    {
    void* res = NULL;
    if ( 0 == exec_lua_method( object_name, function_name, param ) )
        {
        res = tolua_tousertype( L, -1, NULL );
        lua_remove( L, -1 );
        }
    else
        {
#ifdef DEBUG
        Print( "Error during C++ call - \"%s\"\n", c_function_name );
#endif // DEBUG
        }

    return res;
    }
//-----------------------------------------------------------------------------
int lua_manager::exec_lua_method( const char *object_name,
    const char *function_name, int param, int is_use_param,
    int is_use_lua_return_value ) const
    {   
    //-Вычисление времени выполнения функций Lua.
    //LARGE_INTEGER start_time;
    //QueryPerformanceCounter( &start_time );

    //    u_long start_time;
    //    start_time = get_millisec();

    lua_pushcclosure( lua_manager::L, error_trace, 0 );
    instance->err_func = lua_gettop( L );

    int param_count = 0;

    if ( object_name && object_name != "" )
        {
        lua_getfield( L, LUA_GLOBALSINDEX, object_name );
        lua_getfield( L, -1, function_name );
        lua_remove( L, -2 );
        lua_getfield( L, LUA_GLOBALSINDEX, object_name );

        param_count++;
        }
    else
        {
        lua_getfield( L, LUA_GLOBALSINDEX, function_name );
        }
    
    if ( is_use_param )
        {
        lua_pushnumber( L, param );
        param_count++;
        }
    int results_count = is_use_lua_return_value == 1 ? 1 : 0;
    int res = lua_pcall( L, param_count, results_count, err_func );

    lua_remove( L, -results_count - 1 ); //Удаляем функцию error_trace.

    //LARGE_INTEGER finish_time;
    //QueryPerformanceCounter( &finish_time );
    //LARGE_INTEGER tick_per_sec;
    //QueryPerformanceFrequency( &tick_per_sec );
    //LONGLONG call_time = finish_time.QuadPart - start_time.QuadPart;    
    //int tiks_per_mcsec = tick_per_sec.QuadPart / ( 1000 * 1000 );
    //double dt = call_time / tiks_per_mcsec;
    //dt += 1;

    //    u_long finish_time;
    //    finish_time = get_millisec();
    //    u_long call_time = finish_time - start_time;
    //    Print( "Lua call time - %lums\n", call_time );

    return res;
    }
//-----------------------------------------------------------------------------
int lua_manager::error_trace( lua_State * L )
    {
#ifdef DEBUG
    Print( "\t%s\n", lua_tostring( L, -1 ) );
    lua_pop( L, 1 );

    Print( "\tstack traceback:\n" );
    lua_Debug ar;
    int level = 1;

    while( lua_getstack( L, level, &ar ) )
        {
        lua_getinfo( L, "Sln", &ar );
        Print( "\t\t%s:%d: in function '%s'\n",
            ar.source, ar.linedefined, ar.name );
        level++;
        }
#endif // DEBUG

    //////stack: err
    ////const char* err = lua_tostring(L, 1);

    ////printf("Error: %s\n", err);

    ////lua_getglobal(L, "debug"); // stack: err debug
    ////lua_getfield(L, -1, "traceback"); // stack: err debug debug.traceback

    ////// debug.traceback() возвращает 1 значение
    ////if(lua_pcall(L, 0, 1, 0))
    ////    {
    ////    const char* err = lua_tostring(L, -1);

    ////    printf("Error in debug.traceback() call: %s\n", err);
    ////    }
    ////else
    ////    {
    ////    const char* stackTrace = lua_tostring(L, -1);

    ////    printf("C++ stack traceback: %s\n", stackTrace);
    ////    }
    
    return 0;
    }
//-----------------------------------------------------------------------------
int lua_manager::int_no_param_exec_lua_method( const char *object_name, 
    const char *function_name, const char *c_function_name ) const
    {
    int res = 0;
    if ( 0 == exec_lua_method( object_name, function_name, 0, 0, 1 ) )
        {
        res = ( int ) tolua_tonumber( L, -1, 0 );
        lua_remove( L, -1 );
        }
    else
        {
#ifdef DEBUG
        Print( "Error during C++ call - \"%s\"\n", c_function_name );
#endif // DEBUG
        res = 1;
        }

    return res;
    }

const char* lua_manager::char_no_param_exec_lua_method( const char *object_name, 
    const char *function_name, const char *c_function_name ) const
    {
    const char* res = 0;
    if ( 0 == exec_lua_method( object_name, function_name, 0, 0, 1 ) )
        {
        res = tolua_tostring( L, -1, 0 );
        lua_remove( L, -1 );
        }
    else
        {
#ifdef DEBUG
        Print( "Error during C++ call - \"%s\"\n", c_function_name );
#endif // DEBUG
        }

    return res;
    }
//-----------------------------------------------------------------------------
int lua_manager::exec_Lua_str( const char *Lua_str, const char *error_str,
    bool is_print_error_msg /*= true */ ) const
    {
    int res = luaL_dostring( L, Lua_str );

    if( res != 0  )
        {
        if ( is_print_error_msg )
            {
            Print( "Error during C++ call - \"%s\" - %s\n", 
                error_str, lua_tostring( L, -1 ) );
            }

        lua_pop( L, 1 );
        return 1;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
lua_State * lua_manager::get_Lua() const
    {
    return L;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
