#include <errno.h>

#include "l_bus_coupler_io.h"
#include "log.h"


extern int errno;
//-----------------------------------------------------------------------------
int io_manager_linux::net_init( io_node* node )
    {
    int type = SOCK_STREAM;
    int protocol = 0; /* всегда 0 */
    int err;
    int sock = err = socket( AF_INET, type, protocol ); // Cоздание сокета.

    if ( sock < 0 )
        {
        sprintf( G_LOG->msg,
            "Network communication : can't create I/O node socket : %s.",
            strerror( errno ) );
        G_LOG->write_log( i_log::P_CRIT );

        return -4;
        }

    // Адресация мастер-сокета.
    struct sockaddr_in socket_remote_server;
    const int PORT = 502;
    memset( &socket_remote_server, 0, sizeof( socket_remote_server ) );
    socket_remote_server.sin_family = AF_INET;
    socket_remote_server.sin_addr.s_addr = inet_addr( node->ip_address );
    socket_remote_server.sin_port = htons( PORT );

    const int C_ON = 1;

    if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &C_ON, sizeof( C_ON ) ) )
        {
        sprintf( G_LOG->msg,
            "Network communication : can't setsockopt I/O node socket : %s.",
            strerror( errno ) );
        G_LOG->write_log( i_log::P_CRIT );

        close( sock );
        return -5;
        }

    // Переводим в неблокирующий режим.
    err = fcntl( sock, F_SETFL, O_NONBLOCK );
    if ( err != 0 )
        {
        sprintf( G_LOG->msg,
            "Network communication : can't fcntl I/O node socket : %s.",
            strerror( errno ) );
        G_LOG->write_log( i_log::P_CRIT );

        close( sock );
        return -5;
        }

    // Привязка сокета. Сразу возвращает управление в неблокирующем режиме.
    err = connect( sock, ( struct sockaddr* ) & socket_remote_server,
        sizeof( socket_remote_server ) );

    fd_set rdevents;
    struct timeval tv;
    FD_ZERO( &rdevents );
    FD_SET( sock, &rdevents );

    tv.tv_sec = 0;
    tv.tv_usec = io_node::C_CNT_TIMEOUT_US;

    err = select( sock + 1, 0, &rdevents, 0, &tv );

    if ( err <= 0 )
        {
        if ( node->is_set_err == false )
            {
            if ( err < 0 )
                {
                sprintf( G_LOG->msg,
                    "Network device : s%d->\"%s\":\"%s\""
                    " can't connect : %s.",
                    sock, node->name, node->ip_address, strerror( errno ) );
                G_LOG->write_log( i_log::P_CRIT );
                }
            else // = 0
                {
                sprintf( G_LOG->msg,
                    "Network device : s%d->\"%s\":\"%s\""
                    " can't connect : timeout (%d ms).",
                    sock, node->name, node->ip_address,
                    io_node::C_CNT_TIMEOUT_US / 1000 );

                G_LOG->write_log( i_log::P_CRIT );
                }
            }

        close( sock );
        return -5;
        }

    if ( FD_ISSET( sock, &rdevents ) )
        {
        socklen_t err_len;
        int error;

        err_len = sizeof( error );
        if ( getsockopt( sock, SOL_SOCKET, SO_ERROR, &error, &err_len ) < 0 || error != 0 )
            {
            if ( node->is_set_err == false )
                {
                sprintf( G_LOG->msg,
                    "Network device : s%d->\"%s\":\"%s\""
                    " error during connect : %s.",
                    sock, node->name, node->ip_address, strerror( errno ) );
                G_LOG->write_log( i_log::P_CRIT );
                }

            close( sock );
            return -6;
            }
        }

    if ( G_DEBUG )
        {
        printf( "io_manager_linux:net_init() : socket %d is successfully"
            " connected to \"%s\":\"%s\":%d\n",
            sock, node->name, node->ip_address, PORT );
        }

    node->sock = sock;
    node->state = io_node::ST_OK;

    return 0;
    }
//-----------------------------------------------------------------------------
int io_manager_linux::write_outputs()
    {
    if ( 0 == nodes_count ) return 0;

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        io_node* nd = nodes[ i ];
        if ( nd->type == io_node::WAGO_750_XXX_ETHERNET )
            {
            if ( !nd->is_active )
                {
                continue;
                }

            if ( nd->DO_cnt > 0 )
                {
                u_int bytes_cnt = nd->DO_cnt / 8 + ( nd->DO_cnt % 8 > 0 ? 1 : 0 );

                buff[ 0 ] = 's';
                buff[ 1 ] = 's';
                buff[ 2 ] = 0;
                buff[ 3 ] = 0;
                buff[ 4 ] = 0;
                buff[ 5 ] = 7 + bytes_cnt;
                buff[ 6 ] = 0; //nodes[ i ]->number;
                buff[ 7 ] = 0x0F;
                buff[ 8 ] = 0;
                buff[ 9 ] = 0;
                buff[ 10 ] = (unsigned char)nd->DO_cnt >> 8;
                buff[ 11 ] = (unsigned char)nd->DO_cnt & 0xFF;
                buff[ 12 ] = bytes_cnt;

                for ( u_int j = 0, idx = 0; j < bytes_cnt; j++ )
                    {
                    u_char b = 0;
                    for ( u_int k = 0; k < 8; k++ )
                        {
                        if ( idx < nd->DO_cnt )
                            {
                            b = b | ( nd->DO_[ idx ] & 1 ) << k;
                            idx++;
                            }
                        }
                    buff[ j + 13 ] = b;
                    }

                if ( e_communicate( nd, bytes_cnt + 13, 12 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x0F )
                        {
                        memcpy( nd->DO, nd->DO_, nd->DO_cnt );
                        }
                    }// if ( e_communicate( nd, bytes_cnt + 13, 12 ) > 0 )
                else
                    {
                    if ( G_DEBUG )
                        {
                        //printf("\nWrite DO: returned error...\n");
                        }
                    }
                }// if ( nd->DO_cnt > 0 )

            if ( nd->AO_cnt > 0 )
                {
                u_int bytes_cnt = nd->AO_size;

                buff[ 0 ] = 's';
                buff[ 1 ] = 's';
                buff[ 2 ] = 0;
                buff[ 3 ] = 0;
                buff[ 4 ] = 0;
                buff[ 5 ] = 7 + bytes_cnt;
                buff[ 6 ] = 0; //nodes[ i ]->number;
                buff[ 7 ] = 0x10;
                buff[ 8 ] = 0;
                buff[ 9 ] = 0;
                buff[ 10 ] = bytes_cnt / 2 >> 8;
                buff[ 11 ] = bytes_cnt / 2 & 0xFF;
                buff[ 12 ] = bytes_cnt;

                for ( unsigned int idx = 0, l = 0; idx < nd->AO_cnt; idx++ )
                    {
                    switch ( nd->AO_types[ idx ] )
                        {
                        case 638:
                            buff[ 13 + l ] = 0;
                            buff[ 13 + l + 1 ] = 0;
                            buff[ 13 + l + 2 ] = 0;
                            buff[ 13 + l + 3 ] = 0;
                            l += 4;
                            break;

                        default:
                            buff[ 13 + l ] = (u_char)( ( nd->AO_[ idx ] >> 8 ) & 0xFF );
                            buff[ 13 + l + 1 ] = (u_char)( nd->AO_[ idx ] & 0xFF );
                            l += 2;
                            break;
                        }
                    }

                if ( e_communicate( nd, bytes_cnt + 13, 12 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x10 )
                        {
                        memcpy( nd->AO, nd->AO_, sizeof( nd->AO ) );
                        }
                    }// if ( e_communicate( nd, 2 * bytes_cnt + 13, 12 ) == 0 )
                else
                    {
                    if ( G_DEBUG )
                        {
                        //printf("\nWrite AO: returned error...\n");
                        }
                    }
                }// if ( nd->AO_cnt > 0 )

            }// if ( nd->type == io_node::T_750_341 || ...
        }// for ( u_int i = 0; i < nodes_count; i++ )

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        io_node* nd = nodes[ i ];
        if ( nd->type == io_node::PHOENIX_BK_ETH )
            {
            if ( !nd->is_active )
                {
                continue;
                }

            if ( nd->AO_cnt > 0 )
                {
                unsigned int start_register = 0;
                unsigned int start_write_address = PHOENIX_HOLDINGREGISTERS_STARTADDRESS;
                unsigned int registers_count;

                if (nd->AO_cnt > MAX_MODBUS_REGISTERS_PER_QUERY)
                    {
                    registers_count = MAX_MODBUS_REGISTERS_PER_QUERY;
                    }
                else
                    {
                    registers_count = nd->AO_cnt;
                    }

                int bit_src = 0;

                do 
                {
                    for (u_int j = 0; j < registers_count * 2; j++)
                        {
                        u_char b = 0;
                        for (u_int k = 0; k < 8; k++)
                            {
                            b = b | (nd->DO_[bit_src] & 1) << k;
                            bit_src++;
                            }
                        writebuff[j] = b;
                        }

                    u_int ao_module_type = 0;
                    u_int ao_module_offset = 0;

                    for (unsigned int idx = start_register, l = 0; idx < start_register + registers_count; idx++)
                        {
                        if (nd->AO_types[idx] != ao_module_type)
                            {
                            ao_module_type = nd->AO_types[idx];
                            ao_module_offset = 0;
                            }
                        else
                            {
                            ao_module_offset++;
                            }

                        switch (ao_module_type)
                            {
                            case 1027843:           //IOL8
                                ao_module_offset %= 32;	   //if there are same modules one after other on bus
                                if (ao_module_offset > 2)  //first 3 words (bytes 0-5) are reserved, 2nd byte is used for trigger discrete outputs.
                                    {
                                    memcpy(&writebuff[l], &nd->AO_[idx], 2);
                                    }
                                l += 2;
                                break;

                            case 2688093:			//CNT2 INC2
                                ao_module_offset %= 14;	   //if there are same modules one after other on bus
                                if (0 == ao_module_offset) //assign start command and positive increment for both counters
                                    {
                                    writebuff[l] = 0x5;
                                    writebuff[l + 1] = 0x5;
                                    }
                                else
                                    {
                                    writebuff[l] = 0;
                                    writebuff[l + 1] = 0;
                                    }
                                l += 2;
                                break;

                            case 2688527:   //-AXL F AO4 1H
                                writebuff[l] = (u_char)((nd->AO_[idx] >> 8) & 0xFF);
                                writebuff[l + 1] = (u_char)(nd->AO_[idx] & 0xFF);
                                l += 2;
                                break;

                            default:
                                l += 2;
                                break;
                            }
                        }

                    if (write_holding_registers(nd, start_write_address + start_register, registers_count) >= 0)
                        {
                        if (buff[7] == 0x10)
                            {
                            memcpy(&(nd->AO[start_register]), &(nd->AO_[start_register]), registers_count * 2);
                            memcpy(&(nd->DO[start_register * 16]), &(nd->DO_[start_register * 16]), registers_count * 16);
                            nd->flag_error_write_message = false;
                            }
                        else
                            {
                            if (!nd->flag_error_write_message)
                                {
                                G_LOG->error("Write AO: returned error %d", buff[7]);
                                nd->flag_error_write_message = true;
                                }
                            }
                        }
                    else
                        {
                        if (!nd->flag_error_write_message)
                            {
                            G_LOG->error("Write AO: returned error");
                            nd->flag_error_write_message = true;
                            }
                        }

                    start_register += registers_count;
                    registers_count = nd->AO_cnt - start_register;
                    if (registers_count > MAX_MODBUS_REGISTERS_PER_QUERY)
                        {
                        registers_count = MAX_MODBUS_REGISTERS_PER_QUERY;
                        }

                } 
                 while (start_register < nd->AO_cnt);


                }// if ( nd->AO_cnt > 0 )

            }// if ( nd->type == io_node::T_750_341 || ...
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return 0;
    }
//-----------------------------------------------------------------------------
int io_manager_linux::e_communicate( io_node* node, int bytes_to_send,
    int bytes_to_receive )
    {
    // Проверка связи с узлом I/O.
    if ( get_delta_millisec( node->last_poll_time ) > io_node::C_MAX_WAIT_TIME )
        {
        if ( false == node->is_set_err )
            {
            node->is_set_err = true;
            PAC_critical_errors_manager::get_instance()->set_global_error(
                PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_IO_COUPLER, node->number );
            }
        }
    else
        {
        if ( node->is_set_err )
            {
            node->is_set_err = false;
            PAC_critical_errors_manager::get_instance()->reset_global_error(
                PAC_critical_errors_manager::PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_IO_COUPLER, node->number );
            }
        }
    // Проверка связи с узлом I/O.-!>

    // Инициализация сетевого соединения, при необходимости.
    if ( node->state != io_node::ST_OK )
        {
        if ( get_delta_millisec( node->last_init_time ) < node->delay_time )
            {
            return 1;
            }

        net_init( node );
        if ( node->state != io_node::ST_OK )
            {
            node->last_init_time = get_millisec();
            if ( node->delay_time < io_node::C_MAX_DELAY )
                {
                node->delay_time += 10000 + node->number * 200;
                }
            return -100;
            }
        }
    // Инициализация сетевого соединения, при необходимости.-!>

    node->delay_time = 0;

    // Посылка данных.
    int res = tcp_communicator_linux::sendall( node->sock, buff,
        bytes_to_send, 0, io_node::C_RCV_TIMEOUT_US, node->ip_address,
        node->name, &node->send_stat );

    if ( res < 0 )
        {
        disconnect( node );
        return -101;
        }

    // Получение данных.
    res = tcp_communicator_linux::recvtimeout( node->sock, buff,
        bytes_to_receive, 0, io_node::C_RCV_TIMEOUT_US, node->ip_address,
        node->name, &node->recv_stat );

    if ( res <= 0 ) /* read error */
        {
        disconnect( node );
        return -102;
        }
    node->last_poll_time = get_millisec();

    return 0;
    }

int io_manager_linux::read_input_registers(io_node* node, unsigned int address, unsigned int quantity, unsigned char station /*= 0*/)
    {
    buff[0] = 's';
    buff[1] = 's';
    buff[2] = 0;
    buff[3] = 0;
    buff[4] = 0;
    buff[5] = 6;
    buff[6] = station;
    buff[7] = 0x04;
    buff[8] = (u_int_2)address >> 8;
    buff[9] = (u_int_2)address & 0xFF;
    buff[10] = (u_int_2)quantity >> 8;
    buff[11] = (u_int_2)quantity & 0xFF;
    unsigned int bytes_cnt = quantity * 2;
    if (e_communicate(node, 12, bytes_cnt + 9) == 0)
        {
        if (buff[7] == 0x04 && buff[8] == bytes_cnt)
            {
            resultbuff = &buff[9];
            return 1;
            }
        else
            {
            return 0;
            }
        }
    return -1;
    }

int io_manager_linux::write_holding_registers(io_node* node, unsigned int address, unsigned int quantity, unsigned char station)
    {
    unsigned int bytes_cnt = quantity * 2;
    buff[0] = 's';
    buff[1] = 's';
    buff[2] = 0;
    buff[3] = 0;
    buff[4] = 0;
    buff[5] = 7 + bytes_cnt;
    buff[6] = station;
    buff[7] = 0x10;
    buff[8] = (u_int_2)address >> 8;
    buff[9] = (u_int_2)address & 0xFF;
    buff[10] = (u_int_2)quantity >> 8;
    buff[11] = (u_int_2)quantity & 0xFF;
    buff[12] = bytes_cnt;
    if (e_communicate(node, bytes_cnt + 13, 12) == 0)
        {
        if (buff[7] == 0x10)
            {
            return 1;
            }
        else
            {
            return 0;
            }
        }
    return -1;
    }

//-----------------------------------------------------------------------------
int io_manager_linux::read_inputs()
    {
    if ( 0 == nodes_count ) return 0;

    for (u_int i = 0; i < nodes_count; i++ )
        {
        io_node* nd = nodes[ i ];

        if ( nd->type == io_node::WAGO_750_XXX_ETHERNET ) // Ethernet I/O nodes.
            {
            if ( !nd->is_active )
                {
                continue;
                }

            if ( nd->DI_cnt > 0 )
                {
                buff[ 0 ] = 's';
                buff[ 1 ] = 's';
                buff[ 2 ] = 0;
                buff[ 3 ] = 0;
                buff[ 4 ] = 0;
                buff[ 5 ] = 6;
                buff[ 6 ] = 0; //nd->number;
                buff[ 7 ] = 0x02;
                buff[ 8 ] = 0;
                buff[ 9 ] = 0;
                buff[ 10 ] = (unsigned char)nd->DI_cnt >> 8;
                buff[ 11 ] = (unsigned char)nd->DI_cnt & 0xFF;

                u_int bytes_cnt = nd->DI_cnt / 8 + ( nd->DI_cnt % 8 > 0 ? 1 : 0 );

                if ( e_communicate( nd, 12, bytes_cnt + 9 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x02 && buff[ 8 ] == bytes_cnt )
                        {
                        for ( u_int j = 0, idx = 0; j < bytes_cnt; j++ )
                            {
                            for ( int k = 0; k < 8; k++ )
                                {
                                if ( idx < nd->DI_cnt )
                                    {
                                    nd->DI[ idx ] = ( buff[ j + 9 ] >> k ) & 1;
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
                        sprintf( G_LOG->msg, "Read DI:bus coupler returned error. Node %d)",
                            nd->number );
                        G_LOG->write_log( i_log::P_ERR );

                        if ( G_DEBUG )
                            {
                            //printf("\nRead DI:I/O returned error...\n");
                            }
                        }
                    }// if ( e_communicate( nd, 12, bytes_cnt + 9 ) == 0 )
                }// if ( nd->DI_cnt > 0 )

            if ( nd->AI_cnt > 0 )
                {
                buff[ 0 ] = 's';
                buff[ 1 ] = 's';
                buff[ 2 ] = 0;
                buff[ 3 ] = 0;
                buff[ 4 ] = 0;
                buff[ 5 ] = 6;
                buff[ 6 ] = 0; //nd->number;
                buff[ 7 ] = 0x04;
                buff[ 8 ] = 0;
                buff[ 9 ] = 0;

                u_int bytes_cnt = nd->AI_size;

                buff[ 10 ] = (unsigned char)bytes_cnt / 2 >> 8;
                buff[ 11 ] = (unsigned char)bytes_cnt / 2 & 0xFF;

                if ( e_communicate( nd, 12, bytes_cnt + 9 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x04 && buff[ 8 ] == bytes_cnt )
                        {
                        int idx = 0;
                        for ( unsigned int l = 0; l < nd->AI_cnt; l++ )
                            {
                            switch ( nd->AI_types[ l ] )
                                {
                                case 638:
                                    nd->AI[ l ] = 256 * buff[ 9 + idx + 2 ] +
                                        buff[ 9 + idx + 3 ];
                                    idx += 4;
                                    break;

                                default:
                                    nd->AI[ l ] = 256 * buff[ 9 + idx ] +
                                        buff[ 9 + idx + 1 ];
                                    idx += 2;
                                    break;
                                }
                            }
                        }
                    else
                        {
                        sprintf( G_LOG->msg, "Read AI:bus coupler returned error. Node %d (bytes_cnt = %d, %d %d )",
                            nd->number, (int)buff[ 7 ], (int)buff[ 8 ], bytes_cnt );
                        G_LOG->write_log( i_log::P_ERR );
                        }
                    }
                }// if ( nd->AI_cnt > 0 )

            }// if ( nd->type == io_node::T_750_341 || ...
        }// for ( u_int i = 0; i < nodes_count; i++ )

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        io_node* nd = nodes[ i ];

        if ( nd->type == io_node::PHOENIX_BK_ETH ) // Ethernet I/O nodes.
            {
            if ( !nd->is_active )
                {
                continue;
                }

            if (nd->AI_cnt > 0)
                {
                unsigned int start_register = 0;
                unsigned int start_read_address = PHOENIX_INPUTREGISTERS_STARTADDRESS;
                unsigned int registers_count;

                if (nd->AI_cnt > MAX_MODBUS_REGISTERS_PER_QUERY)
                    {
                    registers_count = MAX_MODBUS_REGISTERS_PER_QUERY;
                    }
                else
                    {
                    registers_count = nd->AI_cnt;
                    }

                int res, k, index_source = 0, analog_dest = 0, bit_dest = 0;

                do
                    {
#ifdef DEBUG_BK_MIN
                    G_LOG->warning("Read %d node registers from %d", registers_count, start_read_address + start_register);
#endif // DEBUG_BK_MIN
                    res = read_input_registers(nd, start_read_address + start_register, registers_count);

#ifdef TEST_NODE_IO
                    printf("\n\r");
                    for (int ideb = 0; ideb < registers_count; ideb++)
                        {
                        printf("%d = %d,", start_read_address + start_register + ideb, 256 * resultbuff[ideb * 2] + resultbuff[ideb * 2 + 1]);
                        }
#endif
                    

                    if (res >= 0)
                        {
                        if (res)
                            {
                            for (index_source = 0; analog_dest < start_register + registers_count; analog_dest++)
                                {
                                switch (nd->AI_types[analog_dest])
                                    {
                                    case 1027843:                               //IOL8
                                        memcpy(&nd->AI[analog_dest], resultbuff + index_source, 2);
                                        index_source += 2;
                                        break;

                                    default:
                                        nd->AI[analog_dest] = 256 * resultbuff[index_source] + resultbuff[index_source + 1];
                                        index_source += 2;
                                        break;
                                    }
#ifdef DEBUG_BK
                                G_LOG->warning("%d %u", analog_dest, nd->AI[analog_dest]);
#endif // DEBUG_BK
                                }

                            for (index_source = 0; bit_dest < (start_register + registers_count) * 2 * 8; index_source++)
                                {
                                for (k = 0; k < 8; k++)
                                    {
                                    nd->DI[bit_dest] = (resultbuff[index_source] >> k) & 1;
#ifdef DEBUG_BK
                                    G_LOG->notice("%d %d", bit_dest, (resultbuff[index_source] >> k) & 1);
#endif // DEBUG_BK
                                    bit_dest++;
                                    }
                                }
                            }
                        else
                            {
                            G_LOG->error("Read AI:bus coupler returned error. Node %d (bytes_cnt = %d, %d %d )",
                                nd->number, (int)buff[7], (int)buff[8], registers_count * 2);
                            break;
                            }
                        }
                    else
                        {
                        //node doesn't respond
                        break;
                        }
                    start_register += registers_count;
                    registers_count = nd->AI_cnt - start_register;
                    if (registers_count > MAX_MODBUS_REGISTERS_PER_QUERY)
                        {
                        registers_count = MAX_MODBUS_REGISTERS_PER_QUERY;
                        }
                    } 
                while (start_register < nd->AI_cnt);
                }

            }// nd->type == io_node::PHOENIX_BK_ETH
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return 0;
    }
//-----------------------------------------------------------------------------
void io_manager_linux::disconnect( io_node* node )
    {
    if ( node->sock )
        {
        shutdown( node->sock, SHUT_RDWR );
        close( node->sock );
        node->sock = 0;
        }
    node->state = io_node::ST_NO_CONNECT;
    }
//-----------------------------------------------------------------------------
io_manager_linux::io_manager_linux()
    {
    writebuff = &buff[13];
    }
//-----------------------------------------------------------------------------
io_manager_linux::~io_manager_linux()
    {
    }
//-----------------------------------------------------------------------------
