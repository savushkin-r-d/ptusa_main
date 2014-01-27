#include "modbus_serv.h"

#ifdef KHUTOR
#include "PAC_dev.h"
#endif //KHUTOR

#ifdef KHUTOR
i_DO_device* ModbusServ::KOAG_HL1[ ModbusServ::KOAG_CNT ];
i_AI_device* ModbusServ::KOAG_TE[ KOAG_CNT ][ 2 ];
#endif //KHUTOR

unsigned char ModbusServ::UTable[128][2] =
    {
    {0x4,0x2},
    {0x4,0x3},
    {0x20,0x1A},
    {0x4,0x53},
    {0x20,0x1E},
    {0x20,0x26},
    {0x20,0x20},
    {0x20,0x21},
    {0x20,0xAC},
    {0x20,0x30},
    {0x4,0x9},
    {0x20,0x39},
    {0x4,0xA},
    {0x4,0xC},
    {0x4,0xB},
    {0x4,0xF},
    {0x4,0x52},
    {0x20,0x18},
    {0x20,0x19},
    {0x20,0x1C},
    {0x20,0x1D},
    {0x20,0x22},
    {0x20,0x13},
    {0x20,0x14},
    {0x0,0x0},
    {0x21,0x22},
    {0x4,0x59},
    {0x20,0x3A},
    {0x4,0x5A},
    {0x4,0x5C},
    {0x4,0x5B},
    {0x4,0x5F},
    {0x0,0xA0},
    {0x4,0xE},
    {0x4,0x5E},
    {0x4,0x8},
    {0x0,0xA4},
    {0x4,0x90},
    {0x0,0xA6},
    {0x0,0xA7},
    {0x4,0x1},
    {0x0,0xA9},
    {0x4,0x4},
    {0x0,0xAB},
    {0x0,0xAC},
    {0x0,0xAD},
    {0x0,0xAE},
    {0x4,0x7},
    {0x0,0xB0},
    {0x0,0xB1},
    {0x4,0x6},
    {0x4,0x56},
    {0x4,0x91},
    {0x0,0xB5},
    {0x0,0xB6},
    {0x0,0xB7},
    {0x4,0x51},
    {0x21,0x16},
    {0x4,0x54},
    {0x0,0xBB},
    {0x4,0x58},
    {0x4,0x5},
    {0x4,0x55},
    {0x4,0x57},
    {0x4,0x10},
    {0x4,0x11},
    {0x4,0x12},
    {0x4,0x13},
    {0x4,0x14},
    {0x4,0x15},
    {0x4,0x16},
    {0x4,0x17},
    {0x4,0x18},
    {0x4,0x19},
    {0x4,0x1A},
    {0x4,0x1B},
    {0x4,0x1C},
    {0x4,0x1D},
    {0x4,0x1E},
    {0x4,0x1F},
    {0x4,0x20},
    {0x4,0x21},
    {0x4,0x22},
    {0x4,0x23},
    {0x4,0x24},
    {0x4,0x25},
    {0x4,0x26},
    {0x4,0x27},
    {0x4,0x28},
    {0x4,0x29},
    {0x4,0x2A},
    {0x4,0x2B},
    {0x4,0x2C},
    {0x4,0x2D},
    {0x4,0x2E},
    {0x4,0x2F},
    {0x4,0x30},
    {0x4,0x31},
    {0x4,0x32},
    {0x4,0x33},
    {0x4,0x34},
    {0x4,0x35},
    {0x4,0x36},
    {0x4,0x37},
    {0x4,0x38},
    {0x4,0x39},
    {0x4,0x3A},
    {0x4,0x3B},
    {0x4,0x3C},
    {0x4,0x3D},
    {0x4,0x3E},
    {0x4,0x3F},
    {0x4,0x40},
    {0x4,0x41},
    {0x4,0x42},
    {0x4,0x43},
    {0x4,0x44},
    {0x4,0x45},
    {0x4,0x46},
    {0x4,0x47},
    {0x4,0x48},
    {0x4,0x49},
    {0x4,0x4A},
    {0x4,0x4B},
    {0x4,0x4C},
    {0x4,0x4D},
    {0x4,0x4E},
    {0x4,0x4F}
    };

long ModbusServ::ModbusService( long len, unsigned char *data,unsigned char *outdata )
    {
    unsigned int numberofElements;
    unsigned int startingAddress;
    unsigned char numberofBytes;
    unsigned int i;

#ifdef LINUX_OS
    __attribute__((__unused__))
#endif
    unsigned int objnumber;
#ifdef LINUX_OS
    __attribute__((__unused__))
#endif
    unsigned int coilgroup;

    switch ( data[1] ) //Modbus command
        {
        case 0x01: //Read Coils
            startingAddress = data[2] * 256 + data[3];
            numberofElements = data[4] * 256 + data[5];
            numberofBytes = numberofElements / 8;
            if (numberofElements % 8) numberofBytes++;
            outdata[2] = numberofBytes;
            for (i = 0; i < numberofElements; i++)
                {
                objnumber = i+startingAddress;
                coilgroup = data[0];
#ifdef KHUTOR
                if ( coilgroup >= 1 && coilgroup <= 16 )
                    {
                    int coag_idx = ( coilgroup - 1 ) * 2 + 1;
                    if( objnumber >= 100 ) coag_idx++;

                    if ( 0 == objnumber || 100 == objnumber )
                        {
                        ForceBit( i, &outdata[ 3 ],
                            KOAG_HL1[ coag_idx - 1 ]->get_state() );
                        }

                    if ( ( objnumber >= 1 && objnumber <= 16 ) ||
                        ( objnumber >= 101 && objnumber <= 116 ) )
                        {
                        ForceBit( i, &outdata[ 3 ],
                            G_TECH_OBJECTS( coag_idx )->get_mode( objnumber ) );
                        }
                    //Откачка сыворотки из верхней части.
                    if ( 17 == objnumber || 117 == objnumber )
                        {
                        ForceBit( i, &outdata[ 3 ],
                            G_TECH_OBJECTS( coag_idx )->rt_par_uint[ 1 ] );
                        }
                    //Откачка сыворотки из средней части.
                    if ( 18 == objnumber || 118 == objnumber )
                        {
                        ForceBit( i, &outdata[ 3 ],
                            G_TECH_OBJECTS( coag_idx )->rt_par_uint[ 2 ] );
                        }
                    //Откачка сыворотки из нижней части.
                    if ( 19 == objnumber || 119 == objnumber )
                        {
                        ForceBit( i, &outdata[ 3 ],
                            G_TECH_OBJECTS( coag_idx )->rt_par_uint[ 3 ] );
                        }

                    //Подача основы в дренаж.
                    if ( 20 == objnumber || 120 == objnumber )
                        {
                        ForceBit( i, &outdata[ 3 ],
                            G_TECH_OBJECTS( coag_idx )->rt_par_uint[ 5 ] );
                        }
                    //Подача основы в танк.
                    if ( 21 == objnumber || 121 == objnumber )
                        {
                        ForceBit( i, &outdata[ 3 ],
                            G_TECH_OBJECTS( coag_idx )->rt_par_uint[ 4 ] );
                        }
                    }
#endif //KHUTOR
                }
            return 3+numberofBytes;

        case 0x03: //Read Holding Registers
            startingAddress = data[2] * 256 + data[3];
            numberofElements = data[4] * 256 + data[5];
            outdata[2] = (numberofElements * 2) & 0xFF;
            for (i = 0; i < numberofElements; i++)
                {
                coilgroup = data[0];
                objnumber = i+startingAddress;

#ifdef KHUTOR
                if ( coilgroup >= 1 && coilgroup <= 16 )
                    {
                    int coag_idx = ( coilgroup - 1 ) * 2 + 1;
                    if( objnumber >= 100 ) coag_idx++;

                    if ( 0 == objnumber || 100 == objnumber )
                        {
                        int value = G_TECH_OBJECTS( coag_idx )->get_number();
                        PackInt16( value, &outdata[ 3 + i * 2 ] );
                        }

                    if ( 1 == objnumber || 101 == objnumber )
                        {
                        float value = KOAG_TE[ coag_idx - 1 ][ 0 ]->get_value();
                        PackFloat( value, &outdata[ 3 + i * 2 ] );
                        }
                    if ( 3 == objnumber || 103 == objnumber )
                        {
                        float value = KOAG_TE[ coag_idx - 1 ][ 1 ]->get_value();
                        PackFloat( value, &outdata[ 3 + i * 2 ] );
                        }
                    if ( 5 == objnumber || 105 == objnumber )
                        {
                        int value = G_TECH_OBJECTS( coag_idx )->get_active_mode();

                        PackInt16( value, &outdata[ 3 + i * 2 ] );
                        }

                    if ( 6 == objnumber || 106 == objnumber )
                        {
                        int_2 value = ( int_2 ) G_TECH_OBJECTS( coag_idx )->par_float[ 1 ];
                        PackInt16( value, &outdata[ 3 + i * 2 ] );
                        }

                    if ( 7 == objnumber || 107 == objnumber )
                        {
                        int value = G_TECH_OBJECTS( coag_idx )->get_active_mode();

                        u_long t = 0;
                        if ( 0 == value )
                            {
                            t = G_TECH_OBJECTS( coag_idx )->get_modes_manager()->get_idle_time();
                            }
                        else
                            {
                            t = ( *G_TECH_OBJECTS( coag_idx
                                )->get_modes_manager() )[ value ]->evaluation_time();
                            }

                        PackTime( t, &outdata[ 3 + i * 2 ] );
                        }

                    if ( 11 == objnumber || 111 == objnumber )
                        {
                        int mode_idx = G_TECH_OBJECTS( coag_idx )->get_active_mode();

                        static const char* no_step = "";
                        const char* step_name = no_step;

                        if ( mode_idx > 0 )
                        	{
                            int step_idx = G_TECH_OBJECTS( coag_idx
                                )->get_modes_manager()[ 0 ][ mode_idx ]->active_step();
                            if ( step_idx )
                                {
                                step_name = G_TECH_OBJECTS( coag_idx
                                    )->get_modes_manager()[ 0 ][ mode_idx ][ 0 ][ step_idx ]->get_name();
                                }
                            }

                        CP1251toUnicode( step_name, &outdata[ 3 + i * 2 ] );
                        }
                    }
#endif // KHUTOR
                }
            return 3+numberofElements*2;

        case 0x05: //Write Single Coil
            startingAddress = data[2] * 256 + data[3];
            objnumber = startingAddress;
            coilgroup = data[0];
#ifdef KHUTOR
            if ( coilgroup >= 1 && coilgroup <= 16 )
                {
                int coag_idx = ( coilgroup - 1 ) * 2 + 1;
                if( objnumber >= 100 ) coag_idx++;

                int value = data[ 4 ] > 0 ? 1 : 0;

                if ( 0 == objnumber || 100 == objnumber )
                    {
                    KOAG_HL1[ coag_idx - 1 ]->set_state( value );
                    }

                if ( ( objnumber >= 1 && objnumber <= 16 ) ||
                    ( objnumber >= 101 && objnumber <= 116 ) )
                    {
                    G_TECH_OBJECTS( coag_idx )->set_mode( objnumber, value );
                    }

                    //Откачка сыворотки из верхней части.
                    if ( 17 == objnumber || 117 == objnumber )
                        {
                        G_TECH_OBJECTS( coag_idx )->rt_par_uint[ 1 ] = value;
                        }
                    //Откачка сыворотки из средней части.
                    if ( 18 == objnumber || 118 == objnumber )
                        {
                        G_TECH_OBJECTS( coag_idx )->rt_par_uint[ 2 ] = value;
                        }
                    //Откачка сыворотки из нижней части.
                    if ( 19 == objnumber || 119 == objnumber )
                        {
                        G_TECH_OBJECTS( coag_idx )->rt_par_uint[ 3 ] = value;
                        }

                    //Подача основы в дренаж.
                    if ( 20 == objnumber || 120 == objnumber )
                        {
                        G_TECH_OBJECTS( coag_idx )->rt_par_uint[ 5 ] = value;
                        }
                    //Подача основы в танк.
                    if ( 21 == objnumber || 121 == objnumber )
                        {
                        G_TECH_OBJECTS( coag_idx )->rt_par_uint[ 4 ] = value;
                        }
                }
#endif //KHUTOR
            return 6;

        case 0x10: //Force Multiply Registers
            startingAddress = data[2] * 256 + data[3];
            numberofElements = data[4] * 256 + data[5];
            //numberofBytes = data[6];
            for (i = 0; i < numberofElements; i++)
                {
                objnumber = i + startingAddress;
                coilgroup = data[ 0 ];

#ifdef KHUTOR
                if ( coilgroup >= 1 && coilgroup <= 16 )
                    {
                    int coag_idx = ( coilgroup - 1 ) * 2 + 1;
                    if( objnumber >= 100 ) coag_idx++;

                    if ( 6 == objnumber || 106 == objnumber )
                        {
                        int_2 value = UnpackInt16( &outdata[ 7 + i * 2 ] );
                        G_TECH_OBJECTS( coag_idx )->par_float[ 1 ] = ( float ) value;
                        }
                    }
#endif // KHUTOR

                }
            return 7;
        }
    return 0;
    }

int ModbusServ::ForceBit(int bit, unsigned char* Buf, int toset)
    {
    int bitpos,bytepos;
    bitpos = bit % 8;
    bytepos = bit / 8;
    if (toset)
        {
        Buf[bytepos] = Buf[bytepos] | (1 << bitpos);
        }
    else
        {
        Buf[bytepos] = Buf[bytepos] & (0xFF ^ (1 << bitpos));
        }
    return 0;
    }

int ModbusServ::PackString( char* String, unsigned char* Buf )
    {
    int i = 0;
    while (0 != String[i])
        {
        if (i % 2)
            {
            Buf[i-1] = String[i];
            }
        else
            {
            Buf[i+1] = String[i];
            }
        i++;
        }
    if (i % 2)
        {
        Buf[i-1] = ' ';
        i++;
        }
    return i;
    }

int ModbusServ::PackTime( unsigned long timevar, unsigned char* Buf, int units /*= 0*/ )
    {
    unsigned long temptime;
    unsigned int seconds, minites, hours;
    if (units)
        {
        temptime = timevar;
        }
    else
        {
        temptime = timevar / 1000;
        }
    seconds = temptime % 60;
    minites = (temptime / 60) % 60;
    hours = (temptime / 3600) % 100;
    sprintf((char*)Buf,"%.1hd%.1hd%.1hd::%.1hd%.1hd%.1hd",
        hours % 10, hours / 10, minites / 10, minites % 10, seconds % 10, seconds / 10);
    return 3;
    }

int ModbusServ::PackFloat( float fvalue, unsigned char* Buf )
    {
    Buf[0] = ((char*)&fvalue)[1];
    Buf[1] = ((char*)&fvalue)[0];
    Buf[2] = ((char*)&fvalue)[3];
    Buf[3] = ((char*)&fvalue)[2];
    return 1;
    }

int ModbusServ::PackWord( unsigned int wvalue,char* Buf )
    {
    Buf[0] = ((char*)&wvalue)[1];
    Buf[1] = ((char*)&wvalue)[0];
    return 0;
    }

int ModbusServ::PackLong( unsigned long lvalue,char* Buf )
    {
    Buf[0] = ((char*)&lvalue)[1];
    Buf[1] = ((char*)&lvalue)[0];
    Buf[2] = ((char*)&lvalue)[3];
    Buf[3] = ((char*)&lvalue)[2];
    return 1;
    }

float ModbusServ::UnpackFloat( unsigned char* Buf )
    {
    float result;
    ((char*)&result)[0] = Buf[1];
    ((char*)&result)[1] = Buf[0];
    ((char*)&result)[2] = Buf[3];
    ((char*)&result)[3] = Buf[2];
    return result;
    }

unsigned int ModbusServ::UnpackWord( unsigned char* Buf )
    {
    unsigned int result;
    ((char*)&result)[0] = Buf[1];
    ((char*)&result)[1] = Buf[0];
    return result;
    }

unsigned long ModbusServ::UnpackLong( unsigned char* Buf )
    {
    unsigned long result;
    ((char*)&result)[0] = Buf[1];
    ((char*)&result)[1] = Buf[0];
    ((char*)&result)[2] = Buf[3];
    ((char*)&result)[3] = Buf[2];
    return result;
    }

int_2 ModbusServ::PackInt16( int_2 wvalue,unsigned char* Buf )
    {
    Buf[0] = ((char*)&wvalue)[1];
    Buf[1] = ((char*)&wvalue)[0];
    return 0;
    }

int_2 ModbusServ::UnpackInt16( unsigned char* Buf )
    {
    int_2 result;
    ((char*)&result)[0] = Buf[1];
    ((char*)&result)[1] = Buf[0];
    return result;
    }

int ModbusServ::CP1251toUnicode( const char* Input, unsigned char* Buf )
    {
    int inputlen = strlen( Input );

    char input_end = 0;
    int i;
    for (i = 0; i < inputlen; i++)
        {
        unsigned char mychar = Input[i];
        if (0 == mychar)
            {
            input_end = 1;
            }
        if (input_end)
            {
            Buf[i*2] = 0;
            Buf[i*2 + 1] = 0;
            }
        else
            {
            if (mychar < 128)
                {
                Buf[i*2] = 0;
                Buf[i*2 + 1] = mychar;
                }
            else
                {
                Buf[i*2] = UTable[mychar - 128][0];
                Buf[i*2 + 1] = UTable[mychar - 128][1];
                }

            }
#ifdef DEBUG
        //Print("\n\r%d. %d %d", i, Buf[i*2], Buf[i*2 + 1]);
#endif //DEBUG
        }
    return 0;
    }

int ModbusServ::UnicodetoCP1251( char* Output, unsigned char* Buf, int inputlen )
    {
    int i;
    unsigned char upperbyte;
    unsigned char lowerbyte;
    for (i = 0; i < inputlen; i++)
        {
        upperbyte = Buf[2 * i];
        lowerbyte = Buf[2 * i + 1];
#ifdef DEBUG
        Print("\n\r%d symbol codes: %d %d", i + 1, upperbyte, lowerbyte);
#endif //DEBUG

        if (i == inputlen - 1 || (0 == upperbyte && 0 == lowerbyte))
            {
            Output[i] = 0;
            break;
            }
        else
            {
            if (0 == upperbyte && lowerbyte < 128)
                {
                Output[i] = lowerbyte;
                }
            else
                {
                for (int j = 0; j<128; j++)
                    {
                    if (UTable[j][0] == upperbyte && UTable[j][1] == lowerbyte)
                        {
                        Output[i] = 128 + j;
                        break;
                        }
                    }
                }

            }
        }
    return i/2 - 1;
    }

void ModbusServ::init()
    {
#ifdef KHUTOR
    char dev [ 10 ] = "";
    for ( int i = 1; i <= KOAG_CNT; i++ )
    	{
        sprintf( dev, "KOAG%dHL1", i );
        KOAG_HL1[ i - 1 ] = HL( dev );

        sprintf( dev, "KOAG%dTE1", i );
        KOAG_TE[ i - 1 ][ 0 ] = TE( dev );

        sprintf( dev, "KOAG%dTE2", i );
        KOAG_TE[ i - 1 ][ 1 ] = TE( dev );
    	}
#endif //KHUTOR
    }
