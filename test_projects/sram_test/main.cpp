/* 
 * File:   main.cpp
 * Author: id
 *
 * Created on 26 Март 2010 г., 11:38
 */

#include "param_ex.h"

/*
 * 
 */

int main( int argc, char** argv )
    {
    NV_memory_manager::set_instance( new NV_memory_manager_W750() );

    params_test::make_test();

    return( 0 );
    }
