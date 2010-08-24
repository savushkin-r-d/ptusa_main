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
#include "tcp_cmctr_linux.h"
#include "prj_mngr_linux.h"

#ifdef PAC_WAGO_750_860
#include "sys_w750.h"
#include "wago_w750.h"
#endif // PAC_WAGO_750_860

#ifdef PAC_PC
#include "sys_PC.h"
#include "wago_PC.h"
#endif // PAC_PC

#include "tech_def.h"
#include "init.h"

#if !defined PAC_WAGO_750_860 && !defined PAC_PC
#error
#endif

#if defined PAC_WAGO_750_860 && defined PAC_PC
#error
#endif

int main( int argc, char *argv[] )
    {    
#if defined LINUX_OS
    time_t t = time( 0 );
    fprintf( stderr, "Program started - %s\n", asctime( localtime( &t ) ) );
#endif // defined LINUX_OS

    project_manager::set_instance( new project_manager_linux() );
    tcp_communicator::set_instance( new tcp_communicator_linux() );
    device_manager::set_instance( new device_manager() );
    device_communicator::set_instance( new device_communicator() );
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

    tech_object_manager::set_instance( new tech_object_manager() );
    PAC_critical_errors_manager::set_instance( new PAC_critical_errors_manager() );

    G_PROJECT_MANAGER->proc_main_params( argc, argv );
    G_PROJECT_MANAGER->load_configuration( "./whey_out.ds5" );

#ifdef DEBUG
    G_DEVICE_MANAGER->print();
#endif // DEBUG

    G_CMMCTR->reg_service( device_communicator::C_SERVICE_N,
        device_communicator::write_devices_states_service );

    init_tech_process();

#ifdef DEBUG
    G_DEVICE_CMMCTR->print();

    ulong st_time;
    ulong all_time = 0;
    ulong cycles_cnt = 0;
#endif // DEBUG

#ifdef DEBUG
    while ( !kbhit() )
#else
    while ( 1 )
#endif // DEBUG
        {
#ifdef DEBUG
        st_time = get_millisec();
        cycles_cnt++;
#endif // DEBUG

#ifndef DEBUG_NO_WAGO_MODULES
       G_WAGO_MANAGER->read_inputs();
#endif // DEBUG_NO_WAGO_MODULES

        evaluate_all();

        G_CMMCTR->evaluate();

#ifndef DEBUG_NO_WAGO_MODULES
       G_WAGO_MANAGER->write_outputs();
#endif // ifndef

        PAC_critical_errors_manager::get_instance()->show_errors();

#ifdef DEBUG
        all_time += get_millisec() - st_time;

#if defined LINUX_OS
#ifdef PAC_PC
       const u_int MAX_ITERATION = 10000;
#endif // PAC_PC
#ifdef PAC_WAGO_750_860
       const u_int MAX_ITERATION = 1000;
#endif // PAC_WAGO_750_860
#endif // defined LINUX_OS

       static char print_cycle_time_count = 0;
       if ( print_cycle_time_count < 10 && cycles_cnt > MAX_ITERATION )
            {
            print_time( "\tMain cycle avg time = %lu\n", all_time / cycles_cnt  );
            all_time = 0;
            cycles_cnt = 0;

            print_cycle_time_count++;
            }
#endif // DEBUG
       }

    return( EXIT_SUCCESS );
    }

