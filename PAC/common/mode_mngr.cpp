/// @file  mode_mngr.cpp
/// @brief Содержит реализацию классов mode_mngr.h.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c USE_NO_COMB - компиляция без гребенки (объекта g_greb).@n
/// @c DEBUG       - отладочная компиляцию с выводом дополнительной информации 
/// в консоль.
/// 
/// @par Текущая версия:
/// @$Rev$.@n
/// @$Author$.@n
/// @$Date::                     $.

#include "mode_mngr.h"
//-----------------------------------------------------------------------------
action::action( std::string name ) : name( name )
    {
    }
//-----------------------------------------------------------------------------
void action::print( const char* prefix /*= "" */ ) const
    {
    Print( "%s%s: ", prefix, name.c_str() );

    for ( u_int i = 0; i < devices.size(); i++ )
        {
        Print( "%s ", devices[ i ]->get_name() );
        }

    Print( "\n" );
    }
//-----------------------------------------------------------------------------
void action::final()
    {
    for ( u_int i = 0; i < devices.size(); i++ )
        {
        devices[ i ]->off();
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void on_action::evaluate()
    {
    for ( u_int i = 0; i < devices.size(); i++ )
        {
        devices[ i ]->on();
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void off_action::evaluate()
    {
    for ( u_int i = 0; i < devices.size(); i++ )
        {
        devices[ i ]->off();
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int required_DI_action::check( char* reason ) const
    {
    for ( u_int i = 0; i < devices.size(); i++ )
        {
        if ( !devices[ i ]->is_active() ) 
            {
            sprintf( reason, "нет сигнала \'%.25s (%.50s)\'",
                devices[ i ]->get_name(), devices[ i ]->get_description() );
            return 1;
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
step::step( std::string name, bool is_mode /*= false */ ) : start_time( 0 ),
    is_mode( is_mode ),
    name( name )
    {
    actions.push_back( new on_action() );
    actions.push_back( new off_action() );
    actions.push_back( new open_seat_action() );
    
    if ( is_mode )
        {
        actions.push_back( new required_DI_action() );

        actions.push_back( new DI_DO_action() );
        }
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
void step::final() const
    {
    for ( u_int i = 0; i < actions.size(); i++  )
        {
        actions[ i ]->final();
        }
    }
//-----------------------------------------------------------------------------
u_int_4 step::get_start_time() const
    {
    return start_time;
    }
//-----------------------------------------------------------------------------
void step::set_start_time( u_int_4 start_time )
    {
    this->start_time = start_time;
    }
//-----------------------------------------------------------------------------
void step::print( const char* prefix /*= "" */ ) const
    {
    Print( "%s%s \n", prefix, name.c_str() );
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

#ifdef DEBUG
    Print( "Error step::action* operator[] ( int idx ) - idx %d > count %d.\n",
        idx, actions.size() );
#endif // DEBUG

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int DI_DO_action::check( char* reason ) const
    {
    if ( devices.size() % 2 != 0 )
        {
#ifdef DEBUG
        Print( "Error DI_DO_action: devices.size %d не кратен 2.",
            devices.size() );
#endif // DEBUG
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void DI_DO_action::evaluate()
    {
    for ( u_int i = 0; i < devices.size(); i += 2 )
        {
        if ( devices[ i ]->is_active() )
            {
            devices[ i + 1 ]->on();
            }
        else
            {
            devices[ i + 1 ]->off();
            }                
        }
    }
//-----------------------------------------------------------------------------
void DI_DO_action::final()
    {
    for ( u_int i = 0; i < devices.size(); i += 2 )
        {
        devices[ i + 1 ]->off();
        }
    }
//-----------------------------------------------------------------------------
void DI_DO_action::print( const char* prefix /*= "" */ ) const
    {
    Print( "%sDI->DO: ", prefix );
    for ( u_int i = 0; i < devices.size(); i += 2 )
        {
        Print( "%s", devices[ i ]->get_name() );
        Print( "->" );
        Print( "%s", devices[ i + 1 ]->get_name() );             
        Print( " " );
        }

    Print( "\n" );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
open_seat_action::open_seat_action() : action( "Промывка седел" ), 
    phase( P_WAIT ), 
    active_group_n( 0 ),
    wait_time( 0 ),
    wash_time( 0 ),
    start_cycle_time( 0 )
    {
    }
//-----------------------------------------------------------------------------
void open_seat_action::init()
    {
    if ( 0 == wash_upper_seat_devices.size() &&
        0 == wash_lower_seat_devices.size() ) return;

    start_cycle_time  = get_millisec();
    phase             = P_WAIT;
    next_phase        = P_OPEN_UPPER;
    active_group_n    = 0;

    wait_time = ( *( PAC_info::get_instance()->par ) )
        [ PAC_info::P_MIX_FLIP_PERIOD ] * 1000;

    wash_time = ( *( PAC_info::get_instance()->par ) ) 
        [ PAC_info::P_MIX_FLIP_TIME ];

    wait_time /= wash_upper_seat_devices.size() + wash_lower_seat_devices.size();

    active_group_n = 0;
    }
//-----------------------------------------------------------------------------
void open_seat_action::evaluate()
    {
    if ( 0 == wash_lower_seat_devices.size() && 
        0 == wash_upper_seat_devices.size() ) return;

    switch ( phase )
        {
    case P_WAIT:
        // Пора промывать седла.
        if ( get_delta_millisec( start_cycle_time ) > wait_time )
            {
            phase            = next_phase;
            start_cycle_time = get_millisec();
            } 
        break;

    case P_OPEN_UPPER:
        if ( 0 == wash_upper_seat_devices.size() ) 
            {
            phase      = P_OPEN_LOWER;
            next_phase = P_OPEN_LOWER;
            break;
            }

        if ( get_delta_millisec( start_cycle_time ) < wash_time )
            {
            for ( u_int j = 0; j < wash_upper_seat_devices[ active_group_n ].size(); j++ )
                {
                wash_upper_seat_devices[ active_group_n ][ j ]->set_state( 
                    i_mix_proof::ST_UPPER_SEAT );
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
        if ( 0 == wash_lower_seat_devices.size() ) 
            {
            phase      = P_WAIT;
            next_phase = P_OPEN_UPPER;
            break;
            }

        if ( get_delta_millisec( start_cycle_time ) < wash_time )
            {
            for ( u_int j = 0; j < wash_lower_seat_devices[ active_group_n ].size(); j++ )
                {
                wash_lower_seat_devices[ active_group_n ][ j ]->set_state( 
                    i_mix_proof::ST_LOWER_SEAT );
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
    if ( 0 == wash_upper_seat_devices.size() &&
        0 == wash_lower_seat_devices.size() ) return;

    switch ( phase )
        {
        case P_WAIT:
                break;
            
    case P_OPEN_UPPER:
        for ( u_int j = 0; j < wash_upper_seat_devices[ active_group_n ].size(); j++ )
            {
            wash_upper_seat_devices[ active_group_n ][ j ]->off();
            }
        break;

    case P_OPEN_LOWER:
        for ( u_int j = 0; j < wash_lower_seat_devices[ active_group_n ].size(); j++ )
            {
            wash_lower_seat_devices[ active_group_n ][ j ]->off();
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
    case i_mix_proof::ST_UPPER_SEAT:
        seat_group = &wash_upper_seat_devices;
        break;

    case i_mix_proof::ST_LOWER_SEAT:
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
#ifdef DEBUG
        Print( "Error open_seat_action:add_dev: group %d > %d, seat_type %d.\n",
            group, seat_group[ 0 ].size(), seat_type );
#endif // DEBUG
        return;
        }

    seat_group[ 0 ][ group ].push_back( dev );
    }
//-----------------------------------------------------------------------------
void open_seat_action::print( const char* prefix /*= "" */ ) const
    {
    Print( "%s%s: ", prefix, name.c_str() );
    
    if ( wash_upper_seat_devices.size() )
        {
        Print( "верхние -" );
        for ( u_int i = 0; i < wash_upper_seat_devices.size(); i++ )
            {   
            Print( " {" );
            for ( u_int j = 0; j < wash_upper_seat_devices[ i ].size(); j++ )
                {
                Print( "%s",  wash_upper_seat_devices[ i ][ j ]->get_name() );               
                if ( j + 1 < wash_upper_seat_devices[ i ].size() ) Print( " " );
                }   

            Print( "}" );
            }
        }

    if ( wash_lower_seat_devices.size() )
        {
        Print( "; нижние -" );
        for ( u_int i = 0; i < wash_lower_seat_devices.size(); i++ )
            {      
            Print( " {" );
            for ( u_int j = 0; j < wash_lower_seat_devices[ i ].size(); j++ )
                {
                Print( "%s ", wash_lower_seat_devices[ i ][ j ]->get_name() );                
                if ( j + 1 < wash_lower_seat_devices[ i ].size() ) Print( " " );
                }   

            Print( "}" );
            }
        }

    Print( "\n" );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
mode::mode( const char* name, mode_manager *owner ) : name( name ),    
    mode_step(  new step( "Шаг режима", true ) ),
    active_step_n( 0 ),
    start_time( get_millisec() ),
    step_stub( "Шаг-заглушка" ),
    owner( owner )
    {
    }
//-----------------------------------------------------------------------------
step* mode::add_step( const char* name, u_int next_step_n, 
    u_int step_duration_par_n )
    {
    steps.push_back( new step( name ) );
    next_step_ns.push_back( next_step_n );
    step_duration_par_ns.push_back( step_duration_par_n );

    return steps[ steps.size() - 1 ];
    }
//-----------------------------------------------------------------------------
int mode::check_on( char* reason ) const
    {
    return mode_step->check( reason );
    }
//-----------------------------------------------------------------------------
void mode::init( u_int start_step /*= 0 */ )
    {
    start_time = get_millisec();

    if ( 0 == steps.size() )
        {
        //Если нет шагов, то выходим.
        return;
        }

    if ( start_step < steps.size() )
        {
        active_step_n = start_step;

        steps[ start_step ]->init();
        }

#ifdef DEBUG
    Print( " INIT STEP [ %d ]\n", active_step_n );
    steps[ active_step_n ]->print();
    Print( " NEXT STEP -> %d \n", 
        next_step_ns[ active_step_n ] );        
#endif 
    }
//-----------------------------------------------------------------------------
void mode::evaluate()
    {
    mode_step->evaluate();            

    if ( steps.size() )
        {
        steps[ active_step_n ]->evaluate();

        // [ 1 ] Есть параметр длительности шагов.
        // [ 2 ] Есть параметр длительности для этого шага.
        if ( owner->get_param() != 0 &&                      // 1                                          
            step_duration_par_ns[ active_step_n ] != 0 &&    // 2       
            get_millisec() - steps[ active_step_n ]->get_start_time() > 
            owner->get_param()[ 0 ][ step_duration_par_ns[ active_step_n ] ] * 1000L &&
            next_step_ns[ active_step_n ] < 255 )
            {
            steps[ active_step_n ]->final();  

            steps[ next_step_ns[ active_step_n ] ]->init();
            steps[ next_step_ns[ active_step_n ] ]->evaluate();
            }
        }
    }
//-----------------------------------------------------------------------------
void mode::final()
    {
    mode_step->final();
    start_time = get_millisec();

    if ( steps.size() )
        {
        steps[ active_step_n ]->final();
        active_step_n = 0;
        }

#ifdef DEBUG
    Print( " FINAL ACTIVE STEP [ %d ] \n", active_step_n );
#endif     
    }
//-----------------------------------------------------------------------------
step* mode::operator[]( int idx )
    {
    if ( -1 == idx )
        {
        return mode_step;
        }

    if ( idx > 0 && u_int( idx ) <= steps.size() )
        {
        return steps[ idx - 1 ];
        }

#ifdef DEBUG
    Print( "Error mode::step& operator[] ( int idx ) - idx %d > count %d.\n",
        idx, steps.size() );
#endif // DEBUG

    return &step_stub;
    }
//-----------------------------------------------------------------------------
void mode::to_step( u_int new_step )
    {
    if ( new_step <= steps.size() && new_step > 0 )
        {
        new_step--;

        steps[ active_step_n ]->final();
        active_step_n = new_step;

        steps[ active_step_n ]->init();
        steps[ active_step_n ]->evaluate();
        }
    else
        {
#ifdef DEBUG
        Print( "Error mode::to_step step %d > steps size %d.\n",
            new_step, steps.size() );
#endif // DEBUG
        }
    }
//-----------------------------------------------------------------------------
void mode::print( const char* prefix /*= "" */ ) const
    {
    std::string new_prefix = prefix;
    new_prefix += "  ";        

    Print( "%s%s\n", prefix, name.c_str() );
    mode_step->print( new_prefix.c_str() );

    for ( u_int i = 0; i < steps.size(); i++ )
        {
        Print( "%s%d ", new_prefix.c_str(), i + 1 );
        steps[ i ]->print( new_prefix.c_str() );
        }
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
mode* mode_manager::add_mode( const char* name )
    {
    modes.push_back( new mode( name, this ) );
    return modes[ modes.size() - 1 ];
    }
//-----------------------------------------------------------------------------
void mode_manager::set_param( saved_params_u_int_4 *par )
    {
    this->par = par;
    }
//-----------------------------------------------------------------------------
saved_params_u_int_4 * mode_manager::get_param() const
    {
    return par;
    }
//-----------------------------------------------------------------------------
mode* mode_manager::operator[]( unsigned int idx )
    {
    if ( idx < modes.size() )
        {
        return modes[ idx ];
        }

#ifdef DEBUG
    Print( "Error mode_manager::operator[] idx %d > modes count %d.\n",
        idx, modes.size() );
#endif // DEBUG

    return mode_stub;
    }
//-----------------------------------------------------------------------------
unsigned long mode_manager::get_idle_time()
    {
    return get_delta_millisec( last_action_time );
    }
//-----------------------------------------------------------------------------
void mode_manager::print()
    {
    Print( "modes manager, %d\n", modes.size() );    

    for ( u_int i = 0; i < modes.size(); i++ )
        {
        Print( "  %3d", i + 1 );
        modes[ i ]->print( "  " );        
        }
    }
//-----------------------------------------------------------------------------
mode_manager::mode_manager( u_int modes_cnt ): last_action_time( get_millisec() )
    {
    mode_stub = new mode( "Режим-заглушка", this );
    }
//-----------------------------------------------------------------------------
mode_manager::~mode_manager()
    {
    delete mode_stub;
    mode_stub = 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
