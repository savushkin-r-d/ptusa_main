#include "tech_def.h"
#include "string.h"
//-----------------------------------------------------------------------------
tech_object_manager* tech_object_manager::instance;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
tech_object::tech_object( const char* name, u_int number, u_int states_count,
    u_int timers_count,
    u_int par_float_count, u_int runtime_par_float_count,
    u_int par_uint_count,
    u_int runtime_par_uint_count) : states_count( states_count ),
        timers( timers_count ),        
        mode_time( new run_time_params_u_int_4( states_count, "MODE_TIME" ) ),
        par_float( new saved_params_float( par_float_count ) ),
        rt_par_float( new run_time_params_float( runtime_par_float_count ) ),
        par_uint( new saved_params_u_int_4( par_uint_count ) ),
        rt_par_uint( new run_time_params_u_int_4( runtime_par_uint_count ) )
    {
    u_int state_size_in_int4 = states_count / 32; // Размер состояния в double word.
    if ( states_count % 32 > 0 ) state_size_in_int4++;
    for( u_int i = 0; i < state_size_in_int4; i++ )
        {
        state.push_back( 0 );
        }
    for( u_int i = 0; i < states_count; i++ )
        {
        mode_start_time.push_back( 0 );
        }
    
    com_dev = new complex_device( number, name, 8,
            i_complex_device::COMPLEX_DEV );

    com_dev->sub_dev[ 0 ] = new single_state( "SINGLE_STATE", number,
            state.data(), this, single_state::T_TECH_OBJECT, states_count );
    com_dev->sub_dev[ 1 ] = new complex_state( "STATE", number,
            state.data(), this, single_state::T_TECH_OBJECT, state_size_in_int4 );
    com_dev->sub_dev[ 2 ] = new complex_state( "CMD", number,
            &cmd, this,single_state::T_TECH_OBJECT );
    com_dev->sub_dev[ 3 ] = par_float;
    com_dev->sub_dev[ 4 ] = rt_par_float;
    com_dev->sub_dev[ 5 ] = par_uint;
    com_dev->sub_dev[ 6 ] = rt_par_uint;
    com_dev->sub_dev[ 7 ] = mode_time;
    }
//-----------------------------------------------------------------------------
tech_object::~tech_object()
    {
    delete com_dev->sub_dev[ 0 ];
    com_dev->sub_dev[ 0 ] = 0;
    delete com_dev->sub_dev[ 1 ];
    com_dev->sub_dev[ 1 ] = 0;
    delete com_dev->sub_dev[ 2 ];
    com_dev->sub_dev[ 2 ] = 0;
    }
//-----------------------------------------------------------------------------
int tech_object::init_params()
    {
    par_float->reset_to_0();
    par_uint->reset_to_0();

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::init_runtime_params()
    {
    rt_par_float->reset_to_0();
    rt_par_uint->reset_to_0();

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::set_mode( u_int mode, int newm )
    {
    int res = 0;

#ifdef DEBUG
    Print ( "Start %s[ %d ]::set_mode mode ( mode = %d, new state = %d ). \n",
        com_dev->get_name(), number, mode, newm );
#endif

    if ( newm != 0 ) newm = 1;
    if ( mode > states_count - 1 ) res = 3;
    else
        {
        if ( get_mode( mode ) == newm ) res = 1;
        else
            {
            if ( newm == 0 ) // Off mode.
                {

                if ( ( res = can_final_mode( mode ) ) == 0 ) // Check if possible.
                    {
                    state[ mode / 32 ] = state[ mode / 32 ] & ~( 1L << mode % 32 );
                    final_mode( mode );

#ifndef USE_NO_COMB
//                g_greb->close_path( paths[ mode ].in_x, paths[ mode ].in_y,
//                    paths[ mode ].out_x, paths[ mode ].out_y );
#endif // USE_NO_COMB
                    }
                }
            else
                {
                if ( ( res = can_init_mode( mode ) ) == 0 ) // Check if possible.
                    {
                    init_mode( mode );
                    state[ mode / 32 ] = state[ mode / 32 ] | 1L << mode % 32;

#ifndef USE_NO_COMB
//                    g_greb->open_path( paths[ mode ].in_x, paths[ mode ].in_y,
//                        paths[ mode ].out_x, paths[ mode ].out_y, comb_path::OT_COMB,
//                        this, mode );
#endif // USE_NO_COMB
                    }
                }
            }
        }
#ifdef DEBUG
    Print( "End %s[ %d ]::set_mode mode = %d, [ %d ], res = %d. \n",
        com_dev->get_name(), number, mode, newm, res );

    for ( u_int i = 0; i < state.size(); i++ )
        {
        Print( " state[ %d ] = %u \n", i, state[ i ] );
        }
    Print( "\n" );
#endif
    
    return res;
    }
//-----------------------------------------------------------------------------
int tech_object::get_mode( int mode )
    {
    return ( int )( ( unsigned long )( state [ mode / 32 ] >> mode % 32 ) & 1 );
    }
//-----------------------------------------------------------------------------
int tech_object::can_init_mode( int mode )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::init_mode( int mode )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::evaluate()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::can_final_mode( int mode )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object::final_mode( int mode )
    {
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int tech_object_manager::init_params()
    {
    for( u_int i = 0; i < tech_objects.size( ); i++ )
        {
        tech_objects[ i ].init_params( );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int tech_object_manager::init_runtime_params()
    {
    for ( u_int i = 0; i < tech_objects.size(); i++ )
        {
        tech_objects[ i ].init_runtime_params();
        }

    return 0;
    }
//-----------------------------------------------------------------------------
tech_object_manager* tech_object_manager::get_instance()
    {
    return instance;
    }
//-----------------------------------------------------------------------------
int tech_object_manager::set_instance( tech_object_manager* new_instance )
    {
    instance = new_instance;
    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------