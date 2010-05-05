/* 
 * File:   main.cpp
 * Author: id
 *
 * Created on 9 Апрель 2010 г., 15:03
 */

#include <stdlib.h>
#include <time.h>

#include "PAC_dev.h"
#include "tcp_cmctr_w750.h"
#include "wago_w750.h"
#include "prj_mngr_w750.h"

/*
 * 
 */
int main( int argc, char** argv )
    {
    tcp_communicator::set_instance( new tcp_communicator_w750() );
    wago_manager::set_instance( new wago_manager_w750() );
    project_manager::set_instance( new project_manager_w750() );
    device_manager::set_instance( new device_manager() );

#ifdef UCLINUX
    project_manager::get_instance()->load_configuration( "Whey_out.ds5" );
#else
    project_manager::get_instance()->load_configuration(
        "/home/id/src/test_projects/wago_dev_test/Whey_out.ds5" );
#endif // UCLINUX

    device_manager::get_instance()->print();

    while ( 1 )
        {
//        V( 1 )->on();
//        Print( " state = %d\n",V( 1 )->get_state() );
//        sleep( 1 );
//        V( 1 )->off();
//        Print( " state = %d\n",V( 1 )->get_state() );
//        sleep( 1 );
        wago_manager::get_instance()->read_inputs();
        sleep( 1 );

        V( 101 )->set_state( !V( 101 )->get_state() );

        wago_manager::get_instance()->write_outputs();
        }

    return( EXIT_SUCCESS );
    }

