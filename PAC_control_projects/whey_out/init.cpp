#include "init.h"
#include "priem.h"

const int G_PROJECT_ID = 69;
//-----------------------------------------------------------------------------
int init_tech_process()
    {
    params_manager::get_instance()->init( G_PROJECT_ID );

    G_TECH_OBJECT_MNGR()->add_tech_object( new my_comb( 32, 20, 10, 5 ) );

    for ( u_int i = 0; i < 4; i++ )
        {
        G_TECH_OBJECT_MNGR()->add_tech_object( new whey_tank( i + 1 ) );
        }

    G_TECH_OBJECT_MNGR()->add_tech_object( new post( 6 ) );
    G_TECH_OBJECT_MNGR()->add_tech_object( new post( 7 ) );

    for ( u_int i = 0; i < G_TECH_OBJECT_MNGR()->get_count(); i++ )
        {
        G_TECH_OBJECTS( i )->init_runtime_params();

        G_DEVICE_CMMCTR->add_device( G_TECH_OBJECTS( i )->get_complex_dev() );
        }

    params_manager::get_instance()->final_init();

//    g_device_state_manager = new active_dev_manager( 1 + TTank::TankCnt );
//    g_device_state_manager->dev[ 0 ] = g_combs[ 0 ]->active_dev;
//    for ( i = 0; i < TTank::TankCnt; i++ )
//        {
//        g_device_state_manager->dev[ 1 + i ] = g_tanks[ i ]->active_dev;
//        }
    return 0;
    }
