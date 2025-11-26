/// @file  operation_mngr.cpp
/// @brief Содержит реализацию классов operation_mngr.h.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Текущая версия:
/// @$Rev$.@n
/// @$Author$.@n
/// @$Date::                     $.

#include "operation_mngr.h"
#include "g_errors.h"

#include <fmt/chrono.h>

constexpr std::array <const char* const, operation::STATES_MAX> operation::state_str;
constexpr std::array <const char* const, operation::STATES_MAX> operation::en_state_str;
//-----------------------------------------------------------------------------
operation::operation(const char* name, operation_manager *owner, int n) :
    name( name ),
    owner( owner ),
    operation_num( n ),
    stub( "заглушка", owner, -1 )
    {
    for ( auto state_name: en_state_str )
        {
        states.push_back( new operation_state( state_name, owner, n ) );
        }
    }
//-----------------------------------------------------------------------------
operation::~operation()
    {
    for ( auto& operation_state_ptr : states )
        {
        delete operation_state_ptr;
        operation_state_ptr = nullptr;
        }
    }
//-----------------------------------------------------------------------------
operation::state_idx operation::get_state() const
    {
    return current_state;
    }
//-----------------------------------------------------------------------------
int operation::pause()
    {
    switch ( current_state )
        {
        case RUN:
            run_time += states[ RUN ]->evaluation_time();
            run_step = states[ RUN ]->active_step();

            states[ RUN ]->save();
            states[ RUN ]->finalize();

            if ( states[ PAUSING ]->is_empty() )
                {
                current_state = PAUSE;
                }
            else
                {
                current_state = PAUSING;
                }
            states[ current_state ]->init();
            states[ current_state ]->evaluate();
            break;

        case PAUSING:
            states[ PAUSING ]->finalize();
            current_state = PAUSE;
            states[ current_state ]->init();
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
        case PAUSING:
        case PAUSE:  
        case UNPAUSING:
        case STARTING:        
        case RUN:
            states[ current_state ]->finalize();
            if ( states[ STOPPING ]->is_empty() ) current_state = STOP;
            else current_state = STOPPING;

            states[ current_state ]->init();
            states[ current_state ]->evaluate();
            break;

        case STOPPING:
            states[ current_state ]->finalize();
            current_state = STOP;

            states[ current_state ]->init();
            states[ current_state ]->evaluate();
            break;

        default:
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int operation::switch_off()
    {
    if ( current_state != IDLE )
        {        
        states[ current_state ]->finalize();
        current_state = IDLE;
        states[ IDLE ]->init();
        states[ IDLE ]->evaluate();
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int operation::start()
    {
    return start( run_step );   
    }
//-----------------------------------------------------------------------------
int operation::start( int new_run_step )
    {
    switch ( current_state )
        {
        case IDLE:
            states[ IDLE ]->finalize();
            if ( states[ STARTING ]->is_empty() ) current_state = RUN;
            else current_state = STARTING;
            
            states[ current_state ]->init();
            run_time = 0;
            break;

        case PAUSE:
            states[ PAUSE ]->finalize();
            if ( states[ UNPAUSING ]->is_empty() )
                {
                to_run_state( new_run_step );
                break;
                }
            else
                {
                current_state = UNPAUSING;
                states[ UNPAUSING ]->init();
                states[ UNPAUSING ]->evaluate();
                }
            break;

        case RUN:
            break;

        case STOP:
            break;

        case STARTING:
            states[ STARTING ]->finalize();
            current_state = RUN;
            states[ current_state ]->init();
            run_time = 0;
            break;

        case UNPAUSING:
            to_run_state( new_run_step );
            break;

        default:
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void operation::to_run_state( int new_run_step )
    {
    current_state = RUN;
    states[ RUN ]->load();

    if ( new_run_step > 0 )
        {
        states[ RUN ]->init( new_run_step );
        //Если возвращаемся в шаг, активный до паузы, то добавляем
        //его время выполнения.
        if ( new_run_step == run_step ) states[ RUN ]->add_dx_step_time();
        }
    else if ( run_step > 0 )
        {
        states[ RUN ]->init( run_step );
        states[ RUN ]->add_dx_step_time();
        }
    else
        {
        states[ RUN ]->init();
        }
    states[ RUN ]->evaluate();
    }
//-----------------------------------------------------------------------------
int operation::check_devices_on_run_state(char* err_dev_name, unsigned int str_len)
    {
    return states[ RUN ]->check_devices( err_dev_name, str_len );
    }
//-----------------------------------------------------------------------------
int operation::check_steps_params( char* err_dev_name, unsigned int str_len )
    {
    return states[ RUN ]->check_steps_params( err_dev_name, str_len );
    }
//-----------------------------------------------------------------------------
int operation::check_max_step_time( char* err_dev_name, unsigned int str_len )
    {
    return states[ RUN ]->check_max_step_time( err_dev_name, str_len );
    }
//-----------------------------------------------------------------------------
int operation::check_on_run_state( char* reason, unsigned int max_len ) const
    {
    return states[ RUN ]->check_on( reason, max_len );
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

        int next_state = 0;
        auto res = states[ current_state ]->is_goto_next_state( next_state );
        if ( res )
            {
            auto unit = owner->owner;
            auto n_state = static_cast<state_idx>( next_state );
            switch ( current_state )
                {
                case state_idx::IDLE:
                    //Из простоя по сигналам операция может быть включена 
                    //(перейти в состояние выполнения).
                    process_auto_switch_on();
                    break;

                case state_idx::RUN:
                    process_new_state_from_run( next_state );
                    break;

                case state_idx::STARTING:
                    default_process_new_state( n_state, state_idx::RUN );
                    break;

                case state_idx::PAUSING:
                    default_process_new_state( n_state, state_idx::PAUSE );
                    break;

                case state_idx::UNPAUSING:
                    default_process_new_state( n_state, state_idx::RUN );
                    break;

                case state_idx::STOPPING:
                    unit->set_mode( operation_num, state_idx::STOP );
                    break;

                case state_idx::STOP:
                    unit->set_mode( operation_num, state_idx::IDLE );
                    break;

                default:
                    //Для всех остальных состояний ничего не делаем.
                    break;
                }
            }
        else
            {
            is_first_goto_next_state = true;
            was_fail = false;
            }
        }
    }
//-----------------------------------------------------------------------------
int operation::process_auto_switch_on()
    {
    auto unit = owner->owner;
    const auto WARN = tech_object::ERR_MSG_TYPES::ERR_DURING_WORK;
    const auto ERR = tech_object::ERR_MSG_TYPES::ERR_CANT_ON;

    if ( was_fail ) return 1;

    if ( is_first_goto_next_state )
        {
        auto result = unit->set_mode( operation_num, operation::RUN );
        if ( result == 0 )
            {
            unit->set_err_msg( "автовключение по запросу", operation_num, 0, WARN );
            return 0;
            }
        else
            {
            unit->set_err_msg( "нет автовключения по запросу", operation_num, 0, ERR );
            start_warn = get_millisec();
            start_wait = get_millisec();
            is_first_goto_next_state = false;
            }
        }
    else
        {
        auto dt = G_PAC_INFO()->par[ PAC_info::P_AUTO_OPERATION_WARN_TIME ];
        auto wt = G_PAC_INFO()->par[ PAC_info::P_AUTO_OPERATION_WAIT_TIME ];

        if ( unit->check_operation_on( operation_num, false ) == 0 )
            {
            unit->set_err_msg( "автовключение по запросу", operation_num, 0, WARN );
            unit->set_mode( operation_num, operation::RUN );
            return 0;
            }

        // Прошел заданный интервал для уведомления.
        if ( get_delta_millisec( start_warn ) > dt )
            {
            unit->check_operation_on( operation_num );
            unit->set_err_msg( "нет автовключения по запросу", operation_num, 0, ERR );
            start_warn = get_millisec();
            }

        // Прошел заданный интервал для ожидания возможности включения операции.
        if ( get_delta_millisec( start_wait ) > wt )
            {
            unit->set_err_msg( "автовключение по запросу отключено", operation_num, 0, ERR );
            was_fail = true;
            }
        }

    return 1;
    }
//-----------------------------------------------------------------------------
int operation::process_new_state_from_run( int next_state )
    {
    auto unit = owner->owner;
    switch ( static_cast<state_idx>( next_state ) )
        {
        case state_idx::STOP:
            // Из выполнения по сигналам операция может быть остановлена.
            unit->set_err_msg( "автоотключение по запросу",
                operation_num, 0, tech_object::ERR_MSG_TYPES::ERR_DURING_WORK );
            unit->set_mode( operation_num, state_idx::STOP );
            break;

        case state_idx::PAUSE:
            // Из выполнения по сигналам операция может быть
            // поставлена на паузу.
            unit->set_mode( operation_num, state_idx::PAUSE );
            unit->set_err_msg( "пауза по запросу",
                operation_num, 0, tech_object::ERR_MSG_TYPES::ERR_TO_FAIL_STATE );
            break;

        default:
            //Остальные варианты игнорируем.
            break;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int operation::default_process_new_state( state_idx next_state, state_idx def_state )
    {
    auto unit = owner->owner;
    if ( next_state == state_idx::STOP )
        {
        // По сигналам операция может быть остановлена.
        unit->set_err_msg( "автоотключение по запросу",
            operation_num, 0, tech_object::ERR_MSG_TYPES::ERR_DURING_WORK );
        unit->set_mode( operation_num, state_idx::STOP );
        }
    else if ( next_state == def_state )
        {
        // По сигналам операция может перейти в последующее состояние.
        unit->set_mode( operation_num, def_state );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void operation::finalize()
    {
    //Для состояния OFF ничего не делаем, поэтому current_state > 0.
    if ( current_state > 0 && current_state < STATES_MAX )
        {
        states[ current_state ]->finalize();
        for ( int idx = IDLE; idx < STATES_MAX; idx++ )
            {
            states[ idx ]->reset_eval_time();
            }

        current_state = IDLE;
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
int operation::get_run_step() const
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

    for ( int idx = IDLE; idx < STATES_MAX; idx++  )
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
        return states[ current_state ]->get_active_step_set_time();
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
void operation::turn_off_active_step()
    {
    if ( current_state >= 0 && current_state < STATES_MAX )
        {
        states[ current_state ]->turn_off_active_step();
        }
    }
//-----------------------------------------------------------------------------
step* operation::add_step( const char* step_name, int next_step_n /*= -1 */,
    int step_duration_par_n /*= -1 */,
    int step_max_duration_par_n /*= -1 */, state_idx s_idx /*= RUN */)
    {
    if ( s_idx >= IDLE && s_idx < STATES_MAX )
        {
        return states[ s_idx ]->add_step( step_name, next_step_n,
            step_duration_par_n, step_max_duration_par_n );
        }

    return stub.add_step( step_name, next_step_n, step_duration_par_n,
        step_max_duration_par_n );
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
void operation::set_step_cooperate_time_par_n( int step_cooperate_time_par_n )
    {
    states[ RUN ]->set_step_cooperate_time_par_n( step_cooperate_time_par_n );
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
action::action( std::string name, u_int subgropups_cnt ) : 
    subgropups_cnt( subgropups_cnt ), name( name )
    {
    devices.emplace_back();
    for ( u_int i = 0; i < subgropups_cnt; i++ )
        {
        devices[ MAIN_GROUP ].emplace_back();
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
void action::finalize()
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
int action::check_devices( char* err_description, int size ) const
    {
    char *out = err_description;
    auto written_size = 0;
    auto free_size = size - 1;

    for ( auto &group : devices )
        {
        for ( auto &subgroup : group )
            {
            for ( auto dev : subgroup )
                {
                if ( dev->get_state() < 0 )
                    {
                    int par = int( ( *dev->get_err_par() )[ 1 ] );

                    if ( !( par & base_error::P_IS_SUPPRESS ) )
                        {
                        auto res = fmt::format_to_n(
                            out, free_size, "'{}', ", dev->get_name() );
                        free_size -= res.size;
                        written_size += res.size;
                        out = res.out;
                        if ( free_size < 0 )
                            {
                            break;
                            }
                        }
                    }
                }
            }
        }

    *out = '\0';
    if ( written_size > 2 ) //Есть ошибки.
        {
        //Описание не вместилось в заданное количество символов.
        if ( free_size < 0 )
            {     
            *( out - 1 ) = '.';
            *( out - 2 ) = '.';
            *( out - 3 ) = '.';
            return size;
            }
        else
            {
            //Убираем последние символы - ", ".
            *( out - 1 ) = '\0';
            *( out - 2 ) = '\0';
            //Выше перезаписали два символа и учитываем завершающий \0.
            return written_size - 2 + 1;
            }
        }

    return 0;
    }
//----------------------------------------------------------------------------
void action::add_dev( device *dev, u_int group /*= 0 */, u_int subgroup /*= 0 */ )
    {
    while ( group >= devices.size() )
        {
        devices.emplace_back();

        size_t last_idx = devices.size() - 1;
        while ( subgropups_cnt > devices[ last_idx ].size() )
            {
            devices[ last_idx ].emplace_back();
            }
        }

    while ( subgroup >= devices[ group ].size() )
        {
        devices[ group ].emplace_back();
        }

    devices[ group ][ subgroup ].push_back( dev );
    }
//-----------------------------------------------------------------------------
int action::set_int_property( const char* name, size_t idx, int value )
    {
    if ( G_DEBUG )
        {
        G_LOG->info( "\"%s\" set int property \"%s\"[%zu] to \"%d\"",
            this->name.c_str(), name, idx, value );
        }
    return 0;
    }
//-----------------------------------------------------------------------------
void action::clear_dev()
    {
    devices.clear();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
on_action::on_action():action( "Включать" )
    {
    }
//-----------------------------------------------------------------------------
void on_action::evaluate()
    {
    if ( is_empty() )
        {
        return;
        }

    auto& devs = devices[ MAIN_GROUP ][ MAIN_SUBGROUP ];
    for ( u_int i = 0; i < devs.size(); i++ )
        {
        devs[ i ]->on();
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
delay_on_action::delay_on_action() :action( "Включать с задержкой" ), start_time( get_millisec() )
    {
    }
//-----------------------------------------------------------------------------
void delay_on_action::init()
    {
    start_time = get_millisec();
    }
//-----------------------------------------------------------------------------
void delay_on_action::evaluate()
    {
    if ( is_empty() )
        {
        return;
        }

    auto& dev_groups = devices[ MAIN_GROUP ];
    for ( u_int idx = 0; idx < dev_groups.size(); idx++ )
        {
        if ( int param_idx = par_idx.size() > idx ? par_idx[idx] : 0; 
            param_idx > 0 )
            {
            auto dt = ( *par )[ param_idx ];
            if ( get_delta_millisec( start_time ) <= dt )
                {
                continue;
                }
            }

        auto& devs = devices[ MAIN_GROUP ][ idx ];
        for ( u_int i = 0; i < devs.size(); i++ )
            {
            devs[ i ]->on();
            }
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
//-----------------------------------------------------------------------------
delay_off_action::delay_off_action() :action( "Выключать с задержкой" ),
    start_time( get_millisec() )
    {
    }
//-----------------------------------------------------------------------------
void delay_off_action::init()
    {
    start_time = get_millisec();
    }
//-----------------------------------------------------------------------------
void delay_off_action::evaluate()
    {
    if ( is_empty() )
        {
        return;
        }

    auto& dev_groups = devices[ MAIN_GROUP ];
    for ( u_int idx = 0; idx < dev_groups.size(); idx++ )
        {
        int param_idx = par_idx.size() > idx ? par_idx[ idx ] : 0;
        int new_state = 0;
        if ( param_idx > 0 )
            {            
            auto dt = ( *par )[ param_idx ];
            if ( get_delta_millisec( start_time ) <= dt )
                {
                new_state = 1;
                }
            }

        auto& devs = devices[ MAIN_GROUP ][ idx ];
        for ( u_int i = 0; i < devs.size(); i++ )
            {
            devs[ i ]->set_state( new_state );
            }
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int required_DI_action::check( char* reason, unsigned int max_len ) const
    {
    *reason = 0;
    if ( is_empty() )
        {
        return 0;
        }

    auto &devs = devices[ MAIN_GROUP ][ MAIN_SUBGROUP ];
    for ( auto d : devs )
        {
        if ( !d->is_active() )
            {
            auto f_str = "нет сигнала \'{:.25} ({:.50})\'";
            auto out = fmt::format_to_n( reason, max_len - 1, f_str,
                d->get_name(), d->get_description() );
            *out.out = '\0';
            return 1;
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void required_DI_action::finalize()
    {
    // При завершении ничего не делаем.
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
checked_devices_action::checked_devices_action() : action( "Проверяемые устройства" )
    {
    }
//-----------------------------------------------------------------------------
void checked_devices_action::finalize()
    {
    // При завершении ничего не делаем.
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
    actions.push_back( new checked_devices_action() );
    actions.push_back( new on_action() );
    actions.push_back( new on_reverse_action() );
    actions.push_back( new off_action() );
    actions.push_back( new open_seat_action( is_mode, owner ) );

    actions.push_back( new required_DI_action() );
    actions.push_back( new DI_DO_action() );
    actions.push_back( new inverted_DI_DO_action() );
    actions.push_back( new AI_AO_action() );
    actions.push_back( new wash_action() );
    actions.push_back( new enable_step_by_signal() );
    actions.push_back( new delay_on_action() );
    actions.push_back( new delay_off_action() );
          
    if ( is_mode )
        {
        actions.push_back( new jump_if_action( "Переход в состояние по условию" ) );
        }
    else
        {
        actions.push_back( new jump_if_action( "Переход в шаг по условию" ) );
        }
    }
//-----------------------------------------------------------------------------
step::~step()
    {
    for ( u_int i = 0; i < actions.size(); i++  )
        {
        delete actions[ i ];
        actions[ i ] = nullptr;
        }
    }
//-----------------------------------------------------------------------------
int step::check( char* reason, unsigned int max_len ) const
    {

    auto res = actions[ A_DI_DO ]->check( reason, max_len );
    if ( res ) return res;
    res = actions[ A_AI_AO ]->check( reason, max_len );
    if ( res ) return res;
    res = actions[ A_INVERTED_DI_DO ]->check( reason, max_len );
    if ( res ) return res;

    if ( is_mode )
        {
        return actions[ A_REQUIRED_FB ]->check( reason, max_len );
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
void step::finalize()
    {
    for ( u_int i = 0; i < actions.size(); i++  )
        {
        actions[ i ]->finalize();
        }
    dx_time = 0;
    active = false;
    }
//-----------------------------------------------------------------------------
u_long step::get_eval_time() const
    {
    return get_delta_millisec( start_time ) + dx_time;
    }
//-----------------------------------------------------------------------------
u_long step::get_latest_eval_time() const
    {
    return get_delta_millisec( start_time );
    }
//-----------------------------------------------------------------------------
void step::set_start_time( u_int_4 start_time )
    {
    this->start_time = start_time;
    }
//-----------------------------------------------------------------------------
void step::print( const char* prefix /*= "" */ ) const
    {
    printf( "%s\"%s\"\n", prefix, name.c_str() );
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
        int res = actions[ i ]->check_devices( err_dev_name, str_len );

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
DI_DO_action::DI_DO_action( std::string name ) :action( name )
    {
    }
//-----------------------------------------------------------------------------
int DI_DO_action::check( char* reason, unsigned int max_len ) const
    {
    reason[ 0 ] = 0;
    if ( is_empty() )
        {
        return 0;
        }

    auto &devs = devices[ MAIN_GROUP ];
    for ( const auto& dev_group : devs )
        {
        if ( dev_group.empty() )
            {
            continue;
            }

        // Проверяем порядок устройств: сначала DI, затем DO
        bool found_do = false;
        for ( const auto& device_ptr : dev_group )
            {
            auto device_type = device_ptr->get_type();
            
            // Проверяем, что устройство является допустимым типом (DI или DO)
            if ( !is_di_device_type( device_type ) &&
                device_type != device::DT_DO )
                {
                auto format_str = R"(в поле '{}' устройство '{:.25} ({:.50})' )"
                    R"(не является допустимым сигналом (DI, SB, GS, LS, FS, DO))";
                auto out = fmt::format_to_n( reason, max_len - 1, format_str, name.c_str(),
                    device_ptr->get_name(), device_ptr->get_description() );
                *out.out = '\0';
                return 1;
                }
            
            // Проверяем правильность порядка: DI сигналы должны идти перед DO
            auto is_di_type = is_di_device_type( device_type );
            auto is_do_type = device_type == device::DT_DO;
            
            if ( is_do_type )
                {
                found_do = true;
                }
            else if ( is_di_type && found_do )
                {
                // Найден DI сигнал после DO сигнала - ошибка порядка
                auto format_str = R"(в поле '{}' устройство '{:.25} ({:.50})' )"
                    R"(расположено неправильно: DI сигналы должны быть описаны перед DO сигналами)";
                auto out = fmt::format_to_n( reason, max_len - 1, format_str, name.c_str(),
                    device_ptr->get_name(), device_ptr->get_description() );
                *out.out = '\0';
                return 1;
                }
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

    const auto &devs = devices[ MAIN_GROUP ];

    for ( const auto& dev_group : devs )
        {
        if ( dev_group.empty() )
            {
            continue;
            }

        evaluate_DO( dev_group );
        }
    }
//-----------------------------------------------------------------------------
void DI_DO_action::finalize()
    {
    if ( is_empty() )
        {
        return;
        }

    const auto& devs = devices[ MAIN_GROUP ];
    for ( const auto& dev_group : devs )
        {
        if ( dev_group.empty() )
            {
            continue;
            }

        // Найдем количество DI устройств и выключим все DO
        u_int di_count = 0;
        for ( const auto& dev : dev_group )
            {
            if ( is_di_device_type( dev->get_type() ) )
                {
                di_count++;
                }
            }
        
        // Выключаем все DO устройства
        for ( auto it = dev_group.begin() + di_count;
            it != dev_group.end(); ++it )
            {
            (*it)->off();
            }
        }    
    }
//-----------------------------------------------------------------------------
void DI_DO_action::evaluate_DO( std::vector< device* > devices )
    {
    // Поиск активных DI среди всех входных устройств
    bool any_di_active = false;
    u_int di_count = 0;
    
    // Подсчитаем количество DI устройств и проверим их активность
    for ( const auto& dev : devices )
        {
        if ( is_di_device_type( dev->get_type() ) )
            {
            di_count++;
            if ( dev->is_active() )
                {
                any_di_active = true;
                }
            }
        }

    // Управляем DO устройствами (они идут после всех DI)
    for ( auto it = devices.begin() + di_count; it != devices.end(); ++it )
        {
        if ( any_di_active )
            {
            (*it)->on();
            }
        else
            {
            (*it)->off();
            }
        }
    }
//-----------------------------------------------------------------------------
bool DI_DO_action::is_di_device_type( device::DEVICE_TYPE device_type ) const
    {
    if ( device_type == device::DT_DI ||
        device_type == device::DT_SB ||
        device_type == device::DT_GS ||
        device_type == device::DT_LS ||
        device_type == device::DT_FS )
        return true;

    return false;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inverted_DI_DO_action::inverted_DI_DO_action():
    DI_DO_action( "Группы инвертированный DI->DO's" )
    {
    }
//-----------------------------------------------------------------------------
void inverted_DI_DO_action::evaluate_DO( std::vector< device* > devices )
    {
    // Поиск активных DI среди всех входных устройств
    bool any_di_active = false;
    u_int di_count = 0;
    
    // Подсчитаем количество DI устройств и проверим их активность
    for ( const auto& dev : devices )
        {
        if ( is_di_device_type( dev->get_type() ) )
            {
            di_count++;
            if ( dev->is_active() )
                {
                any_di_active = true;
                }
            }
        }

    // Инвертированная логика: DO активно, когда НИ ОДИН DI не активен
    int new_state = any_di_active ? 0 : 1;
    
    // Управляем DO устройствами (они идут после всех DI)
    for ( auto it = devices.begin() + di_count; it != devices.end(); ++it )
        {
        (*it)->set_state( new_state );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AI_AO_action::AI_AO_action() :action( "Группы AI->AO's" )
    {
    }
//-----------------------------------------------------------------------------
int AI_AO_action::check( char* reason, unsigned int max_len ) const
    {
    reason[ 0 ] = 0;
    if ( is_empty() )
        {
        return 0;
        }

    auto& devs = devices[ MAIN_GROUP ];
    for ( u_int i = 0; i < devs.size(); i++ )
        {
        if ( devs[ i ].empty() )
            {
            continue;
            }

        auto do_device = devs[ i ][ 0 ];
        if ( do_device->get_type() != device::DT_AI &&
            do_device->get_type() != device::DT_PT &&
            do_device->get_type() != device::DT_LT &&
            do_device->get_type() != device::DT_FQT &&
            do_device->get_type() != device::DT_QT &&
            do_device->get_type() != device::DT_TE )
            {
            auto format_str = R"(в поле '{}' устройство '{:.25} ({:.50})' )"
                R"(не является входным сигналом (АI, PT, LT, FQT, QT, TE))";
            auto out = fmt::format_to_n( reason, max_len - 1, format_str, name.c_str(),
                do_device->get_name(), do_device->get_description() );
            *out.out = '\0';
            return 1;
            }
        }

    return 0;
    }
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
void AI_AO_action::finalize()
    {
    if ( is_empty() )
        {
        return;
        }

    auto& devs = devices[ MAIN_GROUP ];
    for ( u_int i = 0; i < devs.size(); i++ )
        {
        if ( devs[ i ].empty() )
            {
            continue;
            }

        for ( u_int j = 1; j < devs[ i ].size(); j++ )
            {
            devs[ i ][ j ]->off();
            }
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
open_seat_action::open_seat_action( bool is_mode, operation_state *owner ) :
    action( "Промывка седел" ),
    phase( P_WAITING ),
    next_phase( PHASES::P_OPEN_UPPER ),
    active_group_n( 0 ),
    wait_time( 60000 ),
    start_cycle_time( 0 ),
    is_mode( is_mode ),
    owner( owner )
    {
    }
//-----------------------------------------------------------------------------
#ifdef PTUSA_TEST
void open_seat_action::set_wait_time( int wait_time )
    {
    this->wait_time = wait_time;
    }

int open_seat_action::get_wait_time()
    {
    return wait_time;
    }
#endif
//----------------------------------------------------------------------------
void open_seat_action::init()
    {
    if ( wash_upper_seat_devices.empty() && wash_lower_seat_devices.empty() )
        {
        return;
        }

    start_cycle_time  = get_millisec();
    phase             = P_WAITING;
    next_phase        = P_OPEN_UPPER;
    active_group_n    = 0;

    auto groups_cnt =  wash_upper_seat_devices.size() +
        wash_lower_seat_devices.size();

    saved_params_u_int_4 &par = PAC_info::get_instance()->par;
    if ( is_mode )     
        {
        // Для операции - значение параметра.
        wait_time = par[ PAC_info::P_MIX_FLIP_PERIOD ] * 1000;
        }
    else
        {
        // Для шага - время шага.
        wait_time = owner->get_active_step_set_time();
        }

    // Для одной группы - середина продолжительности, для двух групп - треть и
    // т.д.
    auto wash_time_upper = G_PAC_INFO()->par[ PAC_info::P_MIX_FLIP_UPPER_TIME ];
    auto wash_time_lower = G_PAC_INFO()->par[ PAC_info::P_MIX_FLIP_LOWER_TIME ];
    auto max_flip_time = wash_time_upper > wash_time_lower ?
        wash_time_upper : wash_time_lower;
    wait_time = ( wait_time - max_flip_time * groups_cnt ) / ( groups_cnt + 1 );

    active_group_n = 0;
    }
//-----------------------------------------------------------------------------
void open_seat_action::evaluate()
    {
    if ( wash_lower_seat_devices.empty() && wash_upper_seat_devices.empty() ) return;

    auto wash_time_upper = G_PAC_INFO()->par[ PAC_info::P_MIX_FLIP_UPPER_TIME ];
    auto wash_time_lower = G_PAC_INFO()->par[ PAC_info::P_MIX_FLIP_LOWER_TIME ];

    switch ( phase )
        {
    case P_WAITING:
        {
        bool check_is_wash_seat_active = true;
        switch_off( wash_lower_seat_devices, check_is_wash_seat_active );
        switch_off( wash_upper_seat_devices, check_is_wash_seat_active );

        // Пора промывать седла.
        if ( get_delta_millisec( start_cycle_time ) > wait_time )
            {
            phase = next_phase;
            start_cycle_time = get_millisec();
            }
        break;
        }

    case P_OPEN_UPPER:
        if ( wash_upper_seat_devices.empty() )
            {
            phase      = P_OPEN_LOWER;
            next_phase = P_OPEN_LOWER;
            break;
            }

        if ( get_delta_millisec( start_cycle_time ) < wash_time_upper )
            {
            switch_on_group( wash_upper_seat_devices[ active_group_n ],
                valve::V_UPPER_SEAT );
            }
        else //Время промывки седел вышло.
            {
            finalize();

            phase = P_WAITING;
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
            phase = P_OPEN_UPPER;
            next_phase = P_OPEN_UPPER;
            break;
            }

        if ( get_delta_millisec( start_cycle_time ) < wash_time_lower )
            {
            switch_on_group( wash_lower_seat_devices[ active_group_n ],
                valve::V_LOWER_SEAT );
            }
        else //Время промывки седел вышло.
            {
            finalize();

            phase = P_WAITING;
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
void open_seat_action::finalize()
    {
    if ( wash_upper_seat_devices.empty() && wash_lower_seat_devices.empty() ) return;

    switch ( phase )
        {
        case P_WAITING:
            break;

        case P_OPEN_UPPER:
            if ( wash_upper_seat_devices.empty() ) break;
            switch_off_group( wash_upper_seat_devices[ active_group_n ] );
            break;

        case P_OPEN_LOWER:
            if ( wash_lower_seat_devices.empty() ) break;
            switch_off_group( wash_lower_seat_devices[ active_group_n ] );
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
void open_seat_action::switch_off( std::vector< std::vector< device* > > devices,
    bool is_check )
    {
    for ( u_int i = 0; i < devices.size(); i++ )
        {
        switch_off_group( devices[ i ], is_check );
        }
    };
//-----------------------------------------------------------------------------
void open_seat_action::switch_off_group( std::vector< device* > group,
    bool is_check )
    {
    for ( u_int i = 0; i < group.size(); i++ )
        {
        auto dev = group[ i ];
        auto type = dev->get_type();
        if ( type == device::DT_V )
            {
            valve* v = reinterpret_cast<valve*>( dev );
            if ( is_check )
                {
                if ( !v->is_wash_seat_active() ) v->off();
                }
            else
                {
                v->off();
                v->set_seat_wash_state( false );
                }
            }
        else
            {
            dev->off();
            }
        }
    };
//-----------------------------------------------------------------------------
void open_seat_action::switch_on_group( std::vector< device* > group,
    valve::VALVE_STATE st )
    {
    for ( u_int i = 0; i < group.size(); i++ )
        {
        auto dev = group[ i ];
        auto type = dev->get_type();
        if ( type == device::DT_V )
            {
            valve* v = reinterpret_cast<valve*>( dev );
            v->set_state( st );
            v->set_seat_wash_state( true );
            }
        else
            {
            dev->on();
            }
        }
    };
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

        //Подаем сигналы "ОК".
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
        // сигналы "ОК".
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
void wash_action::finalize()
    {
    if ( is_empty() )
        {
        return;
        }

    for ( u_int idx = 0; idx < devices.size(); idx++ )
        {
        auto& devs = devices[ idx ];

        for ( u_int i = 0; i < devs[ G_DO ].size(); i++ )
            {
            devs[ G_DO ][ i ]->off();
            }
        for ( u_int i = 0; i < devs[ G_DEV ].size(); i++ )
            {
            devs[ G_DEV ][ i ]->off();            
            }
        for ( u_int i = 0; i < devs[ G_REV_DEV ].size(); i++ )
            {
            devs[ G_REV_DEV ][ i ]->off();
            }    
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
jump_if_action::jump_if_action( const char* name ) :
    action( name, G_GROUPS_CNT)
    {
    }
//-----------------------------------------------------------------------------
bool jump_if_action::is_jump( int& next )
    {
    next = -1;
    if ( next_n.empty() )
        {
        return false;
        }

    for ( size_t idx = 0; idx < devices.size(); idx++ )
        {
        if ( idx < next_n.size() ) next = next_n[ idx ];
   
        auto res = check( devices[ idx ][ G_ON_DEVICES ], true ) &&
            check( devices[ idx ][ G_OFF_DEVICES ], false );

        if ( res ) return true;
        }

    return false;
    }
//-----------------------------------------------------------------------------
bool jump_if_action::check(
    const std::vector< device* >& checked_devices, bool check_is_opened ) const
    {
    for ( auto dev : checked_devices )
        {
        auto type = dev->get_type();
        if ( type == device::DT_V )
            {
            auto v = dynamic_cast<valve*>( dev );
            if ( check_is_opened && !v->is_opened() ||
                !check_is_opened && !v->is_closed() ) return false;
            }
        else
            {
            if ( check_is_opened && !dev->is_active() ||
                !check_is_opened && dev->is_active() ) return false;
            }
        }
    return true;
    };
//-----------------------------------------------------------------------------
int jump_if_action::set_int_property( const char* name, size_t idx, int value )
    {
    action::set_int_property( name, idx, value );
    if ( strcmp( name, "next_step_n" ) == 0 || //Для перехода к новому шагу.
        strcmp( name, "next_state_n" ) == 0 )  //Для перехода к новому состоянию.
        {
        while ( idx >= next_n.size() )
            {
            next_n.push_back( -1 );
            }

        next_n[ idx ] = value;
        return 0;
        }
    else
        {
        if ( G_DEBUG )
            {
            G_LOG->warning( "\"%s\" unknown property \"%s\"",
                this->name.c_str(), name );
            }
        }

    return 1;
    };
//-----------------------------------------------------------------------------
int jump_if_action::get_int_property( const char* name, size_t idx )
    {
    if ( strcmp( name, "next_step_n" ) == 0 && idx < next_n.size() )
        {
        return next_n[ idx ];
        }

    return -1;
    }
//-----------------------------------------------------------------------------
void jump_if_action::finalize()
    {
    return;
    }
//-----------------------------------------------------------------------------
void jump_if_action::print( const char* prefix, bool new_line ) const
    {
    action::print( prefix, false );
    printf( " { " );
    std::for_each( next_n.begin(), next_n.end(), [&]( int const& n )
        {
        printf( "%d ", n );
        } );
    printf( "}" );
    if ( new_line )
        {
        printf( "\n" );
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
enable_step_by_signal::enable_step_by_signal() :action( "Включить шаг по сигналам" )
    {
    };
//-----------------------------------------------------------------------------
bool enable_step_by_signal::is_any_group_active() const
    {
    if ( is_empty() )
        {
        return false;
        }

    auto& main_group = devices[ MAIN_GROUP ];
    for( auto& group : main_group )
        {
        auto is_group_ok = true;
        for( auto& dev : group )
            {
            if ( !dev->is_active() )
                {
                is_group_ok = false;
                break;
                }
            }
        if ( is_group_ok ) return true;
        }

    return false;
    };
//-----------------------------------------------------------------------------
bool enable_step_by_signal::should_turn_off() const
    {    
    if ( is_empty() )
        {
        return true;
        }

    return turn_off_flag;
    };
//-----------------------------------------------------------------------------
int enable_step_by_signal::set_bool_property( const char* name, bool value )
    {
    if ( strcmp( name, "should_turn_off" ) == 0 )
        {
        turn_off_flag = value;
        }
    else
        {
        if ( G_DEBUG )
            {
            G_LOG->warning( "\"%s\" unknown property \"%s\"",
                this->name.c_str(), name);
            }
        return 1;
        }

    return 0;
    };
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
    operation_number( n ),
    dx_step_time( 0 )
    {    
    mode_step[ 0 ][ step::A_WASH ]->set_params( owner->get_params() );
    mode_step[ 0 ][ step::A_DELAY_ON ]->set_params( owner->get_params() );
    mode_step[ 0 ][ step::A_DELAY_OFF ]->set_params( owner->get_params() );
    mode_step[ 0 ][ step::A_ENABLE_STEP_BY_SIGNAL ]->set_params( owner->get_params() );
    }
//-----------------------------------------------------------------------------
operation_state::~operation_state()
    {
    delete mode_step;
    mode_step = nullptr;

    for ( size_t idx = 0; idx < steps.size(); idx++ )
        {
        delete steps[ idx ];
        steps[ idx ] = nullptr;
        }
    }
//-----------------------------------------------------------------------------
step* operation_state::add_step( const char* step_name, int next_step_n /*= -1 */,
    int step_duration_par_n /*= -1 */, int step_max_duration_par_n /*= -1 */ )
    {
    steps.push_back( new step( step_name, this ) );
    step* new_step = steps[ steps.size() - 1 ];
    ( *new_step )[ step::A_WASH ]->set_params( owner->get_params() );
    ( *new_step )[ step::A_DELAY_ON ]->set_params( owner->get_params() );
    ( *new_step )[ step::A_DELAY_OFF ]->set_params( owner->get_params() );
    ( *new_step )[ step::A_ENABLE_STEP_BY_SIGNAL ]->set_params( owner->get_params() );

    next_step_ns.push_back( next_step_n );
    step_duration_par_ns.push_back( step_duration_par_n );

    step_max_duration_par_ns.push_back( step_max_duration_par_n );

    return steps[ steps.size() - 1 ];
    }
//-----------------------------------------------------------------------------
int operation_state::check_on( char* reason, unsigned int max_len ) const
    {
    for ( size_t idx = 0; idx < steps.size(); idx++ )
        {
        int res = steps[ idx ]->check( reason, max_len );
        if ( res ) return res;
        }

    return mode_step->check( reason, max_len );
    }
//-----------------------------------------------------------------------------
void operation_state::init( u_int start_step /*= 1 */ )
    {
    if ( G_DEBUG )
        {
        mode_step->print( owner->owner->get_prefix() );
        }
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

    //Process action "enable_step_by_signal".
    size_t idx = 0;
    while ( idx < active_steps.size() )
        {        
        if ( size_t step_n = active_steps[ idx ] - 1; step_n < steps.size() )
            {
            steps[ step_n ]->evaluate();
            if ( auto enable_action = dynamic_cast<enable_step_by_signal*>(
                ( *steps[ step_n ] )[ step::A_ENABLE_STEP_BY_SIGNAL ] ); 
                enable_action && !enable_action->is_empty() &&
                !enable_action->is_any_group_active() &&
                enable_action->should_turn_off() )
                {
                off_extra_step( static_cast<int>( step_n + 1 ) );
                continue;
                }

            auto duration = active_steps_duration[ idx ];
            auto start_t = active_steps_start_time[ idx ];
            if ( duration > 0 && get_delta_millisec( start_t ) > duration )
                {
                off_extra_step( static_cast<int>( step_n + 1 ) );
                continue;
                }
            }

        idx++;
        }
    for ( size_t i = 0; i < steps.size(); i++ )
        {
        if ( !is_active_extra_step( static_cast<int>( i ) + 1 ) )
            {
            auto step = steps[ i ];            
            if ( auto enable_action = dynamic_cast<enable_step_by_signal*>(
                ( *step )[ step::A_ENABLE_STEP_BY_SIGNAL ] ); 
                enable_action && enable_action->is_any_group_active() )
                {
                on_extra_step( static_cast<int>( i ) + 1 );
                }
            }
        }

    if ( active_step_n < 0 ) return;

    //Максимальное время шага
    active_step_max_time = 0;
    int par_n = step_max_duration_par_ns[ active_step_n ];
    if ( par_n > 0 )
        {
        active_step_max_time = u_int( owner->get_step_param( par_n ) );
        }

    //Время шага
    par_n = step_duration_par_ns[ active_step_n ];
    if ( par_n > 0 )
        {
        active_step_time = u_int( owner->get_step_param( par_n ) * 1000L );

        if ( active_step_time == 0 ||
            steps[ active_step_n ]->get_eval_time() > (u_int)active_step_time )
            {
            if ( -1 == active_step_next_step_n )
                {
                if ( operation_number > 0 )
                    {
                    int time = (int)owner->get_step_param( par_n );
                    const int MAX_BUFF_SIZE = 200;
                    char buff[ MAX_BUFF_SIZE ] = { 0 };
                    std::snprintf( buff, MAX_BUFF_SIZE,
                        "вышло время (%u сек) последнего шага (\'%s\')",
                        time, steps[ active_step_n ]->get_name() );
                    owner->owner->set_err_msg( buff, operation_number, 0, i_tech_object::ERR_OFF );
                    owner->off_mode( operation_number );
                    }
                else
                    {
                    finalize(); //Для операции-заглушки.
                    }
                }
            else
                {
                auto step_switch_time = 0UL;
                if ( step_cooperate_time_par_n > 0 &&
                    owner->get_step_param( step_cooperate_time_par_n ) > 0 )
                    {
                    step_switch_time = static_cast<u_long>
                        ( owner->get_step_param( step_cooperate_time_par_n ) );
                    }

                to_step( active_step_next_step_n, step_switch_time );
                }

            return;
            }
        }

    //Переход по условию к следующему шагу.
    auto active_step = steps[ active_step_n ];
    auto action = ( *active_step )[ step::A_JUMP_IF ];
    auto if_action = static_cast<jump_if_action*>( action );
    if ( int next_step = -1; if_action->is_jump( next_step ) )
        {
        if ( next_step >= 0 )
            {
            if ( G_DEBUG )
                {
                printf( "Переход к новому шагу. " );
                if_action->print();
                }
            to_step( next_step );
            }
        }

    steps[ active_step_n ]->evaluate();
    }
//-----------------------------------------------------------------------------
void operation_state::finalize()
    {
    mode_step->finalize();
    start_time = get_millisec();

    //Если активный шаг не завершился, сохраняем время его выполнения для
    //возобновления в случае снятия с паузы.
    if ( active_step_n >= 0 )
        {
        dx_step_time = active_step_evaluation_time();
        }

    if ( active_step_n >= 0 )
        {
        steps[ active_step_n ]->finalize();
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
            steps[ step_n ]->finalize();
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
    active_steps_duration.clear();
    active_steps_start_time.clear();
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

    auto prev_active_step_n = active_step_n;
    active_step_time        = 0;
    active_step_next_step_n = -1;

    active_step_n = new_step - 1;
    active_step_next_step_n = next_step_ns[ active_step_n ];

    if ( prev_active_step_n >= 0 )
        {
        steps[ prev_active_step_n ]->finalize();
        if ( cooperative_time > 0 )
            {
            on_extra_step( prev_active_step_n + 1, cooperative_time );
            }
        }

    //Время шага
    auto par_n = step_duration_par_ns[ active_step_n ];
    if ( par_n > 0 )
        {
        active_step_time = u_int( owner->get_step_param( par_n ) * 1000L );
        }

    if ( active_step_time > 0 || par_n <= 0 )
        {
        steps[ active_step_n ]->init();
        steps[ active_step_n ]->evaluate();
        }

    if ( G_DEBUG )
        {        
        fmt::print( R"({}"{}" operation {} "{}" to_step() -> {})",
            owner->owner->get_prefix(), owner->owner->get_name(),
            operation_number, name.c_str(), new_step );

        if ( active_step_time > 0 )
            {
            fmt::print( ", step time {} ms", active_step_time );
            }
        if ( active_step_next_step_n > 0 )
            {
            fmt::print( ", next step {}", active_step_next_step_n );
            }

        active_step_max_time = 0;
        if ( auto max_t_par_n = step_max_duration_par_ns[active_step_n];
            max_t_par_n > 0 )
            {
            active_step_max_time = u_int( owner->get_step_param( max_t_par_n ) );
            if ( active_step_max_time )
                {
                fmt::print( ", max step time {} s", active_step_max_time );
                }
            }
        if ( cooperative_time )
            {
            fmt::print( ", cooperative time {} ms", cooperative_time );
            }

        fmt::println( "" );

        steps[ active_step_n ]->print( owner->owner->get_prefix() );
        }
    }
//-----------------------------------------------------------------------------
void operation_state::to_next_step()
    {
    if ( active_step_n >= 0 )
        {
        if ( next_step_ns[ active_step_n ] == -1 )
            {
            int current_step = active_step_n + 1;
            int next_step = current_step + 1;
            to_step( next_step );
            }
        else
            {
            int next_step = next_step_ns[ active_step_n ];
            to_step( next_step );
            }
        }
     }
//-----------------------------------------------------------------------------
void operation_state::turn_off_active_step()
    {
    if ( active_step_n >= 0 )
        {
        steps[ active_step_n ]->finalize();
        if ( G_DEBUG )
            {
            printf( "%sFINAL ACTIVE STEP №%d\n",
                owner->owner->get_prefix(), active_step_n + 1 );
            }
        active_step_n = -1;
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
int operation_state::check_devices( char* err_dev_name, unsigned int str_len )
    {
    int res = mode_step->check_devices( err_dev_name, str_len );

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
        res = steps[ active_step_n ]->check_devices( err_dev_name, str_len );

        if ( res )
            {
            return 1;
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int operation_state::check_max_step_time( char* err_dev_name, unsigned int str_len )
    {
    if ( step_max_duration_par_ns.empty() ) // Не заданы параметры максимального времени.
        return 0;

    auto make_str = [&]( unsigned int step_n, int step_max_time )
        {
        auto res = fmt::format_to_n( err_dev_name, str_len - 1,
            "превышено макс. t ({} с) шага {} \'{}\'", step_max_time,
            step_n + 1, steps[ step_n ]->get_name() );        
        if ( res.size > str_len - 1 )
            {
            // Резерв для записи сокращения вида "...'" в конце строки при
            // превышения ограничения длины.
            const unsigned int OFFSET = 4;
            res.out -= OFFSET;
            // Удаляем часть некорректного utf8 символа при его наличии.
            if ( static_cast<unsigned char>( *( res.out - 1 ) ) == 0xD0 )
                res.out--;

            *res.out++ = '.';
            *res.out++ = '.';
            *res.out++ = '.';
            *res.out++ = '\'';
            }
        *res.out = '\0';
        };

    if ( active_step_n >= 0 && (unsigned int)active_step_n < steps.size() &&
        active_step_max_time && // Максимальное время - ненулевое значение.
        steps[ active_step_n ]->get_latest_eval_time() >=
        1000UL * static_cast<u_int_4>( active_step_max_time ) )
        {
        make_str( active_step_n, active_step_max_time );
        return 1;
        }

    for ( auto a_step_n : active_steps )
        {
        int par_n = step_max_duration_par_ns[ a_step_n - 1 ];
        if ( par_n > 0 )
            {
            auto extra_active_step_max_time =
                static_cast<unsigned int> ( owner->get_step_param( par_n ) );

            if ( extra_active_step_max_time &&
                steps[ a_step_n - 1 ]->get_latest_eval_time() >=
                1000UL * extra_active_step_max_time )
                {
                make_str( a_step_n - 1, extra_active_step_max_time );
                return 1;
                }
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int operation_state::check_steps_params( char* err_dev_name, unsigned int str_len )
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
bool operation_state::is_empty() const
    {
    if ( !mode_step->is_empty() ) return false;
    for ( auto st : steps )
        {
        if ( !st->is_empty() ) return false;
        }
    return true;
    }
//-----------------------------------------------------------------------------
bool operation_state::is_goto_next_state( int& next_state ) const
    {
    auto action = ( *mode_step )[ step::A_JUMP_IF ];
    auto to_new_state = static_cast<jump_if_action*>( action );
    return to_new_state->is_jump( next_state );
    }
//-----------------------------------------------------------------------------
void operation_state::set_step_cooperate_time_par_n(
    int step_cooperate_time_par_number )
    {
    step_cooperate_time_par_n = step_cooperate_time_par_number;
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
    // Выключаем перед сохранением все активные шаги, которые должны
    // отключиться по времени.
    size_t idx = 0;
    while ( idx < active_steps.size() )
        {        
        if ( auto duration = active_steps_duration[ idx ]; duration > 0 )
            {
            off_extra_step( active_steps[ idx ] );
            }
        else
            {
            idx++;
            }
        }

    saved_active_steps.assign( active_steps.begin(), active_steps.end() );
    }
//-----------------------------------------------------------------------------
void operation_state::load()
    {
    active_steps.assign( saved_active_steps.begin(), saved_active_steps.end() );

    // Инициализация массивов длительности и времени старта
    // для восстановленных дополнительных шагов.
    const auto now = get_millisec();
    active_steps_duration.assign( active_steps.size(), 0UL );
    active_steps_start_time.assign( active_steps.size(), now );
    }
//-----------------------------------------------------------------------------
int operation_state::on_extra_step( int step_idx, u_long step_time /* = 0UL */ )
    {
    if ( (size_t) step_idx > steps.size() )
        {
        G_LOG->notice( "operation_state:on_extra_step(...) - step (%d) > size (%zu)",
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
        char err_str[ 250 ];
        if ( steps[ step_idx - 1 ]->check( err_str, sizeof( err_str ) ) == 0 )
            {
            active_steps.push_back( step_idx );
            active_steps_duration.push_back( step_time );
            active_steps_start_time.push_back( get_millisec() );
            steps[ step_idx - 1 ]->init();
            steps[ step_idx - 1 ]->evaluate();

            if ( G_DEBUG )
                {
                SetColor( YELLOW );
                printf( "%s\"%s\" operation %d \"%s\" on_extra_step() -> %d (%lu ms).\n",
                    owner->owner->get_prefix(),
                    owner->owner->get_name(), operation_number, name.c_str(),
                    step_idx, step_time );
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
        G_LOG->notice( "operation_state:off_extra_step(...) - step (%d) > size (%zu)",
            step_idx, steps.size() );
        return 0;
        }

    if ( auto res = std::find(active_steps.begin(), active_steps.end(), step_idx); 
        res != active_steps.end() )
        {
        auto pos = std::distance( active_steps.begin(), res );
        steps[ step_idx - 1 ]->finalize();
        active_steps.erase( res );
        active_steps_duration.erase( active_steps_duration.begin() + pos );
        active_steps_start_time.erase( active_steps_start_time.begin() + pos );        
       
        if ( G_DEBUG )
            {
            SetColor( YELLOW );
            printf( "%s\"%s\" operation %d \"%s\" off_extra_step() -> %d.\n",
                owner->owner->get_prefix(),
                owner->owner->get_name(), operation_number, name.c_str(), step_idx );
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
        G_LOG->notice( "operation_state:switch_active_step(...) - step (%d) > size (%zu)",
            on_step, steps.size() );
        return 1;
        }

    if ( 1 + active_step_n == on_step )
        {
        G_LOG->notice( "operation_state:switch_active_step(...) - step (%d) is active",
            on_step );
        return 0;
        }

    if ( auto res = std::find(active_steps.begin(), active_steps.end(), off_step); 
        res != active_steps.end() )
        {
        steps[ off_step - 1 ]->finalize();
        steps[ on_step - 1 ]->init();
        steps[ on_step - 1 ]->evaluate();

        if ( G_DEBUG )
            {
            SetColor( YELLOW );
            printf( "%s\"%s\" operation %d \"%s\" switch_active_extra_step() %d -> %d.\n",
                owner->owner->get_prefix(),
                owner->owner->get_name(), operation_number, name.c_str(),
                off_step, on_step );
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
    if ( auto res = std::find(active_steps.begin(), active_steps.end(), step_idx); 
        res != active_steps.end() ) return true;

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

    return &oper_stub;
    }
//-----------------------------------------------------------------------------
unsigned long operation_manager::get_idle_time()
    {
    auto res = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - start_time_point );

    return static_cast<unsigned long>( res.count() );
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
operation_manager::operation_manager( i_tech_object *owner ):
    owner( owner )    
    {
    }
//-----------------------------------------------------------------------------
operation_manager::~operation_manager()
    {
    for ( size_t i = 0; i < operations.size(); i++ )
        {
        delete operations[ i ];
        operations[ i ] = nullptr;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
