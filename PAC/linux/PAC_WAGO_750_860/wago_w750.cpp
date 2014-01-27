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
            static u_long start_time = get_millisec();
            if ( get_delta_millisec( start_time ) > 200 )
                {
                start_time = get_millisec();

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
                    printf( "%2d -> %d, ", j, nodes[ i ]->DI[ j ] );
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

                    nodes[ i ]->DI[ j ] = ( ( tmp & pstPabIN->uc.Pab[ j / 8 ] ) > 0 );

                    printf( "%2d -> %d, ", j, nodes[ i ]->DI[ j ] );
                    if ( ( j + 1 ) % 8 == 0 )
                        {
                        Print( "\n" );
                        }
                    }

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
                    printf( "%d -> %u, ", j, nodes[ i ]->AI[ j ] );
#endif // DEBUG_KBUS
                    }
#ifdef DEBUG_KBUS
                printf( "\n" );
#endif // DEBUG_KBUS
                }
            }// if ( nodes[ i ]->type == wago_node::T_750_860 ) // KBus 
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return wago_manager_linux::read_inputs();
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
            static u_long start_time = get_millisec();
            if ( get_delta_millisec( start_time ) > 200 )
                {
                start_time = get_millisec();

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

                    pstPabOUT->uc.Pab[ offset ] = val & 0xFF;
                    pstPabOUT->uc.Pab[ offset + 1 ] = val >> 8;

                    nodes[ i ]->AO[ j ] = nodes[ i ]->AO_[ j ];
#ifdef DEBUG_KBUSl
                    printf( "%d -> %u, ", j, nodes[ i ]->AO_[ j ] );
#endif // DEBUG_KBUS
                    }

#ifdef DEBUG_KBUS
                printf( "\n" );
#endif // DEBUG_KBUS

                KbusUpdate();
                }
            }// if ( nodes[ i ]->type == wago_node::T_750_860 ) // KBus
    
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return wago_manager_linux::write_outputs();    
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
