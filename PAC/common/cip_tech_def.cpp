#include "cip_tech_def.h"

cipline_tech_object::cipline_tech_object( const char* name, u_int number, u_int type, 
										 const char* name_Lua, u_int states_count, u_int timers_count, 
										 u_int par_float_count, u_int runtime_par_float_count, u_int par_uint_count, 
										 u_int runtime_par_uint_count ) : tech_object(name, number, type,
										 name_Lua, states_count, timers_count, par_float_count, runtime_par_float_count,
										 par_uint_count, runtime_par_uint_count)
	{
	Print("\n\r Create cip tech_object\n\r");
	PIDP = new PID(nextpidnumber());
	PIDF = new PID(nextpidnumber());
	}

int cipline_tech_object::nextpidnumber()
	{
	int curpid = pidnumber;
	pidnumber++;
	return curpid;
	}

void cipline_tech_object::initline()
	{
	PIDP->init_param(PID::P_k, 111);
	PIDP->save_param();
	Print("Init Line\n\r");
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

saved_params<float, true>* cipline_tech_object::parpar = 0;

int cipline_tech_object::pidnumber = 1;

