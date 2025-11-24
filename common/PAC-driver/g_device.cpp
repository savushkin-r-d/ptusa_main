#include <cstring>
#include <cstdio>

#include "g_device.h"

#include "device/device.h"

#include "PAC_err.h"
#include "g_errors.h"

#include "lua_manager.h"
#include "tech_def.h"
#include "params_recipe_manager.h"

char device_communicator::buff[ tcp_communicator::BUFSIZE ];

auto_smart_ptr < device_communicator > device_communicator::instance;

const u_int_2 G_CURRENT_PROTOCOL_VERSION = 104;

std::vector< i_Lua_save_device* > device_communicator::dev;

bool device_communicator::use_compression = true;
//-----------------------------------------------------------------------------
void print_str( const char *err_str, char is_need_CR )
    {
#ifdef DRIVER
    bug_log::add_msg( "System", "", err_str );
#else
    printf( "%s", err_str  );
    if ( is_need_CR )
        {
        printf( "\n" );
        }
#endif // DRIVER
    }
//-----------------------------------------------------------------------------
long device_communicator::write_devices_states_service(
    long len, u_char *data, u_char *outdata )
    {
    if ( len < 1 )
        {
        return 0;
        }

    u_int answer_size = 0;

#ifdef DEBUG_DEV_CMCTR
    u_long start_time = get_millisec();
#endif // DEBUG_DEV_CMCTR

    u_int param_size = 0;
    static u_int_2 g_devices_request_id = 0;

    switch ( data[ 0 ] )
        {
        case CMD_GET_INFO_ON_CONNECT:
            answer_size = sprintf( ( char* ) outdata,
                "protocol_version = %d; PAC_name = \"%s\"; is_reset_params = %d;"
                "params_CRC=%d;\n",
                G_CURRENT_PROTOCOL_VERSION,
                tcp_communicator::get_instance()->get_host_name_rus(),
                params_manager::get_instance()->par[ 0 ][ params_manager::P_IS_RESET_PARAMS ],
                params_manager::get_instance()->solve_CRC() );

            if ( G_DEBUG )
                {
                printf( "G_CURRENT_PROTOCOL_VERSION = %u, host =[%s]\n",
                    G_CURRENT_PROTOCOL_VERSION,
                    tcp_communicator::get_instance()->get_host_name_rus() );
                }
            answer_size++; // Учитываем завершающий \0.
            break;

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
            outdata[ answer_size++ ] = '\0'; // Учитываем завершающий \0.

#ifdef DEBUG_DEV_CMCTR
            if ( answer_size < 40000 ) //Вывод больших строк тормозит работу.
                {
                std::string source = ( char* ) outdata + 2;
                for ( u_int i = 0; i < source.length(); i++ )
                    {
                    if ( source[ i ] == '\t' )
                        {
                        source[ i ] = ' ';
                        }
                    }

                printf( "%s", source.c_str() );
                }

            printf( "Devices size = %u, g_devices_request_id = %u\n",
                answer_size,
                g_devices_request_id );

            printf( "Operation time = %lu\n", get_delta_millisec( start_time ) );
#endif // DEBUG_DEV_CMCTR
            break;
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
            outdata[ answer_size++ ] = '\0'; // Учитываем завершающий \0.

#ifdef DEBUG_DEV_CMCTR
            //printf( "%s", outdata + 2 );
#endif // DEBUG_DEV_CMCTR

#ifdef DEBUG_DEV_CMCTR
            printf( "Devices states size = %u, g_devices_request_id = %d\n",
                answer_size, g_devices_request_id );

            printf( "Operation time = %lu\n", get_delta_millisec( start_time ) );
#endif // DEBUG_DEV_CMCTR
            break;
            }

        case CMD_EXEC_DEVICE_COMMAND:
            {
#ifdef DEBUG_DEV_CMCTR
            printf( "\nEXEC_DEVICE_CMD\n" );
            printf( "cmd = %s\n",  data + 1 );
#endif // DEBUG_DEV_CMCTR
            int res;
            auto str = reinterpret_cast<char*>( data + 1 );
            if ( strstr( str, "__RECMAN" ) != nullptr )
                {
                res = G_PARAMS_RECIPE_MANAGER()->parseDriverCmd( str );
                }
            else
                {
                res = lua_manager::get_instance()->exec_Lua_str( 
                    str, "CMD_EXEC_DEVICE_COMMAND " );
                }

            outdata[ 0 ] = 0;
            outdata[ 1 ] = 0; //Возвращаем 0.
            if ( res )
                {
                outdata[ 0 ] = 1;
                }

#ifdef DEBUG_DEV_CMCTR
            printf( "Operation time = %lu\n", get_delta_millisec( start_time ) );
#endif // DEBUG_DEV_CMCTR

            answer_size = 2;
            break;
            }

        case CMD_GET_PAC_ERRORS:
            {
#ifdef DEBUG_DEV_CMCTR
            printf( "CMD_GET_PAC_ERRORS\n" );
#endif
            static u_int_2 errors_id = get_millisec() % 100;

            unsigned char project_descr_id = data[ 1 ];
            char *str = ( char* ) outdata;
            str[ 0 ] = 0;

            answer_size = sprintf( str, "alarms[ %d ] = \n  {}",
                project_descr_id );
            answer_size += sprintf( str + answer_size, "alarms[ %d ] = \n  {",
                project_descr_id );

            u_int_2         err_id = 0;
            static u_int_2  prev_PAC_err_id = 0;
            static u_int_2  prev_dev_err_id = 0;

            int err_size =
                PAC_critical_errors_manager::get_instance()->save_as_Lua_str(
                str + answer_size, err_id );
            if ( err_id != prev_PAC_err_id )
                {
                prev_PAC_err_id = err_id;
                errors_id++;
                }

            static u_long start_time = get_millisec();
            answer_size += err_size;
            if ( err_size == 0 &&                   //Нет критических ошибок.
                get_delta_millisec( start_time ) > 5000 )
                {
                answer_size +=
                    G_ERRORS_MANAGER->save_as_Lua_str( str + answer_size, err_id );
                if ( err_id != prev_dev_err_id )
                    {
                    prev_dev_err_id = err_id;
                    errors_id++;
                    }
                }

            answer_size += sprintf( str + answer_size, "  %s %d,\n", "id =", errors_id );
            answer_size += sprintf( str + answer_size, "  %s\n", "}" );

#ifdef DEBUG_DEV_CMCTR
            printf( "Critical errors = \n%s", outdata );
#endif // DEBUG_DEV_CMCTR

            str[ answer_size++ ] = '\0'; // Учитываем завершающий \0.
            break;
            }

        case CMD_SET_PAC_ERROR_CMD:
            {
#ifdef DEBUG_DEV_CMCTR
            printf( "CMD_SET_PAC_ERROR_CMD\n" );
            printf( "cmd = %s\n",  data + 1 );
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
            printf( "Operation time = %lu\n", get_delta_millisec( start_time ) );
#endif // DEBUG_DEV_CMCTR

            answer_size = 2;
            break;
            }

        case CMD_GET_PARAMS:
            answer_size = params_manager::get_instance()->save_params_as_Lua_str(
                ( char* ) outdata );
            answer_size++; // Учитываем завершающий \0.
            break;

        case CMD_RESTORE_PARAMS:
            {
#ifdef DEBUG_DEV_CMCTR
            printf( "CMD_RESTORE_PARAMS\n" );
            printf( "cmd = %s\n",  data + 1 );
#endif // DEBUG_DEV_CMCTR

            int res = params_manager::get_instance(
                )->restore_params_from_server_backup( ( char*) data + 1 );

            outdata[ 0 ] = 0;
            outdata[ 1 ] = 0; //Возвращаем 0.
            if ( res )
                {
                outdata[ 0 ] = 1;
                }

#ifdef DEBUG_DEV_CMCTR
            printf( "Operation time = %lu\n", get_delta_millisec( start_time ) );
#endif // DEBUG_DEV_CMCTR

            answer_size = 2;
            break;
            }

        case CMD_GET_PARAMS_CRC:
            answer_size = sprintf( ( char* ) outdata, "params_CRC=%d; request_id=%d\n",
                params_manager::get_instance()->solve_CRC(),
                g_devices_request_id );
            answer_size++; // Учитываем завершающий \0.
            break;
        }


    // Skip compression for small data (compression overhead is not worth it)
    // and use faster compression level for better performance
    const u_int MIN_COMPRESSION_SIZE = 128;  // Minimum size to benefit from compression
    
    if ( answer_size > MIN_COMPRESSION_SIZE && use_compression )
        {
        unsigned long r = sizeof( buff );
        // Use Z_BEST_SPEED (1) instead of Z_DEFAULT_COMPRESSION (6) for better performance
        // while still providing reasonable compression for network transfer
        int res = compress2( (u_char*)buff, &r, outdata, answer_size, Z_BEST_SPEED );

        // Only use compressed data if it's actually smaller
        if ( res == Z_OK && r > 0 && r < answer_size )
            {
            memcpy( outdata, buff, r );
            answer_size = r;
            }
        else if ( res != Z_OK )
            {
            // Compression failed - return error indicator
            outdata[ 0 ] = 0;
            outdata[ 1 ] = 0; //Возвращаем 0.

            answer_size = 2;
            }
        // If compressed size >= original size, keep original data (no copy needed)
        }

    return answer_size;
    }
//-----------------------------------------------------------------------------
int device_communicator::add_device( i_Lua_save_device *device )
    {
    dev.push_back( device );

    return 0;
    }
//-----------------------------------------------------------------------------
void device_communicator::clear_devices()
    {
    dev.clear();
    }
//-----------------------------------------------------------------------------
void device_communicator::print() const
    {
    char tmp_str[ 200 ];

    sprintf( tmp_str,
        "Device communicator. Dev count = %d.", ( int ) dev.size() );
    print_str( tmp_str, 1 );

    if ( !dev.size() ) return;

    for ( unsigned int i = 0; i < dev.size(); i++ )
        {
        sprintf( tmp_str, "[ %3u ] ", i );
        print_str( tmp_str, 0 );

        print_str( dev[ i ]->get_name_in_Lua(), 1 );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
