/// @par Описание директив препроцессора:
/// @c LINUX_OS         - компиляция для ОС linux.
/// @c PAC_PC           - компиляция для PAC на PC с ОС linux.
/// @c PAC_WAGO_750_860 - компиляция для PAC Wago 750-860.
///
/// @c DEBUG - компиляция c выводом отладочной информации в консоль.

#include <stdlib.h>
#if defined LINUX_OS
#include <time.h>
#endif // defined LINUX_OS

#include "sys.h"
#include "PAC_dev.h"

#ifdef LINUX_OS
#include "tcp_cmctr_linux.h"
#include "prj_mngr_linux.h"
#endif // LINUX_OS

#ifdef WIN_OS

#include <conio.h>

#include "tcp_cmctr_win.h"
#include "prj_mngr_win.h"
#endif // WIN_OS


#ifdef PAC_WAGO_750_860
#include "sys_w750.h"
#include "wago_w750.h"
#endif // PAC_WAGO_750_860

#ifdef PAC_PC
#include "sys_PC.h"
#include "wago_PC.h"
#endif // PAC_PC

#include "PAC_info.h"
#include "tech_def.h"

#include "lua_manager.h"

#if !defined PAC_WAGO_750_860 && !defined PAC_PC
#error
#endif

#if defined PAC_WAGO_750_860 && defined PAC_PC
#error
#endif

PAC_info *g_PAC_system;

int main( int argc, char *argv[] )
    {
    if ( argc < 2 )
        {
        Print( "Usage: main script.plua\n" );
        return EXIT_FAILURE;
        }

#if defined LINUX_OS
    time_t t = time( 0 );
    fprintf( stderr, "Program started - %s\n", asctime( localtime( &t ) ) );
#endif // defined LINUX_OS

#ifdef LINUX_OS
    project_manager::set_instance( new project_manager_linux() );
    tcp_communicator::set_instance( 
        new tcp_communicator_linux( "Выдача сыворотки" ) );
#endif // LINUX_OS
   
#ifdef WIN_OS
    project_manager::set_instance( new project_manager_win() );
    tcp_communicator::set_instance( 
        new tcp_communicator_win( "Установка №2" ) );
#endif // WIN_OS

    #if defined LINUX_OS
#ifdef PAC_WAGO_750_860
    wago_manager::set_instance( new wago_manager_w750() );
    NV_memory_manager::set_instance( new NV_memory_manager_W750() );
#endif // PAC_WAGO_750_860
#ifdef PAC_PC
    wago_manager::set_instance( new wago_manager_PC() );
    NV_memory_manager::set_instance( new NV_memory_manager_PC() );
#endif // PAC_PC
#endif // defined LINUX_OS
    
    device_manager::set_instance( new device_manager() );
    device_communicator::set_instance( new device_communicator() );

#if defined WIN_OS
    wago_manager::set_instance( new wago_manager_PC() );
    NV_memory_manager::set_instance( new NV_memory_manager_PC() );
#endif // defined WIN_OS

    tech_object_manager::set_instance( new tech_object_manager() );
    PAC_critical_errors_manager::set_instance( new PAC_critical_errors_manager() );

    //-Добавление системных тегов контроллера.
    g_PAC_system = new PAC_info();
    G_DEVICE_CMMCTR->add_device( g_PAC_system );

    G_PROJECT_MANAGER->proc_main_params( argc, argv );

    G_CMMCTR->reg_service( device_communicator::C_SERVICE_N,
        device_communicator::write_devices_states_service );

    int res = lua_manager::get_instance()->init( 0, argv[ 1 ] ); //-Инициализация Lua.
    lua_gc( lua_manager::get_instance()->get_Lua(), LUA_GCSTOP, 0 );

    if ( res ) //-Ошибка инициализации.
    	{
        fprintf( stderr, "Lua init error - %d!\n", res );
        return EXIT_FAILURE;
    	}           
    G_TECH_OBJECT_MNGR()->init_objects();

    //-Добавление системных тегов контроллера.
    g_PAC_system = new PAC_info();
    G_DEVICE_CMMCTR->add_device( g_PAC_system );

    params_manager::get_instance()->init( 10 );
    params_manager::get_instance()->final_init();

#ifdef DEBUG
    G_DEVICE_MANAGER()->print();
    G_DEVICE_CMMCTR->print();
    G_TECH_OBJECT_MNGR()->print();
#endif // DEBUG
    
    print_time( "Lua mem = %d b\n",
        lua_gc( lua_manager::get_instance()->get_Lua(), LUA_GCCOUNT, 0 ) * 1024 +
        lua_gc( lua_manager::get_instance()->get_Lua(), LUA_GCCOUNTB, 0 ) );

    lua_gc( lua_manager::get_instance()->get_Lua(), LUA_GCRESTART, 0 );
    lua_gc( lua_manager::get_instance()->get_Lua(), LUA_GCCOLLECT, 0 );

    print_time( "Lua mem = %d b\n",
        lua_gc( lua_manager::get_instance()->get_Lua(), LUA_GCCOUNT, 0 ) * 1024 +
        lua_gc( lua_manager::get_instance()->get_Lua(), LUA_GCCOUNTB, 0 ) );
    fflush( stdout );
    fprintf( stderr, "Start main loop!\n" );
    
#ifdef DEBUG
    while ( !kbhit() )
#else
    while ( 1 )
#endif // DEBUG
        {
        lua_gc( lua_manager::get_instance()->get_Lua(), LUA_GCSTEP, 200 );
#ifdef WIN_OS
        Sleep( 1 );
#else 
        usleep( 1 );
#endif // WIN_OS       
        
        static u_long st_time;
        static u_long all_time   = 0;
        static u_long cycles_cnt = 0;

        st_time = get_millisec();
        cycles_cnt++;

#ifndef DEBUG_NO_WAGO_MODULES
        G_WAGO_MANAGER()->read_inputs();
#endif // DEBUG_NO_WAGO_MODULES

        G_TECH_OBJECT_MNGR()->evaluate();

        G_CMMCTR->evaluate();

#ifndef DEBUG_NO_WAGO_MODULES
        G_WAGO_MANAGER()->write_outputs();
#endif // ifndef

        PAC_critical_errors_manager::get_instance()->show_errors();

        g_PAC_system->eval();

        all_time += get_millisec() - st_time;
#if defined LINUX_OS
#ifdef PAC_PC
        const u_int MAX_ITERATION = 1000;
#endif // PAC_PC
#ifdef PAC_WAGO_750_860
        const u_int MAX_ITERATION = 1500;
#endif // PAC_WAGO_750_860
#endif // defined LINUX_OS

#ifdef WIN_OS
        const u_int MAX_ITERATION = 1000;
#endif // WIN_OS

        //-Информация о времени выполнения цикла программы.
        static u_int max_cycle_time = 0;
        u_int cycle_time = get_millisec() - st_time;
        if ( max_cycle_time < cycle_time ) 
            {
            max_cycle_time = cycle_time;
            print_time( "\tMain cycle avg time = %lu msec, max time = %4u, Lua mem = %d b\n",
                all_time / cycles_cnt, max_cycle_time,
                lua_gc( lua_manager::get_instance()->get_Lua(), LUA_GCCOUNT, 0 ) * 1024 +
                lua_gc( lua_manager::get_instance()->get_Lua(), LUA_GCCOUNTB, 0 ) );
            fflush( stdout );
            }

        static u_int print_cycle_time_count = 0;
        if ( cycles_cnt > MAX_ITERATION )
            {
            if ( print_cycle_time_count < 1000 )
                {
                print_time( "\tMain cycle avg time = %lu msec, max time = %4u, Lua mem = %d b\n",
                    all_time / cycles_cnt, max_cycle_time,
                    lua_gc( lua_manager::get_instance()->get_Lua(), LUA_GCCOUNT, 0 ) * 1024 +
                    lua_gc( lua_manager::get_instance()->get_Lua(), LUA_GCCOUNTB, 0 ) );
                    print_cycle_time_count++;
                fflush( stdout );
                }

            all_time = 0;
            cycles_cnt = 0;
            }
        //-Информация о времени выполнения цикла программы.!->
        }

    delete g_PAC_system;
    g_PAC_system = 0;

    return( EXIT_SUCCESS );
    }
