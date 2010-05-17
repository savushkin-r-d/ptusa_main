#include "wago.h"

wago_manager* wago_manager::instance;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int wago_device::load( file *cfg_file )
    {           
    load_table_from_string( cfg_file->fget_line(), DI_channels );
    for ( u_int i = 0; i < DI_channels.count; i++ )
        {
        DI_channels.char_read_values[ i ] = wago_manager::get_instance()->
            get_DI_read_data( DI_channels.tables[ i ], DI_channels.offsets[ i ] );
        }
    load_table_from_string( cfg_file->fget_line(), DO_channels );
    for ( u_int i = 0; i < DO_channels.count; i++ )
        {        
        DO_channels.char_read_values[ i ] = wago_manager::get_instance()->
            get_DO_read_data( DO_channels.tables[ i ], DO_channels.offsets[ i ] );
        DO_channels.char_write_values[ i ] = wago_manager::get_instance()->
            get_DO_write_data( DO_channels.tables[ i ], DO_channels.offsets[ i ] );
        }
    load_table_from_string( cfg_file->fget_line(), AI_channels );
    for ( u_int i = 0; i < AI_channels.count; i++ )
        {
        AI_channels.int_read_values[ i ] = wago_manager::get_instance()->
            get_AI_read_data( AI_channels.tables[ i ], AI_channels.offsets[ i ] );
        }
    load_table_from_string( cfg_file->fget_line(), AO_channels );
    for ( u_int i = 0; i < AO_channels.count; i++ )
        {
        AO_channels.int_read_values[ i ] = wago_manager::get_instance()->
            get_AO_read_data( AO_channels.tables[ i ], AO_channels.offsets[ i ] );
        AO_channels.int_write_values[ i ] = wago_manager::get_instance()->
            get_AO_write_data( AO_channels.tables[ i ], AO_channels.offsets[ i ] );
        }

    // 2 1.1 2.1
    // количество значение_№1 значение_№2 ...
    char *str = cfg_file->fget_line();
    int pos = sscanf( str, "%u", &params_count );

    if ( params_count > 0 )
        {        
        params = new float [ params_count ];
        for ( u_int i = 0; i < params_count; i++ )
            {
            pos += sscanf( str + pos, " %f", &params[ i ] );
            }
        }
    cfg_file->fget_line();

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_device::get_DO( u_int index )
    {
    if ( index < DO_channels.count &&
        DO_channels.char_read_values &&
        DO_channels.char_read_values[ index ] )
        {
        return *DO_channels.char_read_values[ index ];
        }

#ifdef DEBUG
    Print( "wago_device->get_DO(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_device::set_DO( u_int index, char value )
    {
    if ( index < DO_channels.count && 
        DO_channels.char_write_values &&
        DO_channels.char_write_values[ index ] )
        {
        *DO_channels.char_write_values[ index ] = value;
        if ( debug_mode )
            {
            *DO_channels.char_read_values[ index ] = value;
            }
        return 0;
        }

#ifdef DEBUG
    Print( "wago_device->set_DO(...) - error!\n" );
#endif // DEBUG

    return 1;
    }
//-----------------------------------------------------------------------------
int wago_device::get_DI( u_int index )
    {
    if ( index < DI_channels.count &&
        DI_channels.char_read_values &&
        DI_channels.char_read_values[ index ] )
        {
        return *DI_channels.char_read_values[ index ];
        }

#ifdef DEBUG
    Print( "wago_device->get_DI(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_device::set_DI( u_int index, char value )
    {
    if ( index < DI_channels.count &&
        DI_channels.char_read_values &&
        DI_channels.char_read_values[ index ] )
        {
        *DI_channels.char_read_values[ index ] = value;
        return 0;
        }

#ifdef DEBUG
    Print( "wago_device->set_DI(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
u_int wago_device::get_AO( u_int index )
    {
    if ( index < AO_channels.count &&
        AO_channels.char_read_values &&
        AO_channels.char_read_values[ index ] )
        {
        return *AO_channels.int_read_values[ index ];
        }

#ifdef DEBUG
    Print( "wago_device->get_AO(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_device::set_AO( u_int index, u_int value )
    {
    if ( index < AO_channels.count &&
        AO_channels.char_read_values &&
        AO_channels.char_read_values[ index ] )
        {
        *AO_channels.int_write_values[ index ] = value;
        if ( debug_mode )
            {
            *AO_channels.int_read_values[ index ] = value;
            }
        return 0;
        }

#ifdef DEBUG
    Print( "wago_device->set_AO(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
u_int wago_device::get_AI( u_int index )
    {
    if ( index < AI_channels.count &&
        AI_channels.char_read_values &&
        AI_channels.char_read_values[ index ] )
        {
        return *AI_channels.int_read_values[ index ];
        }

#ifdef DEBUG
    Print( "wago_device->get_AI(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_device::set_AI( u_int index, u_int value )
    {
    if ( index < AI_channels.count &&
        AI_channels.char_read_values &&
        AI_channels.char_read_values[ index ] )
        {
        *AI_channels.int_read_values[ index ] = value;
        return 0;
        }

#ifdef DEBUG
    Print( "wago_device->set_AI(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
void wago_device::print() const
    {
    print_table( "DI", DI_channels );    
    print_table( "DO", DO_channels );    
    print_table( "AI", AI_channels );    
    print_table( "AO", AO_channels );
    Print( "\n" );
    }
//-----------------------------------------------------------------------------
int wago_device::load_table_from_string( char *str, IO_channels &channels )
    {
    // 2 1 2 1 3
    // количество_DI_(например) номер_таблицы_DI_№1 смещение_в_пределах_таблицы_№1 номер_таблицы_DI_№2 ...
    u_int cnt;
    int pos = sscanf( str, "%d", &cnt );

    if ( cnt > 0 )
        {
        channels.count = cnt;

        channels.tables = new u_int[ cnt ];
        channels.offsets = new u_int[ cnt ];

        switch ( channels.type )
            {
            case IO_channels::CT_DI:
                channels.char_read_values = new u_char*[ cnt ];
                break;

            case IO_channels::CT_DO:
                channels.char_read_values = new u_char*[ cnt ];
                channels.char_write_values = new u_char*[ cnt ];
                break;

            case IO_channels::CT_AI:
                channels.int_read_values = new u_int*[ cnt ];
                break;

            case IO_channels::CT_AO:
                channels.int_read_values = new u_int*[ cnt ];
                channels.int_write_values = new u_int*[ cnt ];
                break;
            }

        for ( u_int i = 0; i < cnt; i++ )
            {
            pos += sscanf( str + pos, " %d %d", &channels.tables[ i ],
                &channels.offsets[ i ] );
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void wago_device::print_table( const char *str, 
                              const IO_channels &channels ) const
    {
    if ( channels.count )
        {
        Print( "%s:%d", str, channels.count );
        if ( channels.count )
            {
            Print( "[ " );
            for ( u_int i = 0; i < channels.count; i++ )
                {
                Print("%d:%2d", channels.tables[ i ],
                    channels.offsets[ i ] );
                if ( i < channels.count - 1 ) Print( "; " );
                }
            Print( " ]" );
            }
        Print( "; " );
        }
    }
//-----------------------------------------------------------------------------
float wago_device::get_par( u_int index )
    {
    if ( index < params_count && params )
        {
        return params[ index ];
        }

#ifdef DEBUG
    Print( "wago_device->get_par(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
wago_device::wago_device() :DI_channels( IO_channels::CT_DI ), 
DO_channels( IO_channels::CT_DO ),
AI_channels( IO_channels::CT_AI ),
AO_channels( IO_channels::CT_AO )
    {
    }
//-----------------------------------------------------------------------------
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
u_char* wago_manager::get_DI_read_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->DI_cnt )
            {
            return &nodes[ node_n ]->DI[ offset ];
            }
        }
#ifdef DEBUG
    Print( "get_DI_data() - error!\n" );
    while( 1 ) ;
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
u_char* wago_manager::get_DO_read_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->DO_cnt )
            {
            return &nodes[ node_n ]->DO[ offset ];
            }
        }
#ifdef DEBUG
    Print( "get_DO_data() - error!\n" );
    while( 1 ) ;
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
u_int* wago_manager::get_AI_read_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->AI_cnt )
            {
            return &nodes[ node_n ]->AI[ offset ];
            }
        }
#ifdef DEBUG
    Print( "get_AI_data() - error!\n" );
    while( 1 ) ;
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
u_int* wago_manager::get_AO_read_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->AO_cnt )
            {
            return &nodes[ node_n ]->AO[ offset ];
            }
        }
#ifdef DEBUG
    Print( "get_AO_data() - error!\n" );
    while( 1 ) ;
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
u_char* wago_manager::get_DO_write_data( u_int node_n, u_int offset )
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
//-----------------------------------------------------------------------------
u_int* wago_manager::get_AO_write_data( u_int node_n, u_int offset )
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
        AI = new u_int [ AI_cnt ];
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
        AO = new u_int [ AO_cnt ];
        AO_ = new u_int [ AO_cnt ];
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
