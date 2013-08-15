#include "stdafx.h"

#include "PID.h"

//---------------------------------------------------------------------------------------
DWORD MyGetMS()
    {
    return GetTickCount();
    }
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
PID::PID( int startParamIndex, int startWorkParamsIndex ):state( STATE_OFF ), 
         uk_1( 0 ), 
         ek_1( 0 ), 
         ek_2( 0 ),
         startTime( MyGetMS() ), 
         lastTime( MyGetMS() ), 
         startParamIndex( startParamIndex ),          
         startWorkParamsIndex( startWorkParamsIndex ), 
         prevManualMode( 0 ), 
         isDownToInAccelMode( 0 )         
    {  
    }
//---------------------------------------------------------------------------------------
PID::~PID() 
    {
    }
//---------------------------------------------------------------------------------------
void PID::On( char isDownToInAccelMode ) 
    {
    if ( state != STATE_ON ) 
        {
        startTime = MyGetMS();
        lastTime = MyGetMS();
        state = STATE_ON;
        this->isDownToInAccelMode = isDownToInAccelMode; 
        }
    }
//---------------------------------------------------------------------------------------
void PID::Off() 
    {
    if ( state != STATE_OFF ) 
        {
        state = STATE_OFF;
        }
    }
//---------------------------------------------------------------------------------------
float PID::Eval( float currentValue, int deltaSign ) 
    {
    if ( state == STATE_OFF ) return 0;

    float K = par[ startParamIndex + PAR_k ];
    float TI = par[ startParamIndex + PAR_Ti ];
    float TD = par[ startParamIndex + PAR_Td ];
    float dt = par[ startParamIndex + PAR_dt ];
    float dmax = par[ startParamIndex + PAR_dmax ];
    float dmin = par[ startParamIndex + PAR_dmin ];

    if ( dmax == dmin ) 
        {
        dmax = dmin + 1;
#ifdef DEBUG
        Print( "Error! PID::Eval dmax == dmin!\n" );
        Print( "Press any key!" );
        Getch();
#endif
        } 

    float ek = deltaSign * 100 * ( par[ startParamIndex + PAR_Z ] - currentValue ) / 
        ( dmax - dmin );

    //if ( MyGetMS() - lastTime > dt ) 
    //    {
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
        if ( MyGetMS() - startTime < par[ startParamIndex + PAR_AccelTime ] ) 
            {
            float deltaTime = ( MyGetMS() - startTime );
            float res = 100 * deltaTime / par[ startParamIndex + PAR_AccelTime ];
            if ( isDownToInAccelMode ) 
                { 
                res = 100 - res;
                if ( Uk < res ) Uk = res;
                }
            else
                {
                if ( Uk > res ) Uk = res;
                }
            }
        //-Зона разгона.-!>
        lastTime = MyGetMS();
        //}

    par[ startParamIndex + PAR_Uk ] = Uk;
    par[ startWorkParamsIndex + WPAR_Uk ] = Uk;

    return ( float ) Uk;
    }
//---------------------------------------------------------------------------------------
void PID::Reset() 
    {
    uk_1 = 0;
    ek_1 = 0;
    ek_2 = 0;
    par[ startParamIndex + PAR_Uk ] = 0;
    par[ startWorkParamsIndex + WPAR_Uk ] = 0;
    }
//---------------------------------------------------------------------------------------
void PID::Reset( float new_uk_1 ) 
    {
    uk_1 = new_uk_1;  
    }
//---------------------------------------------------------------------------------------
void  PID::SetZ ( float newZ )
    {
    par[ startParamIndex + PAR_Z ] = newZ;
    par[ startWorkParamsIndex + WPAR_Z ] = newZ;
    }