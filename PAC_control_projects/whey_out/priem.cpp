#include "priem.h"

//-----------------------------------------------------------------------------
#define G_COMB G_TECH_OBJECTS( C_COMB_IDX )
#define POST1  G_TECH_OBJECTS( C_POST1_IDX )
#define POST2  G_TECH_OBJECTS( C_POST2_IDX )
#define TANK1  G_TECH_OBJECTS( C_T1_IDX )
#define TANK2  G_TECH_OBJECTS( C_T2_IDX )
#define TANK3  G_TECH_OBJECTS( C_T3_IDX )
#define TANK4  G_TECH_OBJECTS( C_T4_IDX )
//-----------------------------------------------------------------------------
whey_tank::whey_tank( int n ): tech_object( "TANK",
    n,  // Number.
    20, // States count.
    5,  // Timers_count.
    1,  // Params count float.
    10, // Work params count float.
    1,  // Params count u_int.
    1 ),// Work params count u_int.
    V1( V( number * 100 + 1 ) ),
    V2( V( number * 100 + 2 ) ),
    V3( V( number * 100 + 3 ) ),
    V4( V( number * 100 + 4 ) ),
    V5( V( number * 100 + 5 ) ),
    V7_1( 1 == number || 2 == number ? V( 107 ) : V( 507 ) ),
    V7_2( 1 == number || 2 == number ? V( 207 ) : V( 307 ) ),
    LSL( LS( number * 100 + 1 ) ),
    LE1( LE( number ) ),
    LSH( LS( number * 100 + 2 ) ),        
    T( TE( number ) )
    {
    }
//-----------------------------------------------------------------------------
whey_tank::~whey_tank()
    {
    }
//-----------------------------------------------------------------------------
int whey_tank::evaluate()
    {
    tech_object::evaluate();
    
    for ( u_int i = 0; i < get_modes_count(); i++ )
        {
        if ( get_mode( i ) )
            {
            switch ( i )
                {
                case T_WHEY_ACCEPTING:        
                    V1->on();
                    V2->on();
                    V4->off();
                    V( 106 )->off();

                    if ( LSH->get_state() == LIS ) set_mode( T_WHEY_ACCEPTING, 0 );
                    break;

                case T_WHEY_OUT_P1: 
                    V1->on();

                    if ( 1 == number || 2 == number )
                        {
                        V3->off();
                        if ( POST1->get_mode( POST_WHEY_ACCEPTING ) ) V4->on();
                        else V4->off();
                        }
                    else
                        {
                        V3->on();
                        V4->off();

                        V( 107 )->off();
                        V( 207 )->on();
                        V( 206 )->off();
                        V( 307 )->on();
                        V( 507 )->off();

                        V_1->off();
                        V_2->off();
                        V_3->on();
                        if ( POST1->get_mode( POST_WHEY_ACCEPTING ) ) V_4->on();
                        else V_4->off();
                        }

                    if ( ( LSL->get_state() == LNO ) ||
                        ( POST1->get_mode( POST_WHEY_ACCEPTING ) &&
                        ( ( post* ) POST1 )->flow->get_state() == 0 ) )
                        {
                        int idx = G_TECH_OBJECT_MNGR->get_object_with_active_mode(
                            T_WHEY_WOUT_P1, C_T1_IDX, C_T4_IDX );
                
                        if ( idx >= 0 &&
                            LE1->get_value() < POST1->par_float[ post::S_F__LE_MIN ] )
                            set_mode( T_WHEY_OUT_P1, 0 );
                        else
                            {
                            POST1->set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
                            POST1->rt_par_float[ post::RT_F__WARNING_REASON ] =
                                post::W_NO_FLOW;
                            }
                        }
                    break;

                case T_WHEY_OUT_P2:      
                    V1->on();

                    if ( 3 == number || 4 == number )
                        {
                        V3->off();
                        if ( POST2->get_mode( POST_WHEY_ACCEPTING ) ) V4->on();
                        else V4->off();
                        }
                    else
                        {
                        V3->on();
                        V4->off();

                        V( 107 )->off();
                        V( 207 )->on();
                        V( 206 )->off();
                        V( 307 )->on();
                        V( 507 )->off();

                        V_1->off();
                        V_2->off();
                        V_3->on();

                        if ( POST2->get_mode( POST_WHEY_ACCEPTING ) ) V_4->on();
                        else V_4->off();
                        }

                    if ( ( LSL->get_state() == LNO ) ||
                        ( POST2->get_mode( POST_WHEY_ACCEPTING ) &&
                        ( ( post* ) POST2 )->flow->get_state() == 0 ) )
                        {
                        int idx = G_TECH_OBJECT_MNGR->get_object_with_active_mode(
                            T_WHEY_WOUT_P2, C_T1_IDX, C_T4_IDX );
                        
                        if ( idx >= 0 ) set_mode( T_WHEY_OUT_P2, 0 );
                        else
                            {
                            POST2->set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
                            POST2->rt_par_float[ post::RT_F__WARNING_REASON ] =
                                post::W_NO_FLOW;
                            }
                        }
                    break;

                case T_WASH:                
                    V1->on();
                    V2->off();
                    V3->on();
                    V4->off();
                    V5->on();
                    V7_1->off();
                    V7_2->on();
                    V( 108 )->off();
                    V( 206 )->on();
                    V( 208 )->on();
                    V( 109 )->on();
                    N( 2202 )->on();
                    break;

                case T_WHEY_WACCEPTING:       
                    break;

                case T_WHEY_WOUT_P1:          
                    break;

                case T_WHEY_WOUT_P2:          
                    break;
                }
            }
        }
    return 0;
    }
//-----------------------------------------------------------------------------
int whey_tank::final_mode( u_int mode )
    {
    tech_object::final_mode( mode );

    switch ( mode )
        {
        case T_WHEY_ACCEPTING:          
            V1->off();
            V2->off();
            break;

        case T_WHEY_OUT_P1:
            V1->off();

            if ( 1 == number || 2 == number ) V4->off();
            else
                {
                V3->off();
                V( 207 )->off();
                V( 307 )->off();
                V_3->off();
                V_4->off();
                }
            break;

        case T_WHEY_OUT_P2:               
            V1->off();

            if ( 3 == number || 4 == number ) V4->off();
            else
                {
                V3->off();

                V( 207 )->off();
                V( 307 )->off();

                V_3->off();
                V_4->off();
                }
            break;

        case T_WASH:                     
            V1->off();
            V3->off();
            V5->off();

            V7_2->off();
            V( 206 )->off();
            V( 208 )->off();
            V( 109 )->off();
            N( 2202 )->off();
            break;

        case T_WHEY_WACCEPTING:           
        case T_WHEY_WOUT_P1:              
        case T_WHEY_WOUT_P2:              
            break;
        }

    //-При отключении приёмки/выдачи включаем при наличии ожидающие
    // приёмку/выдачу.
    if ( mode >= T_WHEY_ACCEPTING && mode <= T_WHEY_OUT_P2 )
        {
        int idx = G_TECH_OBJECT_MNGR->get_object_with_active_mode(
            mode + T_WHEY_WACCEPTING, C_T1_IDX, C_T4_IDX );

        if ( idx >= 0 )
            {
            G_TECH_OBJECTS( idx )->set_mode( mode + T_WHEY_WACCEPTING, 0 );
            G_TECH_OBJECTS( idx )->set_mode( mode, 1 );
            if ( T_WHEY_ACCEPTING == mode )
                {
                G_TECH_OBJECTS( idx )->rt_par_float[ T_WARNING_REASON ] =
                   TW_TANK_IN_START;
                }
            }
        else
            {
            if ( T_WHEY_ACCEPTING == mode )
                rt_par_float[ T_WARNING_REASON ] = TW_TANK_IN_STOP;
            }
        }
    //-При отключении приёмки/выдачи включаем при наличии ожидающие
    // приёмку/выдачу-!>

    return 0;
    }
//-----------------------------------------------------------------------------
int whey_tank::exec_cmd( u_int cmd )
    {
    tech_object::exec_cmd( cmd );

    int idx = -1;
    switch ( cmd )
        {
        case CMD_RESET_TANK_POST1:
            idx = G_TECH_OBJECT_MNGR->get_object_with_active_mode(
                T_WHEY_WOUT_P1, C_T1_IDX, C_T4_IDX );

            if ( idx == -1 ) POST1->exec_cmd( CMD_RESET_POST ); // Сбрасываем пост 1.
            set_mode( T_WHEY_OUT_P1, 0 );
            break;

        case CMD_RESET_TANK_POST2:
            idx = G_TECH_OBJECT_MNGR->get_object_with_active_mode(
                T_WHEY_WOUT_P2, C_T1_IDX, C_T4_IDX );

            if ( idx == -1 ) POST2->exec_cmd( CMD_RESET_POST ); // Сбрасываем пост 2.
            set_mode( T_WHEY_OUT_P2, 0 );
            break;

        case CMD_SET_POST1_AND_TANK:
            if ( get_mode ( T_WASH ) ||   // Во время мойки танка нельзя.
                get_mode( T_WHEY_OUT_P2 ) ||
                get_mode( T_WHEY_WOUT_P2 ) ) return 1;

            if ( set_mode( T_WHEY_OUT_P1, 1 ) == 1000 + T_WHEY_OUT_P1 )
                POST1->set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
            break;

        case CMD_SET_POST2_AND_TANK:
            if ( get_mode ( T_WASH ) ||   // Во время мойки танка нельзя.
                get_mode( T_WHEY_OUT_P1 ) ||
                get_mode( T_WHEY_WOUT_P1 ) ) return 1;

            if ( set_mode( T_WHEY_OUT_P2, 1 ) == 1000 + T_WHEY_OUT_P2 )
                POST2->set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
            break;

        case CMD_SET_HEATING_POST1_AND_TANK:
            exec_cmd( CMD_SET_POST1_AND_TANK );
            POST1->set_mode( POST_WHEY_HEATING, 1 );
            break;

        case CMD_SET_HEATING_POST2_AND_TANK:
            exec_cmd( CMD_SET_POST2_AND_TANK );
            POST2->set_mode( POST_WHEY_HEATING, 1 );
            break;
        }

    rt_par_float[ T_WARNING_REASON ] = 0;
    
    return 1000 + cmd;
    }
//-----------------------------------------------------------------------------
int whey_tank::check_on_mode( u_int mode )
    {
    tech_object::check_on_mode( mode );

    switch ( mode )
        {
        case T_WHEY_ACCEPTING:
            if ( get_mode ( T_WASH ) ||               // Во время мойки танка нельзя принимать.
                G_COMB->get_mode( my_comb::C_WASH ) ) // Во время мойки линии приёмки нельзя принимать.
                return 1;
            break;

        case T_WHEY_OUT_P1:
            if ( get_mode ( T_WASH ) ) return 1;       // Во время мойки танка нельзя выдавать.

            if ( 3 == number || 4 == number )
                {
                if ( TANK1->get_mode( T_WHEY_OUT_P2 ) ||  // Выдача сыворотки танком 1.
                    TANK2->get_mode( T_WHEY_OUT_P2 ) ||   // Выдача сыворотки танком 2.
                    TANK1->get_mode( T_WASH ) ||          // Мойка танка 1.
                    TANK2->get_mode( T_WASH ) ||          // Мойка танка 2.
                    TANK3->get_mode( T_WASH ) ||          // Мойка танка 3.
                    TANK4->get_mode( T_WASH ) ||          // Мойка танка 4.
                                                          // Приёмка т. 1 и ожидание т. 2.
                    ( TANK1->get_mode( T_WHEY_ACCEPTING ) && TANK2->get_mode( T_WHEY_WACCEPTING ) ) ||
                                                          // Приёмка т. 2 и ожидание т. 1.
                    ( TANK1->get_mode( T_WHEY_WACCEPTING ) && TANK2->get_mode( T_WHEY_ACCEPTING ) ) )
                    {
                    return 1;
                    }

                if ( TANK1->get_mode( T_WHEY_ACCEPTING ) )
                    {
                    return 1;
                    }
                }
            break;

        case T_WHEY_OUT_P2:
            if ( get_mode ( T_WASH ) ) return 1;       // Во время мойки танка нельзя выдавать.

            if ( 1 == number || 2 == number )
                {
                if ( TANK3->get_mode( T_WHEY_OUT_P1 ) ||   // Выдача сыворотки танком 3.
                    TANK4->get_mode( T_WHEY_OUT_P1 ) ||    // Выдача сыворотки танком 4.
                    TANK1->get_mode( T_WASH ) ||           // Мойка танка 1.
                    TANK2->get_mode( T_WASH ) ||           // Мойка танка 2.
                    TANK3->get_mode( T_WASH ) ||           // Мойка танка 3.
                    TANK4->get_mode( T_WASH ) ||           // Мойка танка 4.
                    ( TANK3->get_mode( T_WHEY_ACCEPTING ) &&
                    TANK4->get_mode( T_WHEY_WACCEPTING ) ) ||   // Приёмка т. 3 и ожидание т. 4.
                    ( TANK3->get_mode( T_WHEY_WACCEPTING ) &&
                    TANK4->get_mode( T_WHEY_ACCEPTING ) )       // Приёмка т. 4 и ожидание т. 3.
                    ) return 1;

                if ( TANK4->get_mode( T_WHEY_ACCEPTING ) )
                    {
                    return 1;
                    }
                }
            break;

        case T_WASH:
            if  ( get_mode ( T_WHEY_WACCEPTING ) ||
                get_mode ( T_WHEY_WOUT_P1 ) ||
                get_mode ( T_WHEY_WOUT_P2 ) ||
                get_mode ( T_WHEY_ACCEPTING ) ||
                get_mode ( T_WHEY_OUT_P1 ) ||
                get_mode ( T_WHEY_OUT_P2 ) ||
                G_COMB->get_mode( my_comb::C_WASH ) ) return 1;

            if ( TANK1->get_mode( T_WHEY_OUT_P2 ) ||
                TANK1->get_mode( T_WHEY_WOUT_P2 )||
                TANK2->get_mode( T_WHEY_OUT_P2 ) ||
                TANK2->get_mode( T_WHEY_WOUT_P2 )||
                TANK3->get_mode( T_WHEY_OUT_P1 ) ||
                TANK3->get_mode( T_WHEY_WOUT_P1 )||
                TANK4->get_mode( T_WHEY_OUT_P1 ) ||
                TANK4->get_mode( T_WHEY_WOUT_P1 ) )
                return 1;
            break;

        case T_WHEY_WOUT_P1:
            if ( get_mode ( T_WASH ) ) return 1;    // Во время мойки танка нельзя выдавать.

            if ( 3 == number || 4 == number )
                {
                if ( TANK1->get_mode( T_WHEY_OUT_P2 ) || // Выдача сыворотки танком 1.
                   TANK2->get_mode( T_WHEY_OUT_P2 ) ||   // Выдача сыворотки танком 2.
                    TANK1->get_mode( T_WASH ) ||         // Мойка танка 1.
                   TANK2->get_mode( T_WASH ) ||          // Мойка танка 2.
                   TANK3->get_mode( T_WASH ) ||          // Мойка танка 3.
                   TANK4->get_mode( T_WASH ) )           // Мойка танка 4.
                    return 1;
                }
            break;

        case T_WHEY_WOUT_P2:
            if ( get_mode ( T_WASH ) ) return 1;    // Во время мойки танка нельзя выдавать.

            if ( 1 == number || 2 == number )
                {
                if( TANK3->get_mode( T_WHEY_OUT_P1 ) || // Выдача сыворотки танком 1.
                    TANK4->get_mode( T_WHEY_OUT_P1 ) || // Выдача сыворотки танком 2.
                    TANK1->get_mode( T_WASH ) ||        // Мойка танка 1.
                    TANK2->get_mode( T_WASH ) ||        // Мойка танка 2.
                    TANK3->get_mode( T_WASH ) ||        // Мойка танка 3.
                    TANK4->get_mode( T_WASH ) )         // Мойка танка 4.
                    return 1;
                }
            break;       
        }

    //-При включени приёмки/выдачи при необходимости ставим в ожидание
    // приёмку/выдачу.
    if ( mode >= T_WHEY_ACCEPTING && mode <= T_WHEY_OUT_P2 )
        {
        int idx = G_TECH_OBJECT_MNGR->get_object_with_active_mode( mode,
            C_T1_IDX, C_T4_IDX );
        if ( idx >= 0 )
            {
            set_mode( mode + T_WHEY_WACCEPTING, 1 );
            return mode + 1000;
            }
        }
    //-При включени приёмки/выдачи при необходимости ставим в ожидание
    // приёмку/выдачу.-!>

    rt_par_float[ T_WARNING_REASON ] = 0;
    return 0;
    }
//-----------------------------------------------------------------------------
void whey_tank::init_mode( u_int mode )
    {
    tech_object::init_mode( mode );

    switch ( mode )
        {
        case T_WHEY_OUT_P1:               
            if ( 3 == number || 4 == number )
                {
                //-В зависимости от того, в какой танк идёт приёмка ( или ни
                // в какой ), определяем клапаны перехода.
                V_1 = V( 101 );
                V_2 = V( 102 );
                V_3 = V( 103 );
                V_4 = V( 104 );
                }
        break;

        case T_WHEY_OUT_P2:             
            if ( 1 == number || 2 == number )
                {
                //-В зависимости от того, в какой танк идёт приёмка ( или ни
                // в какой ), определяем клапаны перехода.
                V_1 = V( 401 );
                V_2 = V( 402 );
                V_3 = V( 403 );
                V_4 = V( 404 );
                }
            break;
        }    
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
post::post( int n ): tech_object( "TANK",
    n,  // Number.
    20, // States count.
    5,  // Timers_count.
    5,  // Params count float.
    20, // Work params count float.
    1,  // Params count u_int.
    1 ),// Work params count u_int.
        
    btnStartPrevState( 0 ),
    btnPausePrevState( 0 ),

    V1( V( number - 5 ) ),
    outTE( 6 == number ? TE( 5 ) : TE( 6 ) ),
    N1( N( number - 5 ) ),
    ctr( CTR( number - 5 ) ),
    flow( 6 == number ? FB( 5 ) : FB( 6 ) ),
        
    lampReady( 6 == number ? UPR( 1 ) : UPR( 3 ) ),
    lampWorking( 6 == number ? UPR( 2 ) : UPR( 4 ) ),
    btnStart( 6 == number ? FB( 1 ) : FB( 3 ) ),
    btnPause( 6 == number ? FB( 2 ) : FB( 4 ) )
    {
    shutDown = 1;

    timers[ TMR_DELAY_TEMP ].set_countdown_time( 2000 );
    timers[ TMR_DELAY_TEMP ].reset();

    flow->set_dt( 5000 );
    }
//-----------------------------------------------------------------------------
post::~post()
    {
    }
//-----------------------------------------------------------------------------
int post::init_params()
    {
    par_float[ S_F__LE_MIN ]         = 1;
    par_float[ S_F__T_OUT_MAX ]      = 50;
    par_float[ S_F__OUT_NORMAL ]     = 40;
    par_float[ S_F__PAUSE_TIME_MAX ] = 5;
    
    return 0;
    }
//-----------------------------------------------------------------------------
int post::evaluate()
    {
    tech_object::evaluate();

    for ( u_int i = 0; i < get_modes_count(); i++ )
        {
        if ( get_mode( i ) )
            {
            switch ( i )
                {
                case POST_WHEY_HEATING:         

                    if ( get_mode( POST_WHEY_ACCEPTING ) && flow->get_state() == 1 &&
                        outTE->get_value() <= par_float[ S_F__OUT_NORMAL ] )
                        {
                        V1->on();
                        }
                    else V1->off();

                    //-Клапан подогрева.
                    if ( get_mode( POST_WHEY_ACCEPTING ) )
                        {
                        if ( outTE->get_value() > par_float[ S_F__T_OUT_MAX ] )
                            {
                            if ( timers[ TMR_DELAY_TEMP ].get_state() )
                                {
                                if ( timers[ TMR_DELAY_TEMP ].is_time_up() )
                                    {
                                    rt_par_float[ RT_F__WARNING_REASON ] =
                                        W_MAX_OUT_TEMPER;

                                    set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
                                    timers[ TMR_DELAY_TEMP ].reset();
                                    }
                                }
                            else
                                {
                                timers[ TMR_DELAY_TEMP ].set_countdown_time( 2000 );
                                timers[ TMR_DELAY_TEMP ].reset();
                                timers[ TMR_DELAY_TEMP ].start();
                                }
                            }
                        else timers[ TMR_DELAY_TEMP ].reset();
                        }
                    //-Клапан подогрева.-!>
                    break;

                case POST_WHEY_ACCEPTING:      
                    N1->on();
                    lampReady->on();
                    lampWorking->on();

                    if ( ctr->get_quantity() >=
                        rt_par_float[ RT_F__TOT_VOL ] ) // Общее заполнение машины.
                        set_mode( CMD_RESET_POST, 1 );
                    else
                        {
                        if ( ctr->get_quantity() - prevSectVol >=
                            rt_par_float[ ( u_int ) rt_par_float[ RT_F__CURRENT_SEC ] +
                            RT_F__SECTION1 - 1 ] ) // Заполнение текущей секции.
                            {
                            prevSectVol += ( u_int ) rt_par_float[ 
                                ( u_int ) rt_par_float[ RT_F__CURRENT_SEC ] +
                                RT_F__SECTION1 - 1 ];
                            rt_par_float[ RT_F__CURRENT_SEC ] =
                                 rt_par_float[ RT_F__CURRENT_SEC ] + 1;

                            if ( rt_par_float[ RT_F__CURRENT_SEC ] <
                                rt_par_float[ RT_F__CURRENT_SEC ] + 1 ) // Есть секции.
                                {
                                set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
                                }
                            else
                                {
                                set_mode( CMD_RESET_POST, 1 );
                                }
                            }
                        }
                    break;

                case POST_WHEY_ACCEPTING_PAUSE:
                    if ( timers[ TMR_PAUSE_TIME ].is_time_up() )
                        {
                        timers[ TMR_PAUSE_TIME ].reset();
                        timers[ TMR_PAUSE_TIME ].start();
                        rt_par_float[ RT_F__WARNING_REASON ] = W_PAUSE_TIME_LEFT;
                        }

                    lampReady->on();
                    break;

                case POST_WHEY_ACCEPTING_END: 
                    break;
                }
            }
        }

    //-Start and stop buttons.
    if ( btnStart->get_state() == 1 && btnStartPrevState == 0  )
        {
#ifdef DEBUG
        Print( "Pressed start button post 1!\n" );
#endif
        if ( lampReady->get_state() == 1 )
            {
            set_mode( POST_WHEY_ACCEPTING, 1 );
            }
        btnStartPrevState = 1;
        }

    if ( btnStart->get_state() == 0 && btnStartPrevState == 1  )
        {
        btnStartPrevState = 0;
        }

    if ( btnPause->get_state() == 0 && btnPausePrevState == 1 )
        {
#ifdef DEBUG
        Print( "Pressed pause button post 1!\n" );
#endif
        if ( lampWorking->get_state() == 1 )
            {
            set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
            }
        btnPausePrevState = 0;
        }

    if ( btnPause->get_state() == 1 && btnPausePrevState == 0 )
        {
        btnPausePrevState = 1;
        }
    //-Start and stop buttons.-!>

    return 0;
    }
//-----------------------------------------------------------------------------
int post::final_mode( u_int mode )
    {
    tech_object::final_mode( mode );

    switch ( mode )
        {
        case POST_WHEY_HEATING:         // Подогрев сыворотки постом.
            V1->off();

            break;
        case POST_WHEY_ACCEPTING:       // Приёмка сыворотки постом.
            N1->off();
            lampWorking->off();
            lampReady->off();
            break;

        case POST_WHEY_ACCEPTING_PAUSE: // Пауза приёмки сыворотки постом.
            lampReady->off();
            break;

        case POST_WHEY_ACCEPTING_END:   //Завершение приёмки сыворотки постом.
            break;
        }
    return 0;
    }
//-----------------------------------------------------------------------------
void post::init_mode( u_int mode )
    {
    tech_object::init_mode( mode );

    switch ( mode )
        {
        case POST_WHEY_ACCEPTING:     
            flow->set_st_state( 1 );

            rt_par_float[ RT_F__WARNING_REASON ] = 0;
            set_mode( POST_WHEY_ACCEPTING_PAUSE, 0 );
            set_mode( POST_WHEY_ACCEPTING_END, 0 );
            break;

        case POST_WHEY_ACCEPTING_PAUSE:
            if ( rt_par_float[ RT_F__IS_RESET_POST ] == 1 )
                {
                ctr->reset();
                ctr->start();

                prevSectVol = 0;
                rt_par_float[ RT_F__CURRENT_SEC ] = 1;
                rt_par_float[ RT_F__IS_RESET_POST ] = 0;
                }

            timers[ TMR_PAUSE_TIME ].set_countdown_time(
                60UL * 1000UL * ( u_long ) par_float[ S_F__PAUSE_TIME_MAX ] );
            timers[ TMR_PAUSE_TIME ].reset();
            timers[ TMR_PAUSE_TIME ].start();

            set_mode( POST_WHEY_ACCEPTING, 0 );
            set_mode( POST_WHEY_ACCEPTING_END, 0 );
            break;
        }    
    }
//-----------------------------------------------------------------------------
int post::exec_cmd( u_int cmd )
    {
    tech_object::exec_cmd( cmd );

    switch ( cmd )
        {
        case CMD_RESET_POST:
            set_mode( POST_WHEY_ACCEPTING_PAUSE, 0 );
            set_mode( POST_WHEY_ACCEPTING, 0 );
            set_mode( POST_WHEY_HEATING, 0 );
            set_mode( POST_WHEY_ACCEPTING_END, 1 );

            int postMode1;
            int postMode2;
            if ( 6 == number )  // Пост №1.
                {
                postMode1 = T_WHEY_OUT_P1;
                postMode2 = T_WHEY_WOUT_P1;
                }
            else                // Пост №2.
                {
                postMode1 = T_WHEY_OUT_P2;
                postMode2 = T_WHEY_WOUT_P2;
                }

            TANK1->set_mode( postMode1, 0 );
            TANK1->set_mode( postMode2, 0 );
            TANK2->set_mode( postMode1, 0 );
            TANK2->set_mode( postMode2, 0 );
            TANK3->set_mode( postMode1, 0 );
            TANK3->set_mode( postMode2, 0 );
            TANK4->set_mode( postMode1, 0 );
            TANK4->set_mode( postMode2, 0 );
            break;

        case CMD_SET_PAUSE_AND_HEATING:
            set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
            set_mode( POST_WHEY_HEATING, 1 );
            break;
        }

    return 1000 + cmd;
    }
//-----------------------------------------------------------------------------
int post::check_on_mode( u_int mode )
    {
    switch ( mode )
        {
        case POST_WHEY_HEATING:
            if ( !( get_mode( POST_WHEY_ACCEPTING ) ||
                get_mode( POST_WHEY_ACCEPTING_PAUSE ) ) ) return 1;
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int my_comb::evaluate()
    {
    tech_object::evaluate();

    for ( u_int i = 0; i < get_modes_count(); i++ )
        {
        if ( get_mode( i ) )
            {
            switch ( i )
                {
                case C_WASH:
                    V( 106 )->on();
                    V( 108 )->on();
                    V( 109 )->on();

                    V( 102 )->off();
                    V( 202 )->off();
                    V( 208 )->off();
                    V( 302 )->off();
                    V( 402 )->off();
                    break;
                }
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int my_comb::final_mode( u_int mode )
    {
    tech_object::final_mode( mode );

    switch ( mode )
        {
        case C_WASH:
            V( 106 )->off();
            V( 108 )->off();
            V( 109 )->off();
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int my_comb::check_on_mode( int mode )
    {
    switch ( mode )
    {
    case C_WASH:
        if ( TANK1->get_mode( T_WHEY_ACCEPTING ) || // Во время приёмки нельзя мыть.
            TANK2->get_mode( T_WHEY_ACCEPTING ) ||
            TANK3->get_mode( T_WHEY_ACCEPTING ) ||
            TANK4->get_mode( T_WHEY_ACCEPTING ) ||

            TANK1->get_mode( T_WASH ) ||             // Мойка танка 1.
            TANK2->get_mode( T_WASH ) ||             // Мойка танка 2.
            TANK3->get_mode( T_WASH ) ||             // Мойка танка 3.
            TANK4->get_mode( T_WASH ) )              // Мойка танка 4.
            return 1;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
my_comb::my_comb( int states_count, int params_count, int rt_param_count,
    int timers_count ): tech_object( "COMB", 1, states_count, timers_count,
    params_count, rt_param_count, 1, 1 )
    {
    }
//-----------------------------------------------------------------------------
my_comb::~my_comb()
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int evaluate_all()
    {
    G_TECH_OBJECT_MNGR->evaluate();

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
