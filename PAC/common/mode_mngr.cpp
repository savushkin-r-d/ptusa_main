/// @file  mode_mngr.cpp
/// @brief �������� ���������� ������� mode_mngr.h.
///
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
/// @c USE_NO_COMB - ���������� ��� �������� (������� g_greb).@n
/// � �������.
///
/// @par ������� ������:
/// @$Rev$.@n
/// @$Author$.@n
/// @$Date::                     $.

#include "mode_mngr.h"
#include "errors.h"
//-----------------------------------------------------------------------------
const char* operation::state_str [] =
    {
    "��������",
    "����������",
    "�����",
    "����������"
    };
//-----------------------------------------------------------------------------
operation::operation(const char* name, operation_manager *owner, int n) :
    current_state ( OFF ), name( name ),
    owner( owner ),
    n( n ),
    stub( "��������", owner, -1 ),
    run_step( -1 ),
    run_time( 0 )
    {
    states.push_back( new operation_state( "off",  owner, n ) );
    states.push_back( new operation_state( "run",  owner, n ) );
    states.push_back( new operation_state( "pause",owner, n ) );
    states.push_back( new operation_state( "stop", owner, n ) );
    }
//-----------------------------------------------------------------------------
operation::state_idx operation::get_state() const
    {
    return ( state_idx ) current_state;
    }
//-----------------------------------------------------------------------------
int operation::pause()
    {
    switch ( current_state )
        {
        case OFF:
            break;

        case PAUSE:
            break;

        case RUN:
            current_state = PAUSE;

            run_time += states[ RUN ]->evaluation_time();
            run_step = states[ RUN ]->active_step();

            states[ RUN ]->final();
            break;

        case STOP:
            break;

        default:
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int operation::stop()
    {
    switch ( current_state )
        {
        case OFF:
            break;

        case PAUSE:
            current_state = STOP;
            states[ PAUSE ]->final();
            break;

        case RUN:
            current_state = STOP;
            run_time += states[ RUN ]->evaluation_time();
            run_step = states[ RUN ]->active_step();

            states[ RUN ]->final();
            break;

        case STOP:
            break;

        default:
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int operation::start()
    {
    switch ( current_state )
        {
        case OFF:
            current_state = RUN;
            states[ RUN ]->init();

            run_time = 0;
            break;

        case PAUSE:
            current_state = RUN;
            if ( run_step > 0 )
                {
                states[ RUN ]->init( run_step );
                }
            else
                {
                states[ RUN ]->init();
                }

            states[ RUN ]->add_dx_step_time();

            break;

        case RUN:
            break;

        case STOP:
            break;

        default:
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int operation::check_devices_on_run_state(char* err_dev_name, int str_len)
    {
    return states[ RUN ]->check_devices( err_dev_name, str_len );
    }
//-----------------------------------------------------------------------------
int operation::check_on_run_state(char* reason) const
    {
    return states[ RUN ]->check_on( reason );
    }
//-----------------------------------------------------------------------------
u_long operation::evaluation_time()
    {
    if ( current_state >= 0 && current_state < STATES_MAX )
        {
        if ( current_state == RUN )
            {
            return run_time + states[ RUN ]->evaluation_time();
            }

        return run_time;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void operation::evaluate()
    {
    if ( current_state >= 0 && current_state < STATES_MAX )
        {
        states[ current_state ]->evaluate();
        }
    }
//-----------------------------------------------------------------------------
void operation::final()
    {
    //��� ��������� OFF ������ �� ������, ������� current_state > 0.
    if ( current_state > 0 && current_state < STATES_MAX )
        {
        states[ current_state ]->final();
        for ( int idx = OFF; idx < STATES_MAX; idx++ )
            {
            states[ idx ]->reset_eval_time();
            }

        current_state = OFF;
        }
    }
//-----------------------------------------------------------------------------
u_int operation::active_step() const
    {
    //��� ��������� OFF  ���������� 0, ������� current_state > 0.
    if ( current_state > 0 && current_state < STATES_MAX )
        {
        return states[ current_state ]->active_step();
        }
    return 0;
    }
//-----------------------------------------------------------------------------
u_int operation::get_run_step() const
    {
    return run_step;
    }
//-----------------------------------------------------------------------------
u_int operation::get_run_steps_count() const
    {
    return states[ RUN ]->steps_count();
    }
//-----------------------------------------------------------------------------
u_int operation::get_run_active_step() const
    {
    return states[ RUN ]->active_step();
    }
//-----------------------------------------------------------------------------
const char* operation::get_name() const
    {
    return name.c_str();
    }
//-----------------------------------------------------------------------------
void operation::print( const char* prefix /*= "" */) const
    {
    printf( "%s\n", name.c_str() );

    for ( int idx = OFF; idx < STATES_MAX; idx++  )
        {
        states[ idx ]->print( prefix );
        }
    }
//-----------------------------------------------------------------------------
u_long operation::active_step_evaluation_time() const
    {
    if ( current_state >= 0 && current_state < STATES_MAX )
        {
        return states[ current_state ]->active_step_evaluation_time();
        }

    return 0;
    }
//-----------------------------------------------------------------------------
u_long operation::get_active_step_set_time() const
    {
    if ( current_state >= 0 && current_state < STATES_MAX )
        {
        return states[ current_state - 1 ]->get_active_step_set_time();
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void operation::to_step( unsigned int new_step, unsigned long cooperative_time /*= 0 */)
    {
    if ( current_state >= 0 && current_state < STATES_MAX )
        {
        states[ current_state ]->to_step( new_step, cooperative_time );
        }
    }
//-----------------------------------------------------------------------------
step* operation::add_step( const char* name, int next_step_n,
                          unsigned int step_duration_par_n, state_idx s_idx /*= RUN */)
    {
    if ( current_state >= 0 && current_state < STATES_MAX )
        {
        return states[ s_idx ]->add_step( name, next_step_n,
            step_duration_par_n );
        }

    return stub.add_step( name, next_step_n, step_duration_par_n );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
action::action( std::string name, u_int group_cnt ) : name( name ), par( 0 )
    {
    for ( u_int i = 0; i < group_cnt; i++ )
        {
        std::vector< device* > tmp;

        devices.push_back( tmp );
        }
    }
//-----------------------------------------------------------------------------
void action::print( const char* prefix /*= "" */ ) const
    {
    if ( devices.empty() )
        {
        return;
        }

    if ( devices[ 0 ].empty() )
        {
        return;
        }

    printf( "%s%s: ", prefix, name.c_str() );

    for ( u_int i = 0; i < devices.size(); i++ )
        {
        if ( devices[ i ].empty() )
            {
            continue;
            }

        printf( "{" );
        for ( u_int j = 0; j < devices[ i ].size(); j++ )
            {
            printf( "%s", devices[ i ][ j ]->get_name() );
            if ( j + 1 < devices[ i ].size() ) printf( " " );
            }
        printf( "} " );
        }

    printf( "\n" );
    }
//-----------------------------------------------------------------------------
void action::final()
    {
    if ( devices.empty() )
        {
        return;
        }

    for ( u_int i = 0; i < devices.size(); i++ )
        {
        for ( u_int j = 0; j < devices[ i ].size(); j++ )
            {
            devices[ i ][ j ]->off();
            }
        }
    }
//-----------------------------------------------------------------------------
bool action::is_empty() const
    {
    if ( devices.empty() )
        {
        return true;
        }

    if ( devices[ 0 ].empty() )
        {
        return true;
        }

    return false;
    }
//-----------------------------------------------------------------------------
int action::check_devices( char* err_dev_name, int max_to_write ) const
    {
    for ( u_int i = 0; i < devices.size(); i++ )
        {
        for ( u_int j = 0; j < devices[ i ].size(); j++ )
            {
            if ( devices[ i ][ j ]->get_state() < 0 )
                {
                int par = int ( ( *devices[ i ][ j ]->get_err_par() )[ 1 ] );

                if ( ( par & base_error::P_IS_SUPPRESS ) == 0 )
                    {
                    max_to_write -= snprintf( err_dev_name + strlen( err_dev_name ),
                        max_to_write, "'%s', ",
                        devices[ i ][ j ]->get_name() );

                    if ( max_to_write < 0 )
                        {
                        break;
                        }
                    }
                }
            }
        }

    if ( err_dev_name[ 0 ] ) //���� ������.
        {
        int length = strlen( err_dev_name );
        if ( max_to_write < 0 )
            {
            err_dev_name[ length - 1 ] = '.';
            err_dev_name[ length - 2 ] = '.';
            err_dev_name[ length - 3 ] = '.';
            }
        else
            {
            //������� ��������� ������� - ", ".
            err_dev_name[ length - 1 ] = 0;
            err_dev_name[ length - 2 ] = 0;
            }

        return 1;
        }

    return 0;
    }
//----------------------------------------------------------------------------
void action::add_dev( device *dev, u_int group /*= 0 */ )
    {
    if ( group >= devices.size() )
        {
        std::vector< device* > vector_dev;

        devices.push_back( vector_dev );
        }

    if (  group >= devices.size() )
        {
        if ( G_DEBUG )
            {
            printf( "Error device:add_dev(...) - group (%d) >= group count"
                "(%d), device \"%s\""
                " action \"%s\".\n",
                group, devices.size(), dev->get_name(), name.c_str() );
            }
        return;
        }

    devices[ group ].push_back( dev );
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void on_action::evaluate()
    {
    if ( devices.empty() )
        {
        return;
        }

    const u_int IDX = 0;
    for ( u_int i = 0; i < devices[ IDX ].size(); i++ )
        {
        devices[ IDX ][ i ]->on();
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void on_reverse_action::evaluate()
    {
    if ( devices.empty() )
        {
        return;
        }

    const u_int IDX = 0;
    for ( u_int i = 0; i < devices[ IDX ].size(); i++ )
        {
        devices[ IDX ][ i ]->set_state( 2 );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void off_action::evaluate()
    {
    if ( devices.size() == 0  )
        {
        return;
        }

    const u_int IDX = 0;

    for ( u_int i = 0; i < devices[ IDX ].size(); i++ )
        {
        if ( devices[ IDX ][ i ]->get_type() == device::DT_V )
            {
            valve *v  = ( valve* ) devices[ IDX ][ i ];
            if ( v->is_wash_seat_active() )
                {
                }
            else
                {
                v->off();
                }
            }
        else
            {
            devices[ IDX ][ i ]->off();
            }
        }
    }
//-----------------------------------------------------------------------------
void off_action::init()
    {
    if ( devices.size() == 0 )
        {
        return;
        }

    const u_int IDX = 0;

    for ( u_int i = 0; i < devices[ IDX ].size(); i++ )
        {
        devices[ IDX ][ i ]->off();
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int required_DI_action::check( char* reason ) const
    {
    if ( devices.size() == 0  )
        {
        return 0;
        }

    const u_int IDX = 0;
    for ( u_int i = 0; i < devices[ IDX ].size(); i++ )
        {
        if ( !devices[ IDX ][ i ]->is_active() )
            {
            sprintf( reason, "��� ������� \'%.25s (%.50s)\'",
                devices[ IDX ][ i ]->get_name(), devices[ IDX ][ i ]->get_description() );
            return 1;
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
step::step( std::string name, operation_state *owner,
    bool is_mode /*= false */ ) : action_stub( "��������" ),
    start_time( 0 ),
    is_mode( is_mode ),
    name( name ),
    dx_time( 0 )
    {
    actions.push_back( new on_action() );
    actions.push_back( new on_reverse_action() );
    actions.push_back( new off_action() );
    actions.push_back( new open_seat_action( is_mode, owner ) );

    actions.push_back( new required_DI_action() );
    actions.push_back( new DI_DO_action() );
    actions.push_back( new AI_AO_action() );
    actions.push_back( new wash_action() );
    }
//-----------------------------------------------------------------------------
step::~step()
    {
    for ( u_int i = 0; i < actions.size(); i++  )
        {
        delete actions[ i ];
        }
    }
//-----------------------------------------------------------------------------
int step::check( char* reason ) const
    {
    if ( is_mode )
        {
        return actions[ A_REQUIRED_FB ]->check( reason );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void step::init()
    {
    start_time = get_millisec();

    for ( u_int i = 0; i < actions.size(); i++  )
        {
        actions[ i ]->init();
        }
    }
//-----------------------------------------------------------------------------
void step::evaluate() const
    {
    for ( u_int i = 0; i < actions.size(); i++  )
        {
        actions[ i ]->evaluate();
        }
    }
//-----------------------------------------------------------------------------
void step::final()
    {
    for ( u_int i = 0; i < actions.size(); i++  )
        {
        actions[ i ]->final();
        }
    dx_time = 0;
    }
//-----------------------------------------------------------------------------
u_int_4 step::get_eval_time() const
    {
    return get_delta_millisec( start_time ) + dx_time;
    }
//-----------------------------------------------------------------------------
void step::set_start_time( u_int_4 start_time )
    {
    this->start_time = start_time;
    }
//-----------------------------------------------------------------------------
void step::print( const char* prefix /*= "" */ ) const
    {
    printf( "%s \n", name.c_str() );
    std::string new_prefix = prefix;
    new_prefix += "  ";

    for ( u_int i = 0; i < actions.size(); i++  )
        {
        actions[ i ]->print( new_prefix.c_str() );
        }
    }
//-----------------------------------------------------------------------------
action* step::operator[]( int idx )
    {
    if ( idx >= 0 && u_int( idx ) < actions.size() )
        {
        return actions[ idx ];
        }

    if ( G_DEBUG )
        {
        printf( "Error step::action* operator[] ( int idx ) - idx %d > count %d.\n",
            idx, actions.size() );
        }

    return &action_stub;
    }
//-----------------------------------------------------------------------------
bool step::is_empty() const
    {
    for ( u_int i = 0; i < actions.size(); i++ )
        {
        if ( !actions[ i ]->is_empty() )
            {
            return false;
            }
        }
    return true;
    }
//----------------------------------------------------------------------------
int step::check_devices( char* err_dev_name, int str_len )
    {
    for ( u_int i = 0; i < actions.size(); i++ )
        {
        int len = strlen( err_dev_name );
        int res = actions[ i ]->check_devices( err_dev_name + len, str_len - len );

        if ( res )
            {
            return 1;
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void step::set_dx_time( u_int_4 dx_time )
    {
    this->dx_time = dx_time;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void DI_DO_action::evaluate()
    {
    if ( devices.empty() )
        {
        return;
        }

    for ( u_int i = 0; i < devices.size(); i++ )
        {
        if ( devices[ i ].empty() )
            {
            continue;
            }

        if ( devices[ i ][ 0 ]->is_active() )
            {
            for ( u_int j = 1; j < devices[ i ].size(); j++ )
                {
                devices[ i ][ j ]->on();
                }
            }
        else
            {
            for ( u_int j = 1; j < devices[ i ].size(); j++ )
                {
                devices[ i ][ j ]->off();
                }
            }
        }
    }
//-----------------------------------------------------------------------------
void DI_DO_action::final()
    {
    if ( devices.empty() )
        {
        return;
        }

    for ( u_int i = 0; i < devices.size(); i++ )
        {
        if ( devices[ i ].empty() )
            {
            continue;
            }

        //��������� �������� �������.
        for ( u_int j = 1; j < devices[ i ].size(); j++ )
            {
            devices[ i ][ j ]->off();
            }
        }
    }
//-----------------------------------------------------------------------------
void DI_DO_action::print( const char* prefix /*= "" */ ) const
    {
    if ( devices.empty() )
        {
        return;
        }
    if ( devices[ 0 ].empty() )
        {
        return;
        }

    printf( "%s%s: ", prefix, name.c_str() );
    for ( u_int i = 0; i < devices.size(); i++ )
        {
        if ( devices[ i ].empty() )
            {
            continue;
            }

        printf( "{%s", devices[ i ][ 0 ]->get_name() );
        printf( "->" );
        for ( u_int j = 1; j < devices[ i ].size(); j++ )
            {
            printf( "%s", devices[ i ][ j ]->get_name() );
            if ( j + 1 < devices[ i ].size() ) printf( " " );
            }
        printf( "} " );
        }

    printf( "\n" );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void AI_AO_action::evaluate()
    {
    if ( devices.empty() )
        {
        return;
        }

    for ( u_int i = 0; i < devices.size(); i++ )
        {
        if ( devices[ i ].empty() )
            {
            continue;
            }

        for ( u_int j = 1; j < devices[ i ].size(); j++ )
            {
            devices[ i ][ j ]->set_value( devices[ i ][ 0 ]->get_value() );
            }
        }
    }
//-----------------------------------------------------------------------------
void AI_AO_action::final()
    {
    if ( devices.empty() )
        {
        return;
        }

    for ( u_int i = 0; i < devices.size(); i++ )
        {
        if ( devices[ i ].empty() )
            {
            continue;
            }

        //��������� �������� �������.
        for ( u_int j = 1; j < devices[ i ].size(); j++ )
            {
            devices[ i ][ j ]->set_value( 0 );
            }
        }
    }
//-----------------------------------------------------------------------------
void AI_AO_action::print( const char* prefix /*= "" */ ) const
    {
    if ( devices.empty() )
        {
        return;
        }
    if ( devices[ 0 ].empty() )
        {
        return;
        }

    printf( "%s%s: ", prefix, name.c_str() );
    for ( u_int i = 0; i < devices.size(); i++ )
        {
        if ( devices[ i ].empty() )
            {
            continue;
            }

        printf( "{%s", devices[ i ][ 0 ]->get_name() );
        printf( "->" );
        for ( u_int j = 1; j < devices[ i ].size(); j++ )
            {
            printf( "%s", devices[ i ][ j ]->get_name() );
            if ( j + 1 < devices[ i ].size() ) printf( " " );
            }
        printf( "} " );
        }

    printf( "\n" );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
open_seat_action::open_seat_action( bool is_mode, operation_state *owner ) :
    action( "�������� �����" ),
    phase( P_WAIT ),
    active_group_n( 0 ),
    wait_time( 60000 ),
    wash_time_upper( 1000 ),
    wash_time_lower( 1000 ),
    start_cycle_time( 0 ),
    is_mode( is_mode ),
    owner( owner )
    {
    }
//-----------------------------------------------------------------------------
void open_seat_action::set_wait_time( int wait_time )
    {
    this->wait_time = wait_time;
    }
//----------------------------------------------------------------------------
void open_seat_action::init()
    {
    if ( wash_upper_seat_devices.empty() && wash_lower_seat_devices.empty() )
        {
        return;
        }

    start_cycle_time  = get_millisec();
    phase             = P_WAIT;
    next_phase        = P_OPEN_UPPER;
    active_group_n    = 0;

    int groups_cnt    =  wash_upper_seat_devices.size() +
        wash_lower_seat_devices.size();

    saved_params_u_int_4 &par = PAC_info::get_instance()->par;

    wait_time = par[ PAC_info::P_MIX_FLIP_PERIOD ] * 1000;
    wait_time /= groups_cnt;

    // ��� ����: ��� ����� ������ - �������� ����������������� ����,
    // ��� ���� ����� - ����� � �.�.
    if ( !is_mode )
        {
        u_int_4 wait_time = owner->get_active_step_set_time() / ( groups_cnt + 1 );
        if ( wait_time > 0 )
            {
            this->wait_time = wait_time;
            }
        }

    wash_time_upper = par[ PAC_info::P_MIX_FLIP_UPPER_TIME ];
    wash_time_lower = par[ PAC_info::P_MIX_FLIP_LOWER_TIME ];

    wait_time -= ( wash_time_upper > wash_time_lower ?
        wash_time_upper: wash_time_lower ) / 2;

    active_group_n = 0;

    for ( u_int i = 0; i < wash_lower_seat_devices.size(); i++ )
        {
        for ( u_int j = 0; j < wash_lower_seat_devices[ i ].size(); j++ )
            {
            wash_lower_seat_devices[ i ][ j ]->off();
            }
        }
    for ( u_int i = 0; i < wash_upper_seat_devices.size(); i++ )
        {
        for ( u_int j = 0; j < wash_upper_seat_devices[ i ].size(); j++ )
            {
            wash_upper_seat_devices[ i ][ j ]->off();
            }
        }
    }
//-----------------------------------------------------------------------------
void open_seat_action::evaluate()
    {
    if ( wash_lower_seat_devices.empty() && wash_upper_seat_devices.empty() ) return;

    switch ( phase )
        {
    case P_WAIT:
        for ( u_int i = 0; i < wash_lower_seat_devices.size(); i++ )
            {
            for ( u_int j = 0; j < wash_lower_seat_devices[ i ].size(); j++ )
                {
                valve *v  = ( valve* ) wash_lower_seat_devices[ i ][ j ];

                if ( !v->is_wash_seat_active() )
                    {
                    v->off();
                    }
                }
            }
        for ( u_int i = 0; i < wash_upper_seat_devices.size(); i++ )
            {
            for ( u_int j = 0; j < wash_upper_seat_devices[ i ].size(); j++ )
                {
                valve *v  = ( valve* ) wash_upper_seat_devices[ i ][ j ];

                if ( !v->is_wash_seat_active() )
                    {
                    v->off();
                    }
                }
            }

        // ���� ��������� �����.
        if ( get_delta_millisec( start_cycle_time ) > wait_time )
            {
            phase            = next_phase;
            start_cycle_time = get_millisec();
            }
        break;

    case P_OPEN_UPPER:
        if ( wash_upper_seat_devices.empty() )
            {
            phase      = P_OPEN_LOWER;
            next_phase = P_OPEN_LOWER;
            break;
            }

        if ( get_delta_millisec( start_cycle_time ) < wash_time_upper )
            {
            for ( u_int j = 0; j < wash_upper_seat_devices[ active_group_n ].size(); j++ )
                {
                valve *v  = ( valve* ) wash_upper_seat_devices[ active_group_n ][ j ];
                v->set_state( valve::V_UPPER_SEAT );
                v->set_seat_wash_state( true );
                }
            }
        else //����� �������� ����� �����.
            {
            final();

            phase            = P_WAIT;
            start_cycle_time = get_millisec();

            //������� � ��������� ������.
            active_group_n++;
            if ( active_group_n >= wash_upper_seat_devices.size() )
                {
                //������� � ������ ������.
                active_group_n = 0;
                next_phase     = P_OPEN_LOWER;
                }
            }
        break;

    case P_OPEN_LOWER:
        if ( wash_lower_seat_devices.empty() )
            {
            phase      = P_OPEN_UPPER;
            next_phase = P_OPEN_UPPER;
            break;
            }

        if ( get_delta_millisec( start_cycle_time ) < wash_time_lower )
            {
            for ( u_int j = 0; j < wash_lower_seat_devices[ active_group_n ].size(); j++ )
                {
                valve *v  = ( valve* ) wash_lower_seat_devices[ active_group_n ][ j ];
                v->set_state( valve::V_LOWER_SEAT );
                v->set_seat_wash_state( true );
                }
            }
        else //����� �������� ����� �����.
            {
            final();

            phase            = P_WAIT;
            start_cycle_time = get_millisec();

            //������� � ��������� ������.
            active_group_n++;
            if ( active_group_n >= wash_lower_seat_devices.size() )
                {
                //������� � ��������.
                active_group_n = 0;
                next_phase     = P_OPEN_UPPER;
                }
            }
        break;
        }
    }
//-----------------------------------------------------------------------------
void open_seat_action::final()
    {
    if ( wash_upper_seat_devices.empty() && wash_lower_seat_devices.empty() ) return;

    switch ( phase )
        {
        case P_WAIT:
            break;

        case P_OPEN_UPPER:
            if ( wash_upper_seat_devices.empty() ) break;

            for ( u_int j = 0; j < wash_upper_seat_devices[ active_group_n ].size(); j++ )
                {
                valve *v  = ( valve* ) wash_upper_seat_devices[ active_group_n ][ j ];
                v->off();
                v->set_seat_wash_state( false );
                }
            break;

        case P_OPEN_LOWER:
            if ( wash_lower_seat_devices.empty() ) break;

            for ( u_int j = 0; j < wash_lower_seat_devices[ active_group_n ].size(); j++ )
                {
                valve *v  = ( valve* ) wash_lower_seat_devices[ active_group_n ][ j ];
                v->off();
                v->set_seat_wash_state( false );
                }
            break;
        }
    }
//-----------------------------------------------------------------------------
void open_seat_action::add_dev( device *dev, u_int group, u_int seat_type )
    {
    std::vector< std::vector< device* > > *seat_group = 0;

    switch ( seat_type )
        {
    case valve::V_UPPER_SEAT:
        seat_group = &wash_upper_seat_devices;
        break;

    case valve::V_LOWER_SEAT:
        seat_group = &wash_lower_seat_devices;
        break;
        }

    if ( group >= seat_group[ 0 ].size() )
        {
        std::vector< device* > vector_dev;

        seat_group[ 0 ].push_back( vector_dev );
        }

    if ( group >= seat_group[ 0 ].size() )
        {
        if ( G_DEBUG )
            {
            printf( "Error open_seat_action:add_dev: group %d > %d, seat_type %d.\n",
                group, seat_group[ 0 ].size(), seat_type );
            }
        return;
        }

    seat_group[ 0 ][ group ].push_back( dev );
    }
//-----------------------------------------------------------------------------
void open_seat_action::print( const char* prefix /*= "" */ ) const
    {
    if ( wash_upper_seat_devices.empty() && wash_lower_seat_devices.empty() )
        {
        return;
        }

    printf( "%s%s: ", prefix, name.c_str() );

    if ( !wash_upper_seat_devices.empty() )
        {
        printf( "������� " );
        for ( u_int i = 0; i < wash_upper_seat_devices.size(); i++ )
            {
            printf( " {" );
            for ( u_int j = 0; j < wash_upper_seat_devices[ i ].size(); j++ )
                {
                printf( "%s",  wash_upper_seat_devices[ i ][ j ]->get_name() );
                if ( j + 1 < wash_upper_seat_devices[ i ].size() ) printf( " " );
                }

            printf( "}" );
            }
        }

    if ( !wash_lower_seat_devices.empty() )
        {
        printf( "; ������ " );
        for ( u_int i = 0; i < wash_lower_seat_devices.size(); i++ )
            {
            printf( " {" );
            for ( u_int j = 0; j < wash_lower_seat_devices[ i ].size(); j++ )
                {
                printf( "%s ", wash_lower_seat_devices[ i ][ j ]->get_name() );
                if ( j + 1 < wash_lower_seat_devices[ i ].size() ) printf( " " );
                }

            printf( "}" );
            }
        }

    printf( "\n" );
    }
//-----------------------------------------------------------------------------
bool open_seat_action::is_empty() const
    {
    if ( wash_upper_seat_devices.empty() &&
        wash_lower_seat_devices.empty() )
        {
        return true;
        }

    if ( wash_upper_seat_devices.size() > 0 && wash_upper_seat_devices[ 0 ].empty() &&
        wash_lower_seat_devices.size() > 0 && wash_lower_seat_devices[ 0 ].empty() )
        {
        return true;
        }

    return false;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void wash_action::final()
    {
    //������� ������� "����� ��".
    for ( u_int i = 0; i < devices[ G_DO ].size(); i++ )
        {
        devices[ G_DO ][ i ]->off();
        }

    //��������� ����������.
    for ( u_int i = 0; i < devices[ G_DEV ].size(); i++ )
        {
        devices[ G_DEV ][ i ]->off();
        }
    for ( u_int i = 0; i < devices[ G_REV_DEV ].size(); i++ )
        {
        devices[ G_REV_DEV ][ i ]->off();
        }
    }
//---------------------------------------------------------------------------
void wash_action::evaluate()
    {
    //������ ������� "����� ��".
    for ( u_int i = 0; i < devices[ G_DO ].size(); i++ )
        {
        devices[ G_DO ][ i ]->on();
        }

    int new_state = 0;

    // ���� ��� ��������, �� ���������� ��������.
    if ( devices[ G_DI ].empty() )
        {
        new_state = 1;
        }
    else
        {
        // � ����������� �� ������� ������� ��������� ��������� ���������
        // ����������.
        for ( u_int i = 0; i < devices[ G_DI ].size(); i++ )
            {
            if ( devices[ G_DI ][ i ]->is_active() )
                {
                new_state = 1;
                break;
                }
            }
        }

    float new_val = -1;
    if ( !par_idx.empty() )
        {
        new_val = ( *par )[ par_idx[ P_PUMP_FREQ ] ];
        }

    //�������� ��� ��������� ����������.
    for ( u_int i = 0; i < devices[ G_DEV ].size(); i++ )
        {
        devices[ G_DEV ][ i ]->set_state( new_state );
        if ( new_val != -1 )
            {
            devices[ G_DEV ][ i ]->set_value( new_state > 0 ? new_val : 0 );
            }
        }
    for ( u_int i = 0; i < devices[ G_REV_DEV ].size(); i++ )
        {
        devices[ G_REV_DEV ][ i ]->set_state( new_state > 0 ? 2 : 0 );
        if ( new_val != -1 )
            {
            devices[ G_REV_DEV ][ i ]->set_value( new_state > 0 ? new_val : 0 );
            }
        }

    bool is_dev_error = false;
    // ���� ������ ������ ����������� ������. ������ ���������
    // ��������� ��������� (�������, ��������).
    for ( u_int i = 0; i < devices[ G_DEV ].size(); i++ )
        {
        if ( devices[ G_DEV ][ i ]->get_state() == -1 )
            {
            is_dev_error = true;
            break;
            }
        }
    for ( u_int i = 0; i < devices[ G_REV_DEV ].size(); i++ )
        {
        if ( devices[ G_REV_DEV ][ i ]->get_state() == -1 )
            {
            is_dev_error = true;
            break;
            }
        }
    // ���� ���� ������ ���������, �� �������� ��� ����������, �������
    // ������� "����� ��".
    if ( is_dev_error )
        {
        for ( u_int i = 0; i < devices[ G_DO ].size(); i++ )
            {
            devices[ G_DO ][ i ]->off();
            }
        }
    }
//-----------------------------------------------------------------------------
void wash_action::print( const char* prefix /*= "" */ ) const
    {
    if ( devices[ G_DI ].size() == 0 && devices[ G_DO ].size() == 0 &&
        devices[ G_DEV ].size() == 0 )
        {
        return;
        }

    printf( "%s%s: ", prefix, name.c_str() );

    if ( !devices[ G_DI ].empty() )
        {
        printf( "FB " );
        printf( "{" );
        for ( u_int j = 0; j < devices[ G_DI ].size(); j++ )
            {
            printf( "%s",  devices[ G_DI ][ j ]->get_name() );
            if ( j + 1 < devices[ G_DI ].size() ) printf( " " );
            }

        printf( "}" );
        }

    if ( !devices[ G_DO ].empty() )
        {
        printf( "; DO " );

        printf( "{" );
        for ( u_int j = 0; j < devices[ G_DO ].size(); j++ )
            {
            printf( "%s", devices[ G_DO ][ j ]->get_name() );
            if ( j + 1 < devices[ G_DO ].size() ) printf( " " );
            }

        printf( "}" );
        }

    if ( !devices[ G_DEV ].empty() )
        {
        printf( "; DEV " );

        printf( "{" );
        for ( u_int j = 0; j < devices[ G_DEV ].size(); j++ )
            {
            printf( "%s", devices[ G_DEV ][ j ]->get_name() );
            if ( j + 1 < devices[ G_DEV ].size() ) printf( " " );
            }

        printf( "}" );
        }

    if ( !devices[ G_REV_DEV ].empty() )
        {
        printf( "; REV_DEV " );

        printf( "{" );
        for ( u_int j = 0; j < devices[ G_REV_DEV ].size(); j++ )
            {
            printf( "%s", devices[ G_REV_DEV ][ j ]->get_name() );
            if ( j + 1 < devices[ G_REV_DEV ].size() ) printf( " " );
            }

        printf( "}" );
        }

    if ( !par_idx.empty() )
        {
        printf( "; FREQ_PARAM " );
        printf( "{" );
        for ( u_int j = 0; j < par_idx.size(); j++ )
            {
            printf( "%d", par_idx[ j ] );
            if ( j + 1 < par_idx.size() ) printf( " " );
            }
        printf( "}" );
        }

    printf( "\n" );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
operation_state::operation_state( const char* name,
    operation_manager *owner, int n ) : name( name ),
    mode_step(  new step( "��� ��������", this, true ) ),
    active_step_n( -1 ),
    start_time( get_millisec() ),
    step_stub( "���-��������", this ),
    owner( owner ),
    n( n ),
    dx_step_time( 0 )
    {
    mode_step[ 0 ][ step::A_WASH ]->set_params( owner->get_params() );
    }
//-----------------------------------------------------------------------------
step* operation_state::add_step( const char* name, int next_step_n,
    u_int step_duration_par_n )
    {
    steps.push_back( new step( name, this ) );

    next_step_ns.push_back( next_step_n );
    step_duration_par_ns.push_back( step_duration_par_n );

    return steps[ steps.size() - 1 ];
    }
//-----------------------------------------------------------------------------
int operation_state::check_on( char* reason ) const
    {
    return mode_step->check( reason );
    }
//-----------------------------------------------------------------------------
void operation_state::init( u_int start_step /*= 1 */ )
    {
    mode_step->init();
    start_time = get_millisec();

    active_step_n = -1;

    if ( steps.empty() )
        {
        //���� ��� �����, �� �������.
        return;
        }

    to_step( start_step );

    if ( G_DEBUG )
        {
        printf( " INIT STEP [ %d ]\n", active_step_n + 1 );
        steps[ active_step_n ]->print( " " );
        printf( " TIME %d ms, NEXT STEP -> %d \n",
            active_step_time, active_step_next_step_n );
        }
    }
//-----------------------------------------------------------------------------
// ���� ���� �������� ���, ��������� �����. ���� ��� �����,
// ��������� �������� ���, � �������� ��������� ���� ������������� ����� ���.
void operation_state::evaluate()
    {
    mode_step->evaluate();

    if ( active_step_n < 0 ) return;

    //����� ����
    int par_n = step_duration_par_ns[ active_step_n ];
    if ( par_n > 0 && owner->get_step_param( par_n ) > 0 )
        {
        active_step_time = u_int( owner->get_step_param( par_n ) * 1000L );
        }

    steps[ active_step_n ]->evaluate();

    if ( active_step_time != 0 &&
        steps[ active_step_n ]->get_eval_time() > ( u_int ) active_step_time )
        {
        if ( -1 == active_step_next_step_n )
            {
            if ( n > 0 )
                {
                owner->off_mode( n );
                }
            else
                {
                final(); //��� ������-��������.
                }
            }
        else
            {
            to_step( active_step_next_step_n, 0 );
            }
        }
    }
//-----------------------------------------------------------------------------
void operation_state::final()
    {
    mode_step->final();
    start_time = get_millisec();

    //���� �������� ��� �� ����������, ��������� ����� ��� ���������� ���
    //������������� � ������ ������ � �����.
    if ( active_step_n >= 0 )
        {
        dx_step_time = active_step_evaluation_time();
        }

    if ( active_step_n >= 0 )
        {
        steps[ active_step_n ]->final();
        if ( G_DEBUG )
            {
            printf( " FINAL ACTIVE STEP [ %d ] \n", active_step_n );
            }
        active_step_n = -1;
        }
    }
//-----------------------------------------------------------------------------
step* operation_state::operator[]( int idx )
    {
    if ( -1 == idx )
        {
        return mode_step;
        }

    if ( idx > 0 && u_int( idx ) <= steps.size() )
        {
        return steps[ idx - 1 ];
        }

    if ( G_DEBUG )
        {
        printf( "Error operation_state::step& operator[] ( int idx ) - idx %d > count %d.\n",
            idx, steps.size() );
        }

    return &step_stub;
    }
//-----------------------------------------------------------------------------
void operation_state::to_step( u_int new_step, u_long cooperative_time )
    {
    if ( new_step > steps.size() || new_step <= 0 )
        {
        if ( G_DEBUG )
            {
            printf( "Error mode::to_step step %d > steps size %d.\n",
                new_step, steps.size() );
            }
        return;
        }

    active_step_time        = 0;
    active_step_next_step_n = -1;

    if ( active_step_n >= 0 )
        {
        steps[ active_step_n ]->final();
        }
    active_step_n = new_step - 1;

    //����� ����
    int par_n = step_duration_par_ns[ active_step_n ];
    if ( par_n > 0 && owner->get_step_param( par_n ) >= 0 )
        {
        active_step_time = u_int( owner->get_step_param( par_n ) * 1000L );
        active_step_next_step_n = next_step_ns[ active_step_n ];
        }

    steps[ active_step_n ]->init();
    steps[ active_step_n ]->evaluate();


    if ( G_DEBUG )
        {
        printf( "\"%s\" mode %d \"%s\" to_step() -> %d, step time %d ms.\n",
            owner->owner->get_name(),
            n, name.c_str(), new_step, active_step_time );
        }
    }
//-----------------------------------------------------------------------------
void operation_state::print( const char* prefix /*= "" */ ) const
    {
    if ( mode_step->is_empty() && steps.empty() )
        {
        return;
        }

    std::string new_prefix = prefix;
    new_prefix += "    ";

    printf( "%s%s\n", prefix, name.c_str() );

    if ( !mode_step->is_empty() )
        {
        printf( "%s0 ", new_prefix.c_str() );
        mode_step->print( new_prefix.c_str() );
        }

    for ( u_int i = 0; i < steps.size(); i++ )
        {
        printf( "%s%d ", new_prefix.c_str(), i + 1 );
        steps[ i ]->print( new_prefix.c_str() );
        }
    }
//----------------------------------------------------------------------------
int operation_state::check_devices( char* err_dev_name, int str_len )
    {
    int res = mode_step->check_devices( err_dev_name +
        strlen( err_dev_name ), str_len - strlen( err_dev_name ) );

    if ( res )
        {
        return 1;
        }

    if ( steps.empty() )
        {
        return 0;
        }

    if ( active_step_n >= 0 && ( unsigned int ) active_step_n < steps.size() )
        {
        res = steps[ active_step_n ]->check_devices( err_dev_name +
            strlen( err_dev_name ), str_len - strlen( err_dev_name ) );

        if ( res )
            {
            return 1;
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void operation_state::add_dx_step_time()
    {
    if ( active_step_n >= 0 )
        {
        steps[ active_step_n ]->set_dx_time( dx_step_time );
        }
    }
//-----------------------------------------------------------------------------
u_long operation_state::evaluation_time()
    {
    return get_delta_millisec( start_time );
    }
//-----------------------------------------------------------------------------
u_long operation_state::active_step_evaluation_time() const
    {
    if ( active_step_n >= 0 )
        {
        return steps[ active_step_n ]->get_eval_time();
        }

    return 0;
    }
//-----------------------------------------------------------------------------
u_long operation_state::get_active_step_set_time() const
    {
    if ( active_step_n >= 0 )
        {
        return active_step_time;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
u_int operation_state::active_step() const
    {
    return active_step_n + 1;
    }
//-----------------------------------------------------------------------------
u_int operation_state::steps_count() const
    {
    return steps.size();
    }
//-----------------------------------------------------------------------------
const char* operation_state::get_name() const
    {
    return name.c_str();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
operation* operation_manager::add_operation( const char* name )
    {
    operations.push_back( new operation( name, this, operations.size() + 1 ) );

    return operations[ operations.size() - 1 ];
    }
//-----------------------------------------------------------------------------
operation* operation_manager::operator[]( unsigned int idx )
    {
    if ( idx > 0 && idx <= operations.size() )
        {
        return operations[ idx - 1 ];
        }

    if ( G_DEBUG )
        {
        printf( "Error operation_manager::operator[] idx %d > operations count %d.\n",
            idx, operations.size() );
        }

    return oper_stub;
    }
//-----------------------------------------------------------------------------
unsigned long operation_manager::get_idle_time()
    {
    return get_delta_millisec( last_action_time );
    }
//-----------------------------------------------------------------------------
void operation_manager::print()
    {
    printf( "operations manager, %d\n", operations.size() );

    for ( u_int i = 0; i < operations.size(); i++ )
        {
        printf( "  %2d ", i + 1 );
        operations[ i ]->print( "    " );
        }
    }
//-----------------------------------------------------------------------------
operation_manager::operation_manager( u_int modes_cnt, i_tech_object *owner ):
    owner( owner ),
    last_action_time( get_millisec() )
    {
    oper_stub = new operation( "��������-��������", this, -1 );
    }
//-----------------------------------------------------------------------------
operation_manager::~operation_manager()
    {
    delete oper_stub;
    oper_stub = 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
