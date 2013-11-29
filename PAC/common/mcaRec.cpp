#include "mcaRec.h"
#include "PAC_dev.h"
#ifdef MSAPANEL
#include "msapanel.h"
#include "dtime.h"
#endif // MSAPANEL


int TRecipeManager::startRecipeBlock = 0;

int TRecipeManager::recipePerLine = 20;

int TRecipeManager::blocksPerRecipe = 2;

int TRecipeManager::recipeNameLength = MAX_REC_NAME_LENGTH - 8;

int TRecipeManager::startRecipeParamsOffset = MAX_REC_NAME_LENGTH;

unsigned char* TRecipeManager::recipeCopyBuffer = NULL;

TRecipeManager::TRecipeManager( int lineNo ): lineNo(lineNo),
	currentRecipe(0),
	lastEvalTime(get_millisec()),
	curRecipeStartBlock(0),
	recipeStartAddr(0L)
	{
	recipeMemorySize = blocksPerRecipe * BLOCK_SIZE * recipePerLine;
	recipeMemory = new unsigned char[recipeMemorySize];
#ifdef WIN_OS
	memset(recipeMemory, 0, recipeMemorySize);
	memFileName = new char[20];
	sprintf(memFileName, "line%drec.bin", lineNo);
	memFile = fopen(memFileName, "r+");
	if (NULL == memFile)
	{
	memFile = fopen(memFileName, "w+");
	}
	fread(recipeMemory, 1, recipeMemorySize, memFile);
#endif // WIN_OS
	currentRecipeName = new char[recipeNameLength];
	recipeList = new char[(recipeNameLength + 6) * recipePerLine];
	strcpy(recipeList,"");
	ReadMem(startAddr(), recipeNameLength, (unsigned char*)currentRecipeName);
	FormRecipeList();
	}

TRecipeManager::~TRecipeManager()
	{
	SaveRecipeName();
	delete currentRecipeName;
	delete recipeList;
#ifdef WIN_OS
	if (memFile)
		{
		fseek(memFile, 0, SEEK_SET);
		fwrite(recipeMemory, 1, recipeMemorySize, memFile);
		fclose(memFile);
		}
#endif //WIN_OS
	delete recipeMemory;

#ifdef WIN_OS
	delete memFileName;
#endif
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
		ModbusServ::UpdateRecipes();
		}
#endif // MSAPANEL
	return setRecipeValue(currentRecipe, valueNo, newValue);
	}

void TRecipeManager::SaveRecipeName()
	{
#ifdef MSAPANEL
	ModbusServ::UpdateRecipes();
#endif // MSAPANEL
	WriteMem(startAddr(), recipeNameLength, (unsigned char*)currentRecipeName);
	}


int TRecipeManager::LoadRecipeToParams( int recipeNo, int recipeStartPos, int paramsStartPos, int parQuantity, run_time_params_float* par )
	{
	if (recipeNo < recipePerLine && recipeStartPos + parQuantity < GetParamsCount() && paramsStartPos + parQuantity < par->get_count())
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

int TRecipeManager::OnRecipeDevices( int recipeNo )
	{
	unsigned char devType;
	unsigned int devNo;
	device* dev;
	int i;
	int errflag = 0;
	for (i = RV_FIRSTVALVEON; i<= RV_LASTVALVEON; i++)
		{
		devType = (((unsigned long)getRecipeValue(recipeNo, i)) >> 16) & 0xFF;
		devNo = ((unsigned long)getRecipeValue(recipeNo, i)) & 0xFFFF;
		switch (devType)
			{
			case 0:
				if (devNo > 0)
					{
					dev = (device*)(V(devNo));
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
				break;
			case 1:
				if (devNo > 0)
					{
					dev = (device*)(DO(devNo));
					if (dev->get_serial_n() > 0)
						{
						dev->on();
						}
					else
						{
#ifdef DEBUG
						Print("\n\rOn recipe devices. URP %d from param %d not found in device list\n\r", devNo, i);
#endif
						errflag = 1;
						}
					}
				break;
			}
		}
	for (i = RV_FIRSTVALVEOFF; i<= RV_LASTVALVEOFF; i++)
		{
		devType = (((unsigned long)getRecipeValue(recipeNo, i)) >> 16) & 0xFF;
		devNo = ((unsigned long)getRecipeValue(recipeNo, i)) & 0xFFFF;
		switch (devType)
			{
			case 0:
				if (devNo > 0)
					{
					dev = (device*)(V(devNo));
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
				break;
			case 1:
				if (devNo > 0)
					{
					dev = (device*)(DO(devNo));
					if (dev->get_serial_n() > 0)
						{
						dev->off();
						}
					else
						{
#ifdef DEBUG
						Print("\n\rOff recipe devices. URP %d from param %d not found in device list\n\r", devNo, i);
#endif
						errflag = 1;
						}
					}
				break;
			}
		}
	return errflag;
	}

int TRecipeManager::OffRecipeDevices( int recipeNo )
	{
	unsigned char devType;
	unsigned int devNo;
	device* dev;
	int i;
	int errflag = 0;
	for (i = RV_FIRSTVALVEON; i<= RV_LASTVALVEON; i++)
		{
		devType = (((unsigned long)getRecipeValue(recipeNo, i)) >> 16) & 0xFF;
		devNo = ((unsigned long)getRecipeValue(recipeNo, i)) & 0xFFFF;
		switch (devType)
			{
			case 0:
				dev = (device*)(V(devNo));
				if (devNo > 0)
					{
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
				break;
			case 1:
				dev = (device*)(DO(devNo));
				if (devNo > 0)
					{
					if (dev->get_serial_n() > 0)
						{
						dev->off();
						}
					else
						{
#ifdef DEBUG
						Print("\n\rOff recipe devices. URP %d from param %d not found in device list\n\r", devNo, i);
#endif
						errflag = 1;
						}
					}
				break;
			}
		}
	return errflag;
	}

void TRecipeManager::CopyRecipe()
	{
	if (recipeCopyBuffer != NULL)
		{
		delete recipeCopyBuffer;
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
