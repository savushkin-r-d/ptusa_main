#ifndef WAGO_H
#define WAGO_H

#include "sys.h"
//-----------------------------------------------------------------------------
/// @brief 
///
/// 
class wago_manager
    {
    public:
        virtual int read_inputs() = 0;

        virtual int write_outputs() = 0;

        int load_from_cfg_file( file *cfg_file );

        static wago_manager* get_instance();

        static void set_instance( wago_manager* new_instance );

        u_char* get_DI_read_data( u_int node_n, u_int offset );

        u_char* get_DO_read_data( u_int node_n, u_int offset );
        u_char* get_DO_write_data( u_int node_n, u_int offset )
            {
            if ( node_n < nodes_count && nodes )
                {
                if ( nodes[ node_n ] && offset < nodes[ node_n ]->DO_cnt )
                    {
                    return &nodes[ node_n ]->DO_[ offset ];
                    }
                }
#ifdef DEBUG
            Print( "get_DO_write_data() - error!\n" );
            while( 1 ) ;
#endif // DEBUG

            return 0;
            }

        u_int* get_AI_read_data( u_int node_n, u_int offset );

        u_int* get_AO_read_data( u_int node_n, u_int offset );
        u_int* get_AO_write_data( u_int node_n, u_int offset )
            {
            if ( node_n < nodes_count && nodes )
                {
                if ( nodes[ node_n ] && offset < nodes[ node_n ]->AO_cnt )
                    {
                    return &nodes[ node_n ]->AO_[ offset ];
                    }
                }
#ifdef DEBUG
            Print( "get_AO_write_data() - error!\n" );
            while( 1 ) ;
#endif // DEBUG

            return 0;
            }


    protected:
        //---------------------------------------------------------------------
        /// @brief
        //
        ///
        struct wago_node
            {
            wago_node();

            int load_from_cfg_file( file *cfg_file );

            int     state;
            u_int   number;
            u_int   type;
            int     ip_addres[ 4 ];

            // Digital outputs ( DO ).
            u_int  DO_cnt;      ///< Amount of DO.
            u_char *DO;         ///< Current values.
            u_char *DO_;        ///< To write.

            // Analog outputs ( AO ).
            u_int AO_cnt;       ///< Amount of AO.
            u_int *AO;          ///< Current values.
            u_int *AO_;         ///< To write.
            u_int *AO_offsets;  ///< Offsets in common data.
            u_int *AO_types;    ///< Channels type.

            // Digital inputs ( DI ).
            u_int  DI_cnt;      ///< Amount of DI.
            u_char *DI;         ///< Current values.

            // Analog inputs ( AI ).
            u_int AI_cnt;       ///< Amount of AI.
            u_int *AI;          ///< Current values.
            u_int *AI_offsets;  ///< Offsets in common data.
            u_int *AI_types;    ///< Channels type.
            };

        u_int       nodes_count;
        wago_node **nodes;

        static wago_manager* instance;
    };
//---------------------------------------------------------------------
#endif // WAGO_H 


