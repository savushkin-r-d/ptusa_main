#include <stdlib.h>
#include <time.h>


#include "sys.h"
#include "PAC_dev.h"
#include "tcp_cmctr_linux.h"
#include "wago_w750.h"
#include "prj_mngr_w750.h"

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
    time_t t = time( 0 );
    fprintf( stderr, "Program started - %s\n", asctime( localtime( &t ) ) );

    project_manager::set_instance( new project_manager_w750() );
    tcp_communicator::set_instance( new tcp_communicator_linux() );
    wago_manager::set_instance( new wago_manager_w750() );
    device_manager::set_instance( new device_manager() );
    device_communicator::set_instance( new device_communicator() );
    NV_memory_manager::set_instance( new NV_memory_manager_W750() );

    tech_object_manager::set_instance( new tech_object_manager() );

    PAC_critical_errors_manager::set_instance( new PAC_critical_errors_manager() );

    G_PROJECT_MANAGER->proc_main_params( argc, argv );

#ifdef UCLINUX
    G_PROJECT_MANAGER->load_configuration( "Whey_out.ds5" );
#endif // UCLINUX

#ifdef LINUX
    G_PROJECT_MANAGER->load_configuration(
        "/home/id/src/PAC_control_projects/whey_out/Whey_out.ds5" );
#endif // LINUX

#ifdef DEBUG
    G_DEVICE_MANAGER->print();
#endif // DEBUG

    G_CMMCTR->reg_service( device_communicator::C_SERVICE_N,
        device_communicator::write_devices_states_service );

    init_tech_process();

#ifdef DEBUG
    G_DEVICE_CMMCTR->print();
#endif // DEBUG

#ifdef DEBUG
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

#if defined UCLINUX && !defined DEBUG_NO_WAGO_MODULES
       G_WAGO_MANAGER->read_inputs();
#endif // defined UCLINUX && !defined DEBUG_NO_WAGO_MODULES

        evaluate_all();

        G_CMMCTR->evaluate();

#if defined UCLINUX && !defined DEBUG_NO_WAGO_MODULES
       G_WAGO_MANAGER->write_outputs();
#endif // defined UCLINUX && !defined DEBUG_NO_WAGO_MODULES

        PAC_critical_errors_manager::get_instance()->show_errors();

#ifdef DEBUG
        all_time += get_millisec() - st_time;

#ifdef LINUX
       const u_int MAX_ITERATION = 10000;
#endif // LINUX
#ifdef UCLINUX
       const u_int MAX_ITERATION = 1000000;
#endif // UCLINUX

        if ( cycles_cnt > MAX_ITERATION )
            {
            Print( "\tMain cycle avg time = %lu\n", all_time / cycles_cnt  );
            all_time = 0;
            cycles_cnt = 0;
            }
#endif // DEBUG
        }

    tcp_communicator::free_instance();

    return( EXIT_SUCCESS );
    }
