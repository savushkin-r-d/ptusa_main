#include "wago.h"

wago_manager* wago_manager::instance;
//-----------------------------------------------------------------------------
int wago_manager::load_from_cfg_file( file *cfg_file )
    {
    cfg_file->fget_line(); // Пропускаем заголовок.

    //-File editor_version.
    int nodes_count = 0;
    sscanf( cfg_file->fget_line(), "%d", &nodes_count );
#ifdef DEBUG
    Print( "Wago total nodes count %d.\n", nodes_count );
#endif // DEBUG
    cfg_file->fget_line();

    this-> nodes_count = nodes_count;
    if ( nodes_count )
        {
        nodes = new wago_node*[ nodes_count ];
        for ( int i = 0; i < nodes_count; i++ )
            {
            nodes[ i ] = new wago_node;
#ifdef DEBUG
            Print( "    %d. ", i + 1 );
#endif // DEBUG
            nodes[ i ]->load_from_cfg_file( cfg_file );
            }
        }

    cfg_file->fget_line(); // Пропускаем пустую строку.

    return 0;
    }
//-----------------------------------------------------------------------------
wago_manager* wago_manager::get_instance()
    {
    return instance;
    }
//-----------------------------------------------------------------------------
void wago_manager::set_instance( wago_manager* new_instance )
    {
    instance = new_instance;
    }
//-----------------------------------------------------------------------------
int wago_manager::get_DI_data( u_int node_n, u_int offset, u_char *p_read )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->DI_cnt )
            {
            p_read = &nodes[ node_n ]->DI[ offset ];
            return 0;
            }
        }
#ifdef DEBUG
    Print( "get_DI_data() - error!\n" );
    while( 1 ) ;
#endif // DEBUG

    return 1;
    }
//-----------------------------------------------------------------------------
int wago_manager::get_DO_data( u_int node_n, u_int offset, u_char *p_read,
                              u_char *p_write )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->DO_cnt )
            {
            p_read = &nodes[ node_n ]->DO[ offset ];
            p_write = &nodes[ node_n ]->DO_[ offset ];
            return 0;
            }
        }
#ifdef DEBUG
    Print( "get_DO_data() - error!\n" );
    while( 1 ) ;
#endif // DEBUG

    return 1;
    }
//-----------------------------------------------------------------------------
int wago_manager::get_AI_data( u_int node_n, u_int offset, float *p_read )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->AI_cnt )
            {
            p_read = &nodes[ node_n ]->AI[ offset ];                    
            return 0;
            }
        }
#ifdef DEBUG
    Print( "get_AI_data() - error!\n" );
    while( 1 ) ;
#endif // DEBUG

    return 1;
    }
//-----------------------------------------------------------------------------
int wago_manager::get_AO_data( u_int node_n, u_int offset, 
                              float *p_read, float *p_write )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->AO_cnt )
            {
            p_read = &nodes[ node_n ]->AO[ offset ];
            p_write = &nodes[ node_n ]->AO_[ offset ];
            return 0;
            }
        }
#ifdef DEBUG
    Print( "get_AO_data() - error!\n" );
    while( 1 ) ;
#endif // DEBUG

    return 1;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
wago_manager::wago_node::wago_node() : state( 0 ),
number( 0 ),
type( 0 ),
DO_cnt( 0 ),
DO( 0 ),
DO_( 0 ),
AO_cnt( 0 ),
AO( 0 ),
AO_( 0 ),
AO_offsets( 0 ),
AO_types( 0 ),
DI_cnt( 0 ),
DI( 0 ),
AI_cnt( 0 ),
AI( 0 ),
AI_offsets( 0 ),
AI_types( 0 )
    {
    memset( ip_addres, 0, 4 * sizeof( int ) );
    }
//-----------------------------------------------------------------------------
int wago_manager::wago_node::load_from_cfg_file( file *cfg_file )
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
    if ( DI_cnt )
        {
        DI = new u_char [ DI_cnt ];
        }

    sscanf( cfg_file->fget_line(), "%d", &DO_cnt );
    if ( DO_cnt )
        {
        DO = new u_char [ DO_cnt ];
        DO_ = new u_char [ DO_cnt ];
        }

    sscanf( cfg_file->fget_line(), "%d", &AI_cnt );
    if ( AI_cnt )
        {
        AI = new float [ AI_cnt ];
        AI_offsets = new u_int [ AI_cnt ];
        AI_types = new u_int [ AI_cnt ];
        }
    for ( u_int i = 0; i < AI_cnt; i++ )
        {
        int tmp;
        sscanf( cfg_file->fget_line(), "%d %u %u",
            &tmp, &AI_types[ i ], &AI_offsets[ i ] );
        }

    sscanf( cfg_file->fget_line(), "%d", &AO_cnt );
    if ( AO_cnt )
        {
        AO = new float [ AO_cnt ];
        AO_ = new float [ AO_cnt ];
        AO_types = new u_int [ AO_cnt ];
        AO_offsets = new u_int [ AO_cnt ];
        }
    for ( u_int i = 0; i < AO_cnt; i++ )
        {
        int tmp;
        sscanf( cfg_file->fget_line(), "%d %u %u",
            &tmp, &AO_types[ i ], &AO_offsets[ i ] );
        }
    cfg_file->fget_line();

#ifdef DEBUG
    Print( "type %d, number %d, ip %d.%d.%d.%d. ",
        type, number, ip_addres[ 0 ], ip_addres[ 1 ], 
        ip_addres[ 2 ], ip_addres[ 3 ] );
    Print( "DI %d, DO %d, AI %d, AO %d.\n",
        DI_cnt, DO_cnt, AI_cnt, AO_cnt );
    for ( u_int i = 0; i < AI_cnt; i++ )
        {
        if ( 0 == i ) Print( "\tAI\n");
        Print( "\t%2.d %u %u\n", i + 1, AI_types[ i ], AI_offsets[ i ] );
        }
    for ( u_int i = 0; i < AO_cnt; i++ )
        {
        if ( 0 == i ) Print( "\tAO\n");
        Print( "\t%2.d %u %u\n", i + 1, AO_types[ i ], AO_offsets[ i ] );
        }
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------