#if !defined WIN_OS && \
    !( defined LINUX_OS && defined PAC_PC ) && \
    !( defined LINUX_OS && defined PAC_WAGO_750_860 ) && \
    !( defined LINUX_OS && defined PAC_WAGO_PFC200 ) && \
    !( defined LINUX_OS && defined PAC_PLCNEXT )
#error You must define OS!
#endif

#include "lua_manager.h"

#include "dtime.h"

#ifdef WIN_OS
#include "bus_coupler_io_PC.h"
#endif

#if defined LINUX_OS && defined PAC_PC
#include "l_bus_coupler_io.h"
#endif

#if defined LINUX_OS && defined PAC_WAGO_750_860
#include "bus_coupler_io_w750.h"
#endif

#if defined LINUX_OS && defined PAC_WAGO_PFC200
#include "bus_coupler_io_PFC200.h"
#endif

#if defined LINUX_OS && defined PAC_PLCNEXT
#include "l_bus_coupler_io.h"
#endif

#include "log.h"

auto_smart_ptr < io_manager > io_manager::instance;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int io_device::get_DO( u_int index )
    {
    if ( index < DO_channels.count &&
        DO_channels.char_write_values &&
        DO_channels.char_write_values[ index ] )
        {
        return *DO_channels.char_write_values[ index ];
        }

    if ( G_DEBUG )
        {
        print();
        printf( "io_device->get_DO(...) - error! " );
        printf( "index = %d, DO_channels.count = %d, "
            "DO_channels.char_write_values = %d",
            index, DO_channels.count, ( int ) DO_channels.char_write_values );
        if ( DI_channels.char_write_values )
            {
            printf( ", DO_channels.char_write_values[ index ]=%d",
                ( int ) DO_channels.char_write_values[ index ] );
            }
        printf( "\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int io_device::set_DO( u_int index, char value )
    {
    if ( index < DO_channels.count &&
        DO_channels.char_write_values &&
        DO_channels.char_write_values[ index ] )
        {
        *DO_channels.char_write_values[ index ] = value;
        return 0;
        }

    if ( G_DEBUG )
        {
        print();
        printf( "io_device->set_DO(...) - error! %d, %d, %d, %d\n",
            index, DO_channels.count, ( int ) DO_channels.char_write_values,
            ( int ) DO_channels.char_write_values[ index ] );
        print();
        }

    return 1;
    }
//-----------------------------------------------------------------------------
int io_device::get_DI( u_int index )
    {
    if ( index < DI_channels.count &&
        DI_channels.char_read_values &&
        DI_channels.char_read_values[ index ] )
        {
        return *DI_channels.char_read_values[ index ];
        }

    if ( G_DEBUG )
        {
        print();
        printf( "io_device->get_DI(...) - error! " );
        printf( "index = %d, DI_channels.count = %d, "
            "DI_channels.char_read_values = %d",
            index, DI_channels.count, ( int ) DI_channels.char_read_values );
        if ( DI_channels.char_read_values )
            {
            printf( ", DI_channels.char_read_values[ index ]=%d",
                ( int ) DI_channels.char_read_values[ index ] );
            }
        printf( "\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
float io_device::get_AO( u_int index, float min_value, float max_value )
    {
    if ( index < AO_channels.count &&
        AO_channels.int_write_values &&
        AO_channels.int_write_values[ index ] )
        {
        float val = ( float ) *AO_channels.int_write_values[ index ];

        u_int table_n = AO_channels.tables[ index ];
        u_int offset = AO_channels.offsets[ index ];
        u_int module_type = G_IO_MANAGER()->get_node( table_n )->AO_types[ offset ];

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
            case 555:
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

                //  Output data     4 mA ... 20 mA
                //-----------------------------------------------------
                //  hex             dec             mA
                //-----------------------------------------------------
                //  7FFF...7F01                     21.3397
                //  7F00            32512           21.3397
                //  7530            30000           20
                //  3A98            15000           12
                //  1               1               4.0005333
                //  0               0               4
                //  FFFF            -1              4
                //  C568            -15000          4
                //  8AD0            -30000          4
                //  8100            -32512          4
                //  80FF...8000*                    Hold last value
                //  8001            Overrange       21.3397
                //  8080            Underrange      Hold last value
                //
                //  * without 8001, 8080

            case 2688527:   //-AXL F AO4 1H
                if (0 == min_value && 0 == max_value)
                    {
                    if (val < 0)
                        {
                        val = 4;
                        }
                    else
                        {
                        val = 4 + val / 1875.0f;
                        }
                    }
                else
                    {
                    if (val < 0)
                        {
                        val = 4;
                        }
                    else
                        {
                        val = 4 + val / 1875.0f;
                        }
                    val = min_value + (val - 4) * (max_value - min_value) / 16;
                    }

            return val;

            default:
                return val;
            }
        }

    if ( G_DEBUG )
        {
        print();
        printf( "io_device->get_AO(...) - error! " );
        printf( "index = %d, AO_channels.count = %d, "
            "AO_channels.int_write_values = %d",
            index, AO_channels.count, ( int ) AO_channels.int_write_values );
        if ( AO_channels.int_write_values )
            {
            printf( ", AO_channels.int_write_values[ index ]=%d",
                ( int ) AO_channels.int_write_values[ index ] );
            }
        printf( "\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int io_device::set_AO( u_int index, float value, float min_value,
                        float max_value )
    {
    if ( index < AO_channels.count &&
        AO_channels.int_write_values &&
        AO_channels.int_write_values[ index ] )
        {
        u_int table_n = AO_channels.tables[ index ];
        u_int offset = AO_channels.offsets[ index ];
        u_int module_type = G_IO_MANAGER()->get_node( table_n )->AO_types[ offset ];

        switch ( module_type )
            {
            case 554:
            case 555:
                if ( 0 != min_value || 0 != max_value )
                    {
                    value = 4 + 16 * ( value - min_value ) / ( max_value - min_value );
                    }
                if ( value < 4 ) value = 4;
                if ( value > 20 ) value = 20;
                value = 2047.5f * ( value - 4 );
                break;

            case 2688527:   //-AXL F AO4 1H
                if ( 0 != min_value || 0 != max_value )
                    {
                    value = 4 + 16 * ( value - min_value ) / ( max_value - min_value );
                    }
                if ( value < 4 ) value = 4;
                if ( value > 20 ) value = 20;
                value = 1875.0f * ( value - 4 );
                break;
            }

        *AO_channels.int_write_values[ index ] = ( u_int ) value;

        //if ( G_DEBUG )
        //    {
        //    printf("set_AO value=%d\n", ( u_int ) value );
        //    }
        return 0;
        }

    if ( G_DEBUG )
        {
        printf( "io_device->set_AO(...) - error!\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
float io_device::get_AI( u_int index, float min_value, float max_value )
    {
    if ( index < AI_channels.count &&
        AI_channels.int_read_values &&
        AI_channels.int_read_values[ index ] )
        {
        float val = ( float ) *AI_channels.int_read_values[ index ];

        u_int table_n = AI_channels.tables[ index ];
        u_int offset = AI_channels.offsets[ index ];
        u_int module_type = G_IO_MANAGER()->get_node( table_n )->AI_types[ offset ];

        switch ( module_type )
            {
            // Выход модуля 461.
            // Выход модуля 450, Pt100 Setting (IEC 751), Standard Format.
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
            case 450:
                if ( val >= -2000 && val <= -1 ) // -0,1..-200 °C
                    {
                    val *= 0.1f;
                    return val;
                    }
                if ( val >= 0 && val < 8500 ) // 0..850 °C
                    {
                    val *= 0.1f;
                    return val;
                    }

                return -1000;

                // Выход модуля 446.
                // Три наименее значащих бита не учитываются.
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
            case 496:
                if ( 3 == val )
                    {
                    return -1.;
                    }

                if ( 0 == min_value && 0 == max_value )
                    {
                    val = 4 + val / 2047.5f;
                    }
                else
                    {
                    val = 4 + val / 2047.5f;
                    val = min_value + ( val - 4 ) * ( max_value - min_value ) / 16;
                    }

                return val;

                //Тензорезистор
                // Process values of module 750-491
                //Signal           Numerical value
                //voltage UD       binary
                //
                //ca.-15.5000   '0111.1111.1111.1111' 0x7FFF 32767  0x41 on
                //ca.-15.5000   '0000.0000.0000.0000' 0x0000 0      0x00 off
                //-15.0000      '1000.1010.1101.0000' 0x8AD0 -30000 0x00 off
                //-10.0000      '1011.0001.1110.0000' 0xB1E0 -20000 0x00 off
                //-5.0000       '1101.1000.1111.0000' 0xD8F0 -10000 0x00 off
                //-0.0005       '1111.1111.1111.1111' 0xFFFF -1     0x00 off
                //0.0000        '0000.0000.0000.0000' 0x0000 0      0x00 off
                //0.0005        '0000.0000.0000.0001' 0x0001 1      0x00 off
                //5.0000        '0010.0111.0001.0000' 0x2710 10000  0x00 off
                //10.0000       '0100.1110.0010.0000' 0x4E20 20000  0x00 off
                //15.0000       '0111.0101.0011.0000' 0x7530 30000  0x00 off
                //>ca.15.5000   '0111.1111.1111.1111' 0x7FFF 32767  0x41 on
                //Broken wire   '0111.1111.1111.1111' 0x7FFF 32767  0x41 on
            case 491:
                if ( val >= 0x8AD0 && val <= 0xFFFF )
                    {
                    val -= 0x10000;
                    val *= 0.0005f;
                    return val;
                    }
                if ( val >= 0x0 && val <= 0x7530 )
                    {
                    val *= 0.0005f;
                    return val;
                    }
                return -1000;

			case 2688556: //RTD4 1H
				if (val < -32000 )
				{
					return -1000;
				}
				val *= 0.1f;
				return val;

			case 2688491: //AI4 1H
				if (val < -32000)
					{
					return -1;
					}

				if (0 == min_value && 0 == max_value)
					{
					val = 4 + val / 1875.0f;
					}
				else
					{
					val = 4 + val / 1875.0f;
					val = min_value + (val - 4) * (max_value - min_value) / 16;
					}
				return val;

            default:
                return val;
            }
        }

    if ( G_DEBUG )
        {
        print();
        printf( "io_device->get_AI(...) - error! " );
        printf( "index = %d, AI_channels.count = %d, "
            "AI_channels.int_read_values = %d",
            index, AI_channels.count, ( int ) AI_channels.int_read_values );
        if ( AI_channels.int_read_values )
            {
            printf( ", AI_channels.int_read_values[ index ]=%d",
                ( int ) AI_channels.int_read_values[ index ] );
            }
        printf( "\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int_2* io_device::get_AI_data( u_int index )
    {
    if ( index < AI_channels.count && AI_channels.int_read_values )
        {
        return AI_channels.int_read_values[ index ];
        }

    if ( G_DEBUG )
        {
        if ( G_USE_LOG )
            {
            sprintf( G_LOG->msg, "io_device->get_AI_data(...) - error");
            G_LOG->write_log(i_log::P_ERR );
            }
        else
            {
            print();

            printf("io_device->get_AI_data(...) - error! ");
            printf("index = %d, AI_channels.count = %d, "
                    "AI_channels.int_read_values = %d", index,
                    AI_channels.count, (int) AI_channels.int_read_values);
            if (AI_channels.int_read_values)
                {
                printf(", AI_channels.int_read_values[ index ]=%d",
                        (int) AI_channels.int_read_values[index]);
                }
            printf("\n");
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
io_device::IOLINKSTATE io_device::get_AI_IOLINK_state(u_int index)
	{
	if (vendor == PHOENIX)
	{
		if (index < AI_channels.count && AI_channels.int_module_read_values[index])
			{
			u_int module_states = *((u_int*)(AI_channels.int_module_read_values[index]));
			int_2 logical_port = AI_channels.logical_ports[index];
			if (logical_port > 0 && logical_port <= 8)
				{
				bool iolinkconnected = module_states & (1 << (logical_port - 1));
				bool iolinkdatavalid = module_states & (1 << (logical_port + 7));
				if (!iolinkconnected) return IOLINKSTATE::NOTCONNECTED;
				if (!iolinkdatavalid) return IOLINKSTATE::DEVICEERROR;
				}
			}
	}
	return IOLINKSTATE::OK;
	}

io_device::IOLINKSTATE io_device::get_AO_IOLINK_state(u_int index)
	{
	if (vendor == PHOENIX)
		{
		if (index < AO_channels.count && AO_channels.int_module_read_values[index])
			{
			u_int module_states = *((u_int*)(AO_channels.int_module_read_values[index]));
			int_2 logical_port = AO_channels.logical_ports[index];
			if (logical_port > 0 && logical_port <= 8)
				{
				bool iolinkconnected = module_states & (1 << (logical_port - 1));
				bool iolinkdatavalid = module_states & (1 << (logical_port + 7));
				if (!iolinkconnected) return IOLINKSTATE::NOTCONNECTED;
				if (!iolinkdatavalid) return IOLINKSTATE::DEVICEERROR;
				}
			}
		}
	return IOLINKSTATE::OK;
	}

//-----------------------------------------------------------------------------
int_2* io_device::get_AO_write_data( u_int index )
    {
    if ( index < AO_channels.count && AO_channels.int_write_values )
        {
        return AO_channels.int_write_values[ index ];
        }

    if ( G_DEBUG )
        {
        print();
        printf( "io_device->get_AO_write_data(...) - error! " );
        printf( "index = %d, AO_channels.count = %d, "
            "AO_channels.int_write_values = %d",
            index, AO_channels.count, ( int ) AO_channels.int_write_values );
        if ( AO_channels.int_write_values )
            {
            printf( ", AO_channels.int_write_values[ index ]=%d",
                ( int ) AO_channels.int_write_values[ index ] );
            }
        printf( "\n" );
        }
    return 0;
    }
//-----------------------------------------------------------------------------
int_2* io_device::get_AO_read_data( u_int index )
    {
    if ( index < AO_channels.count && AO_channels.int_read_values )
        {
        return AO_channels.int_read_values[ index ];
        }

    if ( G_DEBUG )
        {
        print();
        printf( "io_device->get_AO_read_data(...) - error! " );
        printf( "index = %d, AO_channels.count = %d, "
            "AO_channels.int_read_values = %d",
            index, AO_channels.count, ( int ) AO_channels.int_read_values );
        if ( AO_channels.int_read_values )
            {
            printf( ", AO_channels.int_read_values[ index ]=%d",
                ( int ) AO_channels.int_read_values[ index ] );
            }
        printf( "\n" );
        }
    return 0;
    }
//-----------------------------------------------------------------------------
void io_device::print() const
    {
    printf( " " );

    DI_channels.print();
    DO_channels.print();
    AI_channels.print();
    AO_channels.print();
    //printf( "\n" );
    }
//-----------------------------------------------------------------------------
io_device::io_device( const char* name ) : name( name ),
    DI_channels( IO_channels::CT_DI ),
    DO_channels( IO_channels::CT_DO ),
    AI_channels( IO_channels::CT_AI ),
    AO_channels( IO_channels::CT_AO )
    {
    }
//-----------------------------------------------------------------------------
io_device::~io_device()
    {
    }
//-----------------------------------------------------------------------------
void io_device::init( int DO_count, int DI_count, int AO_count,
                       int AI_count )
    {
    this->vendor = vendor;
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
io_device::VENDOR io_device::get_io_vendor() const
    {
    return vendor;
    }
//-----------------------------------------------------------------------------
void io_device::set_io_vendor( VENDOR vendor )
    {
    this->vendor = vendor;
    }
//-----------------------------------------------------------------------------
void io_device::init_channel( int type, int ch_inex, int node, int offset, int module_offset /*= -1*/, int logical_port /*= -1 */ )
    {
    switch ( type )
        {
        case IO_channels::CT_DI:
            DI_channels.init_channel( ch_inex, node, offset, module_offset, logical_port );
            break;

        case IO_channels::CT_DO:
            DO_channels.init_channel( ch_inex, node, offset, module_offset, logical_port );
            break;

        case IO_channels::CT_AI:
            AI_channels.init_channel( ch_inex, node, offset, module_offset, logical_port );
            break;

        case IO_channels::CT_AO:
            AO_channels.init_channel( ch_inex, node, offset, module_offset, logical_port );
            break;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
io_device::IO_channels::IO_channels( CHANNEL_TYPE type ) : count( 0 ),
    tables( 0 ),
    offsets( 0 ),
	module_offsets ( 0 ),
	logical_ports ( 0 ),
    int_read_values( 0 ), int_module_read_values(0),
	int_write_values( 0 ),
    char_read_values( 0 ), char_write_values( 0 ),
    type( type )
    {
    }
//-----------------------------------------------------------------------------
io_device::IO_channels::~IO_channels()
    {
    if ( count )
        {
        delete [] tables;
        delete [] offsets;
		delete [] module_offsets;
		delete [] logical_ports;
        count = 0;
        }
    if ( int_read_values )
        {
        delete [] int_read_values;
        int_read_values = 0;
        }
	if (int_module_read_values)
		{
		delete[] int_module_read_values;
		int_module_read_values = 0;
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
void io_device::IO_channels::init( int ch_count )
    {
    if ( ch_count > 0 )
        {
        count = ch_count;

        tables  = new u_int[ count ];
        offsets = new u_int[ count ];
		module_offsets = new int[ count ];
		logical_ports = new int[ count ];

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
                int_read_values = new int_2*[ count ];
				int_module_read_values = new int_2*[count];
                break;

            case IO_channels::CT_AO:
                int_read_values  = new int_2*[ count ];
				int_module_read_values = new int_2*[count];
                int_write_values = new int_2*[ count ];
                break;
            }
        }
    }
//-----------------------------------------------------------------------------
void io_device::IO_channels::print() const
    {
    if ( count )
        {
        switch ( type )
            {
            case CT_DI:
                printf( "DI" );
                break;

            case CT_DO:
                printf( "DO" );
                break;

            case CT_AI:
                printf( "AI" );
                break;

            case CT_AO:
                printf( "AO" );
                break;
            }

        printf( ":%d; ", count );
        //if ( count )
        //    {
        //    printf( "[ " );
        //    for ( u_int i = 0; i < count; i++ )
        //        {
        //        printf("%d:%2d", tables[ i ], offsets[ i ] );
        //        if ( i < count - 1 ) printf( "; " );
        //        }
        //    printf( " ]" );
        //    }
        //printf( "; " );
        }
    }
//-----------------------------------------------------------------------------
void io_device::IO_channels::init_channel( u_int ch_index, int node, int offset, int module_offset, int logical_port )
    {
    if ( ch_index < count )
        {
        tables[ ch_index ]  = node;
        offsets[ ch_index ] = offset;
		module_offsets[ ch_index ] = module_offset;
		logical_ports[ ch_index ] = logical_port;

        switch ( type )
            {
            case CT_DI:
                char_read_values[ ch_index ] = io_manager::get_instance()->
                    get_DI_read_data( tables[ ch_index ], offsets[ ch_index ] );
                break;

            case CT_DO:
                char_read_values[ ch_index ] = io_manager::get_instance()->
                    get_DO_read_data( tables[ ch_index ], offsets[ ch_index ] );
                char_write_values[ ch_index ] = io_manager::get_instance()->
                    get_DO_write_data( tables[ ch_index ], offsets[ ch_index ] );
                break;

            case CT_AI:
                int_read_values[ ch_index ] = io_manager::get_instance()->
                    get_AI_read_data( tables[ ch_index ], offsets[ ch_index ] );
				if (module_offsets[ch_index] >= 0)
					{
					int_module_read_values[ch_index] = io_manager::get_instance()->
						get_AI_read_data(tables[ch_index], module_offsets[ch_index]);
					}
				else
					{
					int_module_read_values[ch_index] = 0;
					}
                break;

            case CT_AO:
                int_read_values[ ch_index ] = io_manager::get_instance()->
                    get_AO_read_data( tables[ ch_index ], offsets[ ch_index ] );
                int_write_values[ ch_index ] = io_manager::get_instance()->
                    get_AO_write_data( tables[ ch_index ], offsets[ ch_index ] );
				if (module_offsets[ch_index] >= 0)
					{
					int_module_read_values[ch_index] = io_manager::get_instance()->
						get_AI_read_data(tables[ch_index], module_offsets[ch_index]);
					}
				else
					{
					int_module_read_values[ch_index] = 0;
					}
                break;
            }
        }


    else
        {
        if ( G_DEBUG )
            {
            printf( "Error io_device::IO_channels::init_channel - index out of bound!\n" );
            }
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void io_manager::init( int nodes_count )
    {
    this->nodes_count = nodes_count;

    if ( nodes_count )
        {
        nodes = new io_node*[ nodes_count ];
        for ( int i = 0; i < nodes_count; i++ )
            {
            nodes[ i ] = 0;
            }
        }

    }
//-----------------------------------------------------------------------------
io_manager* io_manager::get_instance()
    {
    if ( instance.is_null() )
        {
#ifdef WIN_OS
        instance = new io_manager_PC();
#endif // WIN_OS

#if defined LINUX_OS && defined PAC_PC
        instance = new io_manager_linux();
#endif // defined LINUX_OS && defined PAC_PC

#if defined LINUX_OS && defined PAC_WAGO_750_860
        instance = new io_manager_w750();
#endif // defined LINUX_OS && defined PAC_WAGO_750_860

#if defined LINUX_OS && defined PAC_WAGO_PFC200
        instance = new io_manager_PFC200();
#endif // defined LINUX_OS && defined PAC_WAGO_750_860

#if defined LINUX_OS && defined PAC_PLCNEXT
        instance = new io_manager_linux();
#endif // defined LINUX_OS && defined PAC_PC
        }

    return instance;
    }
//-----------------------------------------------------------------------------
u_char* io_manager::get_DI_read_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->DI_cnt )
            {
            return &nodes[ node_n ]->DI[ offset ];
            }
        }

    if ( G_DEBUG )
        {
        printf( "io_manager::get_DI_data() - error!\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
u_char* io_manager::get_DO_read_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->DO_cnt )
            {
            return &nodes[ node_n ]->DO[ offset ];
            }
        }
    if ( G_DEBUG )
        {
        printf( "io_manager::get_DO_data() - error!\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int_2* io_manager::get_AI_read_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < io_node::C_ANALOG_BUF_SIZE )
            {
            return &( nodes[ node_n ]->AI[ offset ] );
            }
        }
    if ( G_DEBUG )
        {
        printf( "io_manager::get_AI_data() - error!\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int_2* io_manager::get_AO_read_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < io_node::C_ANALOG_BUF_SIZE )
            {
            return &nodes[ node_n ]->AO[ offset ];
            }
        }
    if ( G_DEBUG )
        {
        printf( "io_manager::get_AO_data() - error!\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
u_char* io_manager::get_DO_write_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < nodes[ node_n ]->DO_cnt )
            {
            return &nodes[ node_n ]->DO_[ offset ];
            }
        }
    if ( G_DEBUG )
        {
        printf( "io_manager::get_DO_write_data() - error!\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int_2* io_manager::get_AO_write_data( u_int node_n, u_int offset )
    {
    if ( node_n < nodes_count && nodes )
        {
        if ( nodes[ node_n ] && offset < io_node::C_ANALOG_BUF_SIZE )
            {
            return &nodes[ node_n ]->AO_[ offset ];
            }
        }
    if ( G_DEBUG )
        {
        printf( "io_manager::get_AO_write_data() - error!\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
io_manager::io_manager() :nodes_count( 0 ), nodes( 0 )
    {
    }
//-----------------------------------------------------------------------------
io_manager::~io_manager()
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
io_manager::io_node * io_manager::get_node( int node_n )
    {
    return nodes[ node_n ];
    }
u_int io_manager::get_nodes_count()
	{
	return nodes_count;
	}
//-----------------------------------------------------------------------------
void io_manager::add_node( u_int index, int ntype, int address,
    char* IP_address, char *name,
    int DO_cnt, int DI_cnt,
    int AO_cnt, int AO_size, int AI_cnt, int AI_size )
    {
    if ( index < nodes_count )
        {
        nodes[ index ] = new io_node( ntype, address, IP_address, name, DO_cnt,
            DI_cnt, AO_cnt, AO_size, AI_cnt, AI_size );
        }
    }
//-----------------------------------------------------------------------------
void io_manager::init_node_AO( u_int node_index, u_int AO_index,
                                u_int type, u_int offset )
    {
    if ( node_index < nodes_count && AO_index < nodes[ node_index ]->AO_cnt )
        {
        nodes[ node_index ]->AO_types[ AO_index ]   = type;
        nodes[ node_index ]->AO_offsets[ AO_index ] = offset;
        }
    }
//-----------------------------------------------------------------------------
void io_manager::init_node_AI( u_int node_index, u_int AI_index,
                                u_int type, u_int offset )
    {
    if ( node_index < nodes_count && AI_index < nodes[ node_index ]->AI_cnt )
        {
        nodes[ node_index ]->AI_types[ AI_index ]   = type;
        nodes[ node_index ]->AI_offsets[ AI_index ] = offset;
        }
    }
void io_manager::disconnect(io_node * node)
	{
	return;
	}
//-----------------------------------------------------------------------------
void io_manager::print() const
    {
    printf( "I\\O manager [%d]:\n", nodes_count );
    for ( u_int i = 0; i < nodes_count; i++ )
        {
        nodes[ i ]->print();
        }
    }
//-----------------------------------------------------------------------------
void io_manager::print_log() const
    {
    sprintf(G_LOG->msg, "I\\O manager [%d]:\n", nodes_count);
    G_LOG->write_log(i_log::P_DEBUG);

    for (u_int i = 0; i < nodes_count; i++)
        {
        nodes[i]->print_log();
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
io_manager::io_node::~io_node()
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
io_manager::io_node::io_node( int type, int number, char *str_ip_address,
                                   char *name,
                                   int DO_cnt, int DI_cnt, int AO_cnt, int AO_size, int AI_cnt,
                                   int AI_size ): state( ST_NO_CONNECT ),
                                   type( ( TYPES ) type ),
                                   number( number ),

                                   is_active( true ),

                                   last_poll_time( get_millisec() ),
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

    if ( ip_address[ 0 ] == 0 && type >= WAGO_750_XXX_ETHERNET )
        {
        is_active = false;
        sprintf( G_LOG->msg,
            "Узел \"%s\" отключен, так как не задан его IP адрес.", name );
        G_LOG->write_log( i_log::P_NOTICE );
        }

    if ( type == EMPTY )
        {
        is_active = false;
        sprintf( G_LOG->msg,
            "Узел \"%s\" отключен, так как не задан его тип.", name );
        G_LOG->write_log( i_log::P_NOTICE );
        }

    if ( name )
        {
        strcpy( this->name, name );
        }
    else
        {
        memset( this->name, 0, sizeof( this->name ) );
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

    if (DI_cnt)
        {
        DI = new u_char[DI_cnt];
        memset(DI, 0, DI_cnt);
        }

    if ( DO_cnt )
        {
        DO = new u_char [ DO_cnt ];
        DO_ = new u_char [ DO_cnt ];
        memset( DO, 0, DO_cnt );
        memset( DO_, 0, DO_cnt );
        }
    }

//-----------------------------------------------------------------------------
void io_manager::io_node::print()
    {
    printf( "\"%s\" - type %d, number %d, IP \"%s\". ",
        name, type, number, ip_address );
    printf( "DI %d, DO %d, AI %d [%d], AO %d [%d].\n",
        DI_cnt, DO_cnt, AI_cnt, AI_size, AO_cnt, AO_size );

    for ( u_int i = 0; i < AI_cnt; i++ )
        {
        if ( 0 == i )
            {
            printf( "\tAI\n");
            }
        printf( "\t%2.d %u %2.u\n", i + 1, AI_types[ i ], AI_offsets[ i ] );
        }

    for ( u_int i = 0; i < AO_cnt; i++ )
        {
        if ( 0 == i )
            {
            printf( "\tAO\n");
            }
        printf( "\t%2.d %u %2.u\n", i + 1, AO_types[ i ], AO_offsets[ i ] );
        }
    }
//-----------------------------------------------------------------------------
void io_manager::io_node::print_log()
    {
    sprintf( G_LOG->msg, "\"%s\" - type %d, number %d, IP \"%s\".", name, type,
            number, ip_address);
    G_LOG->write_log(i_log::P_DEBUG);

    sprintf( G_LOG->msg, "DI %d [%d], DO %d [%d], AI %d [%d], AO %d [%d].",
            DI_cnt, DI_cnt / 8, DO_cnt, DO_cnt / 8, AI_cnt, AI_size, AO_cnt,
            AO_size);
    G_LOG->write_log(i_log::P_DEBUG);

    if (AI_cnt > 0)
        {
        sprintf( G_LOG->msg, "AI");
        G_LOG->write_log(i_log::P_DEBUG);
        }
    for (u_int i = 0; i < AI_cnt; i++)
        {
        sprintf( G_LOG->msg, "\t%2.d %u %2.u", i + 1, AI_types[i],
                AI_offsets[i]);
        G_LOG->write_log(i_log::P_DEBUG);
        }

    if (AO_cnt > 0)
        {
        sprintf( G_LOG->msg, "AO");
        G_LOG->write_log(i_log::P_DEBUG);
        }
    for (u_int i = 0; i < AO_cnt; i++)
        {
        sprintf( G_LOG->msg, "\t%2.d %u %2.u", i + 1, AO_types[i],
                AO_offsets[i]);
        G_LOG->write_log(i_log::P_DEBUG);
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
io_manager* G_IO_MANAGER()
    {
    return io_manager::get_instance();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
