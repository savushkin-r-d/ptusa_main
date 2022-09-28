#include "PID.h"
#include "string.h"
//-----------------------------------------------------------------------------
PID::PID( int n ): PID( ( "PID" + std::to_string( n ) ).c_str() )
    {
    is_old_style = true;
    G_DEVICE_CMMCTR->add_device( this );
    }
//-----------------------------------------------------------------------------
PID::PID( const char* name ) :device( name, device::DEVICE_TYPE::DT_REGULATOR,
    device::DEVICE_SUB_TYPE::DST_REGULATOR_PID, PARAMS_COUNT - 1 ),
    uk_1( 0 ),
    ek_1( 0 ),
    ek_2( 0 ),
    q0( 0 ),
    q1( 0 ),
    q2( 0 ),
    Uk( 0 ),
    dUk( 0 ),
    start_time( get_millisec() ),
    last_time( get_millisec() ),
    prev_manual_mode( 0 ),
    state( STATE::OFF ),
    used_par_n( 1 ),
    is_old_style( false ),
    start_value( 0 ),
    sensor( 0 ),
    actuator( 0 )
    {
    out_value = 0;
    set_value = 0;

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
void PID::on( char is_not_zero_start )
    {
    ( *par )[ P_is_zero_start ] = !is_not_zero_start;
    device::on();
    }
//-----------------------------------------------------------------------------
float PID::eval( float currentValue, int deltaSign )
    {
    if ( STATE::OFF == state )
        {
        if ( ( *par )[ P_is_zero_start ] ) return MIN_OUT_VALUE;
        else return MAX_OUT_VALUE;
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

    float dt = ( *par )[ P_dt ] / MSEC_IN_SEC;
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

    float ek = deltaSign * MAX_OUT_VALUE *
        ( set_value - currentValue ) / ( dmax - dmin );

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

    if ( get_delta_millisec( last_time ) > dt * MSEC_IN_SEC )
        {
        q0 = K * ( 1 + TD / dt );
        q1 = K * ( -1 - 2 * TD / dt + 2 * dt / TI );
        q2 = K * TD / dt;

        dUk = q0 * ek + q1 * ek_1 + q2 * ek_2;
        Uk = uk_1 + dUk;
        if ( Uk > MAX_OUT_VALUE ) Uk = MAX_OUT_VALUE;
        if ( Uk < MIN_OUT_VALUE ) Uk = MIN_OUT_VALUE;

        uk_1 = Uk;
        ek_2 = ek_1;
        ek_1 = ek;

        //-Зона разгона.
        unsigned long delta_time = get_delta_millisec( start_time );
        unsigned long acceleration_time = MSEC_IN_SEC *
            static_cast<unsigned int>( ( *par )[ P_acceleration_time ] );
        if ( delta_time < acceleration_time )
            {
            float res = MAX_OUT_VALUE * delta_time / acceleration_time;
            if ( ( *par )[ P_is_zero_start ] )
                {
                res = start_value + res;
                if ( Uk > res ) Uk = res;
                }
            else
                {
                res = start_value - res;
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

        out_value = ( *par )[ P_U_manual ];
        return out_value;
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
        if ( out_max == MIN_OUT_VALUE )
            {
            printf( "Error! PID::eval() - out_max = %f!\n", MIN_OUT_VALUE );
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
        out_value = ( *par )[ P_U_manual ];
        }

    return out_value;
    }
//-----------------------------------------------------------------------------
void  PID::direct_on()
    {
    if ( state != STATE::ON )
        {
        state = STATE::ON;

        start_time = get_millisec(); // Для разгона регулятора.
        last_time = get_millisec();  // Интервал пересчета значений.

        reset();
        }
    }
//-----------------------------------------------------------------------------
void PID::direct_set_state( int st )
    {
    switch ( static_cast<STATE>( st ) )
        {
        case STATE::OFF:
            direct_off();
            break;

        case STATE::ON:
            direct_on();
            break;
        }
    }
//-----------------------------------------------------------------------------
/// @brief Выключение ПИД.
void PID::direct_off()
    {
    if ( state != STATE::OFF )
        {
        reset();
        state = STATE::OFF;
        if ( actuator )
            {
            actuator->off();
            }
        }
    }
//-----------------------------------------------------------------------------
void PID::direct_set_value( float val )
    {
    set_value = val;
    if ( sensor )
        {
        float in_value = sensor->get_value();
        if ( sensor->get_type() == DT_FQT )
            {
            in_value = dynamic_cast <i_counter*> ( sensor )->get_flow();
            }

        int sign = ( *par )[ P_is_reverse ] > 0 ? -1 : 1;
        out_value = eval( in_value, sign );
        if ( actuator )
            {
            if ( STATE::OFF == state )
                {
                actuator->off();
                }
            else
                {
                actuator->on();
                actuator->set_value( out_value );
                }
            }
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
        if ( par_n == P_out_min && val < MIN_OUT_VALUE )
            {
            val = MIN_OUT_VALUE;
            }
        if ( par_n == P_out_max && val > MAX_OUT_VALUE )
            {
            val = MAX_OUT_VALUE;
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
float PID::get_value()
    {
    return out_value;
    }
//-----------------------------------------------------------------------------
void PID::reset()
    {
    uk_1 = 0;
    ek_1 = 0;
    ek_2 = 0;

    bool is_zero_start = ( *par )[ P_is_zero_start ] != 0;
    start_value = is_zero_start ? MIN_OUT_VALUE : MAX_OUT_VALUE;
    Uk = is_zero_start ? MIN_OUT_VALUE : MAX_OUT_VALUE;
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

        if ( 0 == strcmp( prop, WORK_PARAMS_NAME ) )
            {
            if ( idx == WP_Z )
                {
                set_value = (float)val;
                return 0;
                }

            if ( idx == WP_U )
                {
                out_value = (float)val;
                return 0;
                }
            }
        if ( 0 == strcmp( prop, par->get_name() ) )
            {
            if ( idx > 0 && idx <= par->get_count() )
                {
                //Проверка выхода за диапазон допустимых значений.
                if ( idx == P_out_min && val < MIN_OUT_VALUE )
                    {
                    val = MIN_OUT_VALUE;
                    }
                if ( idx == P_out_max && val > MAX_OUT_VALUE )
                    {
                    val = MAX_OUT_VALUE;
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
void PID::set_string_property( const char* field, const char* value )
    {
    if ( !field ) return;

    switch ( field[ 0 ] )
        {
        //IN_VALUE
        case 'I':
            sensor = G_DEVICE_MANAGER()->get_device( value );
            break;

        //OUT_VALUE
        case 'O':
            actuator = G_DEVICE_MANAGER()->get_device( value );
            break;

        default:
            device::set_string_property( field, value );
            break;
        }
    }
//-----------------------------------------------------------------------------
const char* PID::get_name_in_Lua() const
    {
    return get_name();
    }
//-----------------------------------------------------------------------------
int PID::get_state()
    {
    return static_cast<int>( state );
    }
//-----------------------------------------------------------------------------
int PID::save_device_ex( char* buff )
    {
    int answer_size = sprintf( buff, "Z=%.2f, ", set_value );
    return answer_size;
    }
//-----------------------------------------------------------------------------
int PID::save_device( char *buff )
    {
    int answer_size = 0;
    if ( is_old_style )
        {
        answer_size = sprintf( buff, "t.%s = \n\t{\n", get_name() );

        answer_size += sprintf( buff + answer_size, "\tST=%u,\n",
            static_cast<unsigned int>(state) );

        //Параметры.
        answer_size += par->save_device( buff + answer_size, "\t" );
        answer_size += sprintf( buff + answer_size,
            "%s = { %.2f, %.2f }\n", WORK_PARAMS_NAME, set_value, out_value );

        answer_size += sprintf( buff + answer_size, "\t}\n" );
        }
    else
        {
        answer_size = device::save_device( buff, "\t" );
        }

    return answer_size;
    }
//-----------------------------------------------------------------------------
