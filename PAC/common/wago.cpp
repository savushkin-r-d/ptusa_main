#include "wago.h"

auto_smart_ptr < wago_manager > wago_manager::instance;
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
float wago_device::get_AO( u_int index, float min_value, float max_value )
    {
    if ( index < AO_channels.count &&
        AO_channels.int_read_values &&
        AO_channels.int_read_values[ index ] )
        {
        float val = *AO_channels.int_read_values[ index ];

        u_int table_n = AO_channels.tables[ index ];
        u_int offset = AO_channels.offsets[ index ];
        u_int module_type = G_WAGO_MANAGER->get_node( table_n )->AO_types[ offset ];

        switch ( module_type )
            {
            // Выход модуля 554.
            // Три наименне значащих бита не учитываются.
            //    -----------------------------------------------------------------------
            //    Output          Output          Binary value
            //    current 0-20	  current 4-20                            Hex.      Dec.
            //    -----------------------------------------------------------------------
            //    20              20              0111 1111 1111 1111     7F FF     32767
            //    10              12              0100 0000 0000 0xxx     40 00     16384
            //    5               8               0010 0000 0000 0xxx     20 00      8192
            //    2.5             6               0001 0000 0000 0xxx     10 00      4096
            //    0.156           4.125           0000 0001 0000 0xxx     01 00       256
            //    0.01            4.0078          0000 0000 0001 0xxx     00 10        16
            //    0.005           4.0039          0000 0000 0000 1xxx     00 08         8
            //    0               4               0000 0000 0000 0111     00 07         7
            //    0               4               0000 0000 0000 0000     00 00         0
            //
        case 554:
            if ( 0 == min_value && 0 == max_value )
                {
                if ( val < 7 )
                    {
                    val = 0;
                    }
                else
                    {
                    val = 4 + val / 2047.5;
                    }
                }
            else
                {
                if ( val < 7 )
                    {
                    val = 4;
                    }
                else
                    {
                    val = 4 + val / 2047.5;
                    }
                val = min_value + ( val - 4 ) * ( max_value - min_value ) / 16;
                }

            return val;

        default:
            return val;
            }
        }

#ifdef DEBUG
    Print( "wago_device->get_AO(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int wago_device::set_AO( u_int index, float value, float min_value,
    float max_value )
    {
    if ( index < AO_channels.count &&
        AO_channels.int_write_values &&
        AO_channels.int_write_values[ index ] )
        {
        u_int table_n = AI_channels.tables[ index ];
        u_int offset = AI_channels.offsets[ index ];
        u_int module_type = G_WAGO_MANAGER->get_node( table_n )->AI_types[ offset ];

        switch ( module_type )
            {
        case 554:
            if ( 0 != min_value || 0 != max_value )
                {
                value = 4 + 16 * ( value - min_value ) / ( max_value - min_value );
                }
            if ( value < 4 ) value = 4;
            if ( value > 20 ) value = 20;
            value = 2047.5 * ( value - 4 );                   
            }

        *AO_channels.int_write_values[ index ] = ( u_int ) value;

        return 0;
        }

#ifdef DEBUG
    Print( "wago_device->set_AO(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
float wago_device::get_AI( u_int index, float min_value, float max_value )
    {
    if ( index < AI_channels.count &&
        AI_channels.int_read_values &&
        AI_channels.int_read_values[ index ] )
        {
        float val = *AI_channels.int_read_values[ index ];

        u_int table_n = AI_channels.tables[ index ];
        u_int offset = AI_channels.offsets[ index ];
        u_int module_type = G_WAGO_MANAGER->get_node( table_n )->AI_types[ offset ];

        switch ( module_type )
            {
            // Выход модуля 461.
            //   -------------------------------------------------------------------------
            //   Temperature  Voltage     Voltage     Binary value
            //   °C           (Ohm)       (Ohm)                               Hex.     Dec.
            //   -------------------------------------------------------------------------
            //                >400
            //   850          390.481     1384,998    0010 0001 0011 0100     2134     8500
            //   100          138.506     1099,299    0000 0011 1110 1000     03E8     1000
            //   25.5         109.929     1000,391    0000 0000 1111 1111     00FF      255
            //   0.1          100.039     1000        0000 0000 0000 0001     0001        1
            //   0            100         999,619     0000 0000 0000 0000     0000        0
            //  -0.1          99.970      901,929     1111 1111 1111 1111     FFFF       -1
            //  -25.5         90.389      184,936     1111 1111 0000 0001     FF01     -255
            //  -200          18.192                  1111 1000 0011 0000     F830    -2000
            //                <18                     1000 0000 0000 0000     8000   -32767
            //
        case 461:
            val *= 0.1;
            val = val >= -50 && val <= 150 ? val : -1000;
            return val;

            // Выход модуля 446.
            // Три наименне значащих бита не учитываются.
            //    -----------------------------------------------------------------------
            //    Input           Input           Binary value
            //    current 0-20	  current 4-20                            Hex.      Dec.
            //    -----------------------------------------------------------------------
            //   >20.5           >20.5            0111 1111 1111 1111     7F FF     32767
            //    20              20              0111 1111 1111 1111     7F FF     32767
            //    10              12              0100 0000 0000 0xxx     40 00     16384
            //    5               8               0010 0000 0000 0xxx     20 00      8192
            //    2.5             6               0001 0000 0000 0xxx     10 00      4096
            //    0.156           4.125           0000 0001 0000 0xxx     01 00       256
            //    0.01            4.0078          0000 0000 0001 0xxx     00 10        16
            //    0.005           4.0039          0000 0000 0000 1xxx     00 08         8
            //    0               4               0000 0000 0000 0111     00 07         7
            //    0               4               0000 0000 0000 0000     00 00         0
            //
        case 466:                
            if ( 0 == min_value && 0 == max_value )
                {
                if ( val < 7 )
                    {
                    val = 0;
                    }
                else
                    {
                    val = 4 + val / 2047.5;
                    }
                }
            else
                {
                if ( val < 7 )
                    {
                    val = 4;
                    }
                else
                    {
                    val = 4 + val / 2047.5;
                    }
                val = min_value + ( val - 4 ) * ( max_value - min_value ) / 16;
                }

            return val;

        default:
            return val;
            }
        }

#ifdef DEBUG
    Print( "wago_device->get_AI(...) - error!\n" );
    Print( "index=%d, AI_channels.count=%d, AI_channels.char_read_values=%d, AI_channels.char_read_values[ index ]=%d\n",
        index, AI_channels.count, ( int ) AI_channels.char_read_values,
        ( int ) AI_channels.char_read_values[ index ] );
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
    // Пример:
    // 2 1 2 1 3 ...
    // количество_DI номер_таблицы_DI_№1 смещение_в_пределах_таблицы_№1 номер_таблицы_DI_№2 ...
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
            channels.char_read_values  = new u_char*[ cnt ];
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
    Print( "index=%d, params_count=%d, params=%d\n",
        index, params_count, ( int ) params );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
wago_device::wago_device() :DI_channels( IO_channels::CT_DI ), 
    DO_channels( IO_channels::CT_DO ),
    AI_channels( IO_channels::CT_AI ),
    AO_channels( IO_channels::CT_AO ),
    params( 0 )
    {
    }
//-----------------------------------------------------------------------------
wago_device::~wago_device()
    {
    if ( params )
        {
        delete [] params;
        params = 0;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
wago_device::IO_channels::IO_channels( CHANNEL_TYPE type ) : count( 0 ), 
    tables( 0 ),
    offsets( 0 ),
    int_read_values( 0 ), int_write_values( 0 ),
    char_read_values( 0 ), char_write_values( 0 ),
    type( type )
    {
    }
//-----------------------------------------------------------------------------
wago_device::IO_channels::~IO_channels()
    {
    if ( count )
        {
        delete [] tables;
        delete [] offsets;
        count = 0;
        }
    if ( int_read_values )
        {
        delete [] int_read_values;
        int_read_values = 0;
        }
    if ( int_write_values )
        {
        delete [] int_write_values;
        int_write_values = 0;
        }
    if ( char_read_values )
        {
        delete [] char_read_values;  
        char_read_values = 0;
        }

    if ( char_write_values )
        {
        delete [] char_write_values;
        char_write_values = 0;
        }
    }
//-----------------------------------------------------------------------------

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

    this->nodes_count = nodes_count;
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
            return &( nodes[ node_n ]->AI[ offset ] );
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
wago_manager::wago_manager() :nodes_count( 0 ), nodes( 0 )
    {
    }
//-----------------------------------------------------------------------------
wago_manager::~wago_manager()
    {
    if ( nodes_count && nodes )
        {
        for ( u_int i = 0; i < nodes_count; i++ )
            {
            delete nodes[ i ];
            }

        delete [] nodes;
        nodes = 0;
        nodes_count = 0;
        }
    }
//-----------------------------------------------------------------------------
wago_manager::wago_node * wago_manager::get_node( int node_n )
    {
    return nodes[ node_n ];
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
wago_manager::wago_node::~wago_node()
    {
    if ( DO_cnt )
        {
        delete [] DO;
        delete [] DO_;
        DO_cnt = 0;
        }

    if ( AO_cnt )
        {
        delete [] AO;
        delete [] AO_;
        delete [] AO_offsets;
        delete [] AO_types;
        AO_cnt = 0;
        }

    if ( DI_cnt )
        {
        delete [] DI;
        DI_cnt = 0;
        }

    if ( AI_cnt )
        {
        delete [] AI;
        delete [] AI_offsets;
        delete [] AI_types;
        AI_cnt = 0;
        }
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
        memset( DI, 0, DI_cnt );
        }

    sscanf( cfg_file->fget_line(), "%d", &DO_cnt );
    if ( DO_cnt )
        {
        DO = new u_char [ DO_cnt ];
        DO_ = new u_char [ DO_cnt ];
        memset( DO, 0, DO_cnt );
        memset( DO_, 0, DO_cnt );
        }

    sscanf( cfg_file->fget_line(), "%d", &AI_cnt );
    if ( AI_cnt )
        {
        AI = new u_int [ AI_cnt ];
        AI_offsets = new u_int [ AI_cnt ];
        AI_types = new u_int [ AI_cnt ];

        memset( AI, 0, AI_cnt * sizeof( u_int ) );
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

        memset( AO, 0, AO_cnt * sizeof( u_int ) );
        memset( AO_, 0, AO_cnt * sizeof( u_int ) );
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

    //for ( u_int i = 0; i < AI_cnt; i++ )
    //    {
    //    if ( 0 == i )
    //        {
    //        Print( "\tAI\n");
    //        }
    //    Print( "\t%u %u\n", AI_types[ i ], AI_offsets[ i ] );        
    //    }

    //for ( u_int i = 0; i < AO_cnt; i++ )
    //    {
    //    if ( 0 == i ) 
    //        {
    //        Print( "\tAO\n");
    //        }
    //    Print( "\t%2.d %u %u\n", i + 1, AO_types[ i ], AO_offsets[ i ] );
    //    }

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
