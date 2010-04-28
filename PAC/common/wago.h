#ifndef WAGO_H
#define WAGO_H

#include "sys.h"
//-----------------------------------------------------------------------------
/// @brief 
//
/// 
struct wago_table 
    {

    wago_table() : state( 0 ),
        number( 0 ),
        type( 0 ),
        DO_cnt( 0 ),
        DO( 0 ),
        _DO( 0 ),
        AO_cnt( 0 ),
        AO( 0 ),
        _AO( 0 ),
        DI_cnt( 0 ),
        DI( 0 ),
        AI_cnt( 0 ),
        AI( 0 )
        {
        memset( ip_addres, 0, 4 * sizeof( int ) );
        }

    int load_from_cfg_file( file *cfg_file )
        {
        state = 0;
        sscanf( cfg_file->fget_line(), "%d", &type );
        sscanf( cfg_file->fget_line(), "%d", &number );
        char tmp_dot;
        sscanf( cfg_file->fget_line(), "%d%c%d%c%d%c%d",
            &ip_addres[ 0 ], &tmp_dot, &ip_addres[ 1 ], &tmp_dot,
            &ip_addres[ 2 ],  &tmp_dot, &ip_addres[ 3 ] );

        int modules_count = 0;
        sscanf( cfg_file->fget_line(), "%d", &modules_count );
        
        sscanf( cfg_file->fget_line(), "%d", &DI_cnt );
        sscanf( cfg_file->fget_line(), "%d", &DO_cnt );
        sscanf( cfg_file->fget_line(), "%d", &AI_cnt );
        sscanf( cfg_file->fget_line(), "%d", &AO_cnt );
        cfg_file->fget_line();

#ifdef DEBUG
            Print( "type %d, number %d, ip %d.%d.%d.%d. ",
                type, number, ip_addres[ 0 ], ip_addres[ 1 ], ip_addres[ 2 ],
                ip_addres[ 3 ] );
             Print( "DO %d, DI %d, AO %d, AI %d.\n",
                DO_cnt, DI_cnt, AO_cnt, AI_cnt );
#endif // DEBUG

        for ( int i = 0; i < modules_count; i++ )
            {
            cfg_file->fget_line(); // Тип модуля.

            cfg_file->fget_line(); // Комментарий.
            cfg_file->fget_line(); // Комментарий.
            cfg_file->fget_line(); // Комментарий.
            cfg_file->fget_line(); // Комментарий.
            cfg_file->fget_line(); // Комментарий.

            cfg_file->fget_line(); // пустая строка.
            }

        return 0;
        }

    int     state;
    u_int   number;
    u_int   type;
    int     ip_addres[ 4 ];

    // Digital outputs ( DO ).
    int DO_cnt;  ///< Amount of DO.
    u_char *DO;  ///< Current values.
    u_char *_DO; ///< To write.

    // Analog outputs ( AO ).
    int AO_cnt;  ///< Amount of AO.
    float *AO;   ///< Current values.
    float *_AO;  ///< To write.

    // Digital inputs ( DI ).
    int DI_cnt;  ///< Amount of DI.
    u_char *DI;  ///< Current values.

    // Analog inputs ( AI ).
    int AI_cnt;  ///< Amount of AI.
    float *AI;   ///< Current values.
    };
//-----------------------------------------------------------------------------
/// @brief 
///
/// 
class wago_manager
    {
    public:
        virtual int read_inputs() = 0;
        
        virtual int write_outputs() = 0;

        int load_from_cfg_file( file *cfg_file )
            {
            cfg_file->fget_line(); // Пропускаем заголовок.

            //-File editor_version.
            int nodes_count = 0;
            sscanf( cfg_file->fget_line(), "%d", &nodes_count );
#ifdef DEBUG
            Print( "Wago total nodes count %d.\n", nodes_count );
#endif // DEBUG
            cfg_file->fget_line();

            tables_count = nodes_count;
            if ( nodes_count )
                {
                tables = new wago_table*[ nodes_count ];
                for ( int i = 0; i < nodes_count; i++ )
                    {
                     tables[ i ] = new wago_table;
#ifdef DEBUG
                    Print( "    %d. ", i );
#endif // DEBUG
                     tables[ i ]->load_from_cfg_file( cfg_file );
                    }
                }

            cfg_file->fget_line(); // Пропускаем пустую строку.
            
            return 0;
            }

        static wago_manager* get_instance()
            {
            return instance;
            }

        static void set_instance( wago_manager* new_instance )
            {
            instance = new_instance;
            }

        virtual int get_DO( u_int table_n, u_int offset )
            {
            return 0;
            }        
        virtual int set_DO( u_int table_n, u_int offset, char value )
            {
            return 0;
            }

        virtual int get_DI( u_int table_n, u_int offset )
            {
            return 0;
            }
        virtual int set_DI( u_int table_n, u_int offset, char value )
            {
            return 0;
            }

        virtual float get_AO( u_int table_n, u_int offset )
            {
            return 0;
            }
        virtual int   set_AO( u_int table_n, u_int offset, float value )
            {
            return 0;
            }

        virtual float get_AI( u_int table_n, u_int offset )
            {
            return 0;
            }
        virtual int   set_AI( u_int table_n, u_int offset, float value )
            {
            return 0;
            }

    private:
        int        tables_count;
        wago_table **tables;

        static wago_manager* instance;
    };
//-----------------------------------------------------------------------------
#endif // WAGO_H 

