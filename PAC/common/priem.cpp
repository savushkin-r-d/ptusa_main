#include "priem.h"
#include "string.h"

#if defined I7186_E || defined I7188_E
#include "udpc.h"
#endif

int DEB;

TTank        **g_tanks;
dev_stub     *g_tvirt;

#ifndef USE_NO_COMB
TMyComb      *g_greb;
#endif // USE_NO_COMB

int TTank::TankCnt = 0;
int TMyComb::CombCnt = 0;
//-----------------------------------------------------------------------------
int IsAnyMode( int mode, int lastTankN )
    {
    int res = -1;
    int lastTankN1;
    lastTankN1 = TTank::TankCnt < lastTankN ? TTank::TankCnt : lastTankN;

    for ( int i = 0; i < lastTankN1; i++ )
        {
        //if ( g_tanks[ i ]->GetMode( mode ) ) return i;
        }
    return res;
    }
//-----------------------------------------------------------------------------
int IsAnyModeC( int mode1, int mode2 )
    {
    int res = -1;
    for ( int i = 0; i< TTank::TankCnt; i++ )
        {
        if ( g_tanks[ i ]->GetMode( mode1 ) &&
            g_tanks[ i ]->GetMode( mode2 ) ) return i;
        }
    return res;
    }
//-----------------------------------------------------------------------------

int IsAnyModeT( int mode1, char TankType )
    {
    int res = -1;
    for ( int i = 0; i< TTank::TankCnt; i++ )
        {
        if ( ( g_tanks[ i ]->PodTip == TankType ) &&
             ( g_tanks[ i ]->GetMode( mode1 ) ) ) return i;
        }
    return res;
    }
//-----------------------------------------------------------------------------

int ProcMainParams( int argc, char *argv[] )
    {
    if ( argc > 1 )
        {
#if defined I7186_E || defined I7188_E
        if ( strcmp( argv[ 1 ], "ip" ) == 0 )
        {
            ServerIp = argv[ 2 ];
        }
#endif // defined I7186_E || defined I7188_E

        if ( strcmp( argv[ 1 ], "rcrc" ) == 0 )
            {
#ifdef DEBUG
            Print( "\nReset CRC..." );
#endif
            params_manager::get_instance()->reset_CRC();
#ifdef DEBUG
            Print( " Ok!\n" );
#endif
            }

        if ( strcmp( argv[ 1 ], "reseteeprom" ) == 0 )
            {
#ifdef DEBUG
            Print( "\nStarting reset total EEPROM..." );
#endif
            //EnableEEP();
#ifdef I7186_E
            for ( short i = 0; i < 4096; i++ )
                {
                WriteEEP( i/256+48, i%256, 0);
                }
#else
#ifdef I7188_E
            for ( short i = 0; i < 1792; i++ )
                {
                WriteEEP( i/256, i%256, 0);
                }
#else
            for ( short i = 0; i < 2048; i++ )
                {
                //WriteEEP( i/256, i%256, 0);
                }
#endif
#endif
           //ProtectEEP();
#ifdef DEBUG
            Print( " Ok!\n" );
#endif
            }
        }
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int TTank::SetMode( int mode, int newm )
    {
    int res;

    if ( newm != 0 ) newm = 1; //newm может равняться только 0 или 1.
    if ( mode < 0 || mode > 31 ) res = 3;
    else
        {
        if ( GetMode( mode ) == newm ) res = 1;
        else
            {
            if ( newm == 0 )
                {  //off mode
                state = state & ( ~( 1L << mode ) );
                FinalMode( mode );
                res = mode + 2000;
                }
            else
                {  //try to set
                if ( ( res = InitMode( mode ) ) == 0 )
                    {
                    state = state | 1L << mode;
                    res = mode + 1000;
                    }
                }
#ifdef DEBUG
            Print ("TTank[N%d]::SetMode mode = %d,[%d], res = %d, st = %u.\n",
                no, mode, newm, res, state );
#endif
            }
        }
    return res;
    }
//-----------------------------------------------------------------------------
int TTank::GetMode( int mode )
    {
    return ( int )( ( unsigned long )( state >> mode ) & 1 );
    }
//-----------------------------------------------------------------------------
TTank::TTank( saved_params_float *tpar, int n, int tmrCnt ): tmrCnt( tmrCnt ),
no( n ),
par( tpar ),
cmd( 0 )
    {
    //int i;
    //int j;

#if defined NO_TANKS_MODE
    //modes_manager = new mode_manager( 1 );
#else
//    modes_manager = new mode_manager( 32 );
#endif //NO_TANKS_MODE

//    modes_manager->set_param( tpar );

#if defined I7188_E || defined I7186_E
    //active_dev = new active_device( this, active_device::AT_TANK );
#endif // defined I7188_E || defined I7186_E

    com_dev = new complex_device( n, "TANK", 4, i_complex_device::COMPLEX_DEV );
    com_dev->sub_dev[ 0 ] = new single_state( "SINGLE_STATE", n, &state,
        this, single_state::T_TANK, 1 );
    com_dev->sub_dev[ 1 ] = new complex_state( "STATE", n, &state, this,
        single_state::T_TANK );
    com_dev->sub_dev[ 2 ] = new complex_state( "CMD", n, &cmd, this,
        single_state::T_TANK );
    com_dev->sub_dev[ 3 ] = par;

    TMR = new timer*[ tmrCnt ];
    for ( int i = 0; i < tmrCnt; i++) TMR[ i ] = new timer;

    state = 0;
    modeStartTime[ 32 ] = get_sec();
    }
//-----------------------------------------------------------------------------
TTank::~TTank()
    {
    //int i;
    //for ( i = 0; i < tmrCnt; i++ ) delete TMR[ i ];
    //delete TMR;
    }
//-----------------------------------------------------------------------------
int TTank::InitParams( )
    {
    //for ( int j = par->WORK_PARAM_CNT; j < par->getParamCnt(); j++ )
    //    par->setParamM( j, 0 );
    return 0;
    }
//-----------------------------------------------------------------------------
int TTank::InitWorkParams( )
    {
    //for ( int j = 0; j < par->WORK_PARAM_CNT; j++ ) par->setParamM( j, 0 );

    return 0;
    }
//-----------------------------------------------------------------------------
int TTank::InitMode( int mode )
    {
    if ( mode < 33 )
        {
        //Инициализация времени  режима.
        modeStartTime[ mode ] = get_sec( );
        isModeEvaluationTimeLeft[ mode ] = 0;
        }
#ifdef DEBUG
    else Print( "Error TTank::InitMode m = %d!\n", mode );
#endif

    return 0;
    }
//-----------------------------------------------------------------------------
int TTank::FinalMode( int mode )
    {
    //Инициализация времени  режима.
    if ( 0 == state ) modeStartTime[ 32 ] = get_sec( );
    isModeEvaluationTimeLeft[ mode ] = 0;

#if !defined NO_TANKS_MODE
//    modes_manager->final( mode );
#endif // NO_TANKS_MODE
    return 0;
    }
//-----------------------------------------------------------------------------
int TTank::Evaluate( )
    {
    int i;
    for ( i = 0; i < 32; i++ )
        {
        if ( GetMode( i ) )
            {
            int currentMode = i;

            //-Работа с временем режима.
            if ( modeEvaluationTime[ currentMode ] > 0 &&
                get_sec() - modeStartTime[ currentMode ] >
                modeEvaluationTime[ currentMode ] )
                isModeEvaluationTimeLeft[ currentMode ] = 1;
#if !defined NO_TANKS_MODE
//            modes_manager->evaluate( i );
#endif // NO_TANKS_MODE
            }
        }
    return 0;
    }
//-----------------------------------------------------------------------------
int TTank::SetModeTime( int mode, unsigned long modeTime )
    {
    modeEvaluationTime[mode] = modeTime;
    return 0;
    }
//-----------------------------------------------------------------------------
int TTank::save_active_state( char *buff )
    {
    buff++; //Чтобы не было warning'а.
    return 0;
    }
//-----------------------------------------------------------------------------
int TTank::load_active_state( char *buff )
    {
    buff++; //Чтобы не было warning'а.
    return 0;
    }
//-----------------------------------------------------------------------------
int TTank::set_default_state()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int TTank::get_saved_size()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//Возвращаемые значения:
//  0    -  999 : ошибка включения режима;
//  1000 - 1999 : режим (0 - 999) включился;
//  2000 - 2999 : режим (0 - 999) выключился;

int TMyComb::SetMode( int mode, int newm )
    {
    int i, res;

#ifdef DEBUG
    Print ( "Start TMyComb::SetMode mode ( mode = %d, new state = %d ). \n",
        mode, newm );
#endif

    if ( newm != 0 ) newm = 1; //newm может равняться только 0, или 1.
    if ( mode > statesCnt * 32 - 1 ) res = 3;
    else
        {
        if ( GetMode( mode ) == newm ) res = 1;
        else
            {
            if ( newm == 0 )
                {  //off mode
                state[ mode / 32 ] = state[ mode / 32 ] & ~( 1L << mode % 32 );
                FinalMode( mode );

#ifndef USE_NO_COMB
//                g_greb->close_path( paths[ mode ].in_x, paths[ mode ].in_y,
//                    paths[ mode ].out_x, paths[ mode ].out_y );
#endif // USE_NO_COMB

                res = mode + 2000;
                }
            else
                { //check if possible
                if ( ( res = InitMode( mode ) ) == 0 )
                    {
                    state[ mode / 32 ] = state[ mode / 32 ] | 1L << mode % 32;

#ifndef USE_NO_COMB
//                    g_greb->open_path( paths[ mode ].in_x, paths[ mode ].in_y,
//                        paths[ mode ].out_x, paths[ mode ].out_y, comb_path::OT_COMB,
//                        this, mode );
#endif // USE_NO_COMB

                    res = mode + 1000;
                    }
                }
            }
        }
#ifdef DEBUG
    Print ( "End TMyComb::SetMode mode = %d,[%d], res = %d. ", mode, newm, res );
    for ( i = 0; i < statesCnt; i++)
        Print ( " state[ %d ] = %u \n", i, state[ i ] );
    Print( "\n" );
#endif
    return res;
    }
//-----------------------------------------------------------------------------
int TMyComb::GetMode(int mode)
    {
    return ( int )( ( unsigned long )( state [mode / 32] >> mode % 32 ) & 1 );
    }
//-----------------------------------------------------------------------------
int TMyComb::Evaluate(void)
    {
    //eval mix timers
    if (TMR[ TMR_USEATO ]->get_state() == 1 )
        {
        TMR[ TMR_USEATO ]->reset();
        TMR[ TMR_USEATO ]->set_countdown_time( ( *par )[ P_SEAT_PERIOD ] * 1000 );
        TMR[ TMR_USEATC ]->start();
        UseatOn=1;
        };
    if (TMR[TMR_USEATC]->get_state() == 1 )
        {
        TMR[TMR_USEATC]->reset();
        TMR[TMR_USEATC]->set_countdown_time( ( *par )[ P_SEAT_TIME ] );
        TMR[TMR_USEATO]->start();
        UseatOn=0;
        };
    if (TMR[TMR_LSEATO]->get_state( )==1)
        {
        TMR[TMR_LSEATO]->reset();
        TMR[TMR_LSEATO]->set_countdown_time( par[ 0 ][ P_SEAT_PERIOD ]*1000);
        TMR[TMR_LSEATC]->start();
        LseatOn=1;
        };
    if (TMR[TMR_LSEATC]->get_state( )==1)
        {
        TMR[TMR_LSEATC]->reset();
        TMR[TMR_LSEATC]->set_countdown_time( par[ 0 ][ P_SEAT_TIME ] );
        TMR[TMR_LSEATO]->start();
        LseatOn=0;
        };

    return 0;
    }
//-----------------------------------------------------------------------------
int TMyComb::FinalMode( int mode )
    {
#ifdef DEBUG
    Print ( "\nTMyComb::FinalMode mode = %d.", mode );
#endif

    return 0;
    }
//-----------------------------------------------------------------------------
int TMyComb::InitMode( int mode )
    {
#ifdef DEBUG
    Print ( "\nTMyComb::InitMode mode = %d.", mode );
#endif

    return 0;
    }
//-----------------------------------------------------------------------------
int TMyComb::InitParams()
    {
    for ( u_int j = 0; j < wpar->get_count(); j++ )
        {
        wpar[ 0 ][ j ] = 0;
        }
    par[ 0 ][ P_SEAT_PERIOD ] = 60;
    par[ 0 ][ P_SEAT_TIME ] = 1000;

    return 0;
    }
//-----------------------------------------------------------------------------
int TMyComb::InitWorkParams()
    {
    for ( u_int j = 0; j < wpar->get_count(); j++ )
        {
        wpar[ 0 ][ j ] = 0;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
TMyComb::TMyComb( int stCnt, int parCnt, int workParCnt, int tmrCnt ): cmd( 0 ),    
    statesCnt(stCnt),
    tmrCnt( tmrCnt )
    {
#if defined I7188_E || defined I7186_E
    active_dev = new active_device( this, active_device::AT_COMB );
#endif // defined I7188_E || defined I7186_E

    state = new u_int_4 [ statesCnt ];
    int i;
    for ( i = 0; i < statesCnt; i++ ) state[ i ] = 0;

    par = new saved_params_float( parCnt );
    wpar = new run_time_params_float( workParCnt );


    TMR = new timer*[tmrCnt];
    for ( i = 0; i < tmrCnt; i++) TMR[ i ] = new timer;
    //run mix proof timers
    if ( tmrCnt < 4 )
        {
#ifdef DEBUG
        Print( "Error creating comb (tmrCnt[%d] < 4)! Change it to correct value via editor WPRG.EXE.\n",
            tmrCnt );
#endif // DEBUG
        SetGlobalError( EC_DEVICE_DEFINITION, EC_COMB_TIMERS_COUNT, 0 );
        while  ( 1 )
            {
            ShowErrors();
            }
        }
    if ( parCnt < 17 )
        {
#ifdef DEBUG
        Print( "Error creating comb (parCnt[%d] < 17)! Change it to correct value via editor WPRG.EXE.\n",
            tmrCnt );
#endif // DEBUG
        SetGlobalError( EC_DEVICE_DEFINITION, EC_COMB_PARAMS_COUNT, 0 );
        while  ( 1 )
            {
            ShowErrors();
            }
        }

    TMR[TMR_USEATO]->reset();
    TMR[TMR_USEATO]->set_countdown_time( par[ 0 ][ P_SEAT_PERIOD ] * 1000 );
    TMR[TMR_USEATO]->start();

    TMR[TMR_USEATC]->reset();
    TMR[TMR_USEATC]->set_countdown_time( par[ 0 ][ P_SEAT_TIME ] );
    
    TMR[TMR_LSEATO]->reset();
    TMR[TMR_LSEATO]->set_countdown_time( par[ 0 ][ P_SEAT_PERIOD ] * 1000 );
    TMR[TMR_LSEATO]->start();
    //TMR[TMR_LSEATO]->ac=par[ 0 ](P_SEAT_PERIOD)*1000/2;

    TMR[TMR_LSEATC]->reset();
    TMR[TMR_LSEATC]->set_countdown_time( par[ 0 ][ P_SEAT_TIME ] );
    LseatOn=0;
    UseatOn=0;

    com_dev = new complex_device( 1, "COMB", 4, i_complex_device::COMPLEX_DEV );
    com_dev->sub_dev[ 0 ] = new single_state( "SINGLE_STATE", 1, state,
        this, single_state::T_COMB, stCnt );
    com_dev->sub_dev[ 1 ] = new complex_state( "STATE", 1, state,
        this, single_state::T_COMB, stCnt );
    com_dev->sub_dev[ 2 ] = new complex_state( "CMD", 1, &cmd,
        this, single_state::T_COMB );
    com_dev->sub_dev[ 3 ] = par;
    }
//-----------------------------------------------------------------------------
TMyComb::~TMyComb(void)
    {
    int i;
    for ( i = 0; i<tmrCnt; i++) delete TMR[ i ];
    delete TMR;
    }
//-----------------------------------------------------------------------------
