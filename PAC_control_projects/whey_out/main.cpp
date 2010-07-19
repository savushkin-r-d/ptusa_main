#include <stdlib.h>
#include <time.h>

#include "PAC_dev.h"
#include "tcp_cmctr_lin.h"
#include "wago_w750.h"
#include "prj_mngr_w750.h"

#include "tech_def.h"

#if !defined UCLINUX && !defined LINUX
#error
#endif

#if defined UCLINUX && defined LINUX
#error
#endif

int main( int argc, char *argv[] )
    {
    project_manager::set_instance( new project_manager_w750() );
    G_PROJECT_MANAGER->proc_main_params( argc, argv );

    tcp_communicator::set_instance( new tcp_communicator_linux() );
    wago_manager::set_instance( new wago_manager_w750() );    
    device_manager::set_instance( new device_manager() );
    device_communicator::set_instance( new device_communicator() );
    NV_memory_manager::set_instance( new NV_memory_manager_W750() );

    tech_object_manager::set_instance( new tech_object_manager() );

#ifdef UCLINUX
    G_PROJECT_MANAGER->load_configuration( "Whey_out.ds5" );
#endif // UCLINUX 

#ifdef LINUX
    G_PROJECT_MANAGER->load_configuration(
        "/home/id/src/PAC_test_projects/wago_dev_test/Whey_out.ds5" );
#endif // LINUX


    G_CMMCTR->reg_service( device_communicator::C_SERVICE_N,
        device_communicator::write_devices_states_service );

    smart_ptr< tech_object > test( new tech_object( "TANK", 1, 2, 1, 1, 1, 1, 1 ) );
    test->set_mode( 1, 1 );

    G_DEVICE_CMMCTR->add_device( test->get_complex_dev() );

#ifdef DEBUG
    G_DEVICE_MANAGER->print();

    wago_device::debug_mode = 1;

    G_DEVICE_CMMCTR->print();
#endif // DEBUG

//    while ( 1 )
//        {
//#ifdef UCLINUX
//        G_WAGO_MANAGER->read_inputs();
//#endif // UCLINUX
//
//        G_CMMCTR->evaluate();
//
//        static u_long start_time = get_millisec();
//        if ( get_delta_millisec( start_time ) > 2000 )
//            {
//            V( 101 )->set_state( !V( 101 )->get_state() );
//            start_time = get_millisec();
//            }
//
//#ifdef UCLINUX
//            G_WAGO_MANAGER->write_outputs();
//#endif // UCLINUX
//        }

    return( EXIT_SUCCESS );
    }

