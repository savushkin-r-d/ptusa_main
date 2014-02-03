#include "wago_PFC200.h"

#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
wago_manager_PFC200::wago_manager_PFC200(): task_id( 0 )
    {
	adi = adi_GetApplicationInterface(); // Connect to ADI-interface.
	adi->Init();                         // Init interface.
	adi->ScanDevices();                  // Scan devices.
	adi->GetDeviceList( sizeof( deviceList ), deviceList, &nr_devices_found );

	// Find kbus device.
	nr_kbus_found = -1;
	for ( size_t i = 0; i < nr_devices_found; ++i )
        {
		if ( strcmp( deviceList[ i ].DeviceName, "libpackbus" ) == 0 )
            {
			nr_kbus_found = i;
			fprintf( stderr, "KBUS device found as device %i.\n", i );
            }
        }

	// Kbus not found > exit.
	if ( nr_kbus_found < 0 )
        {
		fprintf( stderr, "No KBUS device found. \n" );
		adi->Exit();            // Disconnect ADI-Interface.
		exit( EXIT_FAILURE );   // Exit program.
        }

	// Switch to RT Priority.
    struct sched_param s_param;
    const int KBUS_MAINPRIO = 40;
	s_param.sched_priority = KBUS_MAINPRIO;
	sched_setscheduler( 0, SCHED_FIFO, &s_param );
#ifdef DEBUG
	printf( "Switch to RT Priority 'KBUS_MAINPRIO'\n" );
#endif // DEBUG

	// Open kbus device.
	kbus_device_id = deviceList[ nr_kbus_found ].DeviceId;
	if ( adi->OpenDevice( kbus_device_id ) != DAL_SUCCESS )
        {
		fprintf( stderr, "Kbus device open failed.\n" );
		adi->Exit();
		exit( EXIT_FAILURE );
        }
#ifdef DEBUG
    printf( "KBUS device open OK.\n" );
#endif // DEBUG

    // Set application state to "Unconfigured" to let library drive kbus by
	// themselves. In this mode library set up a thread who drive the kbus cyclic.
    tApplicationStateChangedEvent event;
    event.State = ApplicationState_Unconfigured;
    if ( adi->ApplicationStateChanged( event ) != DAL_SUCCESS )
        {
      	// Set application state to "Unconfigured" failed.
      	fprintf( stderr, "Set application state to 'Unconfigured' failed.\n" );
		adi->CloseDevice( kbus_device_id ); // Close kbus device.
    	adi->Exit();
    	exit( EXIT_FAILURE );
        }

#ifdef DEBUG
	printf( "Set application state to 'Unconfigured'.\n" );
#endif // DEBUG
    }
//-----------------------------------------------------------------------------
wago_manager_PFC200::~wago_manager_PFC200()
    {
    adi->CloseDevice( kbus_device_id ); // Close kbus device.
    adi->Exit();                        // Disconnect ADI-Interface.
    }
//-----------------------------------------------------------------------------
int wago_manager_PFC200::read_inputs()
    {
    if ( 0 == nodes_count ) return 0;

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        if ( nodes[ i ]->type == wago_node::T_750_820x )
            {
            wago_node *nd = nodes[ i ];

            if ( !nd->is_active )
                {
                continue;
                }

            usleep( 1000 );         // Wait 1 ms.
            adi->WatchdogTrigger(); // Trigger Watchdog.

            // Read inputs.
            int size = nd->AI_size + nd->DI_cnt / 8 + 1;

            int res = adi->ReadStart( kbus_device_id, task_id ); // Lock PD-In data.
            res += adi->ReadBytes( kbus_device_id, task_id, 0, size, pd_in );
            res += adi->ReadEnd( kbus_device_id, task_id );      // Unlock PD-In data.

            if ( nd->DI_cnt > 0 )
                {
                u_int bytes_cnt = nd->DI_cnt / 8 + ( nd->DI_cnt % 8 > 0 ? 1 : 0 );
                u_int offset = nd->AI_size;

#ifdef DEBUG_KBUS
                printf( "DI offset = %d\n", offset );
#endif // DEBUG_KBUS

                if ( 0 == res )
                    {
                    for ( u_int j = 0, idx = 0; j < bytes_cnt; j++ )
                        {
                        for ( int k = 0; k < 8; k++ )
                            {
                            if ( idx < nd->DI_cnt )
                                {
                                nd->DI[ idx ] = ( pd_in[ j + offset ] >> k ) & 1;
#ifdef DEBUG_KBUS
                                printf( "%d -> %d, ", idx, nd->DI[ idx ] );
#endif // DEBUG_KBUS
                                idx++;
                                }
                            }
                        }
#ifdef DEBUG_KBUS
                    printf( "\n" );
#endif // DEBUG_KBUS
                    }
                    else
                    {
#ifdef DEBUG
                    Print("\nRead DI:Wago returned error...\n");
#endif // DEBUG
                    }// if ( res == 0 )
                }// if ( nd->DI_cnt > 0 )

            if ( nd->AI_cnt > 0 )
                {
                // AI
                for ( u_int j = 0; j < nd->AI_cnt; j++ )
                    {
                    u_int val = 0;
                    u_int offset = nd->AI_offsets[ j ];

                    switch ( nd->AI_types[ j ] )
                        {
                    case 466:
                    case 461:
                        val = pd_in[ offset ] + 256 * pd_in[ offset + 1 ];
                        break;

                    case 638:
                        val = pd_in[ offset + 2 ] + 256 * pd_in[ offset + 3 ];
                        break;
                        }
                    nd->AI[ j ] = val;
#ifdef DEBUG_KBUS
                    printf( "%d -> %u, ", j, nd->AI[ j ] );
#endif // DEBUG_KBUS
                    }
#ifdef DEBUG_KBUS
                printf( "\n" );
#endif // DEBUG_KBUS
                }// if ( nd->AI_cnt > 0 )

            }// if ( nodes[ i ]->type == wago_node::T_750_820x || ...
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return wago_manager_linux::read_inputs();
    }
//-----------------------------------------------------------------------------
int wago_manager_PFC200::write_outputs()
    {
    if ( 0 == nodes_count ) return 0;

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        wago_node *nd = nodes[ i ];
        if ( nd->type == wago_node::T_750_820x ) // KBus
            {
            // DO
            int start_pos = nd->AO_size;
#ifdef DEBUG_KBUS
            Print( "DO offset = %d\n", start_pos );
#endif // DEBUG_KBUS

            for ( u_int j = 0; j < nd->DO_cnt; j++ )
                {
                char tmp =  1 << ( j % 8 );
                if ( nd->DO_[ j ] )
                    {
                    pd_out[ start_pos + j / 8 ] |= tmp;
                    }
                else
                    {
                    pd_out[ start_pos + j / 8 ] &= ~tmp;
                    }
                nd->DO[ j ] = nd->DO_[ j ];
#ifdef DEBUG_KBUS
                Print( "%d -> %d, ", j, nd->DO_[ j ] );
#endif // DEBUG_KBUS
                }
#ifdef DEBUG_KBUS
            Print( "\n" );
#endif // DEBUG_KBUS

            // AO
            for ( u_int j = 0; j < nd->AO_cnt; j++ )
                {
                int val = nd->AO_[ j ];
                u_int offset = nd->AO_offsets[ j ];

                pd_out[ offset ] = val & 0xFF;
                pd_out[ offset + 1 ] = val >> 8;

                nd->AO[ j ] = nd->AO_[ j ];
#ifdef DEBUG_KBUS
                printf( "%d -> %u, ", j, nd->AO_[ j ] );
#endif // DEBUG_KBUS
                }
#ifdef DEBUG_KBUS
            printf( "\n" );
#endif // DEBUG_KBUS

            usleep( 1000 );         // Wait 1 ms.
            adi->WatchdogTrigger(); // Trigger Watchdog.

            int size = nd->AO_size + nd->DO_cnt / 8 + 1;
            adi->WriteStart( kbus_device_id, task_id );    // Lock PD-In data.
            adi->WriteBytes( kbus_device_id, task_id, 0, size, pd_out );
            adi->WriteEnd( kbus_device_id, task_id );      // Unlock PD-In data.

            }// if ( nd->type == wago_node::T_750_820x ) // KBus
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return wago_manager_linux::write_outputs();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
