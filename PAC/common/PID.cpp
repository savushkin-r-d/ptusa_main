#include "pid.h"
//-----------------------------------------------------------------------------
PID::PID( int n ): par( new saved_params_float( 12 ) ),
    w_par( new run_time_params_float( 2 ) ),
    state( STATE_OFF ), 
    uk_1( 0 ), 
    ek_1( 0 ), 
    ek_2( 0 ),
    start_time( get_millisec() ), 
    last_time( get_millisec() ), 
    used_par_n( 1 ),
    prev_manual_mode( 0 ), 
    is_down_to_inaccel_mode( 0 )
    {  

    com_dev = new complex_device( n, "PID", 3,
        i_complex_device::COMPLEX_DEV );    
    com_dev->sub_dev[ 0 ] = new complex_state( "STATE", n,
        &state, this, single_state::T_PID, 1 );

    com_dev->sub_dev[ 1 ] = par;
    com_dev->sub_dev[ 2 ] = w_par;

    G_DEVICE_CMMCTR->add_device( com_dev ); 
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

    float ek = deltaSign * 100 * ( par[ 0 ][ WP_Z ] - currentValue ) / 
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

    if ( get_millisec() - last_time > dt*1000L ) 
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

        //-«она разгона.        
        if ( get_millisec() - start_time < 
            par[ 0 ][ P_acceleration_time ] * 1000 ) 
            {
            float deltaTime = ( float ) get_delta_millisec( start_time )/1000;
            float res = 100 * deltaTime / 
                par[ 0 ][ P_acceleration_time ];

            if ( is_down_to_inaccel_mode ) 
                { 
                res = 100 - res;
                if ( Uk < res ) Uk = res;
                }
            else
                {
                if ( Uk > res ) Uk = res;
                }
            }
        //-«она разгона.-!>
        last_time = get_millisec();
        } // if ( get_millisec() - last_time > dt*1000L ) 

    //-ћ€гкий пуск.
    if ( par[ 0 ][ P_is_manual_mode ] && 
        prev_manual_mode == 0 ) 
        {
        prev_manual_mode = 1;
        par[ 0 ][ P_U_manual ] = 
            par[ 0 ][ WP_Uk ];
        }

    if ( par[ 0 ][ P_is_manual_mode ] == 0 && 
        prev_manual_mode == 1 ) 
        {
        prev_manual_mode = 0;

        reset(); 

        //—охран€ем предыдущий ручной выход в качестве стартового дл€ ѕ»ƒ.
        uk_1 =  par[ 0 ][ P_U_manual ];                  
        }
    //-ћ€гкий пуск.-!>

    w_par[ 0 ][ WP_Uk ] = Uk;

    if ( par[ 0 ][ P_is_manual_mode ] == 1 ) 
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

        reset(); //—брасываем все переменные.
        }
    }
//-----------------------------------------------------------------------------
void PID::on( char is_down_to_inaccel_mode ) 
    {
    if ( state != STATE_ON ) 
        {
        state = STATE_ON;

        start_time = get_millisec(); // ƒл€ разгона регул€тора.
        last_time  = get_millisec(); // »нтервал пересчета значений.

        this->is_down_to_inaccel_mode = is_down_to_inaccel_mode; 

        reset(); //—брасываем все переменные.
        }
    }
//-----------------------------------------------------------------------------
void  PID::set ( float new_out )
    {
    w_par[ 0 ][ WP_Z ] = new_out;   
    }
//-----------------------------------------------------------------------------
void  PID::set_used_par ( int parN )
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
void PID::init_param( int par_n, float val )
    {
    par[ 0 ][ par_n ] = val;
    }
//-----------------------------------------------------------------------------
void PID::init_work_param( int par_n, float val )
    {
    w_par[ 0 ][ par_n ] = val;
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

    w_par[ 0 ][ WP_Uk ] = 0;

    par[ 0 ][ P_is_manual_mode ] = 0;

    start_time = get_millisec(); //ƒл€ разгона регул€тора.
    }
//-----------------------------------------------------------------------------
float PID::get_assignment()
    {
    return w_par[ 0 ][ WP_Z ];
    }
//-----------------------------------------------------------------------------
