#if !defined WIN_OS && \
    !( defined LINUX_OS && defined PAC_PC ) && \
    !( defined LINUX_OS && defined PAC_WAGO_750_860 ) && \
    !( defined LINUX_OS && defined PAC_WAGO_PFC200 )
#error You must define OS!
#endif

#include "wago.h"
#include "lua_manager.h"

#include "dtime.h"

#ifdef WIN_OS
#include "wago_PC.h"
#endif

#if defined LINUX_OS && defined PAC_PC
#include "l_wago.h"
#endif

#if defined LINUX_OS && defined PAC_WAGO_750_860
#include "wago_w750.h"
#endif

#if defined LINUX_OS && defined PAC_WAGO_PFC200
#include "l_wago.h"
#include "wago_PFC200.h"
#endif

auto_smart_ptr < wago_manager > wago_manager::instance;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int wago_device::get_DO( u_int index )
    {
    if ( index < DO_channels.count &&
        DO_channels.char_write_values &&
        DO_channels.char_write_values[ index ] )
        {
        return *DO_channels.char_write_values[ index ];
        }

#ifdef DEBUG
    Print( "Error: \"%s\" wago_device->get_DO(...) - index=%d, DO_channels_count=%d\n",
        name, index, DO_channels.count );
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
    print();
    Print( "wago_device->set_DO(...) - error! %d, %d, %d, %d\n",
            index, DO_channels.count, ( int ) DO_channels.char_write_values,
            ( int ) DO_channels.char_write_values[ index ] );
    print();
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
    print();
    Print( "wago_device->get_DI(...) - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
float wago_device::get_AO( u_int index, float min_value, float max_value )
    {
    if ( index < AO_channels.count &&
        AO_channels.int_write_values &&
        AO_channels.int_write_values[ index ] )
        {
        float val = ( float ) *AO_channels.int_write_values[ index ];

        u_int table_n = AO_channels.tables[ index ];
        u_int offset = AO_channels.offsets[ index ];
        u_int module_type = G_WAGO_MANAGER()->get_node( table_n )->AO_types[ offset ];

        switch ( module_type )
            {
            // Выход модуля 554.
            // Три наименее значащих бита не учитываются.
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
                    val = 4 + val / 2047.5f;
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
                    val = 4 + val / 2047.5f;
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
        u_int table_n = AO_channels.tables[ index ];
        u_int offset = AO_channels.offsets[ index ];
        u_int module_type = G_WAGO_MANAGER()->get_node( table_n )->AO_types[ offset ];

        switch ( module_type )
            {
        case 554:
            if ( 0 != min_value || 0 != max_value )
                {
                value = 4 + 16 * ( value - min_value ) / ( max_value - min_value );
                }
            if ( value < 4 ) value = 4;
            if ( value > 20 ) value = 20;
            value = 2047.5f * ( value - 4 );
            }

        *AO_channels.int_write_values[ index ] = ( u_int ) value;

        printf( "value=%d\n", ( u_int ) value );

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
        float val = ( float ) *AI_channels.int_read_values[ index ];

        u_int table_n = AI_channels.tables[ index ];
        u_int offset = AI_channels.offsets[ index ];
        u_int module_type = G_WAGO_MANAGER()->get_node( table_n )->AI_types[ offset ];

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
            if ( val >= 0xF830 && val <= 0xFFFF ) // -0,1..-200 °C
                {
                val -= 0x10000; //65536
                val *= 0.1;
                return val;
                }
            if ( val >= 0 && val <= 8500 ) // 0..850 °C
                {
                val *= 0.1;
                return val;
                }

            return -1000;

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
                    val = 4 + val / 2047.5f;
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
                    val = 4 + val / 2047.5f;
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
        index, AI_channels.count, ( int ) AI_channels.int_read_values,
        ( int ) AI_channels.int_read_values[ index ] );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
void wago_device::print() const
    {
    Print( " " );

    DI_channels.print();
    DO_channels.print();
    AI_channels.print();
    AO_channels.print();
    //Print( "\n" );
    }
//-----------------------------------------------------------------------------
wago_device::wago_device( const char* name ) : name( name ),
    DI_channels( IO_channels::CT_DI ),
    DO_channels( IO_channels::CT_DO ),
    AI_channels( IO_channels::CT_AI ),
    AO_channels( IO_channels::CT_AO )
    {
    }
//-----------------------------------------------------------------------------
wago_device::~wago_device()
    {
    }
//-----------------------------------------------------------------------------
void wago_device::init( int DO_count, int DI_count, int AO_count,
    int AI_count )
    {
    if ( DO_count > 0 )
        {
        DO_channels.init( DO_count );
        }
    if ( DI_count > 0 )
        {
        DI_channels.init( DI_count );
        }
    if ( AO_count > 0 )
        {
        AO_channels.init( AO_count );
        }
    if ( AI_count > 0 )
        {
        AI_channels.init( AI_count );
        }
    }
//-----------------------------------------------------------------------------
void wago_device::init_channel( int type, int ch_index, int node, int offset )
    {
    switch ( type )
        {
    case IO_channels::CT_DI:
        DI_channels.init_channel( ch_index, node, offset );
        break;

    case IO_channels::CT_DO:
        DO_channels.init_channel( ch_index, node, offset );
        break;

    case IO_channels::CT_AI:
        AI_channels.init_channel( ch_index, node, offset );
        break;

    case IO_channels::CT_AO:
        AO_channels.init_channel( ch_index, node, offset );
        break;
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
void wago_device::IO_channels::init( int ch_count )
    {
    if ( ch_count > 0 )
        {
        count = ch_count;

        tables  = new u_int[ count ];
        offsets = new u_int[ count ];

        switch ( type )
            {
        case IO_channels::CT_DI:
            char_read_values = new u_char*[ count ];
            break;

        case IO_channels::CT_DO:
            char_read_values  = new u_char*[ count ];
            char_write_values = new u_char*[ count ];
            break;

        case IO_channels::CT_AI:
            int_read_values = new int*[ count ];
            break;

        case IO_channels::CT_AO:
            int_read_values  = new int*[ count ];
            int_write_values = new int*[ count ];
            break;
            }
        }
    }
//-----------------------------------------------------------------------------
void wago_device::IO_channels::print() const
    {
    if ( count )
        {
        switch ( type )
            {
        case CT_DI:
            Print( "DI" );
            break;

        case CT_DO:
            Print( "DO" );
            break;

        case CT_AI:
            Print( "AI" );
            break;

        case CT_AO:
            Print( "AO" );
            break;
            }

        Print( ":%d; ", count );
        //if ( count )
        //    {
        //    Print( "[ " );
        //    for ( u_int i = 0; i < count; i++ )
        //        {
        //        Print("%d:%2d", tables[ i ], offsets[ i ] );
        //        if ( i < count - 1 ) Print( "; " );
        //        }
        //    Print( " ]" );
        //    }
        //Print( "; " );
        }
    }
//-----------------------------------------------------------------------------
void wago_device::IO_channels::init_channel( u_int ch_index, int node, int offset )
    {
    if ( ch_index < count )
        {
        tables[ ch_index ]  = node;
        offsets[ ch_index ] = offset;
        switch ( type )
            {
        case CT_DI:
            char_read_values[ ch_index ] = wago_manager::get_instance()->
                get_DI_read_data( tables[ ch_index ], offsets[ ch_index ] );
            break;

        case CT_DO:
            char_read_values[ ch_index ] = wago_manager::get_instance()->
                get_DO_read_data( tables[ ch_index ], offsets[ ch_index ] );
            char_write_values[ ch_index ] = wago_manager::get_instance()->
                get_DO_write_data( tables[ ch_index ], offsets[ ch_index ] );
            break;

        case CT_AI:
            int_read_values[ ch_index ] = wago_manager::get_instance()->
                get_AI_read_data( tables[ ch_index ], offsets[ ch_index ] );
            break;

        case CT_AO:
            int_read_values[ ch_index ] = wago_manager::get_instance()->
                get_AO_read_data( tables[ ch_index ], offsets[ ch_index ] );
            int_write_values[ ch_index ] = wago_manager::get_instance()->
                get_AO_write_data( tables[ ch_index ], offsets[ ch_index ] );
            break;
            }
        }


    else
        {
#ifdef DEBUG
        Print( "Error wago_device::IO_channels::init_channel - index out of bound!\n" );
#endif // DEBUG
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void wago_manager::init( int nodes_count )
    {
    this->nodes_count = nodes_count;

    if ( nodes_count )
        {
        nodes = new wago_node*[ nodes_count ];
        for ( int i = 0; i < nodes_count; i++ )
            {
            nodes[ i ] = 0;
            }
        }

    }
//-----------------------------------------------------------------------------
wago_manager* wago_manager::get_instance()
    {
    if ( instance.is_null() )
        {
#ifdef WIN_OS
        instance = new wago_manager_PC();
#endif // WIN_OS

#if defined LINUX_OS && defined PAC_PC
        instance = new wago_manager_linux();
#endif // defined LINUX_OS && defined PAC_PC

#if defined LINUX_OS && defined PAC_WAGO_750_860
        instance = new wago_manager_w750();
#endif // defined LINUX_OS && defined PAC_WAGO_750_860

#if defined LINUX_OS && defined PAC_WAGO_PFC200
        instance = new wago_manager_linux();
#endif // defined LINUX_OS && defined PAC_WAGO_750_860
        }

    return instance;
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
    Print( "wago_manager::get_DI_data() - error!\n" );
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
    Print( "wago_manager::get_DO_data() - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int* wago_manager::get_AI_read_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->AI_cnt )
            {
            return &( nodes[ node_n ]->AI[ offset ] );
            }
        }
#ifdef DEBUG
    Print( "wago_manager::get_AI_data() - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int* wago_manager::get_AO_read_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->AO_cnt )
            {
            return &nodes[ node_n ]->AO[ offset ];
            }
        }
#ifdef DEBUG
    Print( "wago_manager::get_AO_data() - error!\n" );
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
    Print( "wago_manager::get_DO_write_data() - error!\n" );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
int* wago_manager::get_AO_write_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->AO_cnt )
            {
            return &nodes[ node_n ]->AO_[ offset ];
            }
        }
#ifdef DEBUG
    Print( "wago_manager::get_AO_write_data() - error!\n" );
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
void wago_manager::add_node( u_int index, int ntype, int address,
    char* IP_address, char *name, int DO_cnt, int DI_cnt, int AO_cnt, int AO_size,
    int AI_cnt, int AI_size )
    {
    if ( index < nodes_count )
        {
        nodes[ index ] = new wago_node( ntype, address, IP_address, name, DO_cnt,
            DI_cnt, AO_cnt, AO_size, AI_cnt, AI_size );
        }
    }
//-----------------------------------------------------------------------------
void wago_manager::init_node_AO( u_int node_index, u_int AO_index,
    u_int type, u_int offset )
    {
    if ( node_index < nodes_count && AO_index < nodes[ node_index ]->AO_cnt )
        {
        nodes[ node_index ]->AO_types[ AO_index ]   = type;
        nodes[ node_index ]->AO_offsets[ AO_index ] = offset;
        }
    }
//-----------------------------------------------------------------------------
void wago_manager::init_node_AI( u_int node_index, u_int AI_index,
    u_int type, u_int offset )
    {
    if ( node_index < nodes_count && AI_index < nodes[ node_index ]->AI_cnt )
        {
        nodes[ node_index ]->AI_types[ AI_index ]   = type;
        nodes[ node_index ]->AI_offsets[ AI_index ] = offset;
        }
    }
//-----------------------------------------------------------------------------
void wago_manager::print() const
    {
    Print( "Wago manager [%d]:\n", nodes_count );
    for ( u_int i = 0; i < nodes_count; i++ )
        {
        nodes[ i ]->print();
        }
    }
//-----------------------------------------------------------------------------
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
        delete [] AI_offsets;
        delete [] AI_types;
        AI_cnt = 0;
        }
    }
//-----------------------------------------------------------------------------
wago_manager::wago_node::wago_node( int type, int number, char *str_ip_address,
    char *name,
    int DO_cnt, int DI_cnt, int AO_cnt, int AO_size, int AI_cnt,
    int AI_size ): state( ST_NO_CONNECT ),
    type( ( TYPES ) type ),
    number( number ),

    is_active( true ),

    last_poll_time( get_sec() ),
    is_set_err( 0 ),
    sock( 0 ),

    DO_cnt( DO_cnt ),
    AO_cnt( AO_cnt ),
    AO_size( AO_size ),
    DI_cnt( DI_cnt ),
    AI_cnt( AI_cnt ),
    AI_size( AI_size ),
    last_init_time( get_millisec() ),
    delay_time( 0 )
    {
    if ( str_ip_address )
        {
        strcpy( ip_address, str_ip_address );
        }
    else
        {
        memset( ip_address, 0, sizeof( ip_address ) );
        }

    if ( ip_address[ 0 ] == 0 && type >= T_750_341 )
        {
        is_active = false;
#ifdef DEBUG
        Print( "Узел Wago \"%s\" отключен, так как не задан его IP адрес.\n",
            name );
#endif
        }

    if ( type == T_EMPTY )
        {
        is_active = false;
#ifdef DEBUG
        Print( "Узел Wago \"%s\" отключен, так как не задан его тип.\n",
            name );
#endif
        }

    if ( name )
        {
        strcpy( this->name, name );
        }
    else
        {
        memset( this->name, 0, sizeof( this->name ) );
        }

    if ( DI_cnt )
        {
        DI = new u_char [ DI_cnt ];
        memset( DI, 0, DI_cnt );
        }
    if ( DO_cnt )
        {
        DO = new u_char [ DO_cnt ];
        DO_ = new u_char [ DO_cnt ];
        memset( DO, 0, DO_cnt );
        memset( DO_, 0, DO_cnt );
        }
    if ( AI_cnt )
        {
        AI_offsets = new u_int [ AI_cnt ];
        AI_types = new u_int [ AI_cnt ];

        memset( AI, 0, sizeof( AI ) );
        }
    if ( AO_cnt )
        {
        AO_types = new u_int [ AO_cnt ];
        AO_offsets = new u_int [ AO_cnt ];

        memset( AO, 0, sizeof( AO ) );
        memset( AO_, 0, sizeof( AO ) );
        }
    }
//-----------------------------------------------------------------------------
void wago_manager::wago_node::print()
    {
#ifdef DEBUG
    Print( "\"%s\" - type %d, number %d, IP \"%s\". ",
        name, type, number, ip_address );
    Print( "DI %d, DO %d, AI %d [%d], AO %d [%d].\n",
        DI_cnt, DO_cnt, AI_cnt, AI_size, AO_cnt, AO_size );

    for ( u_int i = 0; i < AI_cnt; i++ )
        {
        if ( 0 == i )
            {
            Print( "\tAI\n");
            }
        Print( "\t%2.d %u %2.u\n", i + 1, AI_types[ i ], AI_offsets[ i ] );
        }

    for ( u_int i = 0; i < AO_cnt; i++ )
        {
        if ( 0 == i )
            {
            Print( "\tAO\n");
            }
        Print( "\t%2.d %u %2.u\n", i + 1, AO_types[ i ], AO_offsets[ i ] );
        }
#endif // DEBUG
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
wago_manager* G_WAGO_MANAGER()
    {
    return wago_manager::get_instance();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
