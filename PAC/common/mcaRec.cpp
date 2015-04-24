#include "mcaRec.h"
#include "PAC_dev.h"
#ifdef MSAPANEL
#include "msapanel.h"
#include "dtime.h"
#endif // MSAPANEL


int TRecipeManager::startRecipeBlock = 0;

int TRecipeManager::recipePerLine = 25;

int TRecipeManager::blocksPerRecipe = 4;

int TRecipeManager::recipeNameLength = MAX_REC_NAME_LENGTH - 8;

int TRecipeManager::startRecipeParamsOffset = MAX_REC_NAME_LENGTH;

unsigned char* TRecipeManager::recipeCopyBuffer = NULL;

TRecipeManager::TRecipeManager( int lineNo ): lineNo(lineNo),
	currentRecipe(0),
	curRecipeStartBlock(0),
	recipeStartAddr(0L)
	{
	defaultfilename = new char[20];
	sprintf(defaultfilename, "line%drec.bin", lineNo);
	recipeMemorySize = blocksPerRecipe * BLOCK_SIZE * recipePerLine;
	recipeMemory = new unsigned char[recipeMemorySize];
	LoadFromFile(defaultfilename);
    lastEvalTime = get_millisec();
	currentRecipeName = new char[recipeNameLength];
	recipeList = new char[(recipeNameLength + 6) * recipePerLine];
	strcpy(recipeList,"");
	ReadMem(startAddr(), recipeNameLength, (unsigned char*)currentRecipeName);
	FormRecipeList();
	recipechanged = 0;
	recipechangechecktime = get_millisec();
	}

TRecipeManager::~TRecipeManager()
	{
	SaveRecipeName();
	delete currentRecipeName;
	delete recipeList;
	SaveToFile(defaultfilename);
	delete recipeMemory;
	if (recipeCopyBuffer != NULL)
		{
		delete[] recipeCopyBuffer;
		}
	delete[] defaultfilename;
	}

int TRecipeManager::NextRecipe()
	{
	if (currentRecipe < recipePerLine - 1)
		{
		SaveRecipeName();
		currentRecipe++;
		curRecipeStartBlock += blocksPerRecipe;
		LoadRecipeName();
		return 1;
		}
	else
		{
		return 0;
		}
	}

int TRecipeManager::PrevRecipe()
	{
	if (currentRecipe > 0)
		{
		SaveRecipeName();
		currentRecipe--;
		curRecipeStartBlock -= blocksPerRecipe;
		LoadRecipeName();
		return 1;
		}
	else
		{
		return 0;
		}
	}

void TRecipeManager::EvalRecipe()
	{
	if (get_delta_millisec(lastEvalTime) > 10000L)
		{
		lastEvalTime = get_millisec();
		SaveRecipeName();
		FormRecipeList();
		}
	if (get_delta_millisec(recipechangechecktime) > RECIPE_SAVE_INTERVAL)
		{
		if (recipechanged)
			{
			recipechanged = 0;
			SaveToFile(defaultfilename);
			}
		recipechangechecktime = get_millisec();
		}
	if (getValue(RV_TO_DEFAULTS != 0))
		{
		switch((int)getValue(RV_TO_DEFAULTS))
			{
		case 1:
			ResetRecipeToDefaults(currentRecipe);
#ifdef DEBUG
			Print("Reset recipe %d to defaults",currentRecipe);
#endif
			break;
		case 2:
			CopyRecipe();
			break;
		case 3:
			PasteRecipe();
			break;
		case 4:
			NullifyRecipe();
			break;
			}
		setValue(RV_TO_DEFAULTS, 0);
		}
	}

unsigned long TRecipeManager::startAddr()
	{
	return startAddr(currentRecipe);
	}

unsigned long TRecipeManager::startAddr( int recNo )
	{
	return recipeStartAddr + (recNo * blocksPerRecipe) * BLOCK_SIZE;
	}

float TRecipeManager::getRecipeValue( int recNo, int valueNo )
	{
	unsigned long temp;
	ReadMem(startAddr(recNo) + startRecipeParamsOffset + 4 * valueNo, 4, (unsigned char*)&temp);
	if (temp != 0xFFFFFFFF)
		{
		return *((float*)&temp);
		}
	else
		{
		return -1000;
		}
	}

int TRecipeManager::setRecipeValue( int recNo, int valueNo, float newValue )
	{
	recipechanged = 1;
	recipechangechecktime = get_millisec();
	WriteMem(startAddr(recNo) + startRecipeParamsOffset + 4 * valueNo, 4, (unsigned char*)&newValue);
	return 0;
	}

int TRecipeManager::GetParamsCount()
	{
	return (blocksPerRecipe * BLOCK_SIZE - startRecipeParamsOffset) / 4;

	}

float TRecipeManager::getValue( int valueNo )
	{
	return getRecipeValue(currentRecipe, valueNo);
	}

int TRecipeManager::setValue( int valueNo, float newValue )
	{
#ifdef MSAPANEL
	if (valueNo == RV_IS_USED)
		{
		MsaPanel::UpdateRecipes();
		}
#endif // MSAPANEL
	return setRecipeValue(currentRecipe, valueNo, newValue);
	}

void TRecipeManager::SaveRecipeName()
	{
#ifdef MSAPANEL
	MsaPanel::UpdateRecipes();
#endif // MSAPANEL
	WriteMem(startAddr(), recipeNameLength, (unsigned char*)currentRecipeName);
	}


int TRecipeManager::LoadRecipeToParams( int recipeNo, int recipeStartPos, int paramsStartPos, int parQuantity, run_time_params_float* par )
	{
	if (recipeNo < recipePerLine && recipeStartPos + parQuantity < GetParamsCount() && (unsigned int)paramsStartPos + (unsigned int)parQuantity < par->get_count())
		{
		for (int i = 0; i < parQuantity; i++)
			{
			par[0][paramsStartPos + i] = getRecipeValue(recipeNo, recipeStartPos + i);
			#ifdef DEBUG
 Print("\n\rsetparamm %d %f\n\r", paramsStartPos + i, par[0][paramsStartPos + i]);
 #endif
			}
		#ifdef DEBUG
 Print("\n\rLoaded recipe %d\n\r",recipeNo);
 #endif
		return 1;
		}
	else
		{
		return 0;
		}
	}

int TRecipeManager::getCurrentRecipe()
	{
	return currentRecipe;
	}

int TRecipeManager::setCurrentRecipe( int recipeNo )
	{
	if (recipeNo >=0 && recipeNo < recipePerLine)
		{
		SaveRecipeName();
		curRecipeStartBlock += blocksPerRecipe * (recipeNo - currentRecipe);
		currentRecipe = recipeNo;
		return 1;
		}
	else
		{
		return 0;
		}
	}

int TRecipeManager::ResetRecipeToDefaults( int recipeNo )
	{
	if (recipeNo >= 0 && recipeNo < recipePerLine)
		{
		setRecipeValue(recipeNo, RV_IS_USED, 0);
		setRecipeValue(recipeNo, RV_TO_DEFAULTS, 0);
		setRecipeValue(recipeNo, RV_V1, 200);
		setRecipeValue(recipeNo, RV_V2, 200);
		setRecipeValue(recipeNo, RV_OBJ_TYPE, 1);
		setRecipeValue(recipeNo, RV_FLOW, 15);
		setRecipeValue(recipeNo, RV_PODP_CIRC, 0);
		setRecipeValue(recipeNo, RV_DELTA_TR, 10);
		setRecipeValue(recipeNo, RV_T_WP, 30);
		setRecipeValue(recipeNo, RV_T_WSP, 30);
		setRecipeValue(recipeNo, RV_T_WKP, 30);
		setRecipeValue(recipeNo, RV_T_WOP, 5);
		setRecipeValue(recipeNo, RV_T_S, 80);
		setRecipeValue(recipeNo, RV_T_K, 70);
		setRecipeValue(recipeNo, RV_T_D, 90);
		setRecipeValue(recipeNo, RV_T_DEZSR, 60);
		setRecipeValue(recipeNo, RV_DOP_V_PR_OP, 200);
		setRecipeValue(recipeNo, RV_DOP_V_AFTER_S, 200);
		setRecipeValue(recipeNo, RV_DOP_V_AFTER_K, 200);
		setRecipeValue(recipeNo, RV_DOP_V_OK_OP, 200);
		setRecipeValue(recipeNo, RV_RET_STOP, 40);
		setRecipeValue(recipeNo, RV_V_RAB_ML, 15);
		setRecipeValue(recipeNo, RV_V_RET_DEL, 30);
		setRecipeValue(recipeNo, RV_TM_OP, 300);
		setRecipeValue(recipeNo, RV_TM_S, 600);
		setRecipeValue(recipeNo, RV_TM_K, 600);
		setRecipeValue(recipeNo, RV_TM_S_SK, 500);
		setRecipeValue(recipeNo, RV_TM_K_SK, 500);
		setRecipeValue(recipeNo, RV_TM_D, 600);
		setRecipeValue(recipeNo, RV_TM_DEZSR, 600);
		setRecipeValue(recipeNo, RV_TM_DEZSR_INJECT, 120);
		setRecipeValue(recipeNo, RV_N_RET, 0);
		setRecipeValue(recipeNo, RV_N_UPR, 0);
		setRecipeValue(recipeNo, RV_OS, 0);
		setRecipeValue(recipeNo, RV_OBJ_EMPTY, 0);
		setRecipeValue(recipeNo, RV_PROGRAM_MASK, 0);
		setRecipeValue(recipeNo, RV_T_RINSING_CLEAN, 5);
		setRecipeValue(recipeNo, RV_V_RINSING_CLEAN, 600);

		setRecipeValue(recipeNo, RV_T_SANITIZER_RINSING, 30);
		setRecipeValue(recipeNo, RV_V_SANITIZER_RINSING, 500);
		setRecipeValue(recipeNo, RV_TM_MAX_TIME_OPORBACHOK, 30);

		setRecipeValue(recipeNo, RV_TM_RET_IS_EMPTY, 8);
		setRecipeValue(recipeNo, RV_V_LL_BOT, 15);
		setRecipeValue(recipeNo, RV_R_NO_FLOW, 2);
		setRecipeValue(recipeNo, RV_TM_R_NO_FLOW, 20);

		setRecipeValue(recipeNo, RV_TM_NO_FLOW_R, 20);
		setRecipeValue(recipeNo, RV_TM_NO_CONC, 20);

		//-PID1
		setRecipeValue(recipeNo, RV_PIDP_Z, 95);
		setRecipeValue(recipeNo, RV_PIDP_k, 2);
		setRecipeValue(recipeNo, RV_PIDP_Ti, 30);
		setRecipeValue(recipeNo, RV_PIDP_Td, (float)0.2);
		setRecipeValue(recipeNo, RV_PIDP_dt, 500);
		setRecipeValue(recipeNo, RV_PIDP_dmax, 130);
		setRecipeValue(recipeNo, RV_PIDP_dmin, 0);
		setRecipeValue(recipeNo, RV_PIDP_AccelTime, 30);
		setRecipeValue(recipeNo, RV_PIDP_IsManualMode, 0);
		setRecipeValue(recipeNo, RV_PIDP_UManual, 30);
		setRecipeValue(recipeNo, RV_PIDP_Uk, 0);
		//-PID1-!>
		//-PID2
		setRecipeValue(recipeNo, RV_PIDF_Z, 15);
		setRecipeValue(recipeNo, RV_PIDF_k, 0.5);
		setRecipeValue(recipeNo, RV_PIDF_Ti, 10);
		setRecipeValue(recipeNo, RV_PIDF_Td, (float)0.1);
		setRecipeValue(recipeNo, RV_PIDF_dt, 1000);
		setRecipeValue(recipeNo, RV_PIDF_dmax, 40);
		setRecipeValue(recipeNo, RV_PIDF_dmin, 0);
		setRecipeValue(recipeNo, RV_PIDF_AccelTime, 2);
		setRecipeValue(recipeNo, RV_PIDF_IsManualMode, 0);
		setRecipeValue(recipeNo, RV_PIDF_UManual, 15);
		setRecipeValue(recipeNo, RV_PIDF_Uk, 0);
		//-PID2-!>
		setRecipeValue(recipeNo, P_TM_MAX_TIME_OPORCIP, 300);
		for (int i = RV_RESERV_START; i <= RV_LASTVALVEOFF; i++)
			{
			setRecipeValue(recipeNo, i, 0);
			}
		return 1;
		}
	else
		{
		return 0;
		}
	}

void TRecipeManager::FormRecipeList()
	{
	strcpy(recipeList, "");
	char tmpstr[6];
	char tmprecipename[MAX_REC_NAME_LENGTH];
	for (int i = 0; i < recipePerLine; i++)
		{
		if (getRecipeValue(i, RV_IS_USED) != 0)
			{
			ReadMem(startAddr( i ), recipeNameLength, (unsigned char*)tmprecipename);
			sprintf(tmpstr,"%d##", i + 1);
			strcat(recipeList, tmpstr);
			strcat(recipeList, tmprecipename);
			strcat(recipeList, "||");
			}

		}
	}

void TRecipeManager::LoadRecipeName()
	{
	ReadMem(startAddr(), recipeNameLength, (unsigned char*)currentRecipeName);
	}

int TRecipeManager::ToRecipe( int recNo )
	{
	if (recNo >= 0 && recNo < recipePerLine)
		{
		SaveRecipeName();
		curRecipeStartBlock += blocksPerRecipe * (recNo - currentRecipe);
		currentRecipe = recNo;
		LoadRecipeName();
		return 1;
		}
	else
		{
		return 0;
		}
	}

int TRecipeManager::getRecipeName( int recNO, char* recName )
	{
	return ReadMem(startAddr(recNO), recipeNameLength, (unsigned char*)recName);
	}

int TRecipeManager::OnRecipeDevices( int recipeNo, int msaline /*= 1*/ )
{
	unsigned int devNo;
	char devName[25];
	device* dev;
	int i;
	int errflag = 0;
	for (i = RV_FIRSTVALVEON; i<= RV_LASTVALVEON; i++)
		{
		devNo = (unsigned long)getRecipeValue(recipeNo, i);
		if (devNo > 0)
			{
			sprintf(devName,"LINE%dV%d", msaline, devNo);
			dev = (device*)(V(devName));
			if (dev->get_serial_n() > 0)
				{
				dev->on();
				}
			else
				{
				dev = DEVICE(devNo);
				if (dev->get_serial_n() > 0)
					{
					dev->on();
					}
				else
					{
#ifdef DEBUG
					Print("\n\rOn recipe devices. Valve %d from param %d not found in device list\n\r", devNo, i);
#endif
					errflag = 1;
					}
				}
			}
		}
	for (i = RV_FIRSTVALVEOFF; i<= RV_LASTVALVEOFF; i++)
		{
		devNo = (unsigned long)getRecipeValue(recipeNo, i);
		if (devNo > 0)
			{
			sprintf(devName,"LINE%dV%d", msaline, devNo);
			dev = (device*)(V(devName));
			if (dev->get_serial_n() > 0)
				{
				dev->off();
				}
			else
				{
				dev = DEVICE(devNo);
				if (dev->get_serial_n() > 0)
					{
					dev->off();
					}
				else
					{
#ifdef DEBUG
					Print("\n\rOff recipe devices. Valve %d from param %d not found in device list\n\r", devNo, i);
#endif
					errflag = 1;
					}
				}
			}
		}
	return errflag;
	}

int TRecipeManager::OffRecipeDevices( int recipeNo, int msaline /*= 1*/ )
	{
	unsigned int devNo;
	char devName[25];
	device* dev;
	int i;
	int errflag = 0;
	for (i = RV_FIRSTVALVEON; i<= RV_LASTVALVEON; i++)
		{
		devNo = (unsigned long)getRecipeValue(recipeNo, i);
		if (devNo > 0)
			{
			sprintf(devName,"LINE%dV%d", msaline, devNo);
			dev = (device*)(V(devName));
			if (dev->get_serial_n() > 0)
				{
				dev->off();
				}
			else
				{
				dev = DEVICE(devNo);
				if (dev->get_serial_n() > 0)
					{
					dev->off();
					}
				else
					{
#ifdef DEBUG
					Print("\n\rOff recipe devices. Valve %d from param %d not found in device list\n\r", devNo, i);
#endif
					errflag = 1;
					}
				}
			}
		}
	return errflag;
	}

void TRecipeManager::CopyRecipe()
	{
	if (recipeCopyBuffer != NULL)
		{
		delete[] recipeCopyBuffer;
		}
	recipeCopyBuffer = new unsigned char[BLOCK_SIZE * blocksPerRecipe];
	ReadMem(startAddr(), BLOCK_SIZE * blocksPerRecipe, recipeCopyBuffer);
	}

void TRecipeManager::PasteRecipe()
	{
	if (recipeCopyBuffer != NULL)
		{
		WriteMem(startAddr(), BLOCK_SIZE * blocksPerRecipe, recipeCopyBuffer);
		LoadRecipeName();
		}
	}

void TRecipeManager::NullifyRecipe()
	{
	unsigned char* tempbuff = new unsigned char[BLOCK_SIZE * blocksPerRecipe];
	memset(tempbuff, 0, BLOCK_SIZE * blocksPerRecipe);
	WriteMem(startAddr(), BLOCK_SIZE * blocksPerRecipe, tempbuff);
	delete tempbuff;
	LoadRecipeName();
	}

int TRecipeManager::ReadMem( unsigned long startaddr, unsigned long length, unsigned char* buf )
	{
	memcpy(buf, recipeMemory + startaddr, length);
	return 0;
	}

int TRecipeManager::WriteMem( unsigned long startaddr, unsigned long length, unsigned char* buf )
	{
	memcpy(recipeMemory + startaddr, buf, length);
	return 0;
	}

int TRecipeManager::SaveToFile(const char* filename)
	{
#ifdef DEBUG
	Print("Saving recipes to file %s\n", filename);
#endif // DEBUG
	FILE* memFile = NULL;
	memFile = fopen(filename, "r+b");
	if (NULL == memFile)
		{
		memFile = fopen(filename, "w+b");
		}
	if (memFile)
		{
		fseek(memFile, 0, SEEK_SET);
		fwrite(recipeMemory, 1, recipeMemorySize, memFile);
		fclose(memFile);
		}
	return 0;
	}

int TRecipeManager::LoadFromFile( const char* filename )
	{
	FILE* memFile = NULL;
	memset(recipeMemory, 0, recipeMemorySize);
	memFile = fopen(filename, "r+b");
	if (memFile)
		{
		fread(recipeMemory, 1, recipeMemorySize, memFile);
		fclose(memFile);
		}
	return 0;
	}
