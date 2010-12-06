#include "pid.h"
//-----------------------------------------------------------------------------
PID::PID( saved_params_float* par, 
    run_time_params_float *w_par,
    int startParamIndex,
    int startWorkParamsIndex ): par( par ),
    w_par( w_par ),
    state( STATE_OFF ), 
    uk_1( 0 ), 
    ek_1( 0 ), 
    ek_2( 0 ),
    start_time( get_millisec() ), 
    last_time( get_millisec() ), 
    start_param_index( startParamIndex ), 
    used_par_n( 1 ),
    start_work_params_index( startWorkParamsIndex ), 
    prev_manual_mode( 0 ), 
    is_down_to_inaccel_mode( 0 )
    {  
    }
//-----------------------------------------------------------------------------
PID::~PID() 
    {
    }
//-----------------------------------------------------------------------------
void PID::on( char isDownToInAccelMode ) 
    {
    if ( state != STATE_ON ) 
        {
        start_time = get_millisec();
        last_time = get_millisec();
        state = STATE_ON;
        this->is_down_to_inaccel_mode = isDownToInAccelMode; 
        }
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
float PID::eval( float currentValue, int deltaSign ) 
    {
    if ( STATE_OFF == state ) 
        {
        if ( this->is_down_to_inaccel_mode ) return 100;
        else return 0;
        }

    float K = par[ 0 ][ start_param_index + PAR_k ];
    float TI = par[ 0 ][ start_param_index + PAR_Ti ];
    float TD = par[ 0 ][ start_param_index + PAR_Td ];

    if ( used_par_n == 2 )
        {
        K = par[ 0 ][ start_param_index + PAR_k2 ];
        TI = par[ 0 ][ start_param_index + PAR_Ti2 ];
        TD = par[ 0 ][ start_param_index + PAR_Td2 ];
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

    float dt = par[ 0 ][ start_param_index + PAR_dt ] / 1000;
    float dmax = par[ 0 ][ start_param_index + PAR_dmax ];
    float dmin = par[ 0 ][ start_param_index + PAR_dmin ];

    if ( dmax == dmin ) 
        {
        dmax = dmin + 1;
#ifdef DEBUG
        Print( "Error! PID::eval dmax == dmin!\n" );
        Print( "Press any key!" );
        get_char();
#endif
        } 

    float ek = deltaSign * 100 * ( par[ 0 ][ start_param_index + PAR_Z ] - currentValue ) / 
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
            par[ 0 ][ start_param_index + PAR_AccelTime ] * 1000 ) 
            {
            float deltaTime = ( float ) get_delta_millisec( start_time )/1000;
            float res = 100 * deltaTime / 
                par[ 0 ][ start_param_index + PAR_AccelTime ];

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
    if ( par[ 0 ][ start_param_index + PAR_IsManualMode ] && 
        prev_manual_mode == 0 ) 
        {
        prev_manual_mode = 1;
        par[ 0 ][ start_param_index + PAR_UManual ] = 
            par[ 0 ][ start_param_index + PAR_Uk ];
        }

    if ( par[ 0 ][ start_param_index + PAR_IsManualMode ] == 0 && 
        prev_manual_mode == 1 ) 
        {
        prev_manual_mode = 0;
        reset( par[ 0 ][ start_param_index + PAR_UManual ] );    
        }
    //-ћ€гкий пуск.-!>

    par[ 0 ][ start_param_index + PAR_Uk ] = Uk;
    if ( start_work_params_index )
        {
        par[ 0 ][ start_work_params_index + WPAR_Uk ] = Uk;
        }    

    if ( par[ 0 ][ start_param_index + PAR_IsManualMode ] == 1 ) 
        {
        return par[ 0 ][ start_param_index + PAR_UManual ];
        }

    return Uk;
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

    par[ 0 ][ start_param_index + PAR_Uk ] = 0;
    if ( start_work_params_index )
        {
        par[ 0 ][ start_work_params_index + WPAR_Uk ] = 0;
        }

    par[ 0 ][ start_param_index + PAR_IsManualMode ] = 0;

    start_time = get_millisec(); //ƒл€ разгона регул€тора.
    }
//-----------------------------------------------------------------------------
void PID::reset( float new_uk_1 ) 
    {
    uk_1 = new_uk_1;  
    }
//-----------------------------------------------------------------------------
void  PID::set ( float newZ )
    {
    par[ 0 ][ start_param_index + PAR_Z ] = newZ;
    if ( start_work_params_index )
        {
        par[ 0 ][ start_work_params_index + WPAR_Z ] = newZ;
        }    
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
