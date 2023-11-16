#if !defined WIN_OS && \
    !( defined LINUX_OS && defined PAC_PC ) && \
    !( defined LINUX_OS && defined PAC_WAGO_750_860 ) && \
    !( defined LINUX_OS && defined PAC_WAGO_PFC200 ) && \
    !( defined LINUX_OS && defined PAC_PLCNEXT )
#error You must define OS!
#endif

#include "lua_manager.h"

#include "dtime.h"

#if defined LINUX_OS && defined PAC_WAGO_750_860
#include "bus_coupler_io_w750.h"
#endif

#if defined LINUX_OS && defined PAC_WAGO_PFC200
#include "bus_coupler_io_PFC200.h"
#endif

#include "log.h"
#include "PAC_err.h"

#ifdef WIN_OS
#pragma warning(push)
#pragma warning(disable: 26812) //Prefer 'enum class' over 'enum'.
#endif // WIN_OS

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
        printf( "index = %d, count = %u, char_write_values = %p",
            index, DO_channels.count, DO_channels.char_write_values );
        if ( DO_channels.char_write_values )
            {
            printf( ", char_write_values[ index ]=%p",
                DO_channels.char_write_values[ index ] );
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
        printf( "io_device->set_DO(...) - error! %d, %d, %p",
            index, DO_channels.count, DO_channels.char_write_values );
        if ( DO_channels.char_write_values )
            {
            printf( ", char_write_values[ index ]=%p",
                DO_channels.char_write_values[ index ] );
            }
        printf( "\n" );
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
            "DI_channels.char_read_values = %p",
            index, DI_channels.count, DI_channels.char_read_values );
        if ( DI_channels.char_read_values )
            {
            printf( ", DI_channels.char_read_values[ index ]=%p",
                DI_channels.char_read_values[ index ] );
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

            case 2688527:   //AXL F AO4 1H
            case 2702072:   //AXL F AI2 AO2 1H
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


                //  Output data                 4 mA ... 20 mA
                //-----------------------------------------------------
                //  hex             dec                         mА
                //-----------------------------------------------------
                //  8001            Output range overrange      + 21.339
                //  7FFF ... 43BC   32767 ... 17340             + 21.339
                //  43BB            17339                       + 21.339
                //  3E80            16000                       + 20.0
                //  2710            10000                       + 14.0
                //  1770            6000                        + 10.0
                //  1388            5000                        + 9.0
                //  03E8            1000                        + 5.0
                //  0001            1                           + 4.001
                //  0000            0                           + 4.0
                //  FFFF ... 8100   -1 ... -32512               + 4.0
                //  80FF ... 8000*  -32513 ... -32767           Hold last value
                //  8080            Output range underrange     Hold last value
                //  * without 8001, 8080
            case 1088123:   //AXL SE AO4 I 4-20
                if ( 0 == min_value && 0 == max_value )
                    {
                    if ( val < 0 )
                        {
                        val = 4;
                        }
                    else
                        {
                        val = 4 + val / 1000.0f;
                        }
                    }
                else
                    {
                    if ( val < 0 )
                        {
                        val = 4;
                        }
                    else
                        {
                        val = 4 + val / 1000.0f;
                        }
                    val = min_value + ( val - 4 ) * ( max_value - min_value ) / 16;
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
            "AO_channels.int_write_values = %p",
            index, AO_channels.count, AO_channels.int_write_values );
        if ( AO_channels.int_write_values )
            {
            printf( ", AO_channels.int_write_values[ index ]=%p",
                AO_channels.int_write_values[ index ] );
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

            case 2688527:   //AXL F AO4 1H
            case 2702072:   //AXL F AI2 AO2 1H
                if ( 0 != min_value || 0 != max_value )
                    {
                    value = 4 + 16 * ( value - min_value ) / ( max_value - min_value );
                    }
                if ( value < 4 ) value = 4;
                if ( value > 20 ) value = 20;
                value = 1875.0f * ( value - 4 );
                break;

            case 1088123:   //AXL SE AO4 I 4-20
                if ( 0 != min_value || 0 != max_value )
                    {
                    value = 4 + 16 * ( value - min_value ) / ( max_value - min_value );
                    }
                if ( value < 4 ) value = 4;
                if ( value > 20 ) value = 20;
                value = 1000.0f * ( value - 4 );
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
                if ( val >= -30000 && val <= 30000 )
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

            case 2688491:   //AXL F AI4 I 1H
            case 2702072:   //AXL F AI2 AO2 1H
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

            case 1088062:   //AXL SE AI4 I 4-20
                if ( val < -32000 )
                    {
                    return -1;
                    }

                if ( 0 == min_value && 0 == max_value )
                    {
                    val = 4 + val / 1000.0f;
                    }
                else
                    {
                    val = 4 + val / 1000.0f;
                    val = min_value + ( val - 4 ) * ( max_value - min_value ) / 16;
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
            "AI_channels.int_read_values = %p",
            index, AI_channels.count, AI_channels.int_read_values );
        if ( AI_channels.int_read_values )
            {
            printf( ", AI_channels.int_read_values[ index ]=%p",
                AI_channels.int_read_values[ index ] );
            }
        printf( "\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int_2* io_device::get_AI_data( u_int index ) const
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
                    "AI_channels.int_read_values = %p", index,
                    AI_channels.count, AI_channels.int_read_values);
            if (AI_channels.int_read_values)
                {
                printf(", AI_channels.int_read_values[ index ]=%p",
                        AI_channels.int_read_values[index]);
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
            "AO_channels.int_write_values = %p",
            index, AO_channels.count, AO_channels.int_write_values );
        if ( AO_channels.int_write_values )
            {
            printf( ", AO_channels.int_write_values[ index ]=%p",
                AO_channels.int_write_values[ index ] );
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
            "AO_channels.int_read_values = %p",
            index, AO_channels.count, AO_channels.int_read_values );
        if ( AO_channels.int_read_values )
            {
            printf( ", AO_channels.int_read_values[ index ]=%p",
                AO_channels.int_read_values[ index ] );
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
io_device::io_device( const char* name ) : name( name ), vendor( PHOENIX ),
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
                char_read_values = new u_char*[ count ]{ nullptr };
                break;

            case IO_channels::CT_DO:
                char_read_values  = new u_char*[ count ]{ nullptr };
                char_write_values = new u_char*[ count ]{ nullptr };
                break;

            case IO_channels::CT_AI:
                int_read_values = new int_2*[ count ]{ nullptr };
				int_module_read_values = new int_2*[count]{ nullptr };
                break;

            case IO_channels::CT_AO:
                int_read_values  = new int_2*[ count ]{ nullptr };
				int_module_read_values = new int_2*[count]{ nullptr };
                int_write_values = new int_2*[ count ]{ nullptr };
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
        instance = new io_manager();
#endif // WIN_OS

#if defined LINUX_OS && defined PAC_PC
        instance = new io_manager();
#endif // defined LINUX_OS && defined PAC_PC

#if defined LINUX_OS && defined PAC_WAGO_750_860
        instance = new io_manager_w750();
#endif // defined LINUX_OS && defined PAC_WAGO_750_860

#if defined LINUX_OS && defined PAC_WAGO_PFC200
        instance = new io_manager_PFC200();
#endif // defined LINUX_OS && defined PAC_WAGO_750_860

#if defined LINUX_OS && defined PAC_PLCNEXT
        instance = new io_manager();
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
    writebuff = &buff[ 13 ];
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
        nodes = nullptr;
        nodes_count = 0;
        }
    }
//-----------------------------------------------------------------------------
io_manager::io_node* io_manager::get_node(int node_n)
{
    if (node_n >= 0 && node_n < nodes_count)
    {
        return nodes[node_n];
    }
    else
    {
        return nullptr;
    }
}
u_int io_manager::get_nodes_count()
	{
	return nodes_count;
	}
//-----------------------------------------------------------------------------
void io_manager::add_node( u_int index, int ntype, int address,
    const char* IP_address, const char *name,
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
int io_manager::net_init( io_node* node ) const
    {
    int type = SOCK_STREAM;
    int protocol = 0; /* всегда 0 */
    int err;
    int sock = socket( AF_INET, type, protocol ); // Cоздание сокета.

    if ( sock < 0 )
        {
        fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
            "Network communication : can't create I/O node socket : {}.",
            strerror( errno ) );
        G_LOG->write_log( i_log::P_CRIT );

        return -4;
        }

    // Адресация мастер-сокета.
    struct sockaddr_in socket_remote_server;
    const int PORT = 502;
    memset( &socket_remote_server, 0, sizeof( socket_remote_server ) );
    socket_remote_server.sin_family = AF_INET;
    socket_remote_server.sin_addr.s_addr = inet_addr( node->ip_address );
    socket_remote_server.sin_port = htons( PORT );

    const char C_ON = 1;

    if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &C_ON, sizeof( C_ON ) ) )
        {
        fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
            "Network communication : can't setsockopt I/O node socket : {}.",
            strerror( errno ) );
        G_LOG->write_log( i_log::P_CRIT );

#ifdef WIN_OS
        closesocket( sock );
#else
        close( sock );
#endif

        return -5;
        }

    // Переводим в неблокирующий режим.
#ifdef WIN_OS
    u_long mode = 1;
    err = ioctlsocket( sock, FIONBIO, &mode );
#else
    err = fcntl( sock, F_SETFL, O_NONBLOCK );
#endif
    if ( err != 0 )
        {
        fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
            "Network communication : can't fcntl I/O node socket : {}.",
            strerror( errno ) );
        G_LOG->write_log( i_log::P_CRIT );

#ifdef WIN_OS
        closesocket( sock );
#else
        close( sock );
#endif
        return -5;
        }

    // Привязка сокета. Сразу возвращает управление в неблокирующем режиме.
    connect( sock, (struct sockaddr*)&socket_remote_server,
        sizeof( socket_remote_server ) );

    fd_set rdevents;
    struct timeval tv;
    FD_ZERO( &rdevents );
    FD_SET( sock, &rdevents );

    tv.tv_sec = 0;
    tv.tv_usec = io_node::C_CNT_TIMEOUT_US;

    err = select( sock + 1, nullptr, &rdevents, nullptr, &tv );

    if ( err <= 0 )
        {
        if ( node->is_set_err == false )
            {
            if ( err < 0 )
                {
                fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
                    R"raw(Network device : s{}->\"{}\":\"{}\""
                    " can't connect : {}.")raw",
                    sock, node->name, node->ip_address, strerror( errno ) );
                G_LOG->write_log( i_log::P_CRIT );
                }
            else // = 0
                {
                fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
                    R"raw(Network device : s{}->\"{}\":\"{}\""
                    " can't connect : timeout ({} ms).")raw",
                    sock, node->name, node->ip_address,
                    io_node::C_CNT_TIMEOUT_US / 1000 );

                G_LOG->write_log( i_log::P_CRIT );
                }
            }

#ifdef WIN_OS
        closesocket( sock );
#else
        close( sock );
#endif
        return -5;
        }

    if ( FD_ISSET( sock, &rdevents ) )
        {
        socklen_t err_len;
        char error;

        err_len = sizeof( error );
        if ( getsockopt( sock, SOL_SOCKET, SO_ERROR, &error, &err_len ) < 0 || error != 0 )
            {
            if ( node->is_set_err == false )
                {
                fmt::format_to_n( G_LOG->msg, i_log::C_BUFF_SIZE,
                    R"raw(Network device : s{}->\"{}\":\"{}\""
                    " error during connect : {}.")raw",
                    sock, node->name, node->ip_address,
#ifdef WIN_OS
                    WSA_Last_Err_Decode()
#else
                    strerror( errno )
#endif // WIN_OS
                );
                G_LOG->write_log( i_log::P_CRIT );
                }

#ifdef WIN_OS
            closesocket( sock );
#else
            close( sock );
#endif
            return -6;
            }
        }

    if ( G_DEBUG )
        {
        printf( "io_manager_windows:net_init() : socket %d is successfully"
            " connected to \"%s\":\"%s\":%d\n",
            sock, node->name, node->ip_address, PORT );
        }

    node->sock = sock;
    node->state = io_node::ST_OK;

    return 0;
    }
//-----------------------------------------------------------------------------
void io_manager::disconnect( io_node* node ) const
    {
    if ( node->sock )
        {
#ifdef LINUX_OS
        shutdown( node->sock, SHUT_RDWR );
        close( node->sock );        
#else
        shutdown( node->sock, SD_BOTH );
        closesocket( node->sock );
#endif        
        node->sock = 0;
        }
    node->state = io_node::ST_NO_CONNECT;
    }
//-----------------------------------------------------------------------------
int io_manager::write_outputs()
    {
    if ( 0 == nodes_count ) return 0;

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        io_node* nd = nodes[ i ];
        if ( nd->type == io_node::WAGO_750_XXX_ETHERNET )
            {
            if ( !nd->is_active )
                {
                continue;
                }

            if ( nd->DO_cnt > 0 )
                {
                u_int bytes_cnt = nd->DO_cnt / 8 + ( nd->DO_cnt % 8 > 0 ? 1 : 0 );

                buff[ 0 ] = 's';
                buff[ 1 ] = 's';
                buff[ 2 ] = 0;
                buff[ 3 ] = 0;
                buff[ 4 ] = 0;
                buff[ 5 ] = static_cast<unsigned char>(7 + bytes_cnt);
                buff[ 6 ] = 0; //nodes[ i ]->number;
                buff[ 7 ] = 0x0F;
                buff[ 8 ] = 0;
                buff[ 9 ] = 0;
                buff[ 10 ] = (unsigned char)nd->DO_cnt >> 7 >> 1;
                buff[ 11 ] = (unsigned char)nd->DO_cnt & 0xFF;
                buff[ 12 ] = static_cast<unsigned char>(bytes_cnt);

                for ( u_int j = 0, idx = 0; j < bytes_cnt; j++ )
                    {
                    u_char b = 0;
                    for ( u_int k = 0; k < 8 && idx < nd->DO_cnt; k++ )
                        {
                           b |= ( nd->DO_[ idx ] & 1 ) << k;
                           idx++;
                        }
                    buff[ j + 13 ] = b;
                    }

                if ( e_communicate( nd, bytes_cnt + 13, 12 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x0F )
                        {
                        memcpy( nd->DO, nd->DO_, nd->DO_cnt );
                        }
                    }// if ( e_communicate( nd, bytes_cnt + 13, 12 ) > 0 )
                else
                    {
                    if ( G_DEBUG )
                        {
                        //printf("\nWrite DO: returned error...\n");
                        }
                    }
                }// if ( nd->DO_cnt > 0 )

            if ( nd->AO_cnt > 0 )
                {
                u_int bytes_cnt = nd->AO_size;

                buff[ 0 ] = 's';
                buff[ 1 ] = 's';
                buff[ 2 ] = 0;
                buff[ 3 ] = 0;
                buff[ 4 ] = 0;
                buff[ 5 ] = static_cast<unsigned char>(7 + bytes_cnt);
                buff[ 6 ] = 0; //nodes[ i ]->number;
                buff[ 7 ] = 0x10;
                buff[ 8 ] = 0;
                buff[ 9 ] = 0;
                buff[ 10 ] = static_cast<unsigned char>(bytes_cnt / 2 >> 8);
                buff[ 11 ] = bytes_cnt / 2 & 0xFF;
                buff[ 12 ] = static_cast<unsigned char>(bytes_cnt);

                for ( unsigned int idx = 0, l = 0; idx < nd->AO_cnt; idx++ )
                    {
                    if (nd->AO_types[idx] == 638)
                    {
                        buff[13 + l] = 0;
                        buff[13 + l + 1] = 0;
                        buff[13 + l + 2] = 0;
                        buff[13 + l + 3] = 0;
                        l += 4;
                    }
                    else
                    {
                        buff[ 13 + l ] = (u_char)( ( nd->AO_[ idx ] >> 8 ) & 0xFF );
                        buff[ 13 + l + 1 ] = (u_char)( nd->AO_[ idx ] & 0xFF );
                        l += 2;
                    }
                }

                if ( e_communicate( nd, bytes_cnt + 13, 12 ) == 0 )
                    {
                    if ( buff[ 7 ] == 0x10 )
                        {
                        memcpy( nd->AO, nd->AO_, sizeof( nd->AO ) );
                        }
                    }// if ( e_communicate( nd, 2 * bytes_cnt + 13, 12 ) == 0 )
                else
                    {
                    if ( G_DEBUG )
                        {
                        //printf("\nWrite AO: returned error...\n");
                        }
                    }
                }// if ( nd->AO_cnt > 0 )

            }// if ( nd->type == io_node::T_750_341 || ...
        }// for ( u_int i = 0; i < nodes_count; i++ )

    for ( u_int i = 0; i < nodes_count; i++ )
        {
        io_node* nd = nodes[ i ];
        u_int ao_module_type = 0;
        u_int ao_module_offset = 0;

        if ( nd->type == io_node::PHOENIX_BK_ETH )
            {
            if ( !nd->is_active )
                {
                continue;
                }

            if ( nd->AO_cnt > 0 )
                {
                unsigned int start_register = 0;
                auto start_write_address = static_cast<unsigned int>(CONSTANTS::PHOENIX_HOLDINGREGISTERS_STARTADDRESS);
                unsigned int registers_count;

                if ( nd->AO_cnt > static_cast<unsigned int>(CONSTANTS::MAX_MODBUS_REGISTERS_PER_QUERY) )
                    {
                    registers_count = static_cast<unsigned int>(CONSTANTS::MAX_MODBUS_REGISTERS_PER_QUERY);
                    }
                else
                    {
                    registers_count = nd->AO_cnt;
                    }

                int bit_src = 0;

                do
                    {
                    for ( u_int j = 0; j < registers_count * 2; j++ )
                        {
                        u_char b = 0;
                        for ( u_int k = 0; k < 8; k++ )
                            {
                            b |= static_cast<unsigned char>( ( nd->DO_[ bit_src ] & 1 ) << k );
                            bit_src++;
                            }
                        writebuff[ j ] = b;
                        }

                    for ( unsigned int idx = start_register, l = 0; idx < start_register + registers_count; idx++ )
                        {
                        if ( nd->AO_types[ idx ] != ao_module_type )
                            {
                            ao_module_type = nd->AO_types[ idx ];
                            ao_module_offset = 0;
                            }
                        else
                            {
                            ao_module_offset++;
                            }

                        switch ( ao_module_type )
                            {
                            case 1027843:           //AXL F IOL8
                            case 1088132:           //AXL SE IOL4
                                ao_module_offset %= 32;	   //if there are same modules one after other on bus
                                if ( ao_module_offset > 2 )  //first 3 words (bytes 0-5) are reserved, 2nd byte is used for trigger discrete outputs.
                                    {
                                    memcpy( &writebuff[ l ], &nd->AO_[ idx ], 2 );
                                    }
                                l += 2;
                                break;

                            case 2688093:			//CNT2 INC2
                                ao_module_offset %= 14;	   //if there are same modules one after other on bus
                                if ( 0 == ao_module_offset ) //assign start command and positive increment for both counters
                                    {
                                    writebuff[ l ] = 0x5;
                                    writebuff[ l + 1 ] = 0x5;
                                    }
                                else
                                    {
                                    writebuff[ l ] = 0;
                                    writebuff[ l + 1 ] = 0;
                                    }
                                l += 2;
                                break;

                            case 2688527:       //AXL F AO4 1H
                            case 2702072:       //AXL F AI2 AO2 1H
                            case 1088123:       //AXL SE AO4 I 4-20,
                                writebuff[ l ] = (u_char)( ( nd->AO_[ idx ] >> 8 ) & 0xFF );
                                writebuff[ l + 1 ] = (u_char)( nd->AO_[ idx ] & 0xFF );
                                l += 2;
                                break;

                            case 2688666:       //AXL F RS UNI XC
                                writebuff[ l ] = (u_char)( ( nd->AO_[ idx ] >> 8 ) & 0xFF );
                                writebuff[ l + 1 ] = (u_char)( nd->AO_[ idx ] & 0xFF );
                                l += 2;
                                break;

                            default:
                                l += 2;
                                break;
                            }
                        }

                    if ( write_holding_registers( nd, start_write_address + start_register, registers_count ) >= 0 )
                        {
                        if ( buff[ 7 ] == 0x10 )
                            {
                            memcpy( &( nd->AO[ start_register ] ), &( nd->AO_[ start_register ] ), registers_count * 2 );
                            memcpy( &( nd->DO[ start_register * 16 ] ), &( nd->DO_[ start_register * 16 ] ), registers_count * 16 );
                            nd->flag_error_write_message = false;
                            }
                        else
                            {
                            if ( !nd->flag_error_write_message )
                                {
                                G_LOG->error( "Write AO: returned error %d", buff[ 7 ] );
                                nd->flag_error_write_message = true;
                                }
                            }
                        }
                    else
                        {
                        if ( !nd->flag_error_write_message )
                            {
                            G_LOG->error( "Write AO: returned error" );
                            nd->flag_error_write_message = true;
                            }
                        }

                    start_register += registers_count;
                    registers_count = nd->AO_cnt - start_register;
                    if ( registers_count > static_cast<unsigned int>(CONSTANTS::MAX_MODBUS_REGISTERS_PER_QUERY) )
                        {
                        registers_count = static_cast<unsigned int>(CONSTANTS::MAX_MODBUS_REGISTERS_PER_QUERY);
                        }

                    } while ( start_register < nd->AO_cnt );


                }// if ( nd->AO_cnt > 0 )

            }// if ( nd->type == io_node::T_750_341 || ...
        }// for ( u_int i = 0; i < nodes_count; i++ )

    return 0;
    }
//-----------------------------------------------------------------------------
int io_manager::e_communicate( io_node* node, int bytes_to_send, int bytes_to_receive )
    {
    // Проверка связи с узлом I/O.
    if ( get_delta_millisec( node->last_poll_time ) > io_node::C_MAX_WAIT_TIME )
        {
        if ( false == node->is_set_err )
            {
            node->is_set_err = true;
            PAC_critical_errors_manager::get_instance()->set_global_error(
                PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_IO_COUPLER, node->number );
            }
        }
    else
        {
        if ( node->is_set_err )
            {
            node->is_set_err = false;
            PAC_critical_errors_manager::get_instance()->reset_global_error(
                PAC_critical_errors_manager::PAC_critical_errors_manager::AC_NO_CONNECTION,
                PAC_critical_errors_manager::AS_IO_COUPLER, node->number );
            }
        }
    // Проверка связи с узлом I/O.-!>

    // Инициализация сетевого соединения, при необходимости.
    if ( node->state != io_node::ST_OK )
        {
        if ( get_delta_millisec( node->last_init_time ) < node->delay_time )
            {
            return 1;
            }

        net_init( node );
        if ( node->state != io_node::ST_OK )
            {
            node->last_init_time = get_millisec();
            if ( node->delay_time < io_node::C_MAX_DELAY )
                {
                node->delay_time += node->delay_time;
                }
            return -100;
            }
        }
    // Инициализация сетевого соединения, при необходимости.-!>

    node->delay_time = io_node::C_INITIAL_RECONNECT_DELAY;

    // Посылка данных.
    int res = tcp_communicator::sendall( node->sock, buff,
        bytes_to_send, 0, io_node::C_RCV_TIMEOUT_US, node->ip_address,
        node->name, &node->send_stat );

    if ( res < 0 )
        {
        disconnect( node );
        return -101;
        }

    // Получение данных.
    res = tcp_communicator::recvtimeout( node->sock, buff,
        bytes_to_receive, 0, io_node::C_RCV_TIMEOUT_US );

    if ( res <= 0 ) /* read error */
        {
        disconnect( node );
        return -102;
        }
    node->last_poll_time = get_millisec();

    return 0;
    }

int io_manager::read_input_registers( io_node* node, unsigned int address,
    unsigned int quantity, unsigned char station /*= 0*/ )
    {
    buff[ 0 ] = 's';
    buff[ 1 ] = 's';
    buff[ 2 ] = 0;
    buff[ 3 ] = 0;
    buff[ 4 ] = 0;
    buff[ 5 ] = 6;
    buff[ 6 ] = station;
    buff[ 7 ] = 0x04;
    buff[ 8 ] = (u_int_2)address >> 8;
    buff[ 9 ] = (u_int_2)address & 0xFF;
    buff[ 10 ] = (u_int_2)quantity >> 8;
    buff[ 11 ] = (u_int_2)quantity & 0xFF;
    unsigned int bytes_cnt = quantity * 2;
    if ( e_communicate( node, 12, bytes_cnt + 9 ) == 0 )
        {
        if ( buff[ 7 ] == 0x04 && buff[ 8 ] == bytes_cnt )
            {
            resultbuff = &buff[ 9 ];
            return 1;
            }
        else
            {
            return 0;
            }
        }
    return -1;
    }

int io_manager::write_holding_registers( io_node* node, unsigned int address,
    unsigned int quantity, unsigned char station )
    {
    unsigned int bytes_cnt = quantity * 2;
    buff[ 0 ] = 's';
    buff[ 1 ] = 's';
    buff[ 2 ] = 0;
    buff[ 3 ] = 0;
    buff[ 4 ] = 0;
    buff[ 5 ] = static_cast<unsigned char>(7 + bytes_cnt);
    buff[ 6 ] = station;
    buff[ 7 ] = 0x10;
    buff[ 8 ] = (u_int_2)address >> 8;
    buff[ 9 ] = (u_int_2)address & 0xFF;
    buff[ 10 ] = (u_int_2)quantity >> 8;
    buff[ 11 ] = (u_int_2)quantity & 0xFF;
    buff[ 12 ] = static_cast<unsigned char>(bytes_cnt);
    if ( e_communicate( node, bytes_cnt + 13, 12 ) == 0 )
        {
        if ( buff[ 7 ] == 0x10 )
            {
            return 1;
            }
        else
            {
            return 0;
            }
        }
    return -1;
    }
//-----------------------------------------------------------------------------
int io_manager::read_inputs()
{
    if (nodes_count == 0) return 0;

    for (u_int i = 0; i < nodes_count; i++)
    {
        io_node* nd = nodes[i];

        if (!nd->is_active) continue;

        if (nd->type == io_node::WAGO_750_XXX_ETHERNET)
        {
            if (nd->DI_cnt > 0)
            {
                read_DI(nd);
            }
            else if (nd->AI_cnt > 0)
            {
                read_AI(nd);
            }
        }
        else if (nd->type == io_node::PHOENIX_BK_ETH)
        {
            read_phoenix_inputs(nd);
        }
    }

    return 0;
}
void io_manager::read_DI(io_node* nd)
{
    buff[0] = 's';
    buff[1] = 's';
    buff[2] = 0;
    buff[3] = 0;
    buff[4] = 0;
    buff[5] = 6;
    buff[6] = 0;
    buff[7] = 0x02;
    buff[8] = 0;
    buff[9] = 0;
    buff[10] = (unsigned char)nd->DI_cnt >> 7 >> 1;
    buff[11] = (unsigned char)nd->DI_cnt & 0xFF;

    u_int bytes_cnt = nd->DI_cnt / 8 + (nd->DI_cnt % 8 > 0 ? 1 : 0);

    if (e_communicate(nd, 12, bytes_cnt + 9) == 0)
    {
        if (buff[7] == 0x02 && buff[8] == bytes_cnt)
        {
            for (u_int j = 0, idx = 0; j < bytes_cnt; j++)
            {
                for (int k = 0; k < 8; k++)
                {
                    if (idx < nd->DI_cnt)
                    {
                        nd->DI[idx] = (buff[j + 9] >> k) & 1;
#ifdef DEBUG_KBUS
                        printf("%d -> %d, ", idx, nd->DI[idx]);
#endif // DEBUG_KBUS
                        idx++;
                    }
                }
            }
#ifdef DEBUG_KBUS
            printf("\n");
#endif // DEBUG_KBUS
        }
        else
        {
            fmt::format_to_n(G_LOG->msg, i_log::C_BUFF_SIZE,
                "Read DI:bus coupler returned error. Node {})",
                nd->number);
            G_LOG->write_log(i_log::P_ERR);

            if (G_DEBUG)
            {
                //printf("\nRead DI:I/O returned error...\n");
            }
        }
    }
}
void io_manager::read_AI(io_node* nd)
{
    buff[0] = 's';
    buff[1] = 's';
    buff[2] = 0;
    buff[3] = 0;
    buff[4] = 0;
    buff[5] = 6;
    buff[6] = 0;
    buff[7] = 0x04;
    buff[8] = 0;
    buff[9] = 0;

    u_int bytes_cnt = nd->AI_size;

    buff[10] = (unsigned char)bytes_cnt / 2 >> 8;
    buff[11] = (unsigned char)bytes_cnt / 2 & 0xFF;

    if (e_communicate(nd, 12, bytes_cnt + 9) == 0)
    {
        if (buff[7] == 0x04 && buff[8] == bytes_cnt)
        {
            int idx = 0;
            for (unsigned int l = 0; l < nd->AI_cnt; l++)
            {
                if (nd->AI_types[l] == 638)
                {
                    nd->AI[l] = 256 * buff[9 + idx + 2] + buff[9 + idx + 3];
                    idx += 4;
                }
                else
                {
                    nd->AI[l] = 256 * buff[9 + idx] + buff[9 + idx + 1];
                    idx += 2;
                }
            }
        }
        else
        {
            fmt::format_to_n(G_LOG->msg, i_log::C_BUFF_SIZE,
                "Read AI:bus coupler returned error. Node {} (bytes_cnt = {}, {} {} )",
                nd->number, (int)buff[7], (int)buff[8], bytes_cnt);
            G_LOG->write_log(i_log::P_ERR);
        }
    }
}
void io_manager::read_phoenix_inputs(io_node* nd)
{
    if (!is_phoenix_input_reading_required(nd))
    {
        return;
    }

    unsigned int start_register = 0;
    auto start_read_address = static_cast<unsigned int>(CONSTANTS::PHOENIX_INPUTREGISTERS_STARTADDRESS);
    unsigned int registers_count = calculate_registers_count(nd);

    int res;
    int index_source = 0;
    unsigned int analog_dest = 0;
    unsigned int bit_dest = 0;

    do
    {
#ifdef DEBUG_BK_MIN
        G_LOG->warning("Read %d node registers from %d", registers_count, start_read_address + start_register);
#endif // DEBUG_BK_MIN
        res = read_input_registers(nd, start_read_address + start_register, registers_count);

#ifdef TEST_NODE_IO
        printf("\n\r");
        for (int ideb = 0; ideb < registers_count; ideb++)
        {
            printf("%d = %d, ", start_read_address + start_register + ideb, 256 * resultbuff[ideb * 2] + resultbuff[ideb * 2 + 1]);
        }
#endif

        if (res >= 0) {
            if (res) {
                process_analog_inputs(nd, analog_dest, index_source, start_register, registers_count);
                process_digital_inputs(nd, bit_dest, index_source, start_register, registers_count);
            }
            else {
                handle_ai_error(nd, registers_count);
                break;
            }
        }
        else {
            // node doesn't respond
            break;
        }

        start_register += registers_count;
        registers_count = nd->AI_cnt - start_register;
    } while (start_register < nd->AI_cnt);
}
bool io_manager::is_phoenix_input_reading_required(const io_node* nd) const
{
    return (nd->is_active && nd->AI_cnt > 0);
}
unsigned int io_manager::calculate_registers_count(const io_node* nd) const
{
    return (nd->AI_cnt > static_cast<unsigned int>(CONSTANTS::MAX_MODBUS_REGISTERS_PER_QUERY))
        ? static_cast<unsigned int>(CONSTANTS::MAX_MODBUS_REGISTERS_PER_QUERY)
        : nd->AI_cnt;
}
void io_manager::process_analog_inputs(io_node* nd, unsigned int& analog_dest, int& index_source,
    unsigned int start_register, unsigned int registers_count) const
{
    for (index_source = 0; analog_dest < start_register + registers_count; analog_dest++)
    {
        switch (nd->AI_types[analog_dest])
        {
        case 1027843:           //AXL F IOL8
        case 1088132:           //AXL SE IOL4
            memcpy(&nd->AI[analog_dest], resultbuff + index_source, 2);
            index_source += 2;
            break;

        default:
            nd->AI[analog_dest] = 256 * resultbuff[index_source] + resultbuff[index_source + 1];
            index_source += 2;
            break;
        }
#ifdef DEBUG_BK
        G_LOG->warning("%d %u", analog_dest, nd->AI[analog_dest]);
#endif // DEBUG_BK
    }
}
void io_manager::process_digital_inputs(io_node* nd, unsigned int& bit_dest, int& index_source,
    unsigned int start_register, unsigned int registers_count) const
{
    for (index_source = 0; bit_dest < (start_register + registers_count) * 2 * 8; index_source++)
    {
        for (int k = 0; k < 8; k++)
        {
            if (bit_dest >= (start_register + registers_count) * 2 * 8)
            {
                break;
            }
            nd->DI[bit_dest] = (resultbuff[index_source] >> k) & 1;
#ifdef DEBUG_BK
            G_LOG->notice("%d %d", bit_dest, (resultbuff[index_source] >> k) & 1);
#endif // DEBUG_BK
            bit_dest++;
        }
    }
}
void io_manager::handle_ai_error(const io_node* nd, unsigned int registers_count) const
{
    G_LOG->error("Read AI: bus coupler returned error. Node %d (bytes_cnt = %d, %d %d )",
        nd->number, (int)buff[7], (int)buff[8], registers_count * 2);
}
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
io_manager::io_node::io_node( int type, int number, const char* str_ip_address,
    const char* name, int DO_cnt, int DI_cnt, int AO_cnt, int AO_size, int AI_cnt,
    int AI_size ) : state( ST_NO_CONNECT ),
    type( (TYPES)type ),
    number( number ),

    is_active( true ),

    last_poll_time( get_millisec() ),
    is_set_err( 0 ),
    sock( 0 ),

    DO_cnt( DO_cnt ),
    DO( 0 ),
    DO_( 0 ),
    AO_cnt( AO_cnt ),
    AO_size( AO_size ),
    AO{},
    AO_{},
    AO_offsets{},
    AO_types{},
    DI_cnt( DI_cnt ),
    DI{},
    AI_cnt( AI_cnt ),
    AI_size( AI_size ),
    AI{},
    AI_offsets{},
    AI_types{},
    last_init_time( get_millisec() ),
    delay_time( C_INITIAL_RECONNECT_DELAY )
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
        AI_offsets = new u_int[ AI_cnt ];
        AI_types = new u_int[ AI_cnt ];

        memset( AI, 0, sizeof( AI ) );
        }
    if ( AO_cnt )
        {
        AO_types = new u_int[ AO_cnt ];
        AO_offsets = new u_int[ AO_cnt ];

        memset( AO, 0, sizeof( AO ) );
        memset( AO_, 0, sizeof( AO ) );
        }

    if ( DI_cnt )
        {
        DI = new u_char[ DI_cnt ];
        memset( DI, 0, DI_cnt );
        }

    if ( DO_cnt )
        {
        DO = new u_char[ DO_cnt ];
        DO_ = new u_char[ DO_cnt ];
        memset( DO, 0, DO_cnt );
        memset( DO_, 0, DO_cnt );
        }
    }

//-----------------------------------------------------------------------------
void io_manager::io_node::print()
    {
    printf( "\"%s\" - type %d, number %d, IP \"%s\", "
        "DI %d [%d], DO %d [%d], AI %d [%d], AO %d [%d].\n",
        name, type, number, ip_address,
        DI_cnt, DI_cnt / CHAR_BIT, DO_cnt, DO_cnt / CHAR_BIT,
        AI_cnt, AI_size, AO_cnt, AO_size );
    }
//-----------------------------------------------------------------------------
void io_manager::io_node::print_log()
    {
    sprintf( G_LOG->msg, "\"%s\" - type %d, number %d, IP \"%s\", "
        "DI %d [%d], DO %d [%d], AI %d [%d], AO %d [%d].",
        name, type, number, ip_address,
        DI_cnt, DI_cnt / CHAR_BIT, DO_cnt, DO_cnt / CHAR_BIT,
        AI_cnt, AI_size, AO_cnt, AO_size );
    G_LOG->write_log(i_log::P_DEBUG);
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
io_manager* G_IO_MANAGER()
    {
    return io_manager::get_instance();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef WIN_OS
#pragma warning(pop)
#endif // WIN_OS
