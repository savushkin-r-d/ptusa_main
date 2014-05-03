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
        wago_node *nd = nodes[ i ];
        if ( nd->type == wago_node::T_750_86x ) // KBus
            {
            static u_long start_time = get_millisec();
            if ( get_delta_millisec( start_time ) > 200 )
                {
                start_time = get_millisec();

                // DI
                int start_pos = nd->AI_size;//KbusGetBinaryInputOffset();

#ifdef DEBUG_KBUS
                Print( "read_inputs() start_pos = %d\n", start_pos );
#endif // DEBUG_KBUS

                for ( u_int j = 0; j < nd->DI_cnt; j++ )
                    {
                    char tmp =  1 << ( j % 8 );

                    nd->DI[ j ] = ( ( tmp &
                        pstPabIN->uc.Pab[ start_pos + j / 8 ] ) > 0 );
#ifdef DEBUG_KBUS
                    printf( "%2d -> %d, ", j, nd->DI[ j ] );
                    if ( ( j + 1 ) % 8 == 0 )
                        {
                        Print( "\n" );
                        }
#endif // DEBUG_KBUS
                    }
#ifdef DEBUG_KBUS
                printf( "\n" );
#endif // DEBUG_KBUS

#ifdef DEBUG_KBUS
                for ( u_int j = 0; j < 250; j++ )
                    {
                    char tmp =  1 << ( j % 8 );
                    int v = ( ( tmp & pstPabIN->uc.Pab[ j / 8 ] ) > 0 );

                    printf( "%2d -> %d, ", j, v );
                    if ( ( j + 1 ) % 8 == 0 )
                        {
                        printf( "\n" );
                        }
                    }
                printf( "\n" );
#endif // DEBUG_KBUS

                // AI
                u_int idx = 0;
                bool is_first_655 = true;

                for ( u_int j = 0; j < nd->AI_cnt; j++ )
                    {
                    u_int offset = nd->AI_offsets[ j ];
                    u_int val = pstPabIN->uc.Pab[ offset ] +
                        256 * pstPabIN->uc.Pab[ offset + 1 ];

                    if ( nd->AI_types[ j ] != 655 )
                        {
                        // ѕо€вилс€ другой модуль, значит следующий 655 будет
                        // первым.
                        is_first_655 = true;
                        }

                    switch ( nd->AI_types[ j ] )
                        {
                    case 466:
                    case 461:
                        val = pstPabIN->uc.Pab[ offset ] +
                            256 * pstPabIN->uc.Pab[ offset + 1 ];
                        nd->AI[ idx++ ] = val;
                        break;

                    case 638:
                        val = pstPabIN->uc.Pab[ offset + 2 ] +
                            256 * pstPabIN->uc.Pab[ offset + 3 ];
                        nd->AI[ idx++ ] = val;
                        break;

                    case 655:
                        if ( !is_first_655 ) //„итаем по первому каналу все 20 слов!
                            {
                            break;
                            }

                        char* data = ( char* ) ( nd->AI + idx );
#ifdef DEBUG_ASI
                        printf( "750-655 AI, idx = %d\n", idx );
#endif // DEBUG_ASI
                        for ( int l = 0; l < 40; l++ )
                            {
                            data[ l ] = pstPabIN->uc.Pab[ offset + l ];
#ifdef DEBUG_ASI
                            printf( "%d -> %d, ", l, data[ l ] );
#endif // DEBUG_ASI
                            }
#ifdef DEBUG_ASI
                            printf( "\n" );
#endif // DEBUG_ASI
                        is_first_655 = false;
                        idx += 20;
                        break;

                    default:
                        nd->AI[ idx++ ] = val;
                        break;
                        }

#ifdef DEBUG_KBUS
                    printf( "%d -> %u, ", idx, nd->AI[ idx ] );
#endif // DEBUG_KBUS
                    }
#ifdef DEBUG_KBUS
                printf( "\n" );
#endif // DEBUG_KBUS
                }
            }// if ( nd->type == wago_node::T_750_86x ) // KBus
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return wago_manager_linux::read_inputs();
    }
//-----------------------------------------------------------------------------
int wago_manager_w750::write_outputs()
    {
    if ( 0 == nodes_count ) return 0;

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        wago_node *nd = nodes[ i ];
        if ( nd->type == wago_node::T_750_86x ) // KBus
            {
            static u_long start_time = get_millisec();
            if ( get_delta_millisec( start_time ) > 200 )
                {
                start_time = get_millisec();

                // DO
                int start_pos = nd->AO_size;//KbusGetBinaryOutputOffset();

#ifdef DEBUG_KBUS
                Print( "write_outputs() start_pos = %d\n", start_pos );
#endif // DEBUG_KBUS
                for ( u_int j = 0; j < nd->DO_cnt; j++ )
                    {
                    char tmp =  1 << ( j % 8 );
                    if ( nd->DO_[ j ] )
                        {
                        pstPabOUT->uc.Pab[ start_pos + j / 8 ] |= tmp;
                        }
                    else
                        {
                        pstPabOUT->uc.Pab[ start_pos + j / 8 ] &= ~tmp;
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
                int in_idx  = 0;
                bool is_first_655 = true;

                for ( u_int j = 0; j < nd->AO_cnt; j++ )
                    {
                    u_int offset = nd->AO_offsets[ j ];

                    if ( nd->AO_types[ j ] != 655 )
                        {
                        // ѕо€вилс€ другой модуль, значит следующий 655 будет
                        // первым.
                        is_first_655 = true;
                        }

                    switch ( nd->AO_types[ j ] )
                        {
                        case 638:
                            pstPabOUT->uc.Pab[ offset     ] = 0;
                            pstPabOUT->uc.Pab[ offset + 1 ] = 0;
                            pstPabOUT->uc.Pab[ offset + 2 ] = 0;
                            pstPabOUT->uc.Pab[ offset + 3 ] = 0;

                            in_idx++;
                            break;

                        case 655:
                            {
                            if ( !is_first_655 ) //ѕишем по первому каналу все 20 слов!
                                {
                                break;
                                }

                            char* data = ( char* ) ( nd->AO_ + in_idx );
#ifdef DEBUG_ASI
                            printf( "750-655, idx = %d\n", in_idx );
#endif // DEBUG_ASI
                            for ( int l = 0; l < 40; l++ )
                                {
                                pstPabOUT->uc.Pab[ offset + l ] = data[ l ];
#ifdef DEBUG_ASI
                                printf( "%d -> %d, ", l, data[ l ] );
#endif // DEBUG_ASI
                                }
#ifdef DEBUG_ASI
                            printf( "\n" );
#endif // DEBUG_ASI
                            in_idx += 20;
                            is_first_655 = false;
                            break;
                            }

                        default:
                            {
                            int val = nd->AO_[ in_idx ];

                            pstPabOUT->uc.Pab[ offset     ] = val & 0xFF;
                            pstPabOUT->uc.Pab[ offset + 1 ] = val >> 8;
                            in_idx++;
                            break;
                            }
                        }

#ifdef DEBUG_KBUS
                    printf( "%d -> %u, ", j, nd->AO_[ j ] );
#endif // DEBUG_KBUS
                    }

                memcpy( nd->AO, nd->AO_, sizeof( nd->AO ) );

#ifdef DEBUG_KBUS
                printf( "\n" );
#endif // DEBUG_KBUS

                KbusUpdate();
                }
            }// if ( nd->type == wago_node::T_750_86x... ) // KBus
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return wago_manager_linux::write_outputs();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
