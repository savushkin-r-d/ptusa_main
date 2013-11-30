#include <unistd.h>
#include <string.h>

#include "wago_w750.h"

static volatile T_PabVarUnion * pstPabIN =(volatile T_PabVarUnion*) 0xFFE060;
static volatile T_PabVarUnion * pstPabOUT = (volatile T_PabVarUnion*)(0xFFE060+sizeof(T_PabVarUnion));

//-----------------------------------------------------------------------------
wago_manager_w750::wago_manager_w750()
    {
    KbusOpen();
    KbusUpdate();
    }
//-----------------------------------------------------------------------------
wago_manager_w750::~wago_manager_w750()
    {
    KbusClose();
    }
//-----------------------------------------------------------------------------
int wago_manager_w750::read_inputs()
    {
    if ( 0 == nodes_count ) return 0;


    for ( u_int i = 0; i < nodes_count; i++ )
        {
        if ( nodes[ i ]->type == wago_node::T_750_860 ||
                nodes[ i ]->type == wago_node::T_750_863 ) // KBus
            {
            KbusUpdate();

            // DI
            int start_pos = nodes[ i ]->AI_size;//KbusGetBinaryInputOffset();

#ifdef DEBUG_KBUS
            Print( "read_inputs() start_pos = %d\n", start_pos );
#endif // DEBUG_KBUS

            for ( u_int j = 0; j < nodes[ i ]->DI_cnt; j++ )
                {
                char tmp =  1 << ( j % 8 );

                nodes[ i ]->DI[ j ] = ( ( tmp &
                    pstPabIN->uc.Pab[ start_pos + j / 8 ] ) > 0 );
#ifdef DEBUG_KBUS
                printf( "%d -> %d, ", j, nodes[ i ]->DI[ j ] );
#endif // DEBUG_KBUS
                }
#ifdef DEBUG_KBUS
            printf( "\n" );
#endif // DEBUG_KBUS

            // AI
            for ( u_int j = 0; j < nodes[ i ]->AI_cnt; j++ )
                {
                u_int val = 0;
                u_int offset = nodes[ i ]->AI_offsets[ j ];

                switch ( nodes[ i ]->AI_types[ j ] )
                    {
                    case 466:
                    case 461:
                        val = pstPabIN->uc.Pab[ offset ] +
                            256 * pstPabIN->uc.Pab[ offset + 1 ];
                        break;

                    case 638:
                        val = pstPabIN->uc.Pab[ offset + 2 ] +
                            256 * pstPabIN->uc.Pab[ offset + 3 ];
                        break;
                    }
                nodes[ i ]->AI[ j ] = val;
#ifdef DEBUG_KBUS
                Print( "%d -> %u, ", j, nodes[ i ]->AI[ j ] );
#endif // DEBUG_KBUS
                }
#ifdef DEBUG_KBUS
            printf( "\n" );
#endif // DEBUG_KBUS

            }// if ( nodes[ i ]->type == wago_node::T_750_860 ) // KBus

        if ( nodes[ i ]->type == wago_node::T_750_341 || // Ethernet Wago nodes.
                nodes[ i ]->type == wago_node::T_750_841 )
            {

            if ( !nodes[ i ]->is_active )
                {
                continue;
                }

            if ( nodes[ i ]->DI_cnt > 0 )
                {
                /// @todo Модернизировать заполнение заголовка.
//                snprintf( ( char* ) buff, sizeof( buff ), "ss%c%c%c%c",
//                        0, 0, 0, 6 );

                buff[ 0  ] = 's';
                buff[ 1  ] = 's';
                buff[ 2  ] = 0;
                buff[ 3  ] = 0;
                buff[ 4  ] = 0;
                buff[ 5  ] = 6;
                buff[ 6  ] = 1; //nodes[ i ]->number;
                buff[ 7  ] = 0x02;
                buff[ 8  ] = 0;
                buff[ 9  ] = 0;
                buff[ 10 ] = ( unsigned char ) nodes[ i ]->DI_cnt >> 8;
                buff[ 11 ] = ( unsigned char ) nodes[ i ]->DI_cnt & 0xFF;

                u_int bytes_cnt = nodes[ i ]->DI_cnt / 8 +
                    ( nodes[ i ]->DI_cnt % 8 > 0 ? 1 : 0 );

                 if ( e_communicate( nodes[ i ], 12, bytes_cnt + 9 ) == 0 )
                     {
                     if ( buff[ 7 ] == 0x02 && buff[ 8 ] == bytes_cnt )
                         {
                         for ( u_int j = 0, idx = 0; j < bytes_cnt; j++ )
                             {
                             for ( int k = 0; k < 8; k++ )
                                 {
                                 if ( idx < nodes[ i ]->DI_cnt )
                                     {
                                     nodes[ i ]->DI[ idx ] =
                                         ( buff[ j + 9 ] >> k ) & 1;
#ifdef DEBUG_KBUS
                printf( "%d -> %d, ", idx, nodes[ i ]->DI[ idx ] );
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
                         }
                     }// if ( e_communicate( nodes[ i ], 12, bytes_cnt + 9 ) == 0 )
                 }// if ( nodes[ i ]->DI_cnt > 0 )

            if ( nodes[ i ]->AI_cnt > 0 )
                {
                /// @todo Модернизировать заполнение заголовка.
//                snprintf( ( char* ) buff, sizeof( buff ), "ss%c%c%c%c",
//                        0, 0, 0, 6 );

                buff[ 0  ] = 's';
                buff[ 1  ] = 's';
                buff[ 2  ] = 0;
                buff[ 3  ] = 0;
                buff[ 4  ] = 0;
                buff[ 5  ] = 6;
                buff[ 6  ] = nodes[ i ]->number;
                buff[ 7  ] = 0x04;
                buff[ 8  ] = 0;
                buff[ 9  ] = 0;

                u_int bytes_cnt = nodes[ i ]->AI_size;

                buff[ 10 ] = ( unsigned char ) bytes_cnt / 2 >> 8;
                buff[ 11 ] = ( unsigned char ) bytes_cnt / 2 & 0xFF;

                if ( e_communicate( nodes[ i ], 12, bytes_cnt + 9 ) == 0 )
                     {
                     if ( buff[ 7 ] == 0x04 && buff[ 8 ] == bytes_cnt )
                         {
                         memcpy( nodes[ i ]->AI, buff + 9, bytes_cnt );

                         int idx = 0;
                         for ( unsigned int l = 0; l < bytes_cnt; l += 2 )
                             {
                             nodes[ i ]->AI[ idx ] = 256 * buff[ 9 + l ] + buff[ 9 + l + 1 ];
                             idx++;
                             }
                         }
                     else
                         {
#ifdef DEBUG
                         Print("\nRead AI:Wago returned error. Node %d.\n",
                               nodes[ i ]->number );
                         Print( "bytes_cnt = %d, %d %d \n",
                               ( int ) buff[ 7 ], ( int ) buff[ 8 ], bytes_cnt );
#endif // DEBUG
                         }
                     }
                 }// if ( nodes[ i ]->AI_cnt > 0 )

            }// if ( nodes[ i ]->type == wago_node::T_750_341 || ...
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_manager_w750::write_outputs()
    {
    if ( 0 == nodes_count ) return 0;

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        if ( nodes[ i ]->type == wago_node::T_750_860 ||
                nodes[ i ]->type == wago_node::T_750_863 ) // KBus
            {
            // DO
            int start_pos = nodes[ i ]->AO_size;//KbusGetBinaryOutputOffset();

#ifdef DEBUG_KBUS
            Print( "write_outputs() start_pos = %d\n", start_pos );
#endif // DEBUG_KBUS
            for ( u_int j = 0; j < nodes[ i ]->DO_cnt; j++ )
                {
                    char tmp =  1 << ( j % 8 );
                    if ( nodes[ i ]->DO_[ j ] )
                        {
                        pstPabOUT->uc.Pab[ start_pos + j / 8 ] |= tmp;
                        }
                    else
                        {
                        pstPabOUT->uc.Pab[ start_pos + j / 8 ] &= ~tmp;
                        }
                    nodes[ i ]->DO[ j ] = nodes[ i ]->DO_[ j ];
#ifdef DEBUG_KBUS
                Print( "%d -> %d, ", j, nodes[ i ]->DO_[ j ] );
#endif // DEBUG_KBUS
                }
#ifdef DEBUG_KBUS
            Print( "\n" );
#endif // DEBUG_KBUS

            // AO
            for ( u_int j = 0; j < nodes[ i ]->AO_cnt; j++ )
                {
                int val = nodes[ i ]->AO_[ j ];
                u_int offset = nodes[ i ]->AO_offsets[ j ];

                pstPabIN->uc.Pab[ offset ] = val & 0xFF;
                pstPabIN->uc.Pab[ offset + 1 ] = val >> 8;

                nodes[ i ]->AO[ j ] = nodes[ i ]->AO_[ j ];
#ifdef DEBUG_KBUS
                Print( "%d -> %u\n, ", j, nodes[ i ]->AO_[ j ] );
#endif // DEBUG_KBUS
                }

            KbusUpdate();
            }// if ( nodes[ i ]->type == wago_node::T_750_860 ) // KBus

        if ( nodes[ i ]->type == wago_node::T_750_341 ||
                nodes[ i ]->type == wago_node::T_750_841 )
            {
            if ( !nodes[ i ]->is_active )
                {
                continue;
                }

            if ( nodes[ i ]->DO_cnt > 0 )
                {
                /// @todo Модернизировать заполнение заголовка.

                u_int bytes_cnt = nodes[ i ]->DO_cnt / 8 +
                    ( nodes[ i ]->DO_cnt % 8 > 0 ? 1 : 0 );

                buff[ 0  ] = 's';
                buff[ 1  ] = 's';
                buff[ 2  ] = 0;
                buff[ 3  ] = 0;
                buff[ 4  ] = 0;
                buff[ 5  ] = 7 + bytes_cnt;
                buff[ 6  ] = nodes[ i ]->number;
                buff[ 7  ] = 0x0F;
                buff[ 8  ] = 0;
                buff[ 9  ] = 0;
                buff[ 10 ] = ( unsigned char ) nodes[ i ]->DO_cnt >> 8;
                buff[ 11 ] = ( unsigned char ) nodes[ i ]->DO_cnt & 0xFF;
                buff[ 12 ] = bytes_cnt;

                for ( u_int j = 0, idx = 0; j < bytes_cnt; j++ )
                    {
                    u_char b = 0;
                    for ( u_int k = 0; k < 8; k++ )
                        {
                        if ( idx < nodes[ i ]->DO_cnt )
                            {
                            b = b | ( nodes[ i ]->DO_[ idx ] & 1 ) << k;
                            idx++;
                            }
                        }
                    buff[ j + 13 ] = b;
                    }
                buff[ 12 ] = bytes_cnt;

                if ( e_communicate( nodes[ i ], bytes_cnt + 13, 12 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x0F )
                        {
                        memcpy( nodes[ i ]->DO, nodes[ i ]->DO_, nodes[ i ]->DO_cnt );
                        }
                     }// if ( e_communicate( nodes[ i ], bytes_cnt + 13, 12 ) > 0 )
                 else
                     {
#ifdef DEBUG
                     //Print("\nWrite DO:Wago returned error...\n");
#endif // DEBUG
                     }
                 }// if ( nodes[ i ]->DO_cnt > 0 )

            if ( nodes[ i ]->AO_cnt > 0 )
                {
                u_int bytes_cnt = nodes[ i ]->AO_size;

                /// @todo Модернизировать заполнение заголовка.
                /// @todo Необходимо тестирование.
                buff[ 0  ] = 's';
                buff[ 1  ] = 's';
                buff[ 2  ] = 0;
                buff[ 3  ] = 0;
                buff[ 4  ] = 0;
                buff[ 5  ] = 7 + bytes_cnt;
                buff[ 6  ] = nodes[ i ]->number;
                buff[ 7  ] = 0x10;
                buff[ 8  ] = 0;
                buff[ 9  ] = 0;
                buff[ 10 ] = bytes_cnt / 2 >> 8;
                buff[ 11 ] = bytes_cnt / 2 & 0xFF;
                buff[ 12 ] = bytes_cnt;
                memcpy( buff + 13, nodes[ i ]->AO_, nodes[ i ]->AO_size );

                if ( e_communicate( nodes[ i ], bytes_cnt + 13, 12 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x10 )
                        {
                        memcpy( nodes[ i ]->AO, nodes[ i ]->AO_,
                            nodes[ i ]->AO_size );
                        }
                     }// if ( e_communicate( nodes[ i ], 2 * bytes_cnt + 13, 12 ) == 0 )
                 else
                     {
#ifdef DEBUG
                     //Print("\nWrite AO:Wago returned error...\n");
#endif // DEBUG
                     }
                }// if ( nodes[ i ]->AO_cnt > 0 )

            }// if ( nodes[ i ]->type == wago_node::T_750_341 || ...
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
