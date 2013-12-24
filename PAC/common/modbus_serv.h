#ifndef modbus_serv_h__
#define modbus_serv_h__
#include "tech_def.h"
#include "g_device.h"
#include "dtime.h"

class ModbusServ
	{
	public:
		static long ModbusService(long len, unsigned char *data, unsigned  char *outdata);
		static int ForceBit(int bit, unsigned char* Buf, int toset = 1);
		static int PackString(char* String, unsigned char* Buf);
		static int PackTime(unsigned long timevar, unsigned char* Buf, int units = 0); /*units: 0 - ms 1 -s*/
		static int PackFloat(float fvalue, unsigned char* Buf);
		static float UnpackFloat(unsigned char* Buf);
		static int PackWord(unsigned int wvalue,char* Buf);
		static unsigned int UnpackWord(unsigned char* Buf);
        static int_2 PackInt16(int_2 wvalue,unsigned char* Buf);
		static int_2 UnpackInt16(unsigned char* Buf);
		static int PackLong(unsigned long lvalue,char* Buf);
		static unsigned long UnpackLong(unsigned char* Buf);
		static int CP1251toUnicode( const char* Input, unsigned char* Buf );
		static int UnicodetoCP1251(char* Output, unsigned char* Buf, int inputlen);
		static unsigned char UTable[128][2];

        static void init();

#ifdef KHUTOR
        enum CONSTS
            {
            KOAG_CNT = 16,
            };

        static i_DO_device* KOAG_HL1[ KOAG_CNT ];
        static i_AI_device* KOAG_TE[ KOAG_CNT ][ 2 ];
#endif //KHUTOR
	};

#endif // modbus_serv_h__
