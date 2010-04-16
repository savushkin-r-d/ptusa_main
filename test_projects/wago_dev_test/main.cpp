/* 
 * File:   main.cpp
 * Author: id
 *
 * Created on 9 Апрель 2010 г., 15:03
 */

#include <stdlib.h>

#include "PAC_dev.h"
#include "tcp_cmctr_w750.h"

/*
 * 
 */
int main( int argc, char** argv )
    {
    tcp_communicator::set_instance( new tcp_communicator_w750() );
    wago_manager::set_instance( new wago_manager() );

    DO_1 do_test;
    do_test.print();

    while ( 1 )
        {
        V( 1 )->on();
        Print( " state = %d\n",V( 1 )->get_state() );
        sleep( 1 );
        V( 1 )->off();
        Print( " state = %d\n",V( 1 )->get_state() );
        sleep( 1 );
        }

    return( EXIT_SUCCESS );
    }

