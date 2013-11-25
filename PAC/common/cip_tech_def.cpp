#pragma warning(push)
#pragma warning(disable: 4244)
#include "cip_tech_def.h"

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
	tech_object::~tech_object();
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
		 strncpy_s(lineRecipes->currentRecipeName, lineRecipes->recipeNameLength, val, _TRUNCATE);
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
			//res = EvalCipInProgress();
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
			//EvalCipInError();
			if (!cip_in_error)
				{
				//stat_par->setParamM(STP_ERRCOUNT, stat_par->getParam(STP_ERRCOUNT) + 1);
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
	int i;
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
	if (pump_control) {NPC->Off();} else {NP->off();}
	if (PIDP->HI==0) PIDP->Off();
	if (PIDF->HI==0) PIDF->Off();
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
		lineRecipes->ToRecipe(rt_par_float[P_CUR_REC] - 1);
		rt_par_float[P_CUR_REC] = lineRecipes->getCurrentRecipe() + 1;
		}
	//выбор рецепта
	if (rt_par_float[P_SELECT_REC] > 0)
		{
		lineRecipes->LoadRecipeToParams(rt_par_float[P_SELECT_REC] - 1, 2, 16, 39, &rt_par_float);
		lineRecipes->getRecipeName(rt_par_float[P_SELECT_REC] - 1, loadedRecName);
		loadedRecipe = rt_par_float[P_SELECT_REC] - 1;
		rt_par_float[P_SELECT_REC] = 0;
		formProgramList((unsigned int)(rt_par_float[P_PROGRAM_MASK]));
		}
	//выбор программы
	if (rt_par_float[P_SELECT_PRG] > 0)
		{
		loadProgramFromList(rt_par_float[P_SELECT_PRG]);
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
		device* n_ret = (device*)(DO((u_int)(rt_par_float[P_N_UPR])));
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
	PIDP->Eval();
	PIDF->Eval();

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

void MSAPIDInterface::Eval()
	{
	if ( 1 == pidr->get_state() )
		{
		lastEvalInOnState = get_millisec();
		pidr->set( lineparams[0][rp] );
		output->set_value( pidr->eval(input->get_value()));
		}
	}

void MSAPIDInterface::Reset()
	{
	if (get_delta_millisec(lastEvalInOnState) > 3000L)
		{
		pidr->reset();
		}
	}

void MSAPIDInterface::On( int accel /*= 0 */ )
	{
	pidr->on(accel);
	}

void MSAPIDInterface::Off()
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

void TPumpController::On()
	{
	actual_state = ON;
	}

void TPumpController::Off()
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