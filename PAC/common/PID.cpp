#include "PID.h"
#include "string.h"
//-----------------------------------------------------------------------------
PID::PID( int n ): PID( ( "PID" + std::to_string( n ) ).c_str() )
    {
    is_old_style = true;
    G_DEVICE_CMMCTR->add_device( this );
    }
//-----------------------------------------------------------------------------
PID::PID( const char *name ) :device( name, device::DEVICE_TYPE::DT_REGULATOR,
    device::DEVICE_SUB_TYPE::DST_NONE, PARAMS_COUNT - 1 ),
    uk_1( 0 ),
    ek_1( 0 ),
    ek_2( 0 ),
    start_time( get_millisec() ),
    last_time( get_millisec() ),
    prev_manual_mode( 0 ),
    state( STATE_OFF ),
    used_par_n( 1 ),
    is_old_style( false )
    {
    out_value = 0;

    set_par_name( P_k, 0, "P_k" );
    set_par_name( P_Ti, 0, "P_Ti" );
    set_par_name( P_Td, 0, "P_Td" );
    set_par_name( P_dt, 0, "P_dt" );

    set_par_name( P_max, 0, "P_max" );
    set_par_name( P_min, 0, "P_min" );
    set_par_name( P_acceleration_time, 0, "P_acceleration_time" );
    set_par_name( P_is_manual_mode, 0, "P_is_manual_mode" );
    set_par_name( P_U_manual, 0, "P_U_manual" );

    set_par_name( P_k2, 0, "P_k2" );
    set_par_name( P_Ti2, 0, "P_Ti2" );
    set_par_name( P_Td2, 0, "P_Td2" );

    set_par_name( P_out_max, 0, "P_out_max" );
    set_par_name( P_out_min, 0, "P_out_min" );

    set_par_name( P_is_reverse, 0, "P_is_reverse" );
    set_par_name( P_is_zero_start, 0, "P_is_zero_start" );
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
        if ( ( *par )[ P_is_zero_start ] ) return 0;
        else return 100;
        }

    float K = ( *par )[ P_k ];
    float TI = ( *par )[ P_Ti ];
    float TD = ( *par )[ P_Td ];

    if ( used_par_n == 2 )
        {
        K = ( *par )[ P_k2 ];
        TI = ( *par )[ P_Ti2 ];
        TD = ( *par )[ P_Td2 ];
        }
    else
        {
        if ( G_DEBUG )
            {
            if ( used_par_n != 1 )
                {
                printf( "Error: void PID::eval() - used_par_n = %d\n",
                    used_par_n );
                }
            }
        }

    float dt = ( *par )[ P_dt ] / 1000;
    float dmax = ( *par )[ P_max ];
    float dmin = ( *par )[ P_min ];

    if ( dmax == dmin )
        {
        dmax = dmin + 1;
        if ( G_DEBUG )
            {
            printf( "Error! PID::eval() - dmax == dmin!\n" );
            }
        }

    float ek = deltaSign * 100 * ( set_value - currentValue ) /
        ( dmax - dmin );

    if ( G_DEBUG )
        {
        if ( dt == 0 )
            {
            printf( "Error! PID::eval() - dt = 0!\n" );
            }
        if ( TI == 0 )
            {
            printf( "Error! PID::eval() - TI = 0!\n" );
            }
        }

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
        int deltaTime = get_delta_millisec( start_time );
        if ( deltaTime < ( *par )[ P_acceleration_time ] * 1000 )
            {
            float res = 100 * deltaTime / ( *par )[ P_acceleration_time ];
            res += start_value;

            if ( ( *par )[ P_is_zero_start ] )
                {
                if ( Uk > res ) Uk = res;
                }
            else
                {
                res = 100 - res;
                if ( Uk < res ) Uk = res;
                }
            }
        //-Зона разгона.-!>

        last_time = get_millisec();
        } // if ( get_millisec() - last_time > dt*1000L )

    //-Мягкий пуск.
    // Включили ручной режим.
    if ( ( *par )[ P_is_manual_mode ] && 0 == prev_manual_mode )
        {
        prev_manual_mode = 1;
        ( *par )[ P_U_manual ] = out_value;
        }

    // Выключили ручной режим.
    if ( ( *par )[ P_is_manual_mode ] == 0 && 1 == prev_manual_mode )
        {
        prev_manual_mode = 0;
        reset();

        // Начинаем заново разгон регулятора.
        start_time = get_millisec();

        // Устанавливаем начальное значение для разгона регулятора.
        start_value = ( *par )[ P_U_manual ];

        return ( *par )[ P_U_manual ];
        }
    //-Мягкий пуск.-!>

    //-Ограничение на выходное значение.
    float out_max = ( *par )[ P_out_max ];
    float out_min = ( *par )[ P_out_min ];

    if ( G_DEBUG )
        {
        if ( out_max <= out_min )
            {
            printf( "Error! PID::eval() - out_max <= out_min (%f == %f)!\n",
                out_max, out_min );
            }
        if ( out_max == 0 )
            {
            printf( "Error! PID::eval() - out_max = 0!\n" );
            }
        }

    if ( Uk < out_min )
        {
        Uk = out_min;
        }
    if ( Uk > out_max )
        {
        Uk = out_max;
        }

    out_value = Uk;

    if ( 1 == ( *par )[ P_is_manual_mode ] )
        {
        return ( *par )[ P_U_manual ];
        }

    return Uk;
    }
//-----------------------------------------------------------------------------
void  PID::direct_on()
    {
    if ( state != STATE_ON )
        {
        state = STATE_ON;

        start_time = get_millisec(); // Для разгона регулятора.
        last_time = get_millisec();  // Интервал пересчета значений.

        reset(); //Сбрасываем все переменные.
        start_value = 0;
        }
    }
//-----------------------------------------------------------------------------
void  PID::set( float new_out )
    {
    set_value = new_out;
    }
//-----------------------------------------------------------------------------
void  PID::set_used_par( int parN )
    {
    if ( parN < 1 || parN > 2 )
        {
        if ( G_DEBUG )
            {
            printf( "Error: void  PID::set_used_par ( int parN ), parN = %d\n", parN );
            printf( "Press any key!\n" );
            get_char();
            }
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

        ( *par )[ par_n ] = val;
        }
    }
//-----------------------------------------------------------------------------
void PID::init_work_param( int par_n, float val )
    {
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

    Uk = (int)( *par )[ P_is_zero_start ] ? 0.f : 100.f;
    out_value = Uk;

    ( *par )[ P_is_manual_mode ] = 0;

    start_time = get_millisec(); //Для разгона регулятора.
    }
//-----------------------------------------------------------------------------
float PID::get_assignment()
    {
    return set_value;
    }
//-----------------------------------------------------------------------------
int PID::set_cmd( const char* prop, u_int idx, double val )
    {
    if ( is_old_style )
        {
        if ( 0 == strcmp( prop, "RT_PAR_F" ) )
            {
            if ( idx == 1 )
                {
                set_value = (float)val;
                return 0;
                }

            if ( idx == 2 )
                {
                out_value = (float)val;
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
                if ( ( idx == P_out_min && val > ( *par )[ P_out_max ] ) ||
                    ( idx == P_out_max && val < ( *par )[ P_out_min ] ) )
                    {
                    return 0;
                    }

                par->save( idx, (float)val );
                return 0;
                }
            }
        }
    else
        {
        device::set_cmd( prop, idx, val );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
const char* PID::get_name_in_Lua() const
    {
    return get_name();
    }
//-----------------------------------------------------------------------------
int PID::get_state()
    {
    return state;
    }
//-----------------------------------------------------------------------------
int PID::save_device( char *buff )
    {
    int answer_size = 0;
    if ( is_old_style )
        {
        answer_size = sprintf( buff, "t.%s = \n\t{\n", get_name() );

        answer_size += sprintf( buff + answer_size, "\tST=%u,\n", state );

        //Параметры.
        answer_size += par->save_device( buff + answer_size, "\t" );
        answer_size = sprintf( buff, "RT_PAR_F = { %f, %f }\n", set_value, out_value );

        answer_size += sprintf( buff + answer_size, "\t}\n" );
        }
    else
        {
        answer_size = device::save_device( buff, "\t" );
        }

    return answer_size;
    }
//-----------------------------------------------------------------------------
