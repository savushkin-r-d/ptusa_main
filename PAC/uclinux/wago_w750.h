#include "wago.h"

#include "kbusapi.h"

/// @brief
///
///
class wago_manager_w750 : public wago_manager
    {
    public:
        wago_manager_w750()
            {
            KbusOpen();
            }

        virtual ~wago_manager_w750()
            {
            KbusClose();
            }

        int read_inputs()
            {
            if ( 0 == nodes_count ) return 0;

                
            for ( u_int i = 0; i < nodes_count; i++ )
                {
                if ( nodes[ i ]->type == 0 ) // KBus
                    {
                    KbusUpdate();
                    int start_pos = KbusGetBinaryInputOffset() / 8;
#ifdef DEBUG
                    Print( "start_pos=%d\n", start_pos );
#endif // DEBUG

                    for ( u_int j = 0; j < nodes[ i ]->DI_cnt; j++ )
                        {
                        char tmp =  1 << ( j % 8 );

                        nodes[ i ]->DI[ j ] = ( ( tmp &
                            pstPabIN->uc.Pab[ start_pos + j / 8 ] ) > 0 );

#ifdef DEBUG
                        Print( "%d -> %d, ", j, nodes[ i ]->DI[ j ] );
#endif // DEBUG
                        }
#ifdef DEBUG
                        Print( "\n" );
#endif // DEBUG
                    }
                }                
            
            return 0;
            }

        virtual int write_outputs()
            {
            if ( 0 == nodes_count ) return 0;

            for ( u_int i = 0; i < nodes_count; i++ )
                {
                if ( nodes[ i ]->type == 0 ) // KBus
                    {
                    KbusUpdate();
                    int start_pos = KbusGetBinaryOutputOffset() / 8;
#ifdef DEBUG
                    Print( "start_pos=%d\n", start_pos );
#endif // DEBUG

                    for ( u_int j = 0; j < nodes[ i ]->DI_cnt; j++ )
                        {
                        char tmp =  1 << ( j % 8 );
                        if ( nodes[ i ]->_DO[ j ] )
                            {
                            pstPabOUT->uc.Pab[ start_pos + j / 8 ] |= tmp;
                            }
                         else
                            {
                            pstPabOUT->uc.Pab[ start_pos + j / 8 ] &= ~tmp;
                            }
#ifdef DEBUG
                        Print( "%d -> %d, ", j, nodes[ i ]->_DO[ j ] );
#endif // DEBUG
                        nodes[ i ]->DO[ j ] = nodes[ i ]->_DO[ j ];
                        }
#ifdef DEBUG
                        Print( "\n" );
#endif // DEBUG
                    }
                }

            return 0;
            }
    };
    