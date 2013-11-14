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

	//Состояние и команда.
	sprintf( buff + answer_size, "\tCMD=%lu,\n",
		( u_long ) cmd );
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

	}

void cipline_tech_object::resetRecipeName()
	{

	}

void cipline_tech_object::resetProgramList( unsigned long programmask /*= 0xB00*/ )
	{

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
	return 0;
	}

int cipline_tech_object::isLine()
	{
	return 0;
	}

int cipline_tech_object::getValvesConflict()
	{
	return 0;
	}

int cipline_tech_object::set_cmd( const char *prop, u_int idx, double val )
	{
	if ( strcmp( prop, "CMD" ) == 0 )
		{
#ifdef DEBUG
		Print( "cip_tech_object::set_cmd() - prop = \"%s\", val = %f\n",
			prop, val );
#endif // DEBUG

		if ( 0. == val )
			{
			cmd = 0;
			return 0;
			}

		u_int mode     = ( int ) val;
		char new_state = 0;

		if ( mode >= 1000 && mode < 2000 )      // On mode.
			{
			mode = mode - 1000;
			new_state = 1;
			}
		else
			{
			if ( mode >= 2000 && mode < 3000 )  // Off mode.
				{
				mode = mode - 2000;
				new_state = 0;
				}
			else
				{
#ifdef DEBUG
				Print( "Error complex_state::parse_cmd - new_mode = %lu\n",
					( unsigned long int ) mode );
#endif // DEBUG
				return 1;
				}
			}

		if ( mode > get_modes_count() )
			{
			// Command.
			cmd = lua_exec_cmd( mode );
			}
		else
			{
			// On/off mode.
			int res = set_mode( mode, new_state );
			if ( 0 == res )
				{
				cmd = ( int ) val;  // Ok.
				}
			else
				{
				cmd = res;          // Ошибка.
				}
			}

		return 0;
		}

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

void cipline_tech_object::set_station_par( int parno, float newval )
	{
	Print("");
	parpar->save(parno, newval); 
	}

float cipline_tech_object::get_station_par( int parno )
	{
	return parpar[0][parno];
	}

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