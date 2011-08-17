#include <string.h>
#include <stdio.h>

#include "g_device.h"

#include "PAC_dev.h"

#include "PAC_err.h"
#include "errors.h"

#include "lua_manager.h"

auto_smart_ptr < device_communicator > device_communicator::instance;

u_int_2 G_PROTOCOL_VERSION = 100;

std::vector< i_Lua_save_device* > device_communicator::dev;

//-----------------------------------------------------------------------------
void print_str( char *err_str, char is_need_CR )
    {    
#ifdef DRIVER
    bug_log::add_msg( "System", "", err_str );
#else
    Print( "%s", err_str  );
    if ( is_need_CR )
        {
        Print( "\n" );
        }
#endif // DRIVER
    }
//-----------------------------------------------------------------------------
long device_communicator::write_devices_states_service( long len, 
                                                       u_char *data,
                                                       u_char *outdata )
    {
    if ( len < 1 ) return 0;

    u_int answer_size = 0;
    
#ifdef DEBUG_DEV_CMCTR
    u_long start_time = get_millisec();
#endif // DEBUG_DEV_CMCTR             

    u_int param_size = 0;
    static u_int_2 g_devices_request_id = 0;

    switch ( data[ 0 ] )
        {
        case CMD_GET_INFO_ON_CONNECT:            
            sprintf( ( char* ) outdata, 
                "protocol_version = %d\n\r PAC_name = \"%s\"", 
                G_PROTOCOL_VERSION,
                tcp_communicator::get_instance()->get_host_name() );

            answer_size = strlen( ( char* ) outdata ) + 1;

#ifdef DEBUG_DEV_CMCTR
            Print( "G_PROTOCOL_VERSION = %u, host =[%s]\n", G_PROTOCOL_VERSION,
                tcp_communicator::get_instance()->get_host_name() );
#endif // DEBUG_DEV_CMCTR
            return answer_size;

        case CMD_GET_DEVICES:
            {
            param_size = sizeof( g_devices_request_id );
            if ( 0 == g_devices_request_id )
                {
                memcpy( &g_devices_request_id, data + 1, param_size );
                }

            memcpy( outdata, &g_devices_request_id, param_size );
            answer_size += param_size;

            for ( u_int i = 0; i < dev.size(); i++ )
                {
                answer_size += dev[ i ]->save_device( ( char* ) outdata + 
                    answer_size );
                }      
                        
            *( ( char* ) outdata + answer_size ) = 0;
            answer_size++;

#ifdef DEBUG
            Print( "%s", outdata + 2 );
#endif // DEBUG

#ifdef DEBUG_DEV_CMCTR
            Print( "%s", outdata + 2 );

            Print( "Devices size = %u, g_devices_request_id = %u\n",
                answer_size, 
                g_devices_request_id );

            Print( "Operation time = %lu\n", get_delta_millisec( start_time ) );
#endif // DEBUG_DEV_CMCTR
            return answer_size;
            }

        case CMD_GET_DEVICES_STATES:
            {            
            param_size = sizeof( g_devices_request_id );
            memcpy( outdata, &g_devices_request_id, param_size );
            answer_size += param_size;

            for ( u_int i = 0; i < dev.size(); i++ )
                {
                answer_size += dev[ i ]->save_device( ( char* ) outdata +
                    answer_size );
                }

            *( ( char* ) outdata + answer_size ) = 0;
            answer_size++;

#ifdef DEBUG
            //Print( "%s", outdata + 2 );
#endif // DEBUG

#ifdef DEBUG_DEV_CMCTR
            Print( "Devices states size = %u, g_devices_request_id = %d\n",
                answer_size, g_devices_request_id );

            Print( "Operation time = %lu\n", get_delta_millisec( start_time ) );
#endif // DEBUG_DEV_CMCTR
            return answer_size;
            }

     case CMD_EXEC_DEVICE_COMMAND:
            {            
#ifdef DEBUG_DEV_CMCTR
            Print( "\nEXEC_DEVICE_CMD\n" );
            Print( "cmd = %s\n",  data + 1 );
#endif // DEBUG_DEV_CMCTR
			
			int res = lua_manager::get_instance()->exec_Lua_str( ( char* ) data + 1, 
                "CMD_EXEC_DEVICE_COMMAND ");
                      
            outdata[ 0 ] = 0;
            outdata[ 1 ] = 0; //Возвращаем 0.
            if ( res )
                {
                outdata[ 0 ] = 1;
                }
			
#ifdef DEBUG_DEV_CMCTR
            Print( "Operation time = %lu\n", get_delta_millisec( start_time ) );
#endif // DEBUG_DEV_CMCTR

            answer_size = 2;
            return answer_size;
            }

        case CMD_GET_PAC_ERRORS:
            {
#ifdef DEBUG_DEV_CMCTR
            Print( "CMD_GET_PAC_ERRORS\n" );
#endif

//           answer_size = PAC_critical_errors_manager::get_instance()->save_to_stream( 
 //              ( char* ) outdata + answer_size );                

#ifdef DEBUG_DEV_CMCTR
           int critical_errors_size = answer_size;
            Print( "Critical errors count = %d, answer size = %d\n",
                outdata[ 2 ], critical_errors_size );
#endif // DEBUG_DEV_CMCTR

  //          answer_size += G_DEV_ERRORS_MANAGER->save_to_stream( outdata + 
 //               answer_size );

#ifdef DEBUG_DEV_CMCTR
            u_int_2 cnt = 0;
            memcpy( &cnt, outdata + critical_errors_size + 2, sizeof( u_int_2 ) );
            Print( "Simple devices errors errors count = %d, answer size = %d\n", 
                cnt, answer_size - critical_errors_size );
#endif // DEBUG_DEV_CMCTR

            return answer_size;
            }

        case CMD_SET_PAC_ERROR_CMD:            
            u_int_2 count = 0;

            memcpy( &count, data + 1, sizeof( count ) );

#ifdef DEBUG_DEV_CMCTR
                Print( "SET_PAC_ERROR_CMD\t" );
                Print( "Error count = %u\n", count );
#endif // DEBUG_DEV_CMCTR

            u_int_2 uint_cmd[ 1000 ];
            memcpy( uint_cmd, data + 1 + 2, ( 2 * 4 ) * count );

            for ( u_int i = 0; i < count; i++ )
                {
#ifdef DEBUG_DEV_CMCTR
                u_int_2 object_type = uint_cmd[ 4 * i + 1 ];
                u_int_2 object_number = uint_cmd[ 4 * i +  2 ];
                u_int_2 object_alarm_number = uint_cmd[ 4 * i + 3 ];

                Print( "CMD_SET_PAC_ERROR_CMD" );
                Print( "cmd = %u, object_type = %u, object_number = %u, \
                       object_alarm_number = %u\n", uint_cmd[ 4 * i ],
                       object_type, object_number, object_alarm_number );
#endif // DEBUG_DEV_CMCTR

     //           G_DEV_ERRORS_MANAGER->set_cmd( uint_cmd[ 0 ], object_type,
    //                object_number, object_alarm_number );
                }

            const u_int_2 RES = 0;
            memcpy( outdata, &RES, sizeof( RES ) ); // Возвращаем 0.
            answer_size += 2;

            return answer_size;
        }

    return answer_size;
    }                  
//-----------------------------------------------------------------------------
int device_communicator::add_device( i_Lua_save_device *device )
    {
    dev.push_back( device );

    int i = dev.size();
    
    if ( i == 43 )
        {
        i = 43;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void device_communicator::print() const
    {  
    char tmp_str[ 200 ];

    snprintf( tmp_str, sizeof( tmp_str ),
            "\nDevice communicator. Dev count = %d.", ( int ) dev.size() );
    print_str( tmp_str, 1 );

    if ( !dev.size() ) return;

    for ( unsigned int i = 0; i < dev.size(); i++ ) 
        {
        sprintf( tmp_str, "[ %3d ] ", i );
        print_str( tmp_str, 0 );

        dev[ i ]->print();
        Print( "\n" );
        } 
    sprintf( tmp_str, "Done.\n" );
    print_str( tmp_str, 1 );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
