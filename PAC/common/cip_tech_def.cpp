#ifdef WIN_OS
#pragma warning(push)
#pragma warning(disable: 4244)
#endif // WIN_OS

#include "cip_tech_def.h"

int getNexpPrg(int cur, unsigned long prg)
	{
	int i;
	int tmp;

	if (cur<0)
		{
		tmp=15;
		}
	else
		{
		tmp=cur-1;
		}
	if (tmp>=0 && tmp<16)
		{
		for (i=tmp; i>=0; i--)
			{
			if (((prg>>i) & 1) == 1)
				{
				return i;
				}
			}
		return -1;
		}
	return -1;
	}

cipline_tech_object::cipline_tech_object( const char* name, u_int number, u_int type,
										 const char* name_Lua, u_int states_count, u_int timers_count,
										 u_int par_float_count, u_int runtime_par_float_count, u_int par_uint_count,
										 u_int runtime_par_uint_count ) : tech_object(name, number, type,
										 name_Lua, states_count, timers_count, par_float_count, runtime_par_float_count,
										 par_uint_count, runtime_par_uint_count)
	{
#ifdef DEBUG
	Print("\n\r Create cip tech_object\n\r");
#endif //DEBUG
	int i;
	nmr = number;
	PIDPump = new PID(nextpidnumber());
	PIDFlow = new PID(nextpidnumber());
	PIDF = 0;
	PIDP = 0;
	if (0 == parpar)
		{
		parpar = new saved_params<float, true>(30, "PAR_MAIN");
		}
	for (i = 0; i < TMR_CNT; i++)
		{
		T[i]=new timer;
		}
	for (i = 0; i < SAV_CNT; i++)
		{
		SAV[i]=new TSav;
		}
	no_neutro = 0;
	ret_circ_flag = 0;
	ret_circ_delay = get_millisec();
	ret_overrride = 0;
	concentration_ok = 0;
	return_ok = 0;
	tank_is_empty = 0;
	enddelayTimer = 0;
	blocked = 0;
	disable_tank_heating = 0;
	default_programlist = 0x3FF;
	pump_control = 0;
	NPC = 0;
	bachok_lvl_err_delay = get_millisec();
	sort_delay = get_millisec();
	steam_valve_delay = get_millisec();
	loadedRecName = new char[TRecipeManager::recipeNameLength];
	programList = new char[PROGRAM_LIST_MAX_LEN];
	strcpy(programList, "");
	currentProgramName = new char[PROGRAM_MAX_LEN];
	strcpy(currentProgramName, "");

	lineRecipes = new TRecipeManager(number - 1);
	opcip=0;
	curstep=0;
	ncmd=0;
	state=0;
	loadedRecipe = -1;
	curprg=-1;
	cip_in_error = 0;

	resetProgramName();
	resetRecipeName();
	resetProgramList();
	ResetStat();
	rt_par_float[P_CUR_REC] =  (float)(lineRecipes->getCurrentRecipe() + 1);
	rt_par_float[P_SELECT_REC] = 0;
	rt_par_float[P_SELECT_PRG] = 0;
	}

cipline_tech_object::~cipline_tech_object()
	{
	int i;
	if (parpar != 0)
		{
		delete(parpar);
		parpar = 0;
		}
	if (PIDF != 0)
		{
		delete(PIDF);
		}
	if (PIDP != 0)
		{
		delete(PIDP);
		}
	for (i = 0; i < SAV_CNT; i++) {
		delete SAV[i];
		}
	for (i = 0; i < TMR_CNT; i++) {
		delete T[i];
		}
	if (lineRecipes)
		{
		delete lineRecipes;
		}
	}

int cipline_tech_object::save_device( char *buff )
	{
	int i;
	sprintf( buff, "t.%s = t.%s or {}\nt.%s=\n\t{\n",
		name_Lua, name_Lua,
		name_Lua );

	int answer_size = strlen( buff );

	//Команда
	sprintf( buff + answer_size, "\tCMD=%lu,\n", ( u_long ) ncmd );
	answer_size += strlen( buff + answer_size );

	//Состояние
	sprintf( buff + answer_size, "\tSTATE=%lu,\n", ( u_long ) state );
	answer_size += strlen( buff + answer_size );

	//Текущая операция
	sprintf( buff + answer_size, "\tOPER=%lu,\n", ( u_long ) curstep );
	answer_size += strlen( buff + answer_size );

	//Загруженный рецепт
	sprintf(buff + answer_size, "\tLOADED_REC='%s',\n", loadedRecName);
	answer_size += strlen( buff + answer_size );

	//Имя рецепта для редактирования
	sprintf(buff + answer_size, "\tCUR_REC='%s',\n", lineRecipes->currentRecipeName);
	answer_size += strlen( buff + answer_size );

	//Выбранная программа мойки
	sprintf(buff + answer_size, "\tCUR_PRG='%s',\n", currentProgramName);
	answer_size += strlen( buff + answer_size );

	//Список доступных программ мойки
	sprintf(buff + answer_size, "\tPRG_LIST='%s',\n", programList);
	answer_size += strlen( buff + answer_size );

	//Список доступных объектов мойки
	sprintf(buff + answer_size, "\tREC_LIST='%s',\n", lineRecipes->recipeList);
	answer_size += strlen( buff + answer_size );

	//Время простоя.
	char up_time_str [ 50 ];
	u_int_4 up_hours;
	u_int_4 up_mins;
	u_int_4 up_secs;

	up_secs = modes_manager->get_idle_time() / 1000;

	up_hours = up_secs / ( 60 * 60 );
	up_mins = up_secs / 60 % 60 ;
	up_secs %= 60;

	snprintf( up_time_str, 50, "\tIDLE_TIME = \'%02lu:%02lu:%02lu\',\n",
		( u_long ) up_hours, ( u_long ) up_mins, ( u_long ) up_secs );
	sprintf( buff + answer_size, "%s", up_time_str );
	answer_size += strlen( buff + answer_size );

	//Параметры.
	answer_size += par_float.save_device( buff + answer_size, "\t" );
	answer_size += par_uint.save_device( buff + answer_size, "\t" );
	answer_size += rt_par_float.save_device( buff + answer_size, "\t" );
	answer_size += rt_par_uint.save_device( buff + answer_size, "\t" );
	//Параметры станции
	if (1 == number)
		{
		answer_size += parpar->save_device(buff + answer_size, "\t");
		}
	//Параметры текущего рецепта
	sprintf(buff + answer_size, "\tREC_PAR = \n\t{\n\t\t");
	answer_size += strlen( buff + answer_size );
	for (i = 1; i <= lineRecipes->GetParamsCount(); i++)
		{
		sprintf(buff + answer_size, "%.2f, ", lineRecipes->getValue(i-1));
		answer_size += strlen( buff + answer_size );
		}
	sprintf(buff + answer_size, "\n\t}\n");
	answer_size += strlen( buff + answer_size );

	sprintf( buff + answer_size, "\t}\n" );
	answer_size += strlen( buff + answer_size );
	return answer_size;
	}

int cipline_tech_object::set_cmd( const char *prop, u_int idx, double val )
	{
	if ( strcmp( prop, "S_PAR_F" ) == 0 )
		{
		par_float.save( idx, ( float ) val );
		return 0;
		}

	if ( strcmp( prop, "RT_PAR_F" ) == 0 )
		{
		rt_par_float[ idx ] = ( float ) val;
		return 0;
		}

	if ( strcmp( prop, "PAR_MAIN" ) == 0 )
		{
		parpar->save( idx, ( float ) val );
		return 0;
		}

	if ( strcmp( prop, "S_PAR_UI" ) == 0 )
		{
		par_uint.save( idx, ( u_int_4 ) val );
		return 0;
		}

	if ( strcmp( prop, "RT_PAR_UI" ) == 0 )
		{
		rt_par_uint[ idx ] = ( u_int_4 ) val;
		return 0;
		}

	if ( strcmp( prop, "REC_PAR" ) == 0 )
		{
		lineRecipes->setValue(idx - 1, (float)val);
		return 0;
		}

	if ( strcmp( prop, "CMD" ) == 0 )
		{
		ncmd = (u_int_4) val;
		return 0;
		}

#ifdef DEBUG
	Print( "Eror tech_object::set_cmd(...), prop = \"%s\", idx = %u, val = %f\n",
		prop, idx, val );
#endif // DEBUG

	return 1;
	}

int cipline_tech_object::set_cmd( const char *prop, u_int idx, const char* val )
	{
	if (0 == strcmp(prop, "CUR_REC"))
		{
#ifdef WIN_OS
		strncpy_s(lineRecipes->currentRecipeName, lineRecipes->recipeNameLength,
            val, _TRUNCATE);
#else
        strncpy( lineRecipes->currentRecipeName, val, lineRecipes->recipeNameLength );
#endif
		return 0;
		}
#ifdef DEBUG
	Print( "Eror tech_object::set_cmd(...), prop = \"%s\", idx = %u, val = %s\n",
		prop, idx, val );
#endif // DEBUG
	return 1;
	}

int cipline_tech_object::nextpidnumber()
	{
	int curpid = pidnumber;
	pidnumber++;
	return curpid;
	}

void cipline_tech_object::initline()
	{
	V00 = V(number * 100);
	V01 = V(number * 100 + 1);
	V02 = V(number * 100 + 2);
	V03 = V(number * 100 + 3);
	V04 = V(number * 100 + 4);
	V05 = V(number * 100 + 5);
	V06 = V(number * 100 + 6);
	V07 = V(number * 100 + 7);
	V08 = V(number * 100 + 8);
	V09 = V(number * 100 + 9);
	V10 = V(number * 100 + 10);
	V11 = V(number * 100 + 11);
	V12 = V(number * 100 + 12);
	V13 = V(number * 100 + 13);

	NP = M(number * 100 + 3);
	NK = M(2);
	NS = M(1);
	LL = LS(number * 100 + 3);
	LM = LS(number * 100 + 2);
	LH = LS(number * 100 + 1);
	LWH = LS(8);
	LWL = LS(9);
	LSH= LS(4);
	LSL = LS(5);
	LKH = LS(6);
	LKL = LS(7);
	TP = TE(number * 100 + 1);
	TR = TE(number * 100 + 2);
	cnt = FQT(number * 100 + 1);
	Q = QT(number * 100 + 1);
	FL = FS(101);
	ao = AO(number * 10 + 14);

#ifdef DEBUG
	LSL->set_cmd("ST", 0, ((device*)LSL)->get_sub_type() == device::DST_LS_MIN ? OFF:ON);
	LSH->set_cmd("ST", 0, ((device*)LSH)->get_sub_type() == device::DST_LS_MIN ? OFF:ON);
	LKL->set_cmd("ST", 0, ((device*)LKL)->get_sub_type() == device::DST_LS_MIN ? OFF:ON);
	LKH->set_cmd("ST", 0, ((device*)LKH)->get_sub_type() == device::DST_LS_MIN ? OFF:ON);
	LWL->set_cmd("ST", 0, ((device*)LWL)->get_sub_type() == device::DST_LS_MIN ? OFF:ON);
	LWH->set_cmd("ST", 0, ((device*)LWH)->get_sub_type() == device::DST_LS_MIN ? OFF:ON);
	LL->set_cmd("ST", 0, ((device*)LL)->get_sub_type() == device::DST_LS_MIN ? OFF:ON);
	LM->set_cmd("ST", 0, ((device*)LM)->get_sub_type() == device::DST_LS_MIN ? OFF:ON);
	LH->set_cmd("ST", 0, ((device*)LH)->get_sub_type() == device::DST_LS_MIN ? OFF:ON);
	Print("Init Line %d\n\r", number);
#endif //DEBUG
	Mdls[nmr - 1] = this;
	MdlsCNT++;
	}

int cipline_tech_object::evaluate()
	{
	int res;
	if (EvalBlock())
		{
		return -1;
		}

	EvalRecipes();

	if (ncmd!=0)
		{
		EvalCommands();
		ncmd=0;
		}
	else
		{
		if (state>0)
			{
			EvalPIDS();
			res = EvalCipInProgress();
			if (pump_control)
				{
				NPC->Eval();
				}
			if (res < 0)
				{
				return res;
				}
			cip_in_error = 0;
			}
		else
			{
			EvalCipInError();
			if (!cip_in_error)
				{
				rt_par_float[STP_ERRCOUNT] = rt_par_float[STP_ERRCOUNT] + 1;
				cip_in_error = 1;
				}
			}
		}
	return 0;
	}

void cipline_tech_object::resetProgramName()
	{
	sprintf(currentProgramName, "%c%c %c%c%c%c%c%c%c",205,229,226,251,225,240,224,237,0);
	}

void cipline_tech_object::resetRecipeName()
	{
	sprintf(loadedRecName, "%c%c %c%c%c%c%c%c%c",205,229,226,251,225,240,224,237,0);
	}

void cipline_tech_object::resetProgramList( unsigned long programmask /*= 0xB00*/ )
	{
	char tmp_str[32];
#ifdef MSAPANEL
	prgListLen = 0;
	ModbusServ::UpdateLinePrograms(nmr);
#endif
	strcpy(programList,"");
	if ((SPROG_ACID_PREPARATION & programmask) == SPROG_ACID_PREPARATION)
		{
		sprintf(tmp_str, "%d##Наведение кислоты||", SPROG_ACID_PREPARATION);
#ifdef MSAPANEL
		sprintf(prgArray[prgListLen], "Наведение кислоты");
		prgNumber[prgListLen] = SPROG_ACID_PREPARATION;
		prgListLen++;
#endif
		strcat(programList,tmp_str);
		}
	if ((SPROG_CAUSTIC_PREPARATION & programmask) == SPROG_CAUSTIC_PREPARATION)
		{
		sprintf(tmp_str, "%d##Наведение щелочи||", SPROG_CAUSTIC_PREPARATION);
#ifdef MSAPANEL
		sprintf(prgArray[prgListLen], "Наведение щелочи");
		prgNumber[prgListLen] = SPROG_CAUSTIC_PREPARATION;
		prgListLen++;
#endif
		strcat(programList,tmp_str);
		}
#ifdef SELFCLEAN
	if (scline == nmr)
		{
		if ((SPROG_SELF_CLEAN & programmask) == SPROG_SELF_CLEAN)
			{
			sprintf(tmp_str, "%d##Очистка танков||", SPROG_SELF_CLEAN);
#ifdef MSAPANEL
			sprintf(prgArray[prgListLen], "Очистка танков");
			prgNumber[prgListLen] = SPROG_SELF_CLEAN;
			prgListLen++;
#endif
			strcat(programList,tmp_str);
			}
		}
#endif //SELFCLEAN
	}

void cipline_tech_object::formProgramList( unsigned long programmask )
	{
	char tmp_str[32];
#ifdef MSAPANEL
	prgListLen = 0;
	ModbusServ::UpdateLinePrograms(nmr);
#endif
	if (programmask == 0)
		{
		programmask = default_programlist;
		}
	strcpy(programList,"");
	if ((programmask >> 0) & 1)
		{
		sprintf(tmp_str, "%d##Дезинф||", SPROG_HOTWATER);
		strcat(programList,tmp_str);
#ifdef MSAPANEL
		sprintf(prgArray[prgListLen], "Дезинфeкция");
		prgNumber[prgListLen] = SPROG_HOTWATER;
		prgListLen++;
#endif
		}
	if ((programmask >> 1) & 1)
		{
		sprintf(tmp_str, "%d##Ополаск||", SPROG_RINSING);
		strcat(programList,tmp_str);
#ifdef MSAPANEL
		sprintf(prgArray[prgListLen], "Ополаскивание");
		prgNumber[prgListLen] = SPROG_RINSING;
		prgListLen++;
#endif
		}
	if ((programmask >> 2) & 1)
		{
		sprintf(tmp_str, "%d##Опол+Дез||", SPROG_RINSING_HOTWATER);
		strcat(programList,tmp_str);
#ifdef MSAPANEL
		sprintf(prgArray[prgListLen], "Опол+Дезинф");
		prgNumber[prgListLen] = SPROG_RINSING_HOTWATER;
		prgListLen++;
#endif
		}
	if ((programmask >> 3) & 1)
		{
		sprintf(tmp_str, "%d##Кислота||", SPROG_ACID);
		strcat(programList,tmp_str);
#ifdef MSAPANEL
		sprintf(prgArray[prgListLen], "Кислота");
		prgNumber[prgListLen] = SPROG_ACID;
		prgListLen++;
#endif
		}
	if ((programmask >> 4) & 1)
		{
		sprintf(tmp_str, "%d##Кисл+Дез||", SPROG_ACID_HOTWATER);
		strcat(programList,tmp_str);
#ifdef MSAPANEL
		sprintf(prgArray[prgListLen], "Кисл+Дезинф");
		prgNumber[prgListLen] = SPROG_ACID_HOTWATER;
		prgListLen++;
#endif
		}
	if ((programmask >> 5) & 1)
		{
		sprintf(tmp_str, "%d##Щелочь||", SPROG_CAUSTIC);
		strcat(programList,tmp_str);
#ifdef MSAPANEL
		sprintf(prgArray[prgListLen], "Щелочь");
		prgNumber[prgListLen] = SPROG_CAUSTIC;
		prgListLen++;
#endif
		}
	if ((programmask >> 6) & 1)
		{
		sprintf(tmp_str, "%d##Щел+Дез||", SPROG_CAUSTIC_HOTWATER);
		strcat(programList,tmp_str);
#ifdef MSAPANEL
		sprintf(prgArray[prgListLen], "Щелочь+Дезинф");
		prgNumber[prgListLen] = SPROG_CAUSTIC_HOTWATER;
		prgListLen++;
#endif
		}
	if ((programmask >> 7) & 1)
		{
		sprintf(tmp_str, "%d##Щел+Кисл+Дез||", SPROG_CAUSTIC_ACID_HOTWATER);
		strcat(programList,tmp_str);
#ifdef MSAPANEL
		sprintf(prgArray[prgListLen], "Щел+Кисл+Дезинф");
		prgNumber[prgListLen] = SPROG_CAUSTIC_ACID_HOTWATER;
		prgListLen++;
#endif
		}
	if ((programmask >> 8) & 1)
		{
		sprintf(tmp_str, "%d##Щел+Кисл||", SPROG_CAUSTIC_ACID);
		strcat(programList,tmp_str);
#ifdef MSAPANEL
		sprintf(prgArray[prgListLen], "Щел+Кислота");
		prgNumber[prgListLen] = SPROG_CAUSTIC_ACID;
		prgListLen++;
#endif
		}
	if ((programmask >> 9) & 1)
		{
		sprintf(tmp_str, "%d##Опол.чист.водой||", SPROG_RINSING_CLEAN);
		strcat(programList,tmp_str);
#ifdef MSAPANEL
		sprintf(prgArray[prgListLen], "Опол.чист водой");
		prgNumber[prgListLen] = SPROG_RINSING_CLEAN;
		prgListLen++;
#endif
		}
	if ((programmask >> 10) & 1)
		{
		sprintf(tmp_str, "%d##ДезСР||", SPROG_SANITIZER);
		strcat(programList,tmp_str);
#ifdef MSAPANEL
		sprintf(prgArray[prgListLen], "Дезраствор");
		prgNumber[prgListLen] = SPROG_SANITIZER;
		prgListLen++;
#endif
		}
	}

void cipline_tech_object::loadProgramFromList( int selectedPrg )
	{
	switch (selectedPrg)
		{
		case SPROG_RINSING_CLEAN:
			sprintf(currentProgramName, "Опол.чист.водой");
			rt_par_float[P_PROGRAM] = SPROG_RINSING_CLEAN;
			break;
		case SPROG_SANITIZER:
			sprintf(currentProgramName, "ДезРаствор");
			rt_par_float[P_PROGRAM] =  SPROG_SANITIZER;
			break;
		case SPROG_HOTWATER:
			sprintf(currentProgramName, "Дезинфекция");
			rt_par_float[P_PROGRAM] =  SPROG_HOTWATER;
			break;
		case SPROG_RINSING:
			sprintf(currentProgramName, "Ополаскивание");
			rt_par_float[P_PROGRAM] =  SPROG_RINSING;
			break;
		case SPROG_RINSING_HOTWATER:
			sprintf(currentProgramName, "Опол+Дезинф");
			rt_par_float[P_PROGRAM] =  SPROG_RINSING_HOTWATER;
			break;
		case SPROG_ACID:
			sprintf(currentProgramName, "Кислота");
			rt_par_float[P_PROGRAM] =  SPROG_ACID;
			break;
		case SPROG_ACID_HOTWATER:
			sprintf(currentProgramName, "Кислота+Дез");
			rt_par_float[P_PROGRAM] =  SPROG_ACID_HOTWATER;
			break;
		case SPROG_CAUSTIC:
			sprintf(currentProgramName, "Щелочь");
			rt_par_float[P_PROGRAM] =  SPROG_CAUSTIC;
			break;
		case SPROG_CAUSTIC_HOTWATER:
			sprintf(currentProgramName, "Щел+Дезинф");
			rt_par_float[P_PROGRAM] =  SPROG_CAUSTIC_HOTWATER;
			break;
		case SPROG_CAUSTIC_ACID:
			sprintf(currentProgramName, "Щел+Кислота");
			rt_par_float[P_PROGRAM] =  SPROG_CAUSTIC_ACID;
			break;
		case SPROG_CAUSTIC_ACID_HOTWATER:
			sprintf(currentProgramName, "Щел+Кисл+Дез");
			rt_par_float[P_PROGRAM] =  SPROG_CAUSTIC_ACID_HOTWATER;
			break;
		case SPROG_ACID_PREPARATION:
			sprintf(currentProgramName, "Нав. кислоты");
			rt_par_float[P_PROGRAM] =  SPROG_ACID_PREPARATION;
			break;
		case SPROG_CAUSTIC_PREPARATION:
			sprintf(currentProgramName, "Нав. щелочи");
			rt_par_float[P_PROGRAM] =  SPROG_CAUSTIC_PREPARATION;
			break;
#ifdef SELFCLEAN
		case SPROG_SELF_CLEAN:
			sprintf(currentProgramName, "Очистка танков");
			rt_par_float[P_PROGRAM] =  SPROG_SELF_CLEAN;
			break;
#endif //SELFCLEAN
		}
	}

void cipline_tech_object::closeLineValves()
	{
	unsigned int i;
	unsigned int vstart = msa_number * 1000L + nmr * 100;
	unsigned int vend = vstart + 99;
#ifdef DEBUG
	Print("\n\rClosing line valves from %d to %d...", vstart, vend);
#endif // DEBUG
	for (i = vstart; i <= vend; i++)
		{
		V(i)->off();
		}
#ifdef DEBUG
	Print("\n\rDone closing valves\n\r");
#endif // DEBUG
	}

int cipline_tech_object::isTank()
	{
	if ((((int)rt_par_float[P_OBJ_TYPE]) & 0xFF) == 1 || (((int)rt_par_float[P_OBJ_TYPE]) & 0xFF) == 3)
		{
		return 1;
		}
	else
		{
		return 0;
		}
	}

int cipline_tech_object::isLine()
	{
	if ((((int)rt_par_float[P_OBJ_TYPE]) & 0xFF) == 2 || (((int)rt_par_float[P_OBJ_TYPE]) & 0xFF) == 4)
		{
		return 1;
		}
	else
		{
		return 0;
		}
	}

int cipline_tech_object::getValvesConflict()
	{
	int i,j,k;
	for (i = TRecipeManager::RV_FIRSTVALVEON; i<=TRecipeManager::RV_LASTVALVEOFF; i++)
		{
		if (lineRecipes->getRecipeValue(loadedRecipe,i) != 0)
			{
			for (j = 0; j < MdlsCNT; j++)
				{
				if (Mdls[j]->nmr != nmr && Mdls[j]->state != 0 && Mdls[j]->loadedRecipe >=0)
					{
					if (i <= TRecipeManager::RV_LASTVALVEON)
						{
						for (k = TRecipeManager::RV_FIRSTVALVEON; k <= TRecipeManager::RV_LASTVALVEOFF; k++)
							{
							if (lineRecipes->getRecipeValue(loadedRecipe, i) == Mdls[j]->lineRecipes->getRecipeValue(Mdls[j]->loadedRecipe, k))
								{
#ifdef DEBUG
								Print("Opened valve %d on line %d conflicts with valve on line %d", (int)lineRecipes->getRecipeValue(loadedRecipe, i), nmr, Mdls[j]->nmr);
#endif
								return 1;
								}
							}
						}
					else
						{
						for (k = TRecipeManager::RV_FIRSTVALVEON; k <= TRecipeManager::RV_LASTVALVEON; k++)
							{
							if (lineRecipes->getRecipeValue(loadedRecipe, i) == Mdls[j]->lineRecipes->getRecipeValue(Mdls[j]->loadedRecipe, k))
								{
#ifdef DEBUG
								Print("Closed valve %d on line %d conflicts with opened valve on line %d",(int)lineRecipes->getRecipeValue(loadedRecipe, i), nmr, Mdls[j]->nmr);
#endif
								return 1;
								}
							}
						}
					}
				}
			}
		}
	return 0;
	}

void cipline_tech_object::set_station_par( int parno, float newval )
	{
	parpar->save(parno, newval);
	}

float cipline_tech_object::get_station_par( int parno )
	{
	return parpar[0][parno];
	}

void cipline_tech_object::ResetStat( void )
	{
	for ( int i = STP_QAVS; i <= STP_ERRCOUNT; i++ )
		{
		rt_par_float[i] = 0;
		}
	}

void cipline_tech_object::EnablePumpController()
	{
	if (0 == NPC)
		{
		InitPumpController();
		}
	pump_control = 1;
	}

void cipline_tech_object::DisablePumpController()
	{
	pump_control = 0;
	}

void cipline_tech_object::InitPumpController()
	{
	if (0 == NPC)
		{
		NPC = new TPumpController(NP);
		}
	}

int cipline_tech_object::EvalBlock()
	{
	if (ncmd == MCMD_LOCK_MODULE)
		{
		blocked = 1;
		ncmd = 0;
		}
	if (ncmd == MCMD_UNLOCK_MODULE)
		{
		blocked = 0;
		if (curstep>0)
			{
			state = -1;
			}
		else
			{
			state = 0;
			}
		ncmd = 0;
		}
	if (0 == blocked)
		{
		return 0;
		}
	if (state>0)
		{
		Stop(curstep);
		}
	ncmd = 0;
	state = -1000;
	StopDev();
	NS->off();
	NK->off();
	return 1;
	}

void cipline_tech_object::Stop( int step )
	{
	switch (step)
		{
		case 105:
		case 109:
			NS->off();
			break;
		case 115:
		case 119:
			NK->off();
			break;
		}
	RHI();
	PT();
	cnt->pause();
	StopDev();
	state=0;
	}

void cipline_tech_object::StopDev( void )
	{
	V01->off();
	V03->off();
	V02->off();
	V04->off();
	V13->off();
	V05->off();
	V06->off();
	V07->off();
	V08->off();
	V09->off();
	V10->off();
	V11->off();
	V12->off();
	V00->off();
	if (pump_control) {NPC->off();} else {NP->off();}
	if (PIDP->HI==0) PIDP->off();
	if (PIDF->HI==0) PIDF->off();
	ret_overrride = 0;
	SetRet(OFF);
#ifdef MEDIUM_CHANGE
	UPR(MSA_NUMBER * 1000L + 3 +nmr*100)->off();
#endif
#ifdef SELFCLEAN
	if (scline == nmr)
		{
		if (0 == VSMG->HI) VSMG->off();
		if (0 == VSDREN->HI) VSDREN->off();
		if (0 == VKMG->HI) VKMG->off();
		if (0 == VKDREN->HI) VKDREN->off();
		if (0 == VWMG->HI) VWMG->off();
		if (0 == VWDREN->HI) VWDREN->off();
		}
#endif //SELFCLEAN
	RHI();
	}

void cipline_tech_object::RHI( void )
	{
	PIDP->HI=0;
	PIDF->HI=0;
	}

void cipline_tech_object::PT( void )
	{
	int i;
	for (i=0; i<TMR_CNT; i++) T[i]->pause();
	}

void cipline_tech_object::RT( void )
	{
	int i;
	for (i=0; i<TMR_CNT; i++) T[i]->reset();
	}

int cipline_tech_object::EvalRecipes()
	{
	lineRecipes->EvalRecipe();
	rt_par_float[P_LOADED_RECIPE] = loadedRecipe + 1;
	//переход к рецепту
	if (rt_par_float[P_CUR_REC] != lineRecipes->getCurrentRecipe() + 1)
		{
		lineRecipes->ToRecipe( ( int ) rt_par_float[P_CUR_REC] - 1);
		rt_par_float[P_CUR_REC] = lineRecipes->getCurrentRecipe() + 1;
		}
	//выбор рецепта
	if (rt_par_float[P_SELECT_REC] > 0)
		{
		lineRecipes->LoadRecipeToParams( ( int ) rt_par_float[P_SELECT_REC] - 1,
            2, 16, 39, &rt_par_float);
		lineRecipes->getRecipeName( ( int ) rt_par_float[P_SELECT_REC] - 1,
            loadedRecName);
		loadedRecipe = ( int ) rt_par_float[P_SELECT_REC] - 1;
		rt_par_float[P_SELECT_REC] = 0;
		formProgramList((unsigned int)(rt_par_float[P_PROGRAM_MASK]));
		}
	//выбор программы
	if (rt_par_float[P_SELECT_PRG] > 0)
		{
		loadProgramFromList( ( int ) rt_par_float[P_SELECT_PRG]);
		rt_par_float[P_SELECT_PRG] = 0;
		}
	return 0;
	}

int cipline_tech_object::SetRet( int val )
	{
	if (ret_overrride)
		{
		return 0;
		}
	device* n_ret = (device*)(M((u_int)(rt_par_float[P_N_RET])));
	if (0 == n_ret->get_serial_n())
		{
		n_ret = (device*)(DO((u_int)(rt_par_float[P_N_UPR])));
		}

	if (n_ret->get_serial_n() > 0)
		{
		if (val)
			{
			n_ret->on();
			rt_par_float[P_RET_STATE] = ON;
			}
		else
			{
			n_ret->off();
			rt_par_float[P_RET_STATE] = OFF;
			}
		}
	return 0;
	}

int cipline_tech_object::EvalCommands()
	{
	switch (ncmd)
		{
		case MCMD_NEXT:
			if (state !=0 && curstep != 555)
				{
				rt_par_float[STP_STEPS_OVER] = rt_par_float[STP_STEPS_OVER] + 1; //количество пропущенных операций
				curstep=GoToStep(curstep, 0);
				state=1;
				InitStep(curstep, 0);
				}
			break;
		case MCMD_PAUSE:
			if (state>0 && curstep != 555)
				{
				Stop(curstep);
				ncmd=0;
				state=-1;
				return -1;
				}
			break;
		case MCMD_RESET:
			if (state != 0)
				{
				rt_par_float[STP_RESETSTEP] = curstep; //шаг, на котором было сброшено
				Stop(curstep);
				curstep = 555;
				InitStep(curstep, 0);
				state = 1;
				}
			else
				{
				resetProgramName();
				resetRecipeName();
				resetProgramList();
				loadedRecipe = -1;
				rt_par_float[P_PROGRAM] = 0;
				}
			break;
		case MCMD_EVALUATE:
			if (state<0)
				{
				if (!valvesAreInConflict)
					{
					state=1;
					InitStep(curstep, 1);
					}
				}
			if (state == 0 && (loadedRecipe >= 0 || rt_par_float[P_PROGRAM] >= SPROG_ACID_PREPARATION) && rt_par_float[P_PROGRAM] > 0  )
				{
				state=1;
				valvesAreInConflict = getValvesConflict();
				if (valvesAreInConflict)
					{
					state = ERR_VALVES_ARE_IN_CONFLICT;
					}
				else
					{
					closeLineValves();
					lineRecipes->OnRecipeDevices(loadedRecipe);
					InitStep(curstep, 0);
					}
				}
			break;
		case MCMD_RESET_ALARM:
			blockAlarm = 1;
			break;
		case MCMD_NEXT_RECIPE:
			lineRecipes->NextRecipe();
			rt_par_float[P_CUR_REC] = lineRecipes->getCurrentRecipe() + 1;
			break;
		case MCMD_PREV_RECIPE:
			lineRecipes->PrevRecipe();
			rt_par_float[P_CUR_REC] = lineRecipes->getCurrentRecipe() + 1;
			break;
		case MCMD_FORCE_RET_ON:
			if (state)
				{
				ret_overrride = 0;
				SetRet(ON);
				ret_overrride = 1;
				}
			break;
		case MCMD_FORCE_RET_OFF:
			if (state)
				{
				ret_overrride = 0;
				SetRet(OFF);
				ret_overrride = 1;
				}
			break;
		}
	return 0;
	}

int cipline_tech_object::GoToStep( int cur, int param )
	{
	return 0;
	}

int cipline_tech_object::InitStep( int step, int f )
	{
	return 0;
	}

int cipline_tech_object::EvalPIDS()
	{
	PIDP->eval();
	PIDF->eval();

	//Клапан пара
	if (ao->get_value()>1 && PIDP->pidr->get_state() == ON && NP->get_value() > rt_par_float[P_R_NO_FLOW] && NP->get_state() == ON)
		{
		if (get_delta_millisec(steam_valve_delay) > STEAM_VALVE_MIN_DELAY)
			{
			V13->on();
			}
		}
	else
		{
		V13->off();
		steam_valve_delay = get_millisec();
		}
	return 0;
	}

int cipline_tech_object::EvalCipInProgress()
	{
	int res;

	res=DoStep(curstep);

	if (res>0 && curstep != 555)
		{ //All Ok step is over
		curstep=GoToStep(curstep, res);
		InitStep(curstep, 0);
		}
	else
		{
		if (res<0)
			{//was error, time it stopped
			Stop(curstep);
			state=res;
			return res;
			}
		else
			{
			if (isLine()) SetRet(ON);
			}
		}
	return 0;
	}

int cipline_tech_object::DoStep( int step )
	{
	return 0;
	}

int cipline_tech_object::EvalCipInError()
	{
	return 0;
	}

void cipline_tech_object::ResetLinesDevicesBeforeReset( void )
	{
	curprg = -1;
	if (valvesAreInConflict)
		{
		valvesAreInConflict = 0;
		}
	else
		{
		lineRecipes->OffRecipeDevices(loadedRecipe);
		closeLineValves();
		}
	loadedRecipe = -1;
	ResetStat();
	rt_par_float[P_PROGRAM] = 0;
	rt_par_float[P_RET_STATE] = 0;
	}

int cipline_tech_object::SetCommand( int command )
	{
	int l;
	if (ncmd==0)
		{
		ncmd=command;
		return 0;
		}
	l=ncmd;
	ncmd=command;
	return l;
	}

int cipline_tech_object::LoadProgram( void )
	{
	curprg=getNexpPrg(curprg, ( int ) rt_par_float[P_PROGRAM]);
	switch (curprg)
		{
		case PRG_PRO:
			return 5;
		case PRG_CIRC:
			return 10;
		case PRG_S:
			return 22;
		case PRG_K:
			return 42;
		case PRG_S1:
			return 22;
		case PRG_D:
			return 61;
		case PRG_SANITIZER:
			return 71;
		case PRG_OKO:
			if (rt_par_float[P_PROGRAM] == SPROG_RINSING_CLEAN)
				{
				return 83;
				}
			else
				{
				return 81;
				}
		case PRG_NAVS:
			return 105;
		case PRG_NAVK:
			return 115;
#ifdef SELFCLEAN
		case PRG_SELFCLEAN:
			return 151;
#endif //SELFCLEAN
		case -1:
			return 555;
		default:
			return 555;
		};
	}

void cipline_tech_object::ResetWP( void )
	{
	int i;
	for (i = 0; i < P_RESERV_START; i++)
		{
		if (i != P_CONC_RATE)
			{
			rt_par_float[i] = 0;
			}
		}
	rt_par_float[P_SELECT_PRG] = -1;
	rt_par_float[P_SELECT_REC] = -1;
	rt_par_float[P_CUR_REC] = 1;
	rt_par_float[P_LOADED_RECIPE] = 0;
	resetProgramName();
	resetRecipeName();
	resetProgramList();
	}

int cipline_tech_object::GetRetState()
	{
	if (0 == state)
		{
		return 0;
		}
	int n, u;
	device* dev;
	n = ( int ) rt_par_float[P_N_RET];
	u = ( int ) rt_par_float[P_N_UPR];
	if (n > 0)
		{
		dev = (device*)(M(n));
		return dev->get_state();
		}
	if (u > 0)
		{
		dev = (device*)(DO(n));
		return dev->get_state();
		}
	return 0;
	}

int cipline_tech_object::HasRet()
	{
	if (0 == state)
		{
		return 0;
		}
	int n, u;
	n = rt_par_float[P_N_RET];
	u = rt_par_float[P_N_UPR];
	if (n > 0 || u > 0)
		{
		return 1;
		}
	else
		{
		return 0;
		}
	}

void cipline_tech_object::ResetErr( void )
	{
	T[TMR_NO_FLOW]->reset();
	}

int cipline_tech_object::CheckErr( void )
	{
#ifndef DEBUG
	int i;
#endif // DEBUG
	u_int os;
	device* dev;
	float delta=0;
	unsigned long block_flags = (unsigned long)parpar[0][P_BLOCK_ERRORS];

	if (555 == curstep)
		{
		return 0;
		}

#ifndef DEBUG
	if (NP->get_state() == -1) return ERR_PUMP;
#endif


#ifndef DEBUG
	//обратная связь возвратного насоса
	i = rt_par_float[P_N_RET];
	if (i > 0)
		{
		dev = (device*)(M(i));
		if (dev->get_serial_n() > 0)
			{
			if (-1 == dev->get_state()) return ERR_RET;
			}
		else
			{
			return ERR_WRONG_OS_OR_RECIPE_ERROR;
			}
		}
#endif
	//Проверка обратной связи объекта
	os = (int)rt_par_float[P_OS];
	if (os > 0)
		{
		dev = (device*)(DI(os));
		if (dev->get_serial_n() > 0)
			{
			if (!dev->get_state()) return ERR_OS;
			}
		else
			{
			return ERR_WRONG_OS_OR_RECIPE_ERROR;
			}
		}

	//проверка уровней в бачке
	if ((!LL->is_active() && (LM->is_active() || LH->is_active())) || (!LM->is_active() && LH->is_active()))
		{
		if (get_delta_millisec(bachok_lvl_err_delay) > 5000L) //если ошибка уровня больше 5 секунд
			{
			if (!(block_flags & (1 << BE_ERR_LEVEL_BACHOK)))
				{
				return ERR_LEVEL_BACHOK;
				}
			}
		}
	else
		{
		bachok_lvl_err_delay = get_millisec();
		}

	//проверка уровней в танке щелочи
	if (((curstep >= 22) && (curstep <=40)) || ((curstep >= 105) && (curstep <= 111)))
		{
		if (!LSL->is_active() && LSH->is_active())
			{
			if (!(block_flags & (1 << BE_ERR_LEVEL_TANK_S)))
				{
				return ERR_LEVEL_TANK_S;
				}
			}
		}

	//проверка уровней в танке кислоты
	if (((curstep >= 42) && (curstep <=60)) || ((curstep >= 115) && (curstep <= 121)))
		{
		if (!LKL->is_active() && LKH->is_active())
			{
			if (!(block_flags & (1 << BE_ERR_LEVEL_TANK_K)))
				{
				return ERR_LEVEL_TANK_K;
				}
			}
		}

	//проверка уровней в танке вторичной воды
	if (!LWL->is_active() && LWH->is_active())
		{
		if (!(block_flags & (1 << BE_ERR_LEVEL_TANK_W)))
			{
			return ERR_LEVEL_TANK_W;
			}
		}

	// Нет расхода на подаче
	if (T[TMR_NO_FLOW]->get_countdown_time() != (par_float[P_TM_R_NO_FLOW] * 1000L))
		{
		T[TMR_NO_FLOW]->set_countdown_time(par_float[P_TM_R_NO_FLOW] * 1000L);
		}
	if (NP->get_state() == ON)
		{
		delta = par_float[P_R_NO_FLOW];
		if (NP->get_value() <= delta)
			{
			T[TMR_NO_FLOW]->start();
			if (T[TMR_NO_FLOW]->is_time_up())
				{
#ifndef DEBUG
				return ERR_NO_FLOW;
#endif // !DEBUG
				}
			}
		else
			{
			T[TMR_NO_FLOW]->reset();
			}
		}
	return 0;
	}

void cipline_tech_object::SortRR( int where )
	{
	if (get_delta_millisec(sort_delay) < SORT_SWITCH_DELAY)
		{
		return;
		}
	else
		{
		sort_delay = get_millisec();
		}
	float c;
	int d;
	d=0;
	V07->off();
	switch (where)
		{
		case TANK_S:
			V08->off();
			c=GetConc(SHCH);
			if (c>=parpar[0][P_CMIN_S])
				{
				d=2;
				}
			else
				{
				if (c>=parpar[0][P_CKANAL_S])
					{
					d=1;
					}
				else
					{
					d=0;
					}
				}
			switch (d)
				{
				case 0: //kanal
					V09->off();
					V10->off();
					V11->on();
					V12->off();
					break;
				case 1: //neutro
					V09->off();
					V10->off();
					V11->off();
					if (no_neutro) {V11->on();} else {V12->on();}
					break;
				case 2: //tank
					V09->on();
					V10->off();
					V11->off();
					V12->off();
					break;
				}
			break;
		case TANK_K:
			V09->off();
			c=GetConc(KISL);
			if (c>=parpar[0][P_CMIN_K])
				{
				d=2;
				}
			else
				{
				if (c>=parpar[0][P_CKANAL_K])
					{
					d=1;
					}
				else
					{
					d=0;
					}
				}
			switch (d)
				{
				case 0: //kanal
					V08->off();
					V10->off();
					V11->on();
					V12->off();
					break;
				case 1: //neutro
					V08->off();
					V10->off();
					V11->off();
					if (no_neutro) {V11->on();} else {V12->on();}
					break;
				case 2: //tank
					V08->on();
					V10->off();
					V11->off();
					V12->off();
					break;
				}
			break;
		}
	}

float cipline_tech_object::GetConc( int what )
	{
	float x25, a, ms25, c25, c, divider;
	c=0;
	switch (what)
		{
		case SHCH:
			a=parpar[0][P_ALFS]/100;
			ms25=parpar[0][P_MS25S];
			c25=parpar[0][P_CONC25S];
			divider = 1+a*(TR->get_value()-25);
			if (0 == divider)
				{
				divider = 1;
				}
			x25 = Q->get_value() /divider;
			divider = parpar[0][P_MS25W]-ms25;
			if (0 == divider)
				{
				divider = 1;
				}
			c = c25 * (ms25-x25) / divider + c25;
			break;
		case KISL:
			a=parpar[0][P_ALFK]/100;
			ms25=parpar[0][P_MS25K];
			c25=parpar[0][P_CONC25K];
			divider = 1+a*(TR->get_value()-25);
			if (0 == divider)
				{
				divider = 1;
				}
			x25=Q->get_value() / divider;
			divider = parpar[0][P_MS25W]-ms25;
			if (0 == divider)
				{
				divider = 1;
				}
			c = c25 * (ms25-x25) / divider + c25;
			break;
		}
	if (c<0)
		{
		return 0;
		}
	else
		{
		return c;
		}
	}

int cipline_tech_object::InitFilRR( int where )
	{
	float v = 0, pd = 0, kk = 0, kz = 0, ro = 0;
	V05->off();
	V06->on();
	V00->on();
	V01->on();
	V02->off();
	V03->off();
	V04->off();
	switch (where)
		{
		case TANK_K:
			V07->off();
			V08->on();
			V09->off();
			V10->off();
			V11->off();
			V12->off();
			//       NK->on();
			pd=parpar[0][P_PDNK];
			kk=parpar[0][P_K_K]/100;
			kz=parpar[0][P_CZAD_K]/100;
			ro=parpar[0][P_RO_K];
			break;
		case TANK_S:
			V07->off();
			V08->off();
			V09->on();
			V10->off();
			V11->off();
			V12->off();
			//       NS->on();
			pd=parpar[0][P_PDNS];
			kk=parpar[0][P_K_S]/100;
			kz=parpar[0][P_CZAD_S]/100;
			ro=parpar[0][P_RO_S];
			break;
		}

	if (pump_control) {NPC->on();} else {NP->on();}
	rt_par_float[P_ZAD_PODOGR] = parpar[0][P_T_RR];
	PIDP->on();
	V13->on();

	//calculate flow
	float divider = 1 - kz;
	if (0 == divider)
		{
		divider = 1;
		}
	v=kz*1000/((divider)*kk*ro);
	//   printf("\n\rfillRR v1M3: %f,pd: %f, kk: %f, kz: %f, ro: %f", v, pd, kk, kz, ro);
	//   par->setParamM(P_ZAD_FLOW, pd/v);
	rt_par_float[P_ZAD_FLOW] = parpar[0][P_FLOW_RR];
	PIDF->on();

	rt_par_float[P_VRAB] = 0;
	rt_par_float[P_ZAD_CONC] = kz*100;
	rt_par_float[P_CONC] = 0;
	rt_par_float[P_CONC_RATE] = 0;  //  Расход концентрата при наведении
	rt_par_float[P_OP_TIME_LEFT] = 0;
	divider = pd;
	if (0 == divider)
		{
		divider = 1;
		}
	rt_par_float[P_MAX_OPER_TM] = (v/divider)*MAX_OP_TIME*3600;
	T[TMR_OP_TIME]->set_countdown_time(rt_par_float[P_MAX_OPER_TM] * 1000L);
	T[TMR_OP_TIME]->start();
	rt_par_float[P_SUM_OP] = 0;
	cnt->start();
	return 0;
	}

int cipline_tech_object::InitCircRR( int where )
	{
	float z=0;
	V05->off();
	V06->on();
	V00->off();
	V01->off();
	V04->off();
	V07->off();

	V10->off();
	V11->off();
	V12->off();
	switch (where)
		{
		case TANK_K:
			V02->on();
			V03->off();
			V08->on();
			V09->off();
			NK->off();
			z=parpar[0][P_CZAD_K];
			break;
		case TANK_S:
			V02->off();
			V03->on();
			V08->off();
			V09->on();
			NS->off();
			z=parpar[0][P_CZAD_S];
			break;
		}

	if (pump_control) {NPC->on();} else {NP->on();}
	rt_par_float[P_ZAD_PODOGR] = parpar[0][P_T_RR];
	PIDP->on();
	V13->on();

	rt_par_float[P_ZAD_FLOW] = parpar[0][P_FLOW_RR];
	PIDF->on();

	rt_par_float[P_VRAB] = 0;
	rt_par_float[P_ZAD_CONC] = z;
	rt_par_float[P_CONC] = 0;

	rt_par_float[P_OP_TIME_LEFT] = 0;
	rt_par_float[P_MAX_OPER_TM] = parpar[0][P_TM_CIRC_RR];
	T[TMR_OP_TIME]->set_countdown_time(rt_par_float[P_MAX_OPER_TM]*1000L);
	T[TMR_OP_TIME]->start();
	rt_par_float[P_SUM_OP] = 0;
	cnt->start();
	return 0;
	}

int cipline_tech_object::InitCheckConc( int where )
	{
	InitCircRR(where);
	rt_par_float[P_MAX_OPER_TM] = parpar[0][P_TM_CHKC];
	T[TMR_OP_TIME]->reset();
	T[TMR_OP_TIME]->set_countdown_time(rt_par_float[P_MAX_OPER_TM] * 1000L);
	T[TMR_OP_TIME]->start();
	return 0;
	}

int cipline_tech_object::InitAddRR( int where )
	{
	float v, pd, kk, kz, ro, vt, kt;
	V05->off();
	V06->on();
	V00->off();
	V01->off();
	V04->off();
	V07->off();
	V10->off();
	V11->off();
	V12->off();
	switch (where)
		{
		case TANK_K:
			V02->on();
			V03->off();
			V08->on();
			V09->off();
			NK->on();
			pd=parpar[0][P_PDNK];
			kk=parpar[0][P_K_K]/100;
			kz=parpar[0][P_CZAD_K]/100;
			ro=parpar[0][P_RO_K];
			vt=parpar[0][P_VTANKK];
			break;
		case TANK_S:
			V02->off();
			V03->on();
			V08->off();
			V09->on();
			NS->on();
			pd=parpar[0][P_PDNS];
			kk=parpar[0][P_K_S]/100;
			kz=parpar[0][P_CZAD_S]/100;
			ro=parpar[0][P_RO_S];
			vt=parpar[0][P_VTANKS];
			break;
		}
	kt=rt_par_float[P_CONC] / 100;

	float divider = (1-kz)*kk*ro;
	if (0 == divider)
		{
		divider = 1;
		}
	v=(kz-kt)*vt/(divider);

	if (v<1) v=1;

	if (pump_control) {NPC->on();} else {NP->on();}
	rt_par_float[P_ZAD_PODOGR] = parpar[0][P_T_RR];
	PIDP->on();
	V13->on();

	rt_par_float[P_ZAD_FLOW] = parpar[0][P_FLOW_RR];
	PIDF->on();
	rt_par_float[P_ZAD_CONC] = kz*100;
	rt_par_float[P_CONC] = 0;

	rt_par_float[P_VRAB] = 0;

	rt_par_float[P_OP_TIME_LEFT] = 0;
	divider = pd;
	if (0 == divider)
		{
		divider = 1;
		}
	rt_par_float[P_MAX_OPER_TM] = (v/divider)*3600;
	T[TMR_OP_TIME]->set_countdown_time(rt_par_float[P_MAX_OPER_TM] * 1000L);
	T[TMR_OP_TIME]->start();
	rt_par_float[P_SUM_OP] = 0;
	cnt->start();
	return 0;
	}

int cipline_tech_object::InitOpolRR( int where )
	{
	float z=0;
	V05->off();
	V06->on();
	V00->on();
	V01->on();
	V02->off();
	V03->off();
	V04->off();
	switch (where)
		{
		case TANK_K:
			V07->off();
			V08->on();
			V09->off();
			V10->off();
			V11->off();
			V12->off();
			NK->off();
			z=parpar[0][P_CZAD_K];
			break;
		case TANK_S:
			V07->off();
			V08->off();
			V09->on();
			V10->off();
			V11->off();
			V12->off();
			NS->off();
			z=parpar[0][P_CZAD_S];
			break;
		}
	if (pump_control) {NPC->on();} else {NP->on();}
	rt_par_float[P_ZAD_PODOGR] = parpar[0][P_T_RR];
	PIDP->on();
	V13->on();

	rt_par_float[P_ZAD_FLOW] = parpar[0][P_FLOW_RR];
	PIDF->on();

	rt_par_float[P_VRAB] = 0;

	rt_par_float[P_OP_TIME_LEFT] = 0;
	rt_par_float[P_MAX_OPER_TM] = 300;
	T[TMR_OP_TIME]->set_countdown_time(rt_par_float[P_MAX_OPER_TM] * 1000L);
	T[TMR_OP_TIME]->start();
	T[TMR_CHK_CONC]->set_countdown_time(rt_par_float[P_TM_NO_CONC] * 1000L);
	rt_par_float[P_SUM_OP] = 0;

	rt_par_float[P_ZAD_CONC] = z;
	rt_par_float[P_CONC] = 0;

	cnt->start();
	return 0;
	}

int cipline_tech_object::FilRR( int where )
	{
	unsigned long tmp;
	rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
	rt_par_float[P_SUM_OP] = cnt->get_quantity();
	tmp=cnt->get_quantity();


	rt_par_float[STP_WC] = tmp - rt_par_float[STP_LV];
	rt_par_float[STP_LV] = tmp;
	switch (where)
		{
		case TANK_K:
			if (LKH->is_active())
				{
				return 1;
				}
			break;
		case TANK_S:
			if (LSH->is_active())
				{
				return 1;
				}
			break;
		}
	if (!LM->is_active()) V00->on();
	if (LH->is_active()) V00->off();
	return 0;
	}

int cipline_tech_object::CircRR( int where )
	{
	float c = 0;
	switch (where)
		{
		case TANK_S:
			c = rt_par_float[P_CONC_RATE] + parpar[0][P_PDNS] *
				( (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000) - rt_par_float[P_OP_TIME_LEFT ])/3600;
			rt_par_float[P_CONC_RATE] = c;

			c=GetConc(SHCH);
			break;
		case TANK_K:
			c = rt_par_float[P_CONC_RATE] + parpar[0][P_PDNK] *
				( (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000) - rt_par_float[P_OP_TIME_LEFT])/3600;
			rt_par_float[P_CONC_RATE] = c;

			c=GetConc(KISL);
			break;
		}

	rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000L);
	rt_par_float[P_SUM_OP] = cnt->get_quantity();
	rt_par_float[P_CONC] = c;
	if (T[TMR_OP_TIME]->is_time_up())
		{
		return 1;
		}
	return 0;
	}

int cipline_tech_object::CheckConc( int where )
	{
	float c = 0, z = 0;
	rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
	rt_par_float[P_SUM_OP] = cnt->get_quantity();
	switch (where)
		{
		case TANK_K:
			c=GetConc(KISL);
			z=parpar[0][P_CZAD_K];
			break;
		case TANK_S:
			c=GetConc(SHCH);
			z=parpar[0][P_CZAD_S];
			break;
		}
	SAV[SAV_CONC]->Add(c, cnt->get_quantity());
	rt_par_float[P_CONC] = SAV[SAV_CONC]->Q();
	if (T[TMR_OP_TIME]->is_time_up())
		{
		switch (where)
			{
			case TANK_K:
				if ( 0 == rt_par_float[STP_QAVK] )
					{
					rt_par_float[STP_QAVK] = c;
					}
				break;
			case TANK_S:
				if ( 0 == rt_par_float[STP_QAVS] )
					{
					rt_par_float[STP_QAVS] = c;
					}
				break;
			}

		if (c>=z)
			{
			float divider;
			switch (where)
				{
				case TANK_K:
					divider = ( 100 - c ) * parpar[0][P_RO_K] * parpar[0][P_K_K] / 100;
					if (0 == divider)
						{
						divider = 1;
						}
					rt_par_float[STP_QAVK] = (c - rt_par_float[STP_QAVK]) * parpar[0][P_VTANKK] / divider;
					break;
				case TANK_S:
					divider =  ( 100 - c ) * parpar[0][P_RO_S] * parpar[0][P_K_S ] / 100;
					if (0 == divider)
						{
						divider = 1;
						}
					rt_par_float[STP_QAVS] = (c - rt_par_float[STP_QAVS]) * parpar[0][P_VTANKS] / divider ;
					break;
				}
			return 1;
			}
		else return 2;
		}
	return 0;
	}

int cipline_tech_object::AddRR( int where )
	{
	float c = 0;
	switch (where)
		{
		case TANK_S:
			c = rt_par_float[P_CONC_RATE] + parpar[0][P_PDNS] *
				( (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000) - rt_par_float[P_OP_TIME_LEFT])/3600;
			rt_par_float[P_CONC_RATE] = c;

			c=GetConc(SHCH);
			break;
		case TANK_K:
			c = rt_par_float[P_CONC_RATE] + parpar[0][P_PDNK] *
				( (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000) - rt_par_float[P_OP_TIME_LEFT])/3600;
			rt_par_float[P_CONC_RATE] = c;

			c=GetConc(KISL);
			break;
		}

	rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
	rt_par_float[P_SUM_OP] = cnt->get_quantity();
	rt_par_float[P_CONC] = c;
	if (T[TMR_OP_TIME]->is_time_up())
		{
		return 1;
		}
	return 0;
	}

int cipline_tech_object::OpolRR( int where )
	{
	float c = 0;
	rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
	rt_par_float[P_SUM_OP] = cnt->get_quantity();
	switch (where)
		{
		case TANK_K:
			c=GetConc(KISL);
			break;
		case TANK_S:
			c=GetConc(SHCH);
			break;
		}
	rt_par_float[P_CONC] = c;
	SortRR(where);
	if (c>=NOCONC)
		{
		T[TMR_CHK_CONC]->reset();
		}
	else
		{
		T[TMR_CHK_CONC]->start();
		if (T[TMR_CHK_CONC]->is_time_up()) return 1;
		}
	if (!LM->is_active()) V00->on();
	if (LH->is_active()) V00->off();
	return 0;
	}

int cipline_tech_object::InitToObject( int from, int where, int step, int f )
	{
	float v=1, p=1, z=0;
//	int ot;
	V05->on();
	V06->off();

	if (step == 5 || step == 22 || step == 42 || step == 62 || step == 72 || (step == 83 && rt_par_float[P_PROGRAM] == SPROG_HOTWATER))
		{
		tank_is_empty = 1;
		}
	else
		{
		tank_is_empty = 0;
		}

	switch (from)
		{
		case WATER:
		case SANITIZER:
			V00->on();
			V01->on();
			V02->off();
			V03->off();
			V04->off();
			break;
		case TANK_W:
			V00->off();
			V01->off();
			V02->off();
			V03->off();
			V04->on();
			break;
		case TANK_K:
			V00->off();
			V01->off();
			V02->on();
			V03->off();
			V04->off();
			break;
		case TANK_S:
			V00->off();
			V01->off();
			V02->off();
			V03->on();
			V04->off();
			break;
		}
	switch (where)
		{
		case KANAL:
			V07->off();
			V08->off();
			V09->off();
			V10->off();
			V11->on();
			V12->off();
			break;
		case TANK_W:
			if (LWH->is_active())
				{
				V08->off();
				V09->off();
				V10->off();
				V11->on();
				V12->off();
				V07->off();
				}
			else
				{
				V08->off();
				V09->off();
				V10->off();
				V11->off();
				V12->off();
				V07->on();
				}
			break;
		case TANK_K:
			V07->off();
			V08->on();
			V09->off();
			V10->off();
			V11->off();
			V12->off();
			z=parpar[0][P_CZAD_K];
			break;
		case TANK_S:
			V07->off();
			V08->off();
			V09->on();
			V10->off();
			V11->off();
			V12->off();
			z=parpar[0][P_CZAD_S];
			break;
		}

	if (pump_control) {NPC->on();} else {NP->on();}
	SetRet(ON);

	switch (step)
		{
		case 5:
		case 8:
			v=rt_par_float[P_T_WP];
			break;
		case 14:
			v=rt_par_float[P_T_WP];
			break;
		case 22:
			v=rt_par_float[P_T_S];
			break;
		case 33:
		case 37:
			v=rt_par_float[P_T_WSP];
			break;
		case 53:
		case 57:
			v=rt_par_float[P_T_WKP];
			break;
		case 42:
			v=rt_par_float[P_T_K];
			break;
		case 62:
			v=rt_par_float[P_T_D];
			break;
		case 72:
			v=rt_par_float[P_T_SANITIZER];
			break;
		case 83:
		case 86:
			if (rt_par_float[P_PROGRAM] == SPROG_RINSING_CLEAN)
				{
				v=rt_par_float[P_T_CLEAN_RINSING];
				}
			else
				{
				v=rt_par_float[P_T_WOP];
				}
			break;
		}
	switch (step)
		{
		case 5:
		case 22:
		case 33:
		case 42:
		case 53:
		case 62:
		case 72:
		case 83:
			p = rt_par_float[PV1];
			break;
		case 8:
			p = rt_par_float[P_DOP_V_PR_OP];
			break;
		case 37:
			p = rt_par_float[P_DOP_V_AFTER_S];
			break;
		case 57:
			p = rt_par_float[P_DOP_V_AFTER_K];
			break;
		case 86:
			if (rt_par_float[P_PROGRAM] == SPROG_RINSING_CLEAN)
				{
				p = rt_par_float[P_V_CLEAN_RINSING];
				}
			else
				{
				p = rt_par_float[P_DOP_V_OK_OP];
				}
			break;
		}
	rt_par_float[P_ZAD_PODOGR] = v;
	if (disable_tank_heating && isTank() && (22 == step || 42 == step || 62 == step))
		{
		rt_par_float[P_ZAD_PODOGR] = 0;
		PIDP->off();
		}
	else
		{
		PIDP->on();
		}
	V13->on();
	rt_par_float[P_ZAD_FLOW] = rt_par_float[P_FLOW];
	PIDF->on();
	rt_par_float[P_VRAB] = p;
	rt_par_float[P_OP_TIME_LEFT] = 0;
	float divider = rt_par_float[P_ZAD_FLOW];
	if (0 == divider)
		{
		divider = 1;
		}
	rt_par_float[P_MAX_OPER_TM] = 3.6 * MAX_OP_TIME * p / divider;
	T[TMR_OP_TIME]->set_countdown_time(rt_par_float[P_MAX_OPER_TM] * 1000);
	T[TMR_OP_TIME]->start();

	rt_par_float[P_ZAD_CONC] = z;
	rt_par_float[P_CONC] = 0;

	cnt->start();
	return 0;
	}

int cipline_tech_object::InitFromObject( int what, int where, int step, int f )
	{
	float v = 0, p = 0, z=0;
//	int ot;
	if (0 == f)
		{
		return_ok = 0;
		concentration_ok = 0;
		}
	V05->on();
	V06->off();
	switch (what)
		{
		case WATER:
		case SANITIZER:
			V00->on();
			V01->on();
			V02->off();
			V03->off();
			V04->off();
			break;
		case TANK_W:
			V00->off();
			V01->off();
			V02->off();
			V03->off();
			V04->on();
			break;
		case TANK_K:
			V00->off();
			V01->off();
			V02->on();
			V03->off();
			V04->off();
			z=parpar[0][P_CZAD_K];
			break;
		case TANK_S:
			V00->off();
			V01->off();
			V02->off();
			V03->on();
			V04->off();
			z=parpar[0][P_CZAD_S];
			break;
		}
	switch (where)
		{
		case KANAL:
			V07->off();
			V08->off();
			V09->off();
			V10->off();
			V11->on();
			V12->off();
			break;
		case TANK_W:
			V07->on();
			V08->off();
			V09->off();
			V10->off();
			V11->off();
			V12->off();
			break;
		case TANK_K:
			V07->off();
			V08->on();
			V09->off();
			V10->off();
			V11->off();
			V12->off();
			z=parpar[0][P_CZAD_K];
			break;
		case TANK_S:
			V07->off();
			V08->off();
			V09->on();
			V10->off();
			V11->off();
			V12->off();
			z=parpar[0][P_CZAD_S];
			break;
		}

	if (pump_control) {NPC->on();} else {NP->on();}

	SetRet(OFF);

	switch (step)
		{
		case 7:
			v=rt_par_float[P_T_WP];
			break;
		case 24:
			v=rt_par_float[P_T_S];
			break;
		case 35:
			v=rt_par_float[P_T_WSP];
			break;
		case 55:
			v=rt_par_float[P_T_WKP];
			break;
		case 44:
			v=rt_par_float[P_T_K];
			break;
		case 64:
			v=rt_par_float[P_T_D];
			break;
		case 74:
			v=rt_par_float[P_T_SANITIZER];
			break;
		case 85:
			if (rt_par_float[P_PROGRAM] == SPROG_RINSING_CLEAN)
				{
				v=rt_par_float[P_T_CLEAN_RINSING];
				}
			else
				{
				v=rt_par_float[P_T_WOP];
				}
			break;
		}
	p=rt_par_float[PV2];
	rt_par_float[P_ZAD_PODOGR] = v;
	if (isTank() && (24 == step || 44 == step || 64 == step))
		{
		rt_par_float[P_ZAD_PODOGR] = 0;
		PIDP->off();
		}
	else
		{
		PIDP->on();
		}
	V13->on();
	rt_par_float[P_ZAD_FLOW] = rt_par_float[P_FLOW];
	PIDF->on();
	rt_par_float[P_VRAB] = p;
	rt_par_float[P_OP_TIME_LEFT] = 0;
	float divider = rt_par_float[P_ZAD_FLOW];
	if (0 == divider)
		{
		divider = 1;
		}
	rt_par_float[P_MAX_OPER_TM] = 3.6 * MAX_OP_TIME * p / divider;
	T[TMR_OP_TIME]->set_countdown_time(rt_par_float[P_MAX_OPER_TM] * 1000);
	T[TMR_OP_TIME]->start();
	T[TMR_RETURN]->set_countdown_time(rt_par_float[P_TM_NO_FLOW_R] * 1000);
	T[TMR_RETURN]->reset();
	T[TMR_CHK_CONC]->reset();
	T[TMR_CHK_CONC]->set_countdown_time(rt_par_float[P_TM_NO_CONC] * 1000);
	rt_par_float[P_SUM_OP] = 0;

	rt_par_float[P_ZAD_CONC] = z;
	rt_par_float[P_CONC] = 0;

	cnt->start();
	return 0;
	}

int cipline_tech_object::InitOporCIP( int where, int step, int f )
	{
	float z=0;
	opcip=0;
	if (isLine()) return 0;
#ifdef MEDIUM_CHANGE
	DO(msa_number * 1000L + 3 + nmr * 100)->on();
#endif
	V05->on();
	V06->off();
	V00->off();
	V01->off();
	V03->off();
	V02->off();
	V04->off();
	switch (where)
		{
		case KANAL:
			V07->off();
			V08->off();
			V09->off();
			V10->off();
			V11->on();
			V12->off();
			break;
		case TANK_W:
			V07->on();
			V08->off();
			V09->off();
			V10->off();
			V11->off();
			V12->off();
			break;
		case TANK_K:
			V07->off();
			V08->on();
			V09->off();
			V10->off();
			V11->off();
			V12->off();
			z=parpar[0][P_CZAD_K];
			break;
		case TANK_S:
			V07->off();
			V08->off();
			V09->on();
			V10->off();
			V11->off();
			V12->off();
			z=parpar[0][P_CZAD_S];
			break;
		}

	if (pump_control) {NPC->off();} else {NP->off();}
	SetRet(ON);
	rt_par_float[P_ZAD_PODOGR] = 0;
	PIDP->off();
	V13->off();
	rt_par_float[P_ZAD_FLOW] = 0;
	PIDF->off();
	rt_par_float[P_VRAB] = 0;
	rt_par_float[P_OP_TIME_LEFT] = 0;
	rt_par_float[P_MAX_OPER_TM] = 600;
	T[TMR_OP_TIME]->set_countdown_time(rt_par_float[P_MAX_OPER_TM]*1000);
	T[TMR_OP_TIME]->start();
	T[TMR_RETURN]->set_countdown_time(rt_par_float[P_TM_RET_IS_EMPTY]*1000);
	T[TMR_RETURN]->reset();
	rt_par_float[P_SUM_OP] = 0;

	rt_par_float[P_ZAD_CONC] = z;
	rt_par_float[P_CONC] = 0;
	cnt->start();
	return 0;
	}

int cipline_tech_object::InitFilCirc( int with_what, int step, int f )
	{
	V01->on();
	V11->off();
	V12->off();
	switch (with_what)
		{
		case WITH_WATER:
		case SANITIZER:
			V00->on();
			V10->off();
			if (pump_control) {NPC->off();} else {NP->off();}
			SetRet(OFF);
			PIDP->off();
			PIDF->off();
			break;
		case WITH_RETURN:
			V00->off();
			V10->on();
			V07->off();
			V08->off();
			V09->off();
			V05->on();
			V06->off();
			if (pump_control) {NPC->on();} else {NP->on();}
			SetRet(ON);
			if (isTank() && (26 == step || 46 == step || 65 == step))
				{
				PIDP->off();
				}
			else
				{
				PIDP->on();
				}
			PIDF->on();
			break;
		}
	switch (step)
		{
		case 26:
			V03->on();
			break;
		case 46:
			V02->on();
			break;
		}

	//   cnt->start();
	rt_par_float[P_SUM_OP] = 0;
	rt_par_float[P_VRAB] = rt_par_float[P_V_RAB_ML];
	rt_par_float[P_OP_TIME_LEFT] = 0;
	rt_par_float[P_MAX_OPER_TM] = 300;
	T[TMR_OP_TIME]->set_countdown_time(rt_par_float[P_MAX_OPER_TM]*1000);
	T[TMR_OP_TIME]->start();
	return 0;
	}

int cipline_tech_object::InitOporCirc( int where, int step, int f )
	{
	V11->off();
	V00->off();
	V01->on();
	V03->off();
	V02->off();
	V04->off();
	if (pump_control) {NPC->on();} else {NP->on();}
	SetRet(ON);
	V05->on();
	V06->off();
	switch (where)
		{
		case KANAL:
			V07->off();
			V08->off();
			V09->off();
			V10->off();
			V11->on();
			V12->off();
			break;
		case TANK_W:
			V07->on();
			V08->off();
			V09->off();
			V10->off();
			V11->off();
			V12->off();
			break;
		case TANK_S:
			V07->off();
			V08->off();
			V09->on();
			V10->off();
			V11->off();
			V12->off();
			break;
		case TANK_K:
			V07->off();
			V08->on();
			V09->off();
			V10->off();
			V11->off();
			V12->off();
			break;
		}
	PIDF->on();
	if (isTank() && (29 == step || 49 == step))
		{
		PIDP->off();
		}
	else
		{
		PIDP->on();
		}
	//   cnt->start();
	rt_par_float[P_ZAD_FLOW] = rt_par_float[P_FLOW];
	rt_par_float[P_SUM_OP] = 0;
	rt_par_float[P_VRAB] = rt_par_float[P_V_LL_BOT];
	rt_par_float[P_OP_TIME_LEFT] = 0;
	rt_par_float[P_MAX_OPER_TM] = 300;
	T[TMR_OP_TIME]->set_countdown_time(rt_par_float[P_MAX_OPER_TM]*1000);
	T[TMR_OP_TIME]->start();
	return 0;
	}

int cipline_tech_object::InitCirc( int what, int step, int f )
	{
	float t=600, z=0;
	unsigned long tm;
	ret_circ_flag = 0;
	if (rt_par_float[P_PODP_CIRC] != 100)
		{
		V01->on();
		V10->on();
		}
	else
		{
		V01->off();
		V10->off();
		}
	V00->off();
	V03->off();
	V04->off();
	V02->off();
	V05->on();
	V06->off();
	V07->off();
	V08->off();
	V09->off();
	V11->off();
	V12->off();
	switch (what)
		{
		case WATER:
			if (100 == rt_par_float[P_PODP_CIRC])
				{
				V01->on();
				V10->on();
				}
			t=rt_par_float[P_T_WP];
			tm=rt_par_float[PTM_OP]*1000;
			break;
		case SANITIZER:
			t=rt_par_float[P_T_SANITIZER];
			tm=rt_par_float[PTM_SANITIZER]*1000;
			break;
		case HOT_WATER:
			if (100 == rt_par_float[P_PODP_CIRC])
				{
				V01->on();
				V10->on();
				}
			t=rt_par_float[P_T_D];
			tm=rt_par_float[PTM_D]*1000;
			break;
		case SHCH:
			if (100 == rt_par_float[P_PODP_CIRC])
				{
				V03->on();
				V09->on();
				}
			t=rt_par_float[P_T_S];
			if (((int)rt_par_float[P_PROGRAM] & ((1 << PRG_K) + (1 << PRG_S))) == ((1 << PRG_K) + (1 << PRG_S)))
				{
				tm = rt_par_float[PTM_S_SK] * 1000L;
				}
			else
				{
				tm=rt_par_float[PTM_S]*1000;
				}
			z=parpar[0][P_CZAD_S];
			break;
		case KISL:
			if (100 == rt_par_float[P_PODP_CIRC])
				{
				V02->on();
				V08->on();
				}
			z=parpar[0][P_CZAD_K];
			t=rt_par_float[P_T_K];
			if (((int)rt_par_float[P_PROGRAM] & ((1 << PRG_K) + (1 << PRG_S))) == ((1 << PRG_K) + (1 << PRG_S)))
				{
				tm = rt_par_float[PTM_K_SK] * 1000L;
				}
			else
				{
				tm=rt_par_float[PTM_K]*1000;
				}
			break;
		}

	rt_par_float[P_ZAD_PODOGR] = t;
	PIDP->on();
	if (pump_control) {NPC->on();} else {NP->on();}
	SetRet(ON);
	V13->on();
	rt_par_float[P_ZAD_FLOW] = rt_par_float[P_FLOW];
	PIDF->on();
	rt_par_float[P_VRAB] = 0;

	rt_par_float[P_ZAD_CONC] = z;
	rt_par_float[P_CONC] = 0;

	rt_par_float[P_OP_TIME_LEFT] = 0;
	rt_par_float[P_MAX_OPER_TM] = tm/1000;

	T[TMR_OP_TIME]->set_countdown_time(tm);
	rt_par_float[P_SUM_OP] = 0;

	cnt->start();
	return 0;
	}

	int cipline_tech_object::ToObject( int from, int where )
		{
		float c=0;
		unsigned long tmp;
		rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
		rt_par_float[P_SUM_OP] = cnt->get_quantity();


		if (tank_is_empty)
			{
			if ((cnt->get_quantity() >= rt_par_float[P_VRAB]-rt_par_float[P_RET_STOP]) || (cnt->get_quantity() <= rt_par_float[P_RET_STOP]))
				{
				SetRet(OFF);
				}
			else
				{
				SetRet(ON);
				}
			}


		switch (from)
			{
			case WATER:
			case SANITIZER:
				if (!LM->is_active()) V00->on();
				if (LH->is_active()) V00->off();
				tmp=cnt->get_quantity();
				rt_par_float[STP_WC] = rt_par_float[STP_WC] + tmp - rt_par_float[STP_LV];
				rt_par_float[STP_LV] = tmp;
				break;
			case TANK_W:
				if (!LWL->is_active())
					{
					tmp=cnt->get_quantity();
					rt_par_float[STP_WC] = rt_par_float[STP_WC] + tmp - rt_par_float[STP_LV];
					rt_par_float[STP_WC_INST_WS] = rt_par_float[STP_WC_INST_WS] + tmp - rt_par_float[STP_LV];
					rt_par_float[STP_LV] = tmp;
					V04->off();
					V01->on();
					if (!LM->is_active()) V00->on();
					if (LH->is_active()) V00->off();

					}
				else
					{
					tmp=cnt->get_quantity();
					rt_par_float[STP_WS] = rt_par_float[STP_WS] + tmp - rt_par_float[STP_LV];
					rt_par_float[STP_LV] = tmp;
					V04->on();
					V00->off();
					V01->off();
					}
				break;
			case TANK_K:
				c=GetConc(KISL);
				if (!LKL->is_active())
					{
					return NO_ACID;
					}
				break;
			case TANK_S:
				c=GetConc(SHCH);
				if (!LSL->is_active())
					{
					return NO_ALKALINE;
					}
				break;
			}

		if (cnt->get_quantity() >= rt_par_float[P_VRAB])
			{
			return 1;
			}

		if (curstep==86 && TR->get_value()<5 && rt_par_float[P_PROGRAM] != SPROG_RINSING_CLEAN)
			{
			return 1;
			}

		switch (where)
			{
			case TANK_W:
				if ((LWH->is_active()) || (from == TANK_W))
					{
					V08->off();
					V09->off();
					V10->off();
					V11->on();
					V12->off();
					V07->off();
					}
				else
					{
					V08->off();
					V09->off();
					V10->off();
					V11->off();
					V12->off();
					V07->on();
					}
				break;
			case TANK_K:
				c=GetConc(KISL);
				if (LKH->is_active())
					{
					V08->off();
					V09->off();
					V10->off();
					V11->off();
					if (no_neutro) {V11->on();} else {V12->on();}
					V07->off();
					}
				else
					{
					V08->on();
					V09->off();
					V10->off();
					V11->off();
					V12->off();
					V07->off();
					}
				break;
			case TANK_S:
				c=GetConc(SHCH);
				if (LSH->is_active())
					{
					V08->off();
					V09->off();
					V10->off();
					V11->off();
					if (no_neutro) {V11->on();} else {V12->on();}
					V07->off();
					}
				else
					{
					V08->off();
					V09->on();
					V10->off();
					V11->off();
					V12->off();
					V07->off();
					}
				break;
			}
		rt_par_float[P_CONC] = c;

		return 0;
		}

		int cipline_tech_object::FromObject( int what, int where )
			{
			float c;
			int dst;
			unsigned long tmp;
			rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
			rt_par_float[P_SUM_OP] = cnt->get_quantity();
			if (cnt->get_quantity()>=rt_par_float[PV_RET_DEL])
				{
				SetRet(ON);
				}
			if (cnt->get_quantity()>=rt_par_float[P_VRAB])
				{
				if (!return_ok)
					{
					if (FL->get_state()==FLIS) {
						return_ok = 1;
						}
					else
						{
						T[TMR_RETURN]->start();
						if (T[TMR_RETURN]->is_time_up()==1)
							{
							return NO_RETURN;
							}
						}
					}
				dst=0;
				switch (what)
					{
					case TANK_K:
						dst=1;
						c=GetConc(KISL);
						break;
					case TANK_S:
						dst=1;
						c=GetConc(SHCH);
						break;
					}
				switch (where)
					{
					case TANK_K:
						dst=2;
						c=GetConc(KISL);
						break;
					case TANK_S:
						dst=2;
						c=GetConc(SHCH);
						break;
					}
				switch (dst)
					{
					case 0:
						concentration_ok = 1;
						break;
					case 1:  //must BE
						if (c>=NOCONC)
							{
							concentration_ok = 1;
							//	   T[TMR_CHK_CONC]->R();
							}
						else
							{
							T[TMR_CHK_CONC]->start();
							if (T[TMR_CHK_CONC]->is_time_up())
								{
								T[TMR_CHK_CONC]->reset();
								return ERR_NO_CONC;
								}
							concentration_ok = 0;
							}
						break;
					case 2: //must not BE
						if ( ((c<= parpar[0][P_CKANAL_S]) && (where == TANK_S)) ||
							((c<= parpar[0][P_CKANAL_K]) && (where == TANK_K)))
							{
							concentration_ok = 1;
							//	   T[TMR_CHK_CONC]->R();
							}
						else
							{
							T[TMR_CHK_CONC]->start();
							if (T[TMR_CHK_CONC]->is_time_up())
								{
								T[TMR_CHK_CONC]->reset();
								return ERR_IS_CONC;
								}
							concentration_ok = 0;
							}
						break;
					}

				if (return_ok == 1 && concentration_ok == 1) return 1;
				}

			c=0;
			switch (what)
				{
				case WATER:
				case SANITIZER:
					if (!LM->is_active()) V00->on();
					if (LH->is_active()) V00->off();
					tmp=cnt->get_quantity();
					rt_par_float[STP_WC] = rt_par_float[STP_WC] + tmp - rt_par_float[STP_LV] ;
					rt_par_float[STP_LV] = tmp ;
					break;
				case TANK_W:
					if (!LWL->is_active())
						{
						tmp=cnt->get_quantity();
						rt_par_float[STP_WC] = rt_par_float[STP_WC] + tmp - rt_par_float[STP_LV] ;
						rt_par_float[STP_WC_INST_WS] = rt_par_float[STP_WC_INST_WS] + tmp - rt_par_float[STP_LV] ;
						rt_par_float[STP_LV] = tmp ;
						V04->off();
						V01->on();
						if (!LM->is_active()) V00->on();
						if (LH->is_active()) V00->off();
						}
					else
						{
						tmp=cnt->get_quantity();
						rt_par_float[STP_WS] = rt_par_float[STP_WS] + tmp - rt_par_float[STP_LV] ;
						rt_par_float[STP_LV] = tmp ;
						V04->on();
						V00->off();
						V01->off();
						}
					break;
				case TANK_K:
					c=GetConc(KISL);
					if (!LKL->is_active())
						{
						return NO_ACID;
						}
					SortRR(TANK_K);
					break;
				case TANK_S:
					c=GetConc(SHCH);
					if (!LSL->is_active())
						{
						return NO_ALKALINE;
						}
					SortRR(TANK_S);
					break;
				}

			switch (where)
				{
				case TANK_W:
					if (LWH->is_active())
						{
						V08->off();
						V09->off();
						V10->off();
						V11->on();
						V12->off();
						V07->off();
						}
					else
						{
						V08->off();
						V09->off();
						V10->off();
						V11->off();
						V12->off();
						V07->on();
						}
					break;
				case TANK_K:
					c=GetConc(KISL);
					if (LKH->is_active())
						{
						V08->off();
						V09->off();
						V10->off();
						V11->off();
						if (no_neutro) {V11->on();} else {V12->on();}
						V07->off();
						}
					else
						{
						SortRR(TANK_K);
						}
					break;
				case TANK_S:
					c=GetConc(SHCH);
					if (LSH->is_active())
						{
						V08->off();
						V09->off();
						V10->off();
						V11->off();
						if (no_neutro) {V11->on();} else {V12->on();}
						V07->off();
						}
					else
						{
						SortRR(TANK_S);
						}
					break;
				}
			rt_par_float[P_CONC] = c;
			return 0;
			}

			int cipline_tech_object::OporCIP( int where )
				{
//				int ot;
				float c;
				device* OSdev;
				if (isLine()) return 1;
				rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
				rt_par_float[P_SUM_OP] = cnt->get_quantity();

				unsigned int object_empty = (unsigned int)(rt_par_float[P_OBJ_EMPTY]);
				if (object_empty > 0)
					{
					OSdev = (device*)(DI(object_empty));
					if (OSdev->get_serial_n() > 0)
						{
						if (ON == OSdev->get_state())
							{
							return 1;
							}
						}
					else
						{
						return ERR_WRONG_OS_OR_RECIPE_ERROR;
						}
					}

				if (FL->get_state() == FLIS)
					{
					opcip=1;
					T[TMR_RETURN]->reset();
					}
				else
					{
					if (opcip==1)
						{
						T[TMR_RETURN]->start();
						if (T[TMR_RETURN]->is_time_up())
							{
							return 1;
							}
						}
					}

				if (rt_par_float[P_MAX_OPER_TM]*500 < T[TMR_OP_TIME]->get_work_time())
					{
					if ((curstep==6) || (opcip == 0)) return 1;
					}

				if (T[TMR_OP_TIME]->is_time_up())
					{
					return 1;
					}

				c=0;
				switch (where)
					{
					case TANK_W:
						if (LWH->is_active())
							{
							V08->off();
							V09->off();
							V10->off();
							V11->on();
							V12->off();
							V07->off();
							}
						else
							{
							V08->off();
							V09->off();
							V10->off();
							V11->off();
							V12->off();
							V07->on();
							}
						break;
					case TANK_K:
						c=GetConc(KISL);
						if (LKH->is_active())
							{
							V08->off();
							V09->off();
							V10->off();
							V11->off();
							if (no_neutro) {V11->on();} else {V12->on();}
							V07->off();
							}
						else
							{
							SortRR(TANK_K);
							}
						break;
					case TANK_S:
						c=GetConc(SHCH);
						if (LSH->is_active())
							{
							V08->off();
							V09->off();
							V10->off();
							V11->off();
							if (no_neutro) {V11->on();} else {V12->on();}
							V07->off();
							}
						else
							{
							SortRR(TANK_S);
							}
						break;
					}
				rt_par_float[P_CONC] = c;
				return 0;
				}

			int cipline_tech_object::FilCirc( int with_what )
				{
				rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
				rt_par_float[P_SUM_OP] = cnt->get_quantity();
				if (LM->is_active()) cnt->start();
				if (cnt->get_quantity() >= rt_par_float[P_V_RAB_ML]) return 1;
				if (LH->is_active()) return 1;
				return 0;
				}

			int cipline_tech_object::Circ( int what )
				{
				return 0;
				}

cipline_tech_object* cipline_tech_object::Mdls[10];

int cipline_tech_object::MdlsCNT = 0;

int cipline_tech_object::msa_number = 0;

int cipline_tech_object::blockAlarm = 0;

saved_params<float, true>* cipline_tech_object::parpar = 0;

int cipline_tech_object::pidnumber = 1;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MSAPIDInterface::MSAPIDInterface( PID* pid, run_time_params_float* par, int taskpar, i_AO_device* ao /*= 0*/, i_AI_device* ai /*= 0 */ ) :
	pidr(pid),
	input(ai),
	output(ao),
	HI(0),
	rp(taskpar),
	lineparams(par),
	lastEvalInOnState(get_millisec())
	{

	}

void MSAPIDInterface::eval()
	{
	if ( 1 == pidr->get_state() )
		{
		lastEvalInOnState = get_millisec();
		pidr->set( lineparams[0][rp] );
		output->set_value( pidr->eval(input->get_value()));
		}
	}

void MSAPIDInterface::reset()
	{
	if (get_delta_millisec(lastEvalInOnState) > 3000L)
		{
		pidr->reset();
		}
	}

void MSAPIDInterface::on( int accel /*= 0 */ )
	{
	pidr->on(accel);
	}

void MSAPIDInterface::off()
	{
	pidr->off();
	output->set_value(0);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TSav::TSav(void) {
	n=0;
	cn=0;
	integrator=0;
	};

TSav::~TSav(void) { };

void TSav::Add(float val, unsigned long inegr) {
	unsigned long delta, i;
	if (inegr>integrator) {
		delta=inegr-integrator;
		integrator=inegr;
		for (i=0; i<delta; i++) {
			n++;
			cn=(cn*(n-1)+val)/n;
			};
		};
	};

void TSav::R(void) {
	n=0;
	cn=0;
	integrator=0;
	};

float TSav::Q(void) {
	return cn;
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TPumpController::TPumpController( i_DO_AO_device* pump )
	{
	control_pump = pump;
	actual_state = -1;
	last_manual_state = -1;
	manual_state = -1;
	}

TPumpController::~TPumpController()
	{
	control_pump = 0;
	}

void TPumpController::mDisable()
	{
	manual_state = -1;
	last_manual_state = -1;
	}

void TPumpController::on()
	{
	actual_state = ON;
	}

void TPumpController::off()
	{
	actual_state = OFF;
	}

void TPumpController::mOn()
	{
	last_manual_state = manual_state;
	manual_state = ON;
	}

void TPumpController::mOff()
	{
	last_manual_state = manual_state;
	manual_state = OFF;
	}

void TPumpController::Eval()
	{
	if (manual_state >=0)
		{
		if (last_manual_state != manual_state)
			{
			control_pump->set_state(manual_state);
			last_manual_state = manual_state;
			}
		}
	else
		{
		if (actual_state >= 0)
			{
			control_pump->set_state(actual_state);
			actual_state = -1;
			}
		}
	}

#pragma warning(pop)