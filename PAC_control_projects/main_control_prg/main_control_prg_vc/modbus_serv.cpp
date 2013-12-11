#include "modbus_serv.h"

unsigned char ModbusServ::UTable[128][2] = {
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
	unsigned int objnumber;
	unsigned int coilgroup;



	switch (data[1]) //Modbus command
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
				//switch (coilgroup)
				//	{
				//	default:
				//		ForceBit(i,&outdata[3],0);
				//		break;
				//	}
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

				if (objnumber == 0)
					{
					i += PackTime(get_millisec(),&outdata[3+i*2],0 );
					}
//				switch (coilgroup)
//					{
//					default:
//#ifdef DEBUG 
//						Print("\n\rRead Unsigned register");
//						PackInt(0, &outdata[3+i*2]);
//#endif
//						break;
//					} 
				}
			return 3+numberofElements*2;
		case 0x05: //Write Single Coil
			startingAddress = data[2] * 256 + data[3];
			objnumber = startingAddress;
			coilgroup = data[0];
			//switch (coilgroup)
			//	{
			//	default:
			//		//data[4] ? ON : OFF
			//		break;
			//	}
			return 6;
		case 0x10: //Force Multiply Registers
			startingAddress = data[2] * 256 + data[3];
			numberofElements = data[4] * 256 + data[5];
			//numberofBytes = data[6];
			for (i = 0; i < numberofElements; i++)
				{
				objnumber = i+startingAddress;
				coilgroup = data[0];
//				switch (coilgroup)
//					{
//					//UnpackInt(&data[7+i*2])
//					default:
//#ifdef DEBUG 
//						Print("\n\rWrite Unsigned register");
//#endif
//						break;
//					}
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
	sprintf((char*)Buf,"%0.1hd%0.1hd%0.1hd::%0.1hd%0.1hd%0.1hd",hours % 10,hours / 10,minites / 10,minites % 10,seconds % 10,seconds / 10);
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

int ModbusServ::PackInt( int wvalue,unsigned char* Buf )
	{
	Buf[0] = ((char*)&wvalue)[1];
	Buf[1] = ((char*)&wvalue)[0];
	return 0;
	}

int ModbusServ::UnpackInt( unsigned char* Buf )
	{
	int result;
	((char*)&result)[0] = Buf[1];
	((char*)&result)[1] = Buf[0];
	return result;
	}

int ModbusServ::CP1251toUnicode( char* Input, unsigned char* Buf, int inputlen )
	{
	char input_end = 0;
	int i;
	for (i = 0; i < inputlen / 2; i++)
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