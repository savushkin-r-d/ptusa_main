#include "sys.h"
#include "PAC_dev.h"
#include "tcp_cmctr_lin.h"
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
        if ( cycles_cnt > 500 )
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

///*
// * CDDL HEADER START
// *
// * The contents of this file are subject to the terms of the
// * Common Development and Distribution License, Version 1.0 only
// * (the "License").  You may not use this file except in compliance
// * with the License.
// *
// * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
// * or http://www.opensolaris.org/os/licensing.
// * See the License for the specific language governing permissions
// * and limitations under the License.
// *
// * When distributing Covered Code, include this CDDL HEADER in each
// * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
// * If applicable, add the following below this CDDL HEADER, with the
// * fields enclosed by brackets "[]" replaced with your own identifying
// * information: Portions Copyright [yyyy] [name of copyright owner]
// *
// * CDDL HEADER END
// */
///*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
///*	  All Rights Reserved  	*/
//
//
///*
// * Copyright 2000-2003 Sun Microsystems, Inc.  All rights reserved.
// * Use is subject to license terms.
// */
//
//#pragma ident	"%Z%%M%	%I%	%E% SMI"
//
///*
// **	nice
// */
//
//
//#include	<stdio.h>
//#include	<locale.h>
//#include	<stdlib.h>
//#include	<string.h>
//#include	<unistd.h>
//#include	<errno.h>
//
//
//static void usage( void );
//
//int
//main( int argc, char *argv[] )
//{
//    long nicarg = 10;
//
//    ( void ) setlocale( LC_ALL, "" );
//#if !defined(TEXT_DOMAIN)
//#define	TEXT_DOMAIN	"SYS_TEST"
//#endif
//    //( void ) textdomain( TEXT_DOMAIN );
//
//
//    if( argc < 2 )
//        usage( );
//
//
//    if( argv[1][0] == '-' )
//    {
//        if( strcmp( argv[1], "--" ) == 0 )
//        {
//            argv++;
//            argc--;
//        }
//        else
//        {
//            register char *p = argv[1];
//            char *nicarg_str;
//            char *end_ptr;
//
//            if( *++p == 'n' )
//            { /* -n55 new form, XCU4 */
//                /*
//                 * for situations like -n-10
//                 * else case assigns p instead of argv
//                 */
//                if( !( *++p ) )
//                {
//                    /* Next arg is priority */
//                    argv++;
//                    argc--;
//                    if( argc < 2 )
//                        usage( );
//                    nicarg_str = argv[1];
//                }
//                else
//                {
//                    /* Priority embedded eg. -n-10 */
//                    nicarg_str = p;
//                }
//            }
//            else
//            { /* -55 obs form, XCU4 */
//                nicarg_str = &argv[1][1];
//            }
//            nicarg = strtol( nicarg_str, &end_ptr, 10 );
//            if( *end_ptr )
//            {
//                ( void ) fprintf( stderr, ( "nice: argument must be numeric.\n" ) );
//                usage( );
//            }
//
//            if( --argc < 2 )
//                usage( );
//
//            argv++;
//            if( strcmp( argv[1], "--" ) == 0 )
//            {
//                argv++;
//                argc--;
//            }
//        }
//    }
//
//    if( argc < 2 )
//        usage( );
//
//    errno = 0;
//    if( nice( nicarg ) == -1 )
//    {
//        /*
//         * Could be an error or a legitimate return value.
//         * The only error we care about is EINVAL, which will
//         * be returned by the scheduling class we are in if
//         * nice is invalid for this class.
//         * For any error other than EINVAL
//         * we will go ahead and exec the command even though
//         * the priority change failed.
//         */
//        if( errno == EINVAL )
//        {
//            ( void ) fprintf( stderr, (
//                    "nice: invalid operation; "
//                    "scheduling class does not support nice\n" ) );
//            return(2 );
//        }
//    }
//    ( void ) execvp( argv[1], &argv[1] );
//    ( void ) fprintf( stderr, ( "%s: %s\n" ), strerror( errno ), argv[1] );
//    /*
//     * POSIX.2 exit status:
//     * 127 if utility is not found.
//     * 126 if utility cannot be invoked.
//     */
//    return(errno == ENOENT || errno == ENOTDIR ? 127 : 126 );
//}
//
//static void
//usage( )
//{
//    ( void ) fprintf( stderr,
//            ( "nice: usage: nice [-n increment] utility [argument ...]\n" ) );
//    exit( 2 );
//}
