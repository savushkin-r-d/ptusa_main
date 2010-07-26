#include "priem.h"

dev_stub *G_DEV_STUB = new dev_stub;
//-----------------------------------------------------------------------------
whey_tank::whey_tank( int par_count, int n, int tmr_cnt ): tech_object( "TANK",
     n, 20, tmr_cnt, par_count, 10, 1, 1 ),
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
whey_tank::~whey_tank( )
    {
    }
//-----------------------------------------------------------------------------
int whey_tank::set_mode( u_int mode, int newm )
    {
    int res = 0;
    int idx;

#ifdef DEBUG
    Print ("Set tank n=%d mode = %d, %d\n", number, mode, newm );
#ifdef STOP
    //  Print ("Press any key to continue...\n" );
    //  Getch( );
#endif
#endif

    if ( mode > 63 ) return 1;
    if ( get_mode( mode ) == newm ) return 0;
    if ( newm == 0 )
        {  //off mode
        state[ mode / 32 ] = state[ mode / 32 ] & ( ~1UL << mode );
        final_mode( mode );
        res = mode + 2000;

        //-При отключении приёмки/выдачи включаем при наличии ожидающие приёмку/выдачу.
        if ( mode >= T_WHEY_ACCEPTING && mode <= T_WHEY_OUT_P2 )
            {
            idx = G_TECH_OBJECT_MNGR->get_object_with_active_mode( mode + T_WHEY_WACCEPTING,
                C_TANK_1_INDEX, C_TANK_4_INDEX );

            if ( idx >= 0 )
                {
                G_TECH_OBJECTS( idx )->set_mode( mode + T_WHEY_WACCEPTING, 0 );
                G_TECH_OBJECTS( idx )->set_mode( mode, 1 );
                if ( T_WHEY_ACCEPTING == mode )
                    G_TECH_OBJECTS( idx )->rt_par_float[ T_WARNING_REASON ] = tankInStart;
                }
            else
                {
                if ( T_WHEY_ACCEPTING == mode )
                    rt_par_float[ T_WARNING_REASON ] = tankInStop;
                }
            }
        //-При отключении приёмки/выдачи включаем при наличии ожидающие приёмку/выдачу-!>
        }
    else
        {  //try to set
        //-При включени приёмки/выдачи при необходимости ставим в ожидание приёмку/выдачу.
        if ( mode >= T_WHEY_ACCEPTING && mode <= T_WHEY_OUT_P2 )
            {
            idx = G_TECH_OBJECT_MNGR->get_object_with_active_mode( mode,
                C_TANK_1_INDEX, C_TANK_4_INDEX );
            if ( idx >= 0 )
                {
                set_mode( mode + T_WHEY_WACCEPTING, 1 );
                return mode + 1000;
                }
            }
        //-При включени приёмки/выдачи при необходимости ставим в ожидание приёмку/выдачу.-!>

        res = init_mode( mode );
        if ( res == 0 )
            {
            state[ mode / 32 ] = state[ mode / 32 ] | 1UL << mode;
            return mode + 1000;
            }

        if ( res == 2 ) return mode + 1000;
        else return 0;
        }

    return res;
    }
//-----------------------------------------------------------------------------
int whey_tank::evaluate( )
    {
    tech_object::evaluate( );

    int i;
    for ( i = 0; i < 32; i++ )
        {
        if ( get_mode( i ) )
            {
            switch ( i )
                {
                case T_WHEY_ACCEPTING:            //Приёмка сыворотки
                    V1->on( );
                    V2->on( );
                    V4->off( );
                    V( 106 )->off( );

                    if ( LSH->get_state( ) == LIS ) set_mode( T_WHEY_ACCEPTING, 0 );

                    break;
                case T_WHEY_OUT_P1:               //Выдача сыворотки пост 1

                    V1->on( );
                    //        V2->off( );

                    if ( number == 1 || number == 2 )
                        {
                        V3->off( );
                        if ( G_TECH_OBJECTS( C_POST_1_INDEX )->get_mode( POST_WHEY_ACCEPTING ) ) V4->on( );
                        else V4->off( );
                        }
                    else
                        {
                        V3->on( );
                        V4->off( );

                        V( 107 )->off( );
                        V( 207 )->on( );
                        V( 206 )->off( );
                        V( 307 )->on( );
                        V( 507 )->off( );

                        V_1->off( );
                        V_2->off( );
                        V_3->on( );
                        if ( G_TECH_OBJECTS( C_POST_1_INDEX )->get_mode( POST_WHEY_ACCEPTING ) ) V_4->on( );
                        else V_4->off( );
                        }

                    if ( ( LSL->get_state( ) == LNO ) ||
                        ( G_TECH_OBJECTS( C_POST_1_INDEX )->get_mode( POST_WHEY_ACCEPTING ) &&
                        ( ( post* ) G_TECH_OBJECTS( C_POST_1_INDEX ) )->flow->get_state( ) == 0 ) )
                        {
                        int idx = G_TECH_OBJECT_MNGR->get_object_with_active_mode( T_WHEY_WOUT_P1,
                            C_TANK_1_INDEX, C_TANK_4_INDEX );
                
                        if ( idx >= 0 &&
                            LE1->get_value( ) < G_TECH_OBJECTS( C_POST_1_INDEX )->rt_par_float[ post::T_LE_MIN ] )
                            set_mode( T_WHEY_OUT_P1, 0 );
                        else
                            {
                            G_TECH_OBJECTS( C_POST_1_INDEX )->set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
                            G_TECH_OBJECTS( C_POST_1_INDEX )->rt_par_float[ post::P_WARNING_REASON ] = post::noFlowPost;
                            }
                        }
                    break;
                case T_WHEY_OUT_P2:               //Выдача сыворотки пост 2

                    V1->on( );
                    //V2->off( );

                    if ( number == 3 || number == 4 )
                        {
                        V3->off( );
                        if ( G_TECH_OBJECTS( C_POST_2_INDEX )->get_mode( POST_WHEY_ACCEPTING ) ) V4->on( );
                        else V4->off( );
                        }
                    else
                        {
                        V3->on( );
                        V4->off( );

                        V( 107 )->off( );
                        V( 207 )->on( );
                        V( 206 )->off( );
                        V( 307 )->on( );
                        V( 507 )->off( );

                        V_1->off( );
                        V_2->off( );
                        V_3->on( );

                        if ( G_TECH_OBJECTS( C_POST_2_INDEX )->get_mode( POST_WHEY_ACCEPTING ) ) V_4->on( );
                        else V_4->off( );
                        }

                    if ( ( LSL->get_state( ) == LNO ) ||
                        ( G_TECH_OBJECTS( C_POST_2_INDEX )->get_mode( POST_WHEY_ACCEPTING ) &&
                        ( (post*) G_TECH_OBJECTS( C_POST_2_INDEX ) )->flow->get_state( ) == 0 ) )
                        {
                        int idx = G_TECH_OBJECT_MNGR->get_object_with_active_mode( T_WHEY_WOUT_P2,
                            C_TANK_1_INDEX, C_TANK_4_INDEX );
                        
                        if ( idx >= 0 ) set_mode( T_WHEY_OUT_P2, 0 );
                        else
                            {
                            G_TECH_OBJECTS( C_POST_2_INDEX )->set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
                            G_TECH_OBJECTS( C_POST_2_INDEX )->rt_par_float[ post::P_WARNING_REASON ] =
                                post::noFlowPost;
                            }
                        }

                    break;
                case T_WASH:                      //Мойка
                    V1->on( );
                    V2->off( );
                    V3->on( );
                    V4->off( );
                    V5->on( );
                    V7_1->off( );
                    V7_2->on( );
                    V( 108 )->off( );
                    V( 206 )->on( );
                    V( 208 )->on( );
                    V( 109 )->on( );
                    N( 2202 )->on( );

                    break;
                case T_WHEY_WACCEPTING:           //Ожидание приёмки сыворотки
                    break;
                case T_WHEY_WOUT_P1:              //Ожидание выдачи сыворотки пост 1
                    break;
                case T_WHEY_WOUT_P2:              //Ожидание выдачи сыворотки пост 2
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

#ifdef DEBUG
    Print( "Final mode %d\n\r", mode );
#endif

    switch ( mode )
        {
        case T_WHEY_ACCEPTING:            //Приёмка сыворотки.
            V1->off( );
            V2->off( );

            break;
        case T_WHEY_OUT_P1:               //Выдача сыворотки пост 1
            V1->off( );

            if ( number == 1 || number == 2 ) V4->off( );
            else
                {
                V3->off( );
                V( 207 )->off( );
                V( 307 )->off( );
                V_3->off( );
                V_4->off( );
                }

            break;
        case T_WHEY_OUT_P2:               //Выдача сыворотки пост 2
            V1->off( );

            if ( number == 3 || number == 4 ) V4->off( );
            else
                {
                V3->off( );

                V( 207 )->off( );
                V( 307 )->off( );

                V_3->off( );
                V_4->off( );
                }

            break;
        case T_WASH:                      //Мойка
            V1->off( );
            V3->off( );
            V5->off( );

            V7_2->off( );
            V( 206 )->off( );
            V( 208 )->off( );
            V( 109 )->off( );
            N( 2202 )->off( );

            break;
        case T_WHEY_WACCEPTING:           //Ожидание приёмки сыворотки
            break;
        case T_WHEY_WOUT_P1:              //Ожидание выдачи сыворотки пост 1
            break;
        case T_WHEY_WOUT_P2:              //Ожидание выдачи сыворотки пост 2
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int whey_tank::init_mode( u_int mode )
    {
    tech_object::init_mode( mode );

#ifdef DEBUG
    Print( "Init mode %d\n\r", mode );
#endif

    int idx = 0;

    switch ( mode )
        {
        case T_WHEY_ACCEPTING:            //Приёмка сыворотки:

            if ( get_mode ( T_WASH ) ||                //Во время мойки танка нельзя принимать.
                G_TECH_OBJECTS( C_COMB_INDEX )->get_mode( my_comb::C_WASH ) ) return 1;     //Во время мойки линии приёмки нельзя принимать.

            break;
        case T_WHEY_OUT_P1:               //Выдача сыворотки пост 1

            if ( get_mode ( T_WASH ) ) return 1;       //Во время мойки танка нельзя выдавать.

            if ( number == 3 || number == 4 )
                {
                if ( G_TECH_OBJECTS( C_TANK_1_INDEX )->get_mode( T_WHEY_OUT_P2 ) || //Выдача сыворотки танком 1
                   G_TECH_OBJECTS( C_TANK_2_INDEX )->get_mode( T_WHEY_OUT_P2 ) ||    //Выдача сыворотки танком 2
                    G_TECH_OBJECTS( C_TANK_1_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 1
                   G_TECH_OBJECTS( C_TANK_2_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 2
                   G_TECH_OBJECTS( C_TANK_3_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 3
                   G_TECH_OBJECTS( C_TANK_4_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 4
                    ( G_TECH_OBJECTS( C_TANK_1_INDEX )->get_mode( T_WHEY_ACCEPTING ) &&
                   G_TECH_OBJECTS( C_TANK_2_INDEX )->get_mode( T_WHEY_WACCEPTING ) ) || //Приёмка т. 1 и ожидание т. 2
                    ( G_TECH_OBJECTS( C_TANK_1_INDEX )->get_mode( T_WHEY_WACCEPTING ) &&
                   G_TECH_OBJECTS( C_TANK_2_INDEX )->get_mode( T_WHEY_ACCEPTING ) ) //Приёмка т. 2 и ожидание т. 1
                    ) return 1;

                //-В зависимости от того, в какой танк идёт приёмка ( или ни в какой ), определяем клапаны перехода.
                V_1 = V( 101 );
                V_2 = V( 102 );
                V_3 = V( 103 );
                V_4 = V( 104 );

                if ( G_TECH_OBJECTS( C_TANK_1_INDEX )->get_mode( T_WHEY_ACCEPTING ) )
                    {
                    return 1;
                    //V_1 = V( 201 );
                    //V_2 = V( 202 );
                    //V_3 = V( 203 );
                    //V_4 = V( 204 );
                    }
                //-В зависимости от того, в какой танк идёт приёмка ( или ни в какой ), определяем клапаны перехода.-!>
                }

            //( (post*) &G_TECH_OBJECTS[ C_POST_1_INDEX ] )->flow->set_state( 1 );
            break;
        case T_WHEY_OUT_P2:               //Выдача сыворотки пост 2

            if ( get_mode ( T_WASH ) ) return 1;       //Во время мойки танка нельзя выдавать.

            if ( number == 1 || number == 2 )
                {
                if (G_TECH_OBJECTS( C_TANK_3_INDEX )->get_mode( T_WHEY_OUT_P1 ) || //Выдача сыворотки танком 3
                   G_TECH_OBJECTS( C_TANK_4_INDEX )->get_mode( T_WHEY_OUT_P1 ) ||    //Выдача сыворотки танком 4
                    G_TECH_OBJECTS( C_TANK_1_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 1
                   G_TECH_OBJECTS( C_TANK_2_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 2
                   G_TECH_OBJECTS( C_TANK_3_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 3
                   G_TECH_OBJECTS( C_TANK_4_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 4
                    (G_TECH_OBJECTS( C_TANK_3_INDEX )->get_mode( T_WHEY_ACCEPTING ) &&
                   G_TECH_OBJECTS( C_TANK_4_INDEX )->get_mode( T_WHEY_WACCEPTING ) ) || //Приёмка т. 3 и ожидание т. 4
                    (G_TECH_OBJECTS( C_TANK_3_INDEX )->get_mode( T_WHEY_WACCEPTING ) &&
                   G_TECH_OBJECTS( C_TANK_4_INDEX )->get_mode( T_WHEY_ACCEPTING ) ) //Приёмка т. 4 и ожидание т. 3
                    ) return 1;

                //-В зависимости от того, в какой танк идёт приёмка ( или ни в какой ), определяем клапаны перехода.
                V_1 = V( 401 );
                V_2 = V( 402 );
                V_3 = V( 403 );
                V_4 = V( 404 );

                if (G_TECH_OBJECTS( C_TANK_4_INDEX )->get_mode( T_WHEY_ACCEPTING ) )
                    {
                    return 1;
                    //V_1 = V( 301 );
                    //V_2 = V( 302 );
                    //V_3 = V( 303 );
                    //V_4 = V( 304 );
                    }
                //-В зависимости от того, в какой танк идёт приёмка ( или ни в какой ), определяем клапаны перехода.-!>
                }

            //( (post*) G_TECH_OBJECTS( C_POST_2_INDEX ) )->flow->set_state( 1 );
            break;
        case T_WASH:                      //Мойка
            if  ( get_mode ( T_WHEY_WACCEPTING ) ||
                get_mode ( T_WHEY_WOUT_P1 ) ||
                get_mode ( T_WHEY_WOUT_P2 ) ||
                get_mode ( T_WHEY_ACCEPTING ) ||
                get_mode ( T_WHEY_OUT_P1 ) ||
                get_mode ( T_WHEY_OUT_P2 ) ||
                G_TECH_OBJECTS( C_COMB_INDEX )->get_mode( my_comb::C_WASH ) ) return 1;

            if ( G_TECH_OBJECTS( C_TANK_1_INDEX )->get_mode( T_WHEY_OUT_P2 ) ||
                G_TECH_OBJECTS( C_TANK_1_INDEX )->get_mode( T_WHEY_WOUT_P2 )||
               G_TECH_OBJECTS( C_TANK_2_INDEX )->get_mode( T_WHEY_OUT_P2 ) ||
               G_TECH_OBJECTS( C_TANK_2_INDEX )->get_mode( T_WHEY_WOUT_P2 )||
               G_TECH_OBJECTS( C_TANK_3_INDEX )->get_mode( T_WHEY_OUT_P1 ) ||
               G_TECH_OBJECTS( C_TANK_3_INDEX )->get_mode( T_WHEY_WOUT_P1 )||
               G_TECH_OBJECTS( C_TANK_4_INDEX )->get_mode( T_WHEY_OUT_P1 ) ||
               G_TECH_OBJECTS( C_TANK_4_INDEX )->get_mode( T_WHEY_WOUT_P1 ) )
                return 1;
            break;

        case T_WHEY_WACCEPTING:           //Ожидание приёмки сыворотки
            break;

        case T_WHEY_WOUT_P1:              //Ожидание выдачи сыворотки пост 1
            if ( get_mode ( T_WASH ) ) return 1;       //Во время мойки танка нельзя выдавать.

            if ( number == 3 || number == 4 )
                {
                if ( G_TECH_OBJECTS( C_TANK_1_INDEX )->get_mode( T_WHEY_OUT_P2 ) || //Выдача сыворотки танком 1
                   G_TECH_OBJECTS( C_TANK_2_INDEX )->get_mode( T_WHEY_OUT_P2 ) ||    //Выдача сыворотки танком 2
                    G_TECH_OBJECTS( C_TANK_1_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 1
                   G_TECH_OBJECTS( C_TANK_2_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 2
                   G_TECH_OBJECTS( C_TANK_3_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 3
                   G_TECH_OBJECTS( C_TANK_4_INDEX )->get_mode( T_WASH ) )            //Мойка танка 4
                    return 1;
                }
            break;

        case T_WHEY_WOUT_P2:              //Ожидание выдачи сыворотки пост 2
            if ( get_mode ( T_WASH ) ) return 1;       //Во время мойки танка нельзя выдавать.

            if ( number == 1 || number == 2 )
                {
                if (G_TECH_OBJECTS( C_TANK_3_INDEX )->get_mode( T_WHEY_OUT_P1 ) || //Выдача сыворотки танком 1
                   G_TECH_OBJECTS( C_TANK_4_INDEX )->get_mode( T_WHEY_OUT_P1 ) ||    //Выдача сыворотки танком 2
                    G_TECH_OBJECTS( C_TANK_1_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 1
                   G_TECH_OBJECTS( C_TANK_2_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 2
                   G_TECH_OBJECTS( C_TANK_3_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 3
                   G_TECH_OBJECTS( C_TANK_4_INDEX )->get_mode( T_WASH ) )            //Мойка танка 4
                    return 1;
                }
            break;

        case RESET_TANK_POST1:            //Выключает режим выдачи для танка и, если надо, поста 1
            idx = G_TECH_OBJECT_MNGR->get_object_with_active_mode( T_WHEY_WOUT_P1,
                C_TANK_1_INDEX, C_TANK_4_INDEX );

            if ( idx == -1 ) G_TECH_OBJECTS( C_POST_1_INDEX )->set_mode( RESET_POST, 1 ); //Сбрасываем пост 1.
            set_mode( T_WHEY_OUT_P1, 0 );

            return 2;

        case RESET_TANK_POST2:            //Выключает режим выдачи для танка и, если надо, поста 2
            idx = G_TECH_OBJECT_MNGR->get_object_with_active_mode( T_WHEY_WOUT_P2,
                C_TANK_1_INDEX, C_TANK_4_INDEX );

            if ( idx == -1 ) G_TECH_OBJECTS( C_POST_2_INDEX )->set_mode( RESET_POST, 1 ); //Сбрасываем пост 2.
            set_mode( T_WHEY_OUT_P2, 0 );

            return 2;
        case SET_POST1_AND_TANK:          //Включить танк и пост 1
            if ( get_mode ( T_WASH ) ||              //Во время мойки танка нельзя.
                //         get_mode( T_WHEY_ACCEPTING ) ||
                //         get_mode( T_WHEY_WACCEPTING ) ||
                get_mode( T_WHEY_OUT_P2 ) ||
                get_mode( T_WHEY_WOUT_P2 ) ) return 1;

            if ( set_mode( T_WHEY_OUT_P1, 1 ) == 1000 + T_WHEY_OUT_P1 )
                G_TECH_OBJECTS( C_POST_1_INDEX )->set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
            return 2;

        case SET_POST2_AND_TANK:          //Включить танк и пост 2
            if ( get_mode ( T_WASH ) ||              //Во время мойки танка нельзя.
                //         get_mode( T_WHEY_ACCEPTING ) ||
                //         get_mode( T_WHEY_WACCEPTING ) ||
                get_mode( T_WHEY_OUT_P1 ) ||
                get_mode( T_WHEY_WOUT_P1 ) ) return 1;

            if ( set_mode( T_WHEY_OUT_P2, 1 ) == 1000 + T_WHEY_OUT_P2 )
                G_TECH_OBJECTS( C_POST_2_INDEX )->set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
            return 2;

        case SET_HEATING_POST1_AND_TANK:  //Включить танк и подогрев пост 1
            set_mode( SET_POST1_AND_TANK, 1 );
            G_TECH_OBJECTS( C_POST_1_INDEX )->set_mode( POST_WHEY_HEATING, 1 );
            return 2;

        case SET_HEATING_POST2_AND_TANK:  //Включить танк и подогрев пост 2
            set_mode( SET_POST2_AND_TANK, 1 );
            G_TECH_OBJECTS( C_POST_2_INDEX )->set_mode( POST_WHEY_HEATING, 1 );
            return 2;
        }

    rt_par_float[ T_WARNING_REASON ] = 0;
    return 0;
    }
//-----------------------------------------------------------------------------
post::post( int par_count, int n, 
    int tmr_cnt ): tech_object( "TANK", n, 32, tmr_cnt, par_count, 23, 1, 1 ),
    V1 ( G_DEV_STUB ),
    btnStartPrevState( 0 ),
    btnPausePrevState( 0 )    
    {
    shutDown = 1;

    timers[ delayTempTmr ].set_countdown_time( 2000 );
    timers[ delayTempTmr ].reset();
    }
//-----------------------------------------------------------------------------
post::~post( )
    {
    }
//-----------------------------------------------------------------------------
int post::init_params( )
    {
    par_float[ T_OUT_MAX ] = 50;
    par_float[ post::T_OUT_NORMAL ] = 40;
    par_float[ post::T_LE_MIN ] = 1;
    par_float[ post::PAUSE_TIME_MAX ] = 5;
    par_float.save_all();

    return 0;
    }
//-----------------------------------------------------------------------------
int post::evaluate()
    {
    tech_object::evaluate( );

    int i;
    for ( i = 0; i < 32; i++ )
        {
        if ( get_mode( i ) )
            {
            switch ( i )
                {
                case POST_WHEY_HEATING:         //Подогрев сыворотки постом

                    if ( get_mode( POST_WHEY_ACCEPTING ) && flow->get_state() == 1 &&
                        outTE->get_value() <= par_float[ T_OUT_NORMAL ] )
                        {
                        V1->on( );
                        }
                    else V1->off( );

                    //-Клапан подогрева.
                    if ( get_mode( POST_WHEY_ACCEPTING ) )
                        {
                        if ( outTE->get_value() > par_float[ T_OUT_MAX ] )
                            {
                            if ( timers[ delayTempTmr ].get_state() )
                                {
                                if ( timers[ delayTempTmr ].is_time_up() )
                                    {
                                    rt_par_float[ P_WARNING_REASON ] = maxTPost;

                                    set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
                                    timers[ delayTempTmr ].reset();
                                    }
                                }
                            else
                                {
                                timers[ delayTempTmr ].set_countdown_time( 2000 );
                                timers[ delayTempTmr ].reset();
                                timers[ delayTempTmr ].start();
                                }
                            }
                        else timers[ delayTempTmr ].reset();
                        }
                    //-Клапан подогрева.-!>
                    break;

                case POST_WHEY_ACCEPTING:       //Приёмка сыворотки постом
                    N1->on( );
                    lampReady->on( );
                    lampWorking->on( );

                    if ( ctr->get_quantity() >= par_float[ TOT_VOL ] )/*Общее заполнение машины*/
                        set_mode( RESET_POST, 1 );
                    else
                        {
                        if ( ctr->get_quantity() - prevSectVol >=
                            rt_par_float[ ( u_int ) rt_par_float[ CURRENT_SEC ] + SECTION1 - 1 ] ) //Заполнение текущей секции.
                            {
                            prevSectVol += ( u_int ) rt_par_float[ ( u_int ) rt_par_float[ CURRENT_SEC ] + SECTION1 - 1 ];
                            rt_par_float[ CURRENT_SEC ] = rt_par_float[ CURRENT_SEC ] + 1;

                            if ( rt_par_float[ CURRENT_SEC ] < rt_par_float[ CURRENT_SEC ] + 1 )/*Есть секции*/
                                set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
                            else set_mode( RESET_POST, 1 );
                            }
                        }
                    break;

                case POST_WHEY_ACCEPTING_PAUSE: //Пауза приёмки сыворотки постом
                    if ( timers[ pauseTimeTmr ].is_time_up() )
                        {
                        timers[ pauseTimeTmr ].reset();
                        timers[ pauseTimeTmr ].start();
                        rt_par_float[ P_WARNING_REASON ] = pauseTimeLeft;
                        }

                    lampReady->on( );
                    break;

                case POST_WHEY_ACCEPTING_END: //Завершение приёмки сыворотки постом
                    break;
                }
            }
        }

    //-Start and stop buttons.
    if ( btnStart->get_state( ) == 1 && btnStartPrevState == 0  )
        {
#ifdef DEBUG
        Print("Pressed start button post 1!\n\r");
#endif
        if ( lampReady->get_state( ) == 1 ) set_mode( POST_WHEY_ACCEPTING, 1 );
        btnStartPrevState = 1;
        }

    if ( btnStart->get_state( ) == 0 && btnStartPrevState == 1  )
        {
        btnStartPrevState = 0;
        }

    if ( btnPause->get_state( ) == 0 && btnPausePrevState == 1 )
        {
#ifdef DEBUG
        Print("Pressed pause button post 1!\n\r");
#endif
        if ( lampWorking->get_state( ) == 1 ) set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
        btnPausePrevState = 0;
        }

    if ( btnPause->get_state( ) == 1 && btnPausePrevState == 0 )
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

#ifdef DEBUG
    Print( "Final mode %d\n\r", mode );
#endif

    switch ( mode )
        {
        case POST_WHEY_HEATING:         //Подогрев сыворотки постом
            V1->off( );

            break;
        case POST_WHEY_ACCEPTING:       //Приёмка сыворотки постом
            N1->off( );
            lampWorking->off( );
            lampReady->off( );
            break;

        case POST_WHEY_ACCEPTING_PAUSE: //Пауза приёмки сыворотки постом
            lampReady->off( );
            break;

        case POST_WHEY_ACCEPTING_END: //Завершение приёмки сыворотки постом
            break;
        }
    return 0;
    }
//-----------------------------------------------------------------------------
int post::init_mode( u_int mode )
    {
    tech_object::init_mode( mode );

#ifdef DEBUG
    Print( "TPost init mode %d\n\r", mode );
#endif

    switch ( mode )
        {
        case POST_WHEY_HEATING:         //Подогрев сыворотки постом
            if ( !( get_mode( POST_WHEY_ACCEPTING ) ||
                get_mode( POST_WHEY_ACCEPTING_PAUSE ) ) ) return 1;
            break;

        case POST_WHEY_ACCEPTING:       //Приёмка сыворотки постом
            //flow->set_state( 1 );

            rt_par_float[ P_WARNING_REASON ] = 0;
            set_mode( POST_WHEY_ACCEPTING_PAUSE, 0 );
            set_mode( POST_WHEY_ACCEPTING_END, 0 );
            break;

        case POST_WHEY_ACCEPTING_PAUSE: //Пауза приёмки сыворотки постом
            if ( rt_par_float[ IS_RESET_POST ] == 1 )
                {
                ctr->reset();
                ctr->start();

                prevSectVol = 0;
                rt_par_float[ CURRENT_SEC ] = 1;
                rt_par_float[ IS_RESET_POST ] = 0;
                }

            timers[ pauseTimeTmr ].set_countdown_time(
                60UL * 1000UL * ( u_long ) par_float[ PAUSE_TIME_MAX ] );
            timers[ pauseTimeTmr ].reset();
            timers[ pauseTimeTmr ].start();

            set_mode( POST_WHEY_ACCEPTING, 0 );
            set_mode( POST_WHEY_ACCEPTING_END, 0 );
            break;

        case POST_WHEY_ACCEPTING_END: //Завершение приёмки сыворотки постом
            break;

        case RESET_POST:             //Выключает пауза/работу, подогрев и включает режим выдача завершена
            set_mode( POST_WHEY_ACCEPTING_PAUSE, 0 );
            set_mode( POST_WHEY_ACCEPTING, 0 );
            set_mode( POST_WHEY_HEATING, 0 );
            set_mode( POST_WHEY_ACCEPTING_END, 1 );

            int postMode1, postMode2;
            if ( number == 6 ) //Пост №1
                {
                postMode1 = T_WHEY_OUT_P1;
                postMode2 = T_WHEY_WOUT_P1;
                }
            else           //Пост №2
                {
                postMode1 = T_WHEY_OUT_P2;
                postMode2 = T_WHEY_WOUT_P2;
                }

            G_TECH_OBJECTS( C_TANK_1_INDEX )->set_mode( postMode1, 0 );
            G_TECH_OBJECTS( C_TANK_1_INDEX )->set_mode( postMode2, 0 );
           G_TECH_OBJECTS( C_TANK_2_INDEX )->set_mode( postMode1, 0 );
           G_TECH_OBJECTS( C_TANK_2_INDEX )->set_mode( postMode2, 0 );
           G_TECH_OBJECTS( C_TANK_3_INDEX )->set_mode( postMode1, 0 );
           G_TECH_OBJECTS( C_TANK_3_INDEX )->set_mode( postMode2, 0 );
           G_TECH_OBJECTS( C_TANK_4_INDEX )->set_mode( postMode1, 0 );
           G_TECH_OBJECTS( C_TANK_4_INDEX )->set_mode( postMode2, 0 );
            return 1;

        case SET_PAUSE_AND_HEATING:       //Включает пауза и подогрев
            set_mode( POST_WHEY_ACCEPTING_PAUSE, 1 );
            set_mode( POST_WHEY_HEATING, 1 );

            return 1;
        }
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int my_comb::evaluate()
    {
    tech_object::evaluate( );

    return 0;
    }
//-----------------------------------------------------------------------------
int my_comb::final_mode( u_int mode )
    {
    tech_object::final_mode( mode );

    return 0;
    }
//-----------------------------------------------------------------------------
int my_comb::init_mode( u_int mode )
    {
#ifdef DEBUG
    Print( "Init comb mode = %d", mode );
#endif

    switch ( mode )
        {
        case C_WASH:     //
            if ( G_TECH_OBJECTS( C_TANK_1_INDEX )->get_mode ( T_WHEY_ACCEPTING ) ||  //Во время приёмки нельзя мыть.
               G_TECH_OBJECTS( C_TANK_2_INDEX )->get_mode ( T_WHEY_ACCEPTING ) ||
               G_TECH_OBJECTS( C_TANK_3_INDEX )->get_mode ( T_WHEY_ACCEPTING ) ||
               G_TECH_OBJECTS( C_TANK_4_INDEX )->get_mode ( T_WHEY_ACCEPTING ) ||
                    
                G_TECH_OBJECTS( C_TANK_1_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 1
               G_TECH_OBJECTS( C_TANK_2_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 2
               G_TECH_OBJECTS( C_TANK_3_INDEX )->get_mode( T_WASH ) ||           //Мойка танка 3
               G_TECH_OBJECTS( C_TANK_4_INDEX )->get_mode( T_WASH ) )            //Мойка танка 4
                return 1;
            break;
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
my_comb::~my_comb( )
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

