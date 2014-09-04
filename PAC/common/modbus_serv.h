#ifndef modbus_serv_h__
#define modbus_serv_h__
#include "tech_def.h"
#include "cip_tech_def.h"
#include "mcaRec.h"
#include "g_device.h"
#include "dtime.h"

#define MAX_UPDATE_CONFIRMS 10

enum CoilGroups
	{
	C_V,
	C_N,
	C_M,
	C_LS,
	C_TE,
	C_FE,
	C_FS,
	C_CTR,
	C_AO,
	C_LE,
	C_FB,
	C_UPR,
	C_QE,
	C_AI,

	C_COMBPARAMS = 30,


	C_MSA_RECIPES = 100,
	C_MSA_STATIONPARAMS,
	C_MSA_CONTROL,
	C_MSA_LINE1PARAMS,
	C_MSA_LINE2PARAMS,
	C_MSA_LINE3PARAMS,
	C_MSA_LINE4PARAMS,
	C_MSA_LINE5PARAMS,
	C_MSA_LINE6PARAMS,
	C_MSA_LINE7PARAMS,
	C_MSA_LINE8PARAMS,
	C_MSA_LINE9PARAMS,
	};

enum ControlEnum
	{
	MSACTRL_COMMAND = 0,
	MSACTRL_OPERATION,
	MSACTRL_STATE,
	MSACTRL_RETTEMP,
	MSACTRL_MSANUMBER = 5,
	};

enum RetPumpEnum
	{
	RETPUMP_STATE = 0,
	RETPUMP_ISUSED,
	};

enum RecipeControl
	{
	RC_RECIPE_CONTROL = 0,
	RC_RECIPE_CONTROL_NO = 1,
	RC_SELECTED_REC = 100,
	RC_SELECTED_PRG = 164,
	RC_EDITED_REC = 228,
	RC_SELECT_REC = 300,
	RC_LIST_UPDATE,
	RC_LIST_LENGTH,
	RC_LIST_START,
	RC_PRG_SELECT = 1300,
	RC_PRG_UPDATE,
	RC_PRG_LENGTH,
	RC_PRG_START,
	RC_RECIPE_PAR_START = 2300,
	};

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
		static int CP1251toUnicode( const char* Input, unsigned char* Buf);
		static int UnicodetoCP1251(char* Output, unsigned char* Buf, int inputlen);
		static unsigned char UTable[128][2];
		static char updateRecFlag[11];
		static int confirmRecUpdateCtr[11];
		static char updatePrgFlag[11];
		static int confirmPrgUpdateCtr[11];
		static void UpdateRecipes();
		static void UpdateLineRecipes(int line);
		static void UpdatePrograms();
		static void UpdateLinePrograms(int line);

        static int_2 UnpackInt16( unsigned char* Buf, int offset )
            {
            return UnpackInt16( Buf + offset );
            }

        static float UnpackFloat( unsigned char* Buf, int offset )
            {
            return UnpackFloat( Buf + offset );
            }

		static device* get_device(unsigned int group, unsigned int number);
	};

#endif // modbus_serv_h__
