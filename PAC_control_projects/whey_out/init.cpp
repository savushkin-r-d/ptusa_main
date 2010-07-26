#include "init.h"
#include "priem.h"

int G_PROJECT_ID = 69;
//-----------------------------------------------------------------------------
int init_tech_process()
    {
    params_manager::get_instance()->init( G_PROJECT_ID );

    G_TECH_OBJECT_MNGR->add_tech_object( new my_comb( 32, 20, 10, 5 ) );

    for ( u_int i = 0; i < 4; i++ )
        {
        whey_tank *t = new whey_tank( 10, i + 1, 5 );
        G_TECH_OBJECT_MNGR->add_tech_object( t );
        }

    G_TECH_OBJECT_MNGR->add_tech_object( new post( 10, 6, 5 ) );
    G_TECH_OBJECT_MNGR->add_tech_object( new post( 10, 7, 5 ) );

    for ( u_int i = 0; i < G_TECH_OBJECT_MNGR->get_count(); i++ )
        {
        G_TECH_OBJECTS( i )->init_runtime_params();

        G_DEVICE_CMMCTR->add_device( G_TECH_OBJECTS( i )->get_complex_dev() );
        }

    post *t2;
    //-Init post 1.
    t2 = ( post* ) G_TECH_OBJECTS( C_POST_1_INDEX );

//    t2->lampReady = UPR( 1 );
//    t2->lampWorking = UPR( 2 );
//    t2->btnStart = FB( 1 );
//    t2->btnPause = FB( 2 );
//    t2->flow = FB( POST1_FLOW );
////    t2->flow->InitTmr( );
//
//    t2->V1 = V( 1 );
//    t2->N1 = N( 1 );
//    t2->ctr = CTR( 1 );
//    t2->outTE = TE( 5 );
//    //-Init post 1.-!>
//
//    //-Init post 2.
//    t2 = ( post* ) G_TECH_OBJECTS( C_POST_2_INDEX );
//l
//    t2->lampReady = UPR( 3 );
//    t2->lampWorking = UPR( 4 );
//    t2->btnStart = FB( 3 );
//    t2->btnPause = FB( 4 );
//    t2->flow = FB( POST2_FLOW );
////    t2->flow->InitTmr( );
//
//    t2->V1 = V( 2 );
//    t2->N1 = N( 2 );
//    t2->ctr = CTR( 2 );
//    t2->outTE = TE( 6 );
    //-Init post 2.-!>

    params_manager::get_instance()->final_init();
    //TParams::FinalInit( );

//    g_device_state_manager = new active_dev_manager( 1 + TTank::TankCnt );
//    g_device_state_manager->dev[ 0 ] = g_combs[ 0 ]->active_dev;
//    for ( i = 0; i < TTank::TankCnt; i++ )
//        {
//        g_device_state_manager->dev[ 1 + i ] = g_tanks[ i ]->active_dev;
//        }
    return 0;
    }
