#include "PID.h"
#include "string.h"
//-----------------------------------------------------------------------------
PID::PID( int n ):
    uk_1( 0 ),
    ek_1( 0 ),
    ek_2( 0 ),
    start_time( get_millisec() ),
    last_time( get_millisec() ),
    prev_manual_mode( 0 ),
    is_down_to_inaccel_mode( 0 ),
    par( new saved_params_float( PARAM_CNT ) ),
    w_par( new run_time_params_float( 2 ) ),
    state( STATE_OFF ),
    used_par_n( 1 ),
    start_value( 0 ),
    number( n )
    {
    G_DEVICE_CMMCTR->add_device( this );

    sprintf( name, "%s%d", "PID", n );

    init_work_param( WP_U, 0 );
    }
//-----------------------------------------------------------------------------
PID::~PID()
    {
    }
//-----------------------------------------------------------------------------
float PID::eval( float currentValue, int deltaSign )
    {
    if ( STATE_OFF == state )
        {
        if ( this->is_down_to_inaccel_mode ) return 100;
        else return 0;
        }

    float K = par[ 0 ][ P_k ];
    float TI = par[ 0 ][ P_Ti ];
    float TD = par[ 0 ][ P_Td ];

    if ( used_par_n == 2 )
        {
        K = par[ 0 ][ P_k2 ];
        TI = par[ 0 ][ P_Ti2 ];
        TD = par[ 0 ][ P_Td2 ];
        }
#ifdef DEBUG
    else
        {
        if ( used_par_n != 1 )
            {
            Print( "Error: void  PID::eval( float currentValue, int deltaSign ), used_par_n = %d\n",
                used_par_n );
            Print( "Press any key!\n" );
            get_char();
            }
        }
#endif

    float dt = par[ 0 ][ P_dt ] / 1000;
    float dmax = par[ 0 ][ P_max ];
    float dmin = par[ 0 ][ P_min ];

    if ( dmax == dmin )
        {
        dmax = dmin + 1;
#ifdef DEBUG
        Print( "Error! PID::eval dmax == dmin!\n" );
        Print( "Press any key!" );
        get_char();
#endif
        }

    float ek = deltaSign * 100 * ( w_par[ 0 ][ WP_Z ] - currentValue ) /
        ( dmax - dmin );

#ifdef DEBUG
    if ( dt == 0 )
        {
        Print( "Error! PID::eval dt = 0!\n" );
        Print( "Press any key!" );
        get_char();
        }
    if ( TI == 0 )
        {
        Print( "Error! PID::eval TI = 0!\n" );
        Print( "Press any key!" );
        get_char();
        }
#endif

    if ( dt == 0 ) dt = 1;
    if ( TI == 0 ) TI = 0.0001f;

    if ( get_delta_millisec( last_time ) > dt*1000L )
        {
        q0 = K * ( 1 + TD / dt );
        q1 = K * ( -1 - 2 * TD / dt + 2 * dt / TI );
        q2 = K * TD / dt;

        dUk = q0 * ek + q1 * ek_1 + q2 * ek_2;
        Uk = uk_1 + dUk;
        if ( Uk > 100 ) Uk = 100;
        if ( Uk < 0 ) Uk = 0;

        uk_1 = Uk;
        ek_2 = ek_1;
        ek_1 = ek;

        //-Зона разгона.
        if ( get_delta_millisec( start_time ) <
            par[ 0 ][ P_acceleration_time ] * 1000 )
            {
            acceleration( par[ 0 ][ P_acceleration_time ] );
            }
        //-Зона разгона.-!>

        last_time = get_millisec();
        } // if ( get_millisec() - last_time > dt*1000L )

    //-Мягкий пуск.
    // Включили ручной режим.
    if ( par[ 0 ][ P_is_manual_mode ] && 0 == prev_manual_mode )
        {
        prev_manual_mode = 1;
        par[ 0 ][ P_U_manual ] = w_par[ 0 ][ WP_U ];
        }

    // Выключили ручной режим.
    if ( par[ 0 ][ P_is_manual_mode ] == 0 && 1 == prev_manual_mode )
        {
        prev_manual_mode = 0;
        reset();

        // Начинаем заново разгон регулятора.
        start_time = get_millisec();

        // Устанавливаем начальное значение для разгона регулятора.
        start_value = par[ 0 ][ P_U_manual ];

        return par[ 0 ][ P_U_manual ];
        }
    //-Мягкий пуск.-!>

    //-Ограничение на выходное значение.
    float out_max = par[ 0 ][ P_out_max ];
    float out_min = par[ 0 ][ P_out_min ];
    if ( Uk < out_min )
        {
        Uk = out_min;
        }
    if ( Uk > out_max )
        {
        Uk = out_max;
        }

    w_par[ 0 ][ WP_U ] = Uk;

    if ( 1 == par[ 0 ][ P_is_manual_mode ] )
        {
        return par[ 0 ][ P_U_manual ];
        }

    return Uk;
    }
//-----------------------------------------------------------------------------
void PID::off()
    {
    if ( state != STATE_OFF )
        {
        state = STATE_OFF;
        }
    }
//-----------------------------------------------------------------------------
void PID::on( char is_down_to_inaccel_mode )
    {
    if ( state != STATE_ON )
        {
        state = STATE_ON;

        start_time = get_millisec(); // Для разгона регулятора.
        last_time  = get_millisec(); // Интервал пересчета значений.

        this->is_down_to_inaccel_mode = is_down_to_inaccel_mode;

        reset(); //Сбрасываем все переменные.
        start_value = 0;
        }
    }
//-----------------------------------------------------------------------------
void  PID::set( float new_out )
    {
    w_par[ 0 ][ WP_Z ] = new_out;
    }
//-----------------------------------------------------------------------------
void  PID::set_used_par( int parN )
    {
    if ( parN < 1 || parN > 2 )
        {
#ifdef DEBUG
        Print( "Error: void  PID::set_used_par ( int parN ), parN = %d\n", parN );
        Print( "Press any key!\n" );
        get_char();
#endif
        used_par_n = 1;
        }
    else used_par_n = parN;
    }
//-----------------------------------------------------------------------------
void PID::init_param( PARAM par_n, float val )
    {
    if ( par_n > 0 && ( u_int ) par_n <= par->get_count() )
        {
        //Проверка выхода за диапазон допустимых значений.
        if ( par_n == P_out_min && val < 0 )
            {                
            val = 0;
            }
        if ( par_n == P_out_max && val > 100 )
            {                
            val = 100;
            }    

        par[ 0 ][ par_n ] = val;
        }
    }
//-----------------------------------------------------------------------------
void PID::init_work_param( WORK_PARAM par_n, float val )
    {
    if ( par_n > 0 && ( u_int ) par_n <= w_par->get_count() )    
        {
        w_par[ 0 ][ par_n ] = val;
        }
    }
//-----------------------------------------------------------------------------
void PID::save_param()
    {
    par->save_all();
    }
//-----------------------------------------------------------------------------
void PID::reset()
    {
    uk_1 = 0;
    ek_1 = 0;
    ek_2 = 0;

    Uk = 0;
    if ( is_down_to_inaccel_mode )
        {
        Uk = 100;
        }

    w_par[ 0 ][ WP_U ] = 0;

    par[ 0 ][ P_is_manual_mode ] = 0;

    start_time = get_millisec(); //Для разгона регулятора.
    }
//-----------------------------------------------------------------------------
float PID::get_assignment()
    {
    return w_par[ 0 ][ WP_Z ];
    }
//-----------------------------------------------------------------------------
void PID::print() const
    {
    Print( "PID\n" );

    par->print();
    Print( "P_k = %f\n", par[ 0 ][ P_k ] );

    w_par->print();
    }
//-----------------------------------------------------------------------------
void PID::acceleration( float accel_time )
    {
    float deltaTime = ( float ) get_delta_millisec( start_time ) / 1000;
    float res = 100 * deltaTime / accel_time;

    if ( is_down_to_inaccel_mode )
        {
        res = 100 - res + start_value;
        if ( Uk < res ) Uk = res;
        }
    else
        {
        if ( Uk > res ) Uk = res + start_value;
        }
    }
//-----------------------------------------------------------------------------
int PID::set_cmd( const char *prop, u_int idx, double val )
	{
	if ( 0 == strcmp( prop, "RT_PAR_F" ) )
		{
		if ( idx > 0 && idx <= w_par->get_count() )
			{
			w_par[ 0 ][ idx ] = ( float ) val;
			return 0;
			}
		}
	if ( 0 == strcmp( prop, "S_PAR_F" ) )
		{
		if ( idx > 0 && idx <= par->get_count() )
			{
            //Проверка выхода за диапазон допустимых значений.
            if ( idx == P_out_min && val < 0 )
                {                
                val = 0;
                }
            if ( idx == P_out_max && val > 100 )
                {                
                val = 100;
                }              
            if ( ( idx == P_out_min && val > par[ 0 ][ P_out_max ] ) ||
                ( idx == P_out_max && val < par[ 0 ][ P_out_min ] ) )
                {                
                return 0;
                }
           
			par->save( idx, ( float ) val );
			return 0;
			}
		}
	return 0;
	}
//-----------------------------------------------------------------------------
int PID::set_cmd( const char *prop, u_int idx, char *val )
	{
#ifdef WIN_OS
	throw std::exception( "The method or operation is not implemented." );
#endif
    
    return 0;
	}
//-----------------------------------------------------------------------------
u_int_4 PID::get_state()
	{
	return state;
	}
//-----------------------------------------------------------------------------
int PID::save_device( char *buff )
    {
    int answer_size = sprintf( buff, "t.%s = \n\t{\n", name );

    answer_size += sprintf( buff + answer_size, "\tST=%u,\n", state );

    //Параметры.
    answer_size += par->save_device( buff + answer_size, "\t" );
    answer_size += w_par->save_device( buff + answer_size, "\t" );

    return answer_size += sprintf( buff + answer_size, "\t}\n" );
    }
//-----------------------------------------------------------------------------
