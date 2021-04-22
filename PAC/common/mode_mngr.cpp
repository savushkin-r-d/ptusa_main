/// @file  mode_mngr.cpp
/// @brief Содержит реализацию классов mode_mngr.h.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c USE_NO_COMB - компиляция без гребенки (объекта g_greb).@n
/// в консоль.
///
/// @par Текущая версия:
/// @$Rev$.@n
/// @$Author$.@n
/// @$Date::                     $.

#include "mode_mngr.h"
#include "g_errors.h"
//-----------------------------------------------------------------------------
const char* operation::state_str [] =
    {
    "Отключен",
    "Выполнение",
    "Пауза",
    "Остановлен"
    };
//-----------------------------------------------------------------------------
const char* operation::en_state_str[] =
    {
    "OFF",
    "RUN",
    "PAUSE",
    "STOP"
    };
//-----------------------------------------------------------------------------
operation::operation(const char* name, operation_manager *owner, int n) :
    current_state ( OFF ), name( name ),
    owner( owner ),
    n( n ),
    stub( "заглушка", owner, -1 ),
    run_step( -1 ),
    run_time( 0 )
    {
    states.push_back( new operation_state( "off",  owner, n ) );
    states.push_back( new operation_state( "run",  owner, n ) );
    states.push_back( new operation_state( "pause",owner, n ) );
    states.push_back( new operation_state( "stop", owner, n ) );
    }
//-----------------------------------------------------------------------------
operation::~operation()
    {
    for ( size_t i = 0; i < states.size(); i++ )
        {
        delete states[ i ];
        states[ i ] = 0;
        }
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

            states[ RUN ]->save();
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
            states[ PAUSE ]->final();
            states[ RUN ]->load();

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
int operation::check_steps_params( char* err_dev_name, int str_len )
    {
    return states[ RUN ]->check_steps_params( err_dev_name, str_len );
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
    //Для состояния OFF ничего не делаем, поэтому current_state > 0.
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
    //Для состояния OFF  возвращаем 0, поэтому current_state > 0.
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
void operation::to_next_step()
    {
    if ( current_state >= 0 && current_state < STATES_MAX )
        {
        states[ current_state ]->to_next_step();
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
int operation::on_extra_step( int step_idx )
    {
    if ( current_state >= 0 && current_state < STATES_MAX )
        {
        states[ current_state ]->on_extra_step( step_idx );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int operation::off_extra_step( int step_idx )
    {
    if ( current_state >= 0 && current_state < STATES_MAX )
        {
        states[ current_state ]->off_extra_step( step_idx );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
bool operation::is_active_run_extra_step( int step_idx ) const
    {
    return states[ RUN ]->is_active_extra_step( step_idx );
    }
//-----------------------------------------------------------------------------
bool operation::is_active_extra_step( int step_idx ) const
    {
    if ( current_state >= 0 && current_state < STATES_MAX )
        {
       return states[ current_state ]->is_active_extra_step( step_idx );

        }

    return false;
    }
//-----------------------------------------------------------------------------
int operation::switch_active_extra_step( int off_step, int on_step )
    {
    if ( current_state >= 0 && current_state < STATES_MAX )
        {
        states[ current_state ]->switch_active_extra_step( off_step, on_step );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
action::action( std::string name, u_int subgropups_cnt ) : name( name ),
    par( 0 ), subgropups_cnt( subgropups_cnt )
    {
    devices.push_back( std::vector < std::vector< device* > >() );
    for ( u_int i = 0; i < subgropups_cnt; i++ )
        {
        devices[ MAIN_GROUP ].push_back( std::vector< device* >() );
        }
    }
//-----------------------------------------------------------------------------
void action::print( const char* prefix /*= "" */,
    bool new_line/* = true*/ ) const
    {
    if ( is_empty() )
        {
        return;
        }

    printf( "%s%s: ", prefix, name.c_str() );

    for ( u_int i = 0; i < devices.size(); i++ )
        {
        printf( "{ " );
        for ( u_int j = 0; j < devices[ i ].size(); j++ )
            {
            printf( "{" );
            for ( u_int k = 0; k < devices[ i ][ j ].size(); k++ )
                {
                printf( "%s", devices[ i ][ j ][ k ]->get_name() );
                if ( k + 1 < devices[ i ][ j ].size() ) printf( " " );
                }
            printf( "} " );
            }
        printf( "} " );
        }

    if ( new_line )
        {
        printf( "\n" );
        }
    }
//-----------------------------------------------------------------------------
void action::final()
    {
    if ( is_empty() )
        {
        return;
        }

    for ( u_int i = 0; i < devices.size(); i++ )
        {
        for ( u_int j = 0; j < devices[ i ].size(); j++ )
            {
            for ( u_int k = 0; k < devices[ i ][ j ].size(); k++ )
                {
#ifdef DEBUG_NO_IO_MODULES
                auto dev = devices[ i ][ j ][ k ];
                auto type = dev->get_type();
                if ( type != device::DT_DI && type != device::DT_AI )
                    {
                    dev->off();
                    }
#else
                devices[ i ][ j ][ k ]->off();
#endif // DEBUG_NO_IO_MODULES
                }
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

    for ( u_int i = 0; i < devices.size(); i++ )
        {
        if ( !devices[ i ].empty() )
            {
            for ( u_int j = 0; j < devices[ i ].size(); j++ )
                {
                if ( !devices[ i ][ j ].empty() ) return false;
                }
            }
        }

    return true;
    }
//-----------------------------------------------------------------------------
int action::check_devices( char* err_dev_name, int max_to_write ) const
    {
    for ( u_int i = 0; i < devices.size(); i++ )
        {
        for ( u_int j = 0; j < devices[ i ].size(); j++ )
            {
            for ( u_int k = 0; k < devices[ i ][ j ].size(); k++ )
                {
                auto dev = devices[ i ][ j ][ k ];
                if ( dev->get_state() < 0 )
                    {
                    int par = int( ( *dev->get_err_par() )[ 1 ] );

                    if ( ( par & base_error::P_IS_SUPPRESS ) == 0 )
                        {
                        max_to_write -= snprintf( err_dev_name + strlen( err_dev_name ),
                            max_to_write, "'%s', ", dev->get_name() );

                        if ( max_to_write < 0 )
                            {
                            break;
                            }
                        }
                    }
                }
            }
        }

    if ( err_dev_name[ 0 ] ) //Есть ошибки.
        {
        size_t length = strlen( err_dev_name );
        if ( max_to_write < 0 )
            {
            err_dev_name[ length - 1 ] = '.';
            err_dev_name[ length - 2 ] = '.';
            err_dev_name[ length - 3 ] = '.';
            }
        else
            {
            //Убираем последние символы - ", ".
            err_dev_name[ length - 1 ] = 0;
            err_dev_name[ length - 2 ] = 0;
            }

        return 1;
        }

    return 0;
    }
//----------------------------------------------------------------------------
void action::add_dev( device *dev, u_int group /*= 0 */, u_int subgroup /*= 0 */ )
    {
    while ( group >= devices.size() )
        {
        devices.push_back( std::vector < std::vector< device* > >() );

        size_t last_idx = devices.size() - 1;
        while ( subgropups_cnt > devices[ last_idx ].size() )
            {
            devices[ last_idx ].push_back( std::vector< device* >() );
            }
        }

    while ( subgroup >= devices[ group ].size() )
        {
        devices[ group ].push_back( std::vector< device* >() );
        }

    devices[ group ][ subgroup ].push_back( dev );
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void on_action::evaluate()
    {
    if ( is_empty() )
        {
        return;
        }

    auto &devs = devices[ MAIN_GROUP ][ MAIN_SUBGROUP ];
    for ( u_int i = 0; i < devs.size(); i++ )
        {
        devs[ i ]->on();
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void on_reverse_action::evaluate()
    {
    if ( is_empty() )
        {
        return;
        }

    auto &devs = devices[ MAIN_GROUP ][ MAIN_SUBGROUP ];
    for ( u_int i = 0; i < devs.size(); i++ )
        {
        devs[ i ]->set_state( 2 );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void off_action::evaluate()
    {
    if ( is_empty() )
        {
        return;
        }

    auto &devs = devices[ MAIN_GROUP ][ MAIN_SUBGROUP ];
    for ( u_int i = 0; i < devs.size(); i++ )
        {
        if ( devs[ i ]->get_type() == device::DT_V )
            {
            valve* v = (valve*)devs[ i ];
            if ( !v->is_wash_seat_active() )
                {
                v->off();
                }
            }
        else
            {
            devs[ i ]->off();
            }
        }
    }
//-----------------------------------------------------------------------------
void off_action::init()
    {
    if ( is_empty() )
        {
        return;
        }

    auto &devs = devices[ MAIN_GROUP ][ MAIN_SUBGROUP ];
    for ( u_int i = 0; i < devs.size(); i++ )
        {
        devs[ i ]->off();
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int required_DI_action::check( char* reason ) const
    {
    if ( is_empty() )
        {
        return 0;
        }

    auto &devs = devices[ MAIN_GROUP ][ MAIN_SUBGROUP ];
    for ( u_int i = 0; i < devs.size(); i++ )
        {
        if ( !devs[ i ]->is_active() )
            {
            sprintf( reason, "нет сигнала \'%.25s (%.50s)\'",
                devs[ i ]->get_name(), devs[ i ]->get_description() );
            return 1;
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
step::step( std::string name, operation_state *owner,
    bool is_mode /*= false */ ) : action_stub( "Заглушка" ),
    start_time( 0 ),
    is_mode( is_mode ),
    name( name ),
    dx_time( 0 ),
    active( false )
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
        actions[ i ] = 0;
        }
    }
//-----------------------------------------------------------------------------
int step::check( char* reason ) const
    {
    int res = actions[ A_DI_DO ]->check( reason );
    if ( res ) return res;

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
    active = true;
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
    active = false;
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
    printf( "%s\"%s\" \n", prefix, name.c_str() );
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
        printf( "Error step::action* operator[] ( int idx ) - idx %d > count %zd.\n",
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
        int len = (int) strlen( err_dev_name );
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
int DI_DO_action::check( char* reason ) const
    {
    if ( is_empty() )
        {
        return 0;
        }

    auto &devs = devices[ MAIN_GROUP ];
    for ( u_int i = 0; i < devs.size(); i++ )
        {
        if ( devs[ i ].empty() )
            {
            continue;
            }

        auto d_i_device = devs[ i ][ 0 ];
        if ( d_i_device->get_type() != device::DT_DI )
            {
            sprintf( reason, "в поле \'Группы DI - DO\' устройство \'%.25s (%.50s)\'"
                " не является сигналом DI",
                d_i_device->get_name(), d_i_device->get_description() );
            return 1;
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void DI_DO_action::evaluate()
    {
    if ( is_empty() )
        {
        return;
        }

    auto &devs = devices[ MAIN_GROUP ];

    for ( u_int i = 0; i < devs.size(); i++ )
        {
        if ( devs[ i ].empty() )
            {
            continue;
            }

        if ( devs[ i ][ 0 ]->is_active() )
            {
            for ( u_int j = 1; j < devs[ i ].size(); j++ )
                {
                devs[ i ][ j ]->on();
                }
            }
        else
            {
            for ( u_int j = 1; j < devs[ i ].size(); j++ )
                {
                devs[ i ][ j ]->off();
                }
            }
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void AI_AO_action::evaluate()
    {
    if ( is_empty() )
        {
        return;
        }

    auto &devs = devices[ MAIN_GROUP ];

    for ( u_int i = 0; i < devs.size(); i++ )
        {
        if ( devs[ i ].empty() )
            {
            continue;
            }

        for ( u_int j = 1; j < devs[ i ].size(); j++ )
            {
            devs[ i ][ j ]->set_value( devs[ i ][ 0 ]->get_value() );
            }
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
open_seat_action::open_seat_action( bool is_mode, operation_state *owner ) :
    action( "Промывка седел" ),
    phase( P_WAIT ),
    next_phase( PHASES::P_OPEN_UPPER ),
    active_group_n( 0 ),
    wait_time( 60000 ),
    wait_seat_time( 0 ),
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

    size_t groups_cnt    =  wash_upper_seat_devices.size() +
        wash_lower_seat_devices.size();

    saved_params_u_int_4 &par = PAC_info::get_instance()->par;

    wait_time = par[ PAC_info::P_MIX_FLIP_PERIOD ] * 1000;
    wait_time /= (u_int_4)groups_cnt;

    // Для шага: для одной группы - середина продолжительности шага,
    // для двух групп - треть и т.д.
    if ( !is_mode )
        {
        u_int_4 wait_time = owner->get_active_step_set_time() / ( (u_int_4)groups_cnt + 1 );
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

        // Пора промывать седла.
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
        else //Время промывки седел вышло.
            {
            final();

            phase            = P_WAIT;
            start_cycle_time = get_millisec();

            //Переход к следующей группе.
            active_group_n++;
            if ( active_group_n >= wash_upper_seat_devices.size() )
                {
                //Переход к нижним седлам.
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
        else //Время промывки седел вышло.
            {
            final();

            phase            = P_WAIT;
            start_cycle_time = get_millisec();

            //Переход к следующей группе.
            active_group_n++;
            if ( active_group_n >= wash_lower_seat_devices.size() )
                {
                //Переход к ожиданию.
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
    auto seat_group = seat_type == valve::V_LOWER_SEAT ?
        &wash_lower_seat_devices : &wash_upper_seat_devices;

    if ( group >= seat_group[ 0 ].size() )
        {
        std::vector< device* > vector_dev;

        seat_group[ 0 ].push_back( vector_dev );
        }

    if ( group >= seat_group[ 0 ].size() )
        {
        if ( G_DEBUG )
            {
            printf( "Error open_seat_action:add_dev: group %d > %zd, seat_type %d.\n",
                group, seat_group[ 0 ].size(), seat_type );
            }
        return;
        }

    seat_group[ 0 ][ group ].push_back( dev );
    }
//-----------------------------------------------------------------------------
void open_seat_action::print( const char* prefix /*= "" */,
    bool new_line/* = true*/ ) const
    {
    if ( wash_upper_seat_devices.empty() && wash_lower_seat_devices.empty() )
        {
        return;
        }

    printf( "%s%s: ", prefix, name.c_str() );

    if ( !wash_upper_seat_devices.empty() )
        {
        printf( "верхние " );
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
        printf( "; нижние " );
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
void wash_action::evaluate()
    {
    if ( is_empty() )
        {
        return;
        }

    for ( u_int idx = 0; idx < devices.size(); idx++ )
        {
        auto &devs = devices[ idx ];

        //Подаем сигналы "Мойка ОК".
        for ( u_int i = 0; i < devs[ G_DO ].size(); i++ )
            {
            devs[ G_DO ][ i ]->on();
            }

        int new_state = 0;

        // Если нет сигналов, то устройства включаем.
        if ( devs[ G_DI ].empty() )
            {
            new_state = 1;
            }
        else
            {
            // В зависимости от сигнала запроса включения устройств выключаем
            // устройства.
            for ( u_int i = 0; i < devs[ G_DI ].size(); i++ )
                {
                if ( devs[ G_DI ][ i ]->is_active() )
                    {
                    new_state = 1;
                    break;
                    }
                }
            }

        float new_val = -1;
        if ( !devs[ G_PUMP_FREQ ].empty() )
            {
            new_val = devs[ G_PUMP_FREQ ][ 0 ]->get_value();
            }
        else
            {
            u_int param_idx = par_idx.size() > idx ? par_idx[ idx ] : 0;
            if ( param_idx > 0 && !par_idx.empty() )
                {
                new_val = ( *par )[ param_idx ];
                }
            }

        //Включаем или выключаем устройства.
        for ( u_int i = 0; i < devs[ G_DEV ].size(); i++ )
            {
            auto dev = devs[ G_DEV ][ i ];
            dev->set_state( new_state );

            auto type = dev->get_type();
            if ( new_val != -1 &&
                ( type == device::DT_M || type == device::DT_VC ||
                type == device::DT_AO || type == device::DT_REGULATOR ) )
                {
                dev->set_value( new_state > 0 ? new_val : 0 );
                }
            }
        for ( u_int i = 0; i < devs[ G_REV_DEV ].size(); i++ )
            {
            auto dev = devs[ G_REV_DEV ][ i ];
            dev->set_state( new_state > 0 ? 2 : 0 );

            if ( new_val != -1 && dev->get_type() == device::DT_M )
                {
                dev->set_value( new_state > 0 ? new_val : 0 );
                }
            }

        bool is_dev_error = false;
        // Чуть раньше подали управляющий сигнал. Сейчас проверяем
        // состояния устройств (насосов, клапанов).
        for ( u_int i = 0; i < devs[ G_DEV ].size(); i++ )
            {
            if ( devs[ G_DEV ][ i ]->get_state() == -1 )
                {
                is_dev_error = true;
                break;
                }
            }
        for ( u_int i = 0; i < devs[ G_REV_DEV ].size(); i++ )
            {
            if ( devs[ G_REV_DEV ][ i ]->get_state() == -1 )
                {
                is_dev_error = true;
                break;
                }
            }
        // Если есть ошибки устройств, не отключая все устройства, снимаем
        // сигналы "Мойка ОК".
        if ( is_dev_error )
            {
            for ( u_int i = 0; i < devs[ G_DO ].size(); i++ )
                {
                devs[ G_DO ][ i ]->off();
                }
            }
        }
    }
//-----------------------------------------------------------------------------
void wash_action::print( const char* prefix /*= "" */,
    bool new_line/* = true*/ ) const
    {
    if ( is_empty() )
        {
        return;
        }

    action::print( prefix, false );

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
    mode_step(  new step( "Шаг операции", this, true ) ),
    active_step_n( -1 ),
    active_step_time( 0 ),
    active_step_next_step_n( -1 ),
    start_time( get_millisec() ),
    step_stub( "Шаг-заглушка", this ),
    owner( owner ),
    n( n ),
    dx_step_time( 0 )
    {
    mode_step[ 0 ][ step::A_WASH ]->set_params( owner->get_params() );
    }
//-----------------------------------------------------------------------------
operation_state::~operation_state()
    {
    delete mode_step;
    mode_step = 0;

    for ( size_t idx = 0; idx < steps.size(); idx++ )
        {
        delete steps[ idx ];
        steps[ idx ] = 0;
        }
    }
//-----------------------------------------------------------------------------
step* operation_state::add_step( const char* name, int next_step_n,
    u_int step_duration_par_n )
    {
    steps.push_back( new step( name, this ) );
    steps[ steps.size() - 1 ][ 0 ][ step::A_WASH ]->set_params(owner->get_params());

    next_step_ns.push_back( next_step_n );
    step_duration_par_ns.push_back( step_duration_par_n );

    return steps[ steps.size() - 1 ];
    }
//-----------------------------------------------------------------------------
int operation_state::check_on( char* reason ) const
    {
    for ( size_t idx = 0; idx < steps.size(); idx++ )
        {
        int res = steps[ idx ]->check( reason );
        if ( res ) return res;
        }

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
        //Если нет шагов, то выходим.
        return;
        }

    to_step( start_step );

    for ( size_t idx = 0; idx < active_steps.size(); idx++ )
        {
        size_t step_n = active_steps[ idx ] - 1;
        if ( step_n < steps.size() ) steps[ step_n ]->init();
        if ( G_DEBUG )
            {
            SetColor( YELLOW );
            printf( "%sINIT EXTRA STEP №%d\n",
                owner->owner->get_prefix(), active_steps[ idx ] );
            steps[ step_n ]->print( owner->owner->get_prefix() );
            SetColor( RESET );
            }
        }
    }
//-----------------------------------------------------------------------------
// Если есть активный шаг, проверяем время. Если оно вышло,
// отключаем активный шаг, в качестве активного шага переназначаем новый шаг.
void operation_state::evaluate()
    {
    mode_step->evaluate();

    for ( size_t idx = 0; idx < active_steps.size(); idx++ )
        {
        size_t step_n = active_steps[ idx ] - 1;
        if ( step_n < steps.size() ) steps[ step_n ]->evaluate();
        }

    if ( active_step_n < 0 ) return;

    //Время шага
    int par_n = step_duration_par_ns[ active_step_n ];
    if ( par_n > 0 )
        {
        active_step_time = u_int( owner->get_step_param( par_n ) * 1000L );

        if ( active_step_time == 0 ||
            steps[ active_step_n ]->get_eval_time() > (u_int)active_step_time )
            {
            if ( -1 == active_step_next_step_n )
                {
                if ( n > 0 )
                    {
                    int time = (int)owner->get_step_param( par_n );
                    const int MAX_BUFF_SIZE = 200;
                    char buff[ MAX_BUFF_SIZE ] = { 0 };
                    std::snprintf( buff, MAX_BUFF_SIZE,
                        "вышло время (%u сек) последнего шага (\'%s\')",
                        time, steps[ active_step_n ]->get_name() );
                    owner->owner->set_err_msg( buff, n, 0, i_tech_object::ERR_OFF );
                    owner->off_mode( n );
                    }
                else
                    {
                    final(); //Для операции-заглушки.
                    }
                }
            else
                {
                to_step( active_step_next_step_n, 0 );
                }

            return;
            }
        }

    steps[ active_step_n ]->evaluate();
    }
//-----------------------------------------------------------------------------
void operation_state::final()
    {
    mode_step->final();
    start_time = get_millisec();

    //Если активный шаг не завершился, сохраняем время его выполнения для
    //возобновления в случае снятия с паузы.
    if ( active_step_n >= 0 )
        {
        dx_step_time = active_step_evaluation_time();
        }

    if ( active_step_n >= 0 )
        {
        steps[ active_step_n ]->final();
        if ( G_DEBUG )
            {
            printf( "%sFINAL ACTIVE STEP №%d\n",
                owner->owner->get_prefix(), active_step_n + 1 );
            }
        active_step_n = -1;
        }

    for ( size_t idx = 0; idx < active_steps.size(); idx++ )
        {
        size_t step_n = active_steps[ idx ] - 1;
        if ( step_n < steps.size() )
            {
            steps[ step_n ]->final();
            if ( G_DEBUG )
                {
                SetColor( YELLOW );
                printf( "%sFINAL ACTIVE EXTRA STEP №%d\n",
                    owner->owner->get_prefix(), active_steps[ idx ] );
                SetColor( RESET );
                }
            }

        }

    active_steps.clear();
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
        printf( "Error operation_state::step& operator[] ( int idx ) - idx %d > count %zd.\n",
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
            printf( "Error mode::to_step step %d > steps size %zd.\n",
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

    //Время шага
    int par_n = step_duration_par_ns[ active_step_n ];
    if ( par_n > 0 )
        {
        active_step_time = u_int( owner->get_step_param( par_n ) * 1000L );
        active_step_next_step_n = next_step_ns[ active_step_n ];
        }

    if ( active_step_time > 0 || par_n <= 0 )
        {
        steps[ active_step_n ]->init();
        steps[ active_step_n ]->evaluate();
        }

    if ( G_DEBUG )
        {
        printf( "%s\"%s\" operation %d \"%s\" to_step() -> %d, step time %d ms, "
            "next step %d.\n",
            owner->owner->get_prefix(), owner->owner->get_name(),
            n, name.c_str(), new_step, active_step_time, active_step_next_step_n );
        steps[ active_step_n ]->print( owner->owner->get_prefix() );
        }
    }
//-----------------------------------------------------------------------------
void operation_state::to_next_step()
    {
    if ( active_step_n >= 0 )
        {
        int current_step = active_step_n + 1;
        int next_step = current_step + 1;
        to_step( next_step );
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
int operation_state::check_steps_params( char* err_dev_name, int str_len )
    {
    if ( !step_duration_par_ns.empty() )
        {
        auto iter = step_duration_par_ns.begin();
        auto all_params_is_empty = true;
        while ( iter != step_duration_par_ns.end() )
            {
            if ( *iter <= 0 || owner->get_step_param( *iter ) > 0 )
                {
                all_params_is_empty = false;
                break;
                }
            iter++;
            }
        if ( all_params_is_empty )
            {
            snprintf( err_dev_name, str_len,
                "все длительности шагов имеют значение 0" );
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
    return (u_int)steps.size();
    }
//-----------------------------------------------------------------------------
const char* operation_state::get_name() const
    {
    return name.c_str();
    }
//-----------------------------------------------------------------------------
void operation_state::save()
    {
    saved_active_steps.assign( active_steps.begin(), active_steps.end() );
    }
//-----------------------------------------------------------------------------
void operation_state::load()
    {
    active_steps.assign( saved_active_steps.begin(), saved_active_steps.end() );
    }
//-----------------------------------------------------------------------------
int operation_state::on_extra_step( int step_idx )
    {
    if ( (size_t) step_idx > steps.size() )
        {
        G_LOG->notice( "operation_state:on_extra_step(...) - step (%d) > size (%d)",
            step_idx, steps.size() );
        return 1;
        }

    if ( 1 + active_step_n == step_idx )
        {
        G_LOG->notice( "operation_state:on_extra_step(...) - step (%d) is active",
            step_idx );
        return 0;
        }

    if ( std::find( active_steps.begin(), active_steps.end(),
        step_idx ) == active_steps.end() )
        {
        active_steps.push_back( step_idx );

        char err_str[ 250 ];
        if ( steps[ step_idx - 1 ]->check( err_str ) == 0 )
            {
            steps[ step_idx - 1 ]->init();
            steps[ step_idx - 1 ]->evaluate();

            if ( G_DEBUG )
                {
                SetColor( YELLOW );
                printf( "%s\"%s\" operation %d \"%s\" on_extra_step() -> %d.\n",
                    owner->owner->get_prefix(),
                    owner->owner->get_name(), n, name.c_str(), step_idx );
                steps[ step_idx - 1 ]->print( owner->owner->get_prefix() );
                SetColor( RESET );
                }
            }
        else
            {
            G_LOG->warning( "operation_state:on_extra_step(...) - %s", err_str );
            return 1;
            }
        }
    else
        {
        G_LOG->notice( "operation_state:on_extra_step(...) - step (%d) is already extra on",
            step_idx );
        return 0;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int operation_state::off_extra_step( int step_idx )
    {
    if ( (size_t) step_idx > steps.size() )
        {
        G_LOG->notice( "operation_state:off_extra_step(...) - step (%d) > size (%d)",
            step_idx, steps.size() );
        return 0;
        }

    auto res = std::find( active_steps.begin(), active_steps.end(), step_idx );
    if ( res != active_steps.end() )
        {
        steps[ step_idx - 1 ]->final();
        active_steps.erase( res );

        if ( G_DEBUG )
            {
            SetColor( YELLOW );
            printf( "%s\"%s\" operation %d \"%s\" off_extra_step() -> %d.\n",
                owner->owner->get_prefix(),
                owner->owner->get_name(), n, name.c_str(), step_idx );
            SetColor( RESET );
            }
        }
    else
        {
        G_LOG->warning( "operation_state:off_extra_step(...) - step (%d) not found",
            step_idx );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
/// @brief Выключение заданного активного шага и включение другого.
///
/// @param off_step - номер выключаемого шага (с единицы).
/// @param on_step - номер включаемого шага (с единицы).
int operation_state::switch_active_extra_step( int off_step, int on_step )
    {
    if ( (size_t) on_step > steps.size() )
        {
        G_LOG->notice( "operation_state:switch_active_step(...) - step (%d) > size (%d)",
            on_step, steps.size() );
        return 1;
        }

    if ( 1 + active_step_n == on_step )
        {
        G_LOG->notice( "operation_state:switch_active_step(...) - step (%d) is active",
            on_step );
        return 0;
        }

    auto res = std::find( active_steps.begin(), active_steps.end(), off_step );
    if ( res != active_steps.end() )
        {
        steps[ off_step - 1 ]->final();
        steps[ on_step - 1 ]->init();
        steps[ on_step - 1 ]->evaluate();

        if ( G_DEBUG )
            {
            SetColor( YELLOW );
            printf( "%s\"%s\" operation %d \"%s\" switch_active_extra_step() %d -> %d.\n",
                owner->owner->get_prefix(),
                owner->owner->get_name(), n, name.c_str(), off_step, on_step );
            steps[ on_step - 1 ]->print( owner->owner->get_prefix() );
            SetColor( RESET );
            }

        *res = on_step;
        }
    else
        {
        G_LOG->warning( "operation_state:switch_active_step(...) - step (%d) not found",
            off_step );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
bool operation_state::is_active_extra_step( int step_idx ) const
    {
    auto res = std::find( active_steps.begin(), active_steps.end(), step_idx );
    if ( res != active_steps.end() ) return true;

    return false;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
operation* operation_manager::add_operation( const char* name )
    {
    operations.push_back( new operation( name, this, (int)operations.size() + 1 ) );

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
        printf( "Error operation_manager::operator[] idx %d > operations count %zd.\n",
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
    printf( "operations manager, %zd\n", operations.size() );

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
    oper_stub = new operation( "Операция-заглушка", this, -1 );
    }
//-----------------------------------------------------------------------------
operation_manager::~operation_manager()
    {
    for ( size_t i = 0; i < operations.size(); i++ )
        {
        delete operations[ i ];
        operations[ i ] = 0;
        }

    delete oper_stub;
    oper_stub = 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
