#include <unistd.h>

#include "wago_w750.h"
//-----------------------------------------------------------------------------
wago_manager_w750::wago_manager_w750()
    {
    KbusOpen();
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
        if ( nodes[ i ]->type == 0 ) // KBus
            {
            KbusUpdate();

            // DI
            int start_pos = KbusGetBinaryInputOffset() / 8;
#ifdef DEBUG
//            Print( "start_pos=%d\n", start_pos );
#endif // DEBUG
            for ( u_int j = 0; j < nodes[ i ]->DI_cnt; j++ )
                {
                char tmp =  1 << ( j % 8 );

                nodes[ i ]->DI[ j ] = ( ( tmp &
                    pstPabIN->uc.Pab[ start_pos + j / 8 ] ) > 0 );
#ifdef DEBUG
                //Print( "%d -> %d, ", j, nodes[ i ]->DI[ j ] );
#endif // DEBUG
                }
#ifdef DEBUG
            //Print( "\n" );
#endif // DEBUG

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
#ifdef DEBUG
//                Print( "%d -> %u\n, ", j, nodes[ i ]->AI[ j ] );
#endif // DEBUG
                }
            }
        }                

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_manager_w750::write_outputs()
    {
    if ( 0 == nodes_count ) return 0;

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        if ( nodes[ i ]->type == 0 ) // KBus
            {
            // DO
            int start_pos = KbusGetBinaryOutputOffset() / 8;
#ifdef DEBUG
            //Print( "start_pos = %d\n", start_pos );
#endif // DEBUG
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
#ifdef DEBUG
                //Print( "%d -> %d, ", j, nodes[ i ]->DO_[ j ] );
#endif // DEBUG
                }
#ifdef DEBUG
            //Print( "\n" );
#endif // DEBUG

            // AO
            for ( u_int j = 0; j < nodes[ i ]->AO_cnt; j++ )
                {
                int val = nodes[ i ]->AO_[ j ];
                u_int offset = nodes[ i ]->AO_offsets[ j ];

                pstPabIN->uc.Pab[ offset ] = val & 0xFF;
                pstPabIN->uc.Pab[ offset + 1 ] = val >> 8;

                nodes[ i ]->AO[ j ] = nodes[ i ]->AO_[ j ];
#ifdef DEBUG
                //Print( "%d -> %u\n, ", j, nodes[ i ]->AO_[ j ] );
#endif // DEBUG
                }

            KbusUpdate();
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
