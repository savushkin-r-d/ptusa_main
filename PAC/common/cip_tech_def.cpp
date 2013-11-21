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
	rt_par_float[P_CUR_REC] =  lineRecipes->getCurrentRecipe() + 1;
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
	}

int cipline_tech_object::evaluate()
	{
	//Print("Evaluate\n\r");
	return 0;
	}

int cipline_tech_object::save_device( char *buff )
	{
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
	if (1 == number)
		{
		answer_size += parpar->save_device(buff + answer_size, "\t");
		}


	sprintf( buff + answer_size, "\t}\n" );
	answer_size += strlen( buff + answer_size );
	return answer_size;
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

	}

void cipline_tech_object::loadProgramFromList( int selectedPrg )
	{

	}

void cipline_tech_object::closeLineValves()
	{

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
	return 0;
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
		 strcpy(lineRecipes->currentRecipeName, val);
		 return 0;
		}
#ifdef DEBUG
	Print( "Eror tech_object::set_cmd(...), prop = \"%s\", idx = %u, val = %s\n",
		prop, idx, val );
#endif // DEBUG
	return 1;
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