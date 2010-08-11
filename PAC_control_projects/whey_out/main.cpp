#include <stdlib.h>
#if defined UCLINUX || defined LINUX
#include <time.h>
#endif // defined UCLINUX || defined LINUX

#include "sys.h"
#include "PAC_dev.h"
#include "tcp_cmctr_linux.h"
#include "prj_mngr_linux.h"

#ifdef PAC_W750
#include "wago_w750.h"
#endif // PAC_W750

#ifdef PAC_PC
#include "sys_PC.h"
#include "wago_PC.h"
#endif // PAC_PC

#include "tech_def.h"
#include "init.h"

#if !defined UCLINUX && !defined LINUX
#error
#endif

#if defined UCLINUX && defined LINUX
#error
#endif

int main( int argc, char *argv[] )
    {
#if defined UCLINUX || defined LINUX
    time_t t = time( 0 );
    fprintf( stderr, "Program started - %s\n", asctime( localtime( &t ) ) );
#endif // defined UCLINUX || defined LINUX

    project_manager::set_instance( new project_manager_linux() );
    tcp_communicator::set_instance( new tcp_communicator_linux() );
    device_manager::set_instance( new device_manager() );
    device_communicator::set_instance( new device_communicator() );
#ifdef PAC_W750
    wago_manager::set_instance( new wago_manager_w750() );
    NV_memory_manager::set_instance( new NV_memory_manager_W750() );
#endif // PAC_W750
#ifdef PAC_PC
    wago_manager::set_instance( new wago_manager_PC() );
    NV_memory_manager::set_instance( new NV_memory_manager_PC() );
#endif // PAC_PC
//
    tech_object_manager::set_instance( new tech_object_manager() );
    PAC_critical_errors_manager::set_instance( new PAC_critical_errors_manager() );

    G_PROJECT_MANAGER->proc_main_params( argc, argv );

#ifdef UCLINUX
    G_PROJECT_MANAGER->load_configuration( "Whey_out.ds5" );
#endif // UCLINUX

#ifdef PAC_PC
    G_PROJECT_MANAGER->load_configuration(
        "/home/id/src/PAC_control_projects/whey_out/Whey_out.ds5" );
#endif // PAC_PC

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

#ifdef LINUX
       const u_int MAX_ITERATION = 10000;
#endif // LINUX
#ifdef UCLINUX
       const u_int MAX_ITERATION = 1000;
#endif // UCLINUX

        if ( cycles_cnt > MAX_ITERATION )
            {
            print_time( "\tMain cycle avg time = %lu\n", all_time / cycles_cnt  );
            all_time = 0;
            cycles_cnt = 0;
            }
#endif // DEBUG
        }

    return( EXIT_SUCCESS );
    }
