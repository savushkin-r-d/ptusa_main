#ifdef DRIVER
#include "stdafx.h"
#else
#include <errors.h>
#endif

const char *ALARM_CLASS_STR[ 4 ] =
    {
    "Неизвестная ошибка",
    "Нет соединения",
    "Ошибка COM-порта",
    "Внимание"
    };

const char *ALARM_SUBCLASS_STR[ 4 ][ 5 ] =        ///< Подкласс тревоги.
    {
        { 
        "?"
        },

        {
        "?",
        "узел Wago №",
        "панель EasyView №",
        "устройство, опрашиваемое по Modbus, №",
        "EasyServer"
        },

        { 
        "?"
        },
        
        {
        "?",
        "нажата аварийная кнопка №",
        }
    };

const int ALARM_CLASS_PRIORITY[ 4 ] = // Приоритет тревог.
    {
    0,
    100,  ///< Ошибка связи.
    200,
    300
    };

#ifdef DRIVER 

#include "stdafx.h"

//-----------------------------------------------------------------------------
alarm::alarm( alarm const& copy )
    {
    int descr_copy_len = strlen( copy.description );
    description = 0;
    if ( descr_copy_len > 0 )
        {
        description = new char[ descr_copy_len + 1 ];
        strcpy_s( description, descr_copy_len + 1, copy.description );
        }
    else
        {
        description = new char[ 1 ];
        description[ 0 ] = 0;
        }

    int group_copy_len = strlen( copy.group );
    group = 0;
    if ( group_copy_len > 0 )
        {
        group = new char[ group_copy_len + 1 ];
        strcpy_s( group, group_copy_len + 1, copy.group );
        }
    else
        {
        group = new char[ 1 ];
        group[ 0 ] = 0;
        }

    params      = copy.params;
    type        = copy.type;
    enable      = copy.enable;
    inhibit     = copy.inhibit;
    priority    = copy.priority;
    state       = copy.state;
    suppress    = copy.suppress;
    id          = copy.id;
    driver_id   = copy.driver_id;
    }
//-----------------------------------------------------------------------------
alarm & alarm::operator = ( const alarm & copy )
    {
    if ( this != &copy ) // Защита от неправильного самоприсваивания.
        {
        int descr_copy_len = strlen( copy.description );
        description = 0;
        if ( descr_copy_len > 0 )
            {
            description = new char[ descr_copy_len + 1 ];
            strcpy_s( description, descr_copy_len + 1, copy.description );
            }
        else
            {
            description = new char[ 1 ];
            description[ 0 ] = 0;
            }

        int group_copy_len = strlen( copy.group );
        group = 0;
        if ( group_copy_len > 0 )
            {
            group = new char[ group_copy_len + 1 ];
            strcpy_s( group, group_copy_len + 1, copy.group );
            }
        else
            {
            group = new char[ 1 ];
            group[ 0 ] = 0;
            }

        params      = copy.params;
        type        = copy.type;
        enable      = copy.enable;
        inhibit     = copy.inhibit;
        priority    = copy.priority;
        state       = copy.state;
        suppress    = copy.suppress;
        id          = copy.id;
        driver_id   = copy.driver_id;
        }
    // По соглашению всегда возвращаем *this.
    return *this;
    }
//-----------------------------------------------------------------------------
alarm::alarm(): description( 0 ),
type( AT_SPECIAL ),
enable( true ),
group( 0 ),

inhibit( true ),    
priority( 100 ),
state( AS_ALARM ),
suppress( true )
    {
    params.param1  = 0;
    params.param2  = 0;
    params.param3  = 0;
    params.param4  = 0;
    params.param5  = 0;
    params.param6  = 0;
    params.param7  = 0;
    params.param8  = 0;
    params.param9  = 0;
    params.param10 = 0;

    id.object_number  = 0;
    id.object_type    = 0;
    id.object_alarm_number = 0;
    }
//-----------------------------------------------------------------------------
alarm::~alarm()
    {
    if ( description )
        {
        delete [] description;
        description = 0;
        }
    if ( group )
        {
        delete [] group;
        group = 0;
        }
    }
//-----------------------------------------------------------------------------
int alarm::load_from_stream_as_simple_error( char *stream )
    { 
    int res = 0;
    //[ 0  ] - тип тревоги
    //[ 1  ] - блокировка тревоги на этапе проектирования.
    //[ 2  ] - блокировка тревоги во время работы.
    //[ 3  ] - подавление тревоги клиентами.  
    //
    //[ 4  ] - состояние тревоги
    //[ 5  ] - 
    //[ 6  ] - приоритет тревоги
    //[ 7  ] - 
    //[ 8  ] - номер устройства
    //[ 9  ] - 
    //[ 10 ] - номер ошибки устройства
    //[ 11 ] - 
    //[ 12 ] - тип устройства
    //[ 13 ]
    //[ .. ]
    //[ 62 ] - описание ошибки
    type     = ( ALARM_TYPE ) stream[ 0 ];
    enable   = stream[ 1 ];
    inhibit  = stream[ 2 ];
    suppress = stream[ 3 ];

    state    = ( ALARM_STATE ) ( ( int_2* ) ( stream + 4 ) ) [ 0 ];
    priority = ( ( int_2* ) ( stream + 6 ) ) [ 0 ];

    id.object_number       = ( ( u_int_2* ) ( stream + 8 ) ) [ 0 ];
    id.object_alarm_number = ( ( int_2* ) ( stream + 10 ) ) [ 0 ];
    id.object_type         = ( stream + 12 )[ 0 ];

    group = new char[ 11 ];
    sprintf_s( group, 11, "устройства" );

    description = new char[ 50 ];
    strcpy_s( description, 50, "" );
    sprintf_s( description, 50, "%s", stream + 13 );
    res = 12 + 50;
    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int alarm_manager::add_no_PAC_connection_error( const char *description,
                                               uchar project_id )
    {
    alarm  alarm_new;
    int len = strlen( description );
    alarm_new.description = new char[ len + 1 ];
    strcpy_s( alarm_new.description, len + 1, description );    

    alarm_new.type = AT_SPECIAL;
    alarm_new.enable = true;
    alarm_new.group = new char[ 15 ];
    strcpy_s( alarm_new.group, 15, "Ошибка связи" );
    alarm_new.inhibit = false; 
#ifdef MONITORING_DRIVER
    alarm_new.priority = 301;
#else
    alarm_new.priority = 1;
#endif // MONITORING_DRIVER
    alarm_new.state = AS_ALARM;
    alarm_new.suppress = false;

    alarm_new.driver_id = ( uchar ) project_id;

#ifdef MONITORING_DRIVER
    static int object_number = 1;
    // Каждая новая ошибка соединения получает новый номер.
    alarm_new.id.object_number  = object_number++;
#else
    alarm_new.id.object_number  = project_id;
    // Удаляем все предыдущие ошибки, так как контроллер только один и 
    // остальные ошибки уже не имеют смысла.
    clear( project_id );
#endif // MONITORING_DRIVER

    add_alarm( alarm_new );  
    return 0;
    }
//-----------------------------------------------------------------------------
int alarm_manager::remove_no_PAC_connection_error( const char *description, 
                                                  uchar driver_id )
    {
    alarm  r_alarm;
    int len = strlen( description );
    r_alarm.description = new char[ len + 1 ];    
    strcpy_s( r_alarm.description, len + 1, description ); 
    r_alarm.driver_id = driver_id;

    remove_alarm( r_alarm );
    return 0;
    }
//-----------------------------------------------------------------------------
int alarm_manager::add_alarm( alarm &new_alarm )
    {
    int res = 1;

    synch_all_projects_alarms[ new_alarm.driver_id ].WaitToWrite();
    int result = binary_search( alarms_vector[ new_alarm.driver_id ].begin( ), 
        alarms_vector[ new_alarm.driver_id ].end( ), new_alarm );

    if ( 0 == result )
        {
        id[ new_alarm.driver_id ]++;
        alarms_vector[ new_alarm.driver_id ].push_back( new_alarm );
        sort( alarms_vector[ new_alarm.driver_id ].begin(),
            alarms_vector[ new_alarm.driver_id ].end() );
        synch_project_alarms( new_alarm.driver_id );

        res = 0;
        }
    synch_all_projects_alarms[ new_alarm.driver_id ].Done();

    return res;
    }
//-----------------------------------------------------------------------------
int alarm_manager::remove_alarm( alarm &r_alarm )
    {    
    int res = 1;

    synch_all_projects_alarms[ r_alarm.driver_id ].WaitToWrite();
    int result = binary_search( alarms_vector[ r_alarm.driver_id ].begin( ), 
        alarms_vector[ r_alarm.driver_id ].end( ), r_alarm );

    if ( result )
        {
        vector< alarm >::iterator result = 
            alarms_vector[ r_alarm.driver_id ].begin();

        result = find( alarms_vector[ r_alarm.driver_id ].begin(),
            alarms_vector[ r_alarm.driver_id ].end(), 
            r_alarm );

        if ( result != alarms_vector[ r_alarm.driver_id ].end() )
            {       
            id[ r_alarm.driver_id ]++;        
            alarms_vector[ r_alarm.driver_id ].erase( result );
            synch_project_alarms( r_alarm.driver_id );

            res = 0;  
            }
        }
    synch_all_projects_alarms[ r_alarm.driver_id ].Done();

    return res; 
    }
//-----------------------------------------------------------------------------
alarm_manager::alarm_manager()
    {
    for ( int i = 0; i < AM_MAX_COUNT; i++ )
        {
        id[ i ]  = 1;
        all_projects_alarms_array[ i ] = 0;
        }
    }
//-----------------------------------------------------------------------------
alarm_manager::~alarm_manager()
    {
    for ( int i = 0; i < AM_MAX_COUNT; i++ )
        {
        if ( all_projects_alarms_array[ i ] != 0 )
            {
            delete [] all_projects_alarms_array[ i ];
            all_projects_alarms_array[ i ] = 0;
            }
        }
    }
//-----------------------------------------------------------------------------
int alarm_manager::get_alarms( uchar driver_id, all_alarm &project_alarms )
    {
    synch_all_projects_alarms[ driver_id ].WaitToRead();
    project_alarms.alarms = all_projects_alarms_array[ driver_id ];
    project_alarms.id = id[ driver_id ];
    project_alarms.cnt = alarms_vector[ driver_id ].size();    
    synch_all_projects_alarms[ driver_id ].Done();

    return 0;     
    }
//-----------------------------------------------------------------------------
int alarm_manager::synch_project_alarms( uchar driver_id )
    {
    //Удаляем старые ошибки, если они были.
    if ( all_projects_alarms_array[ driver_id ] )
        {
        delete [] all_projects_alarms_array[ driver_id ];
        all_projects_alarms_array[ driver_id ] = 0;
        }

    int alarms_cnt = alarms_vector[ driver_id ].size();
    if ( alarms_cnt > 0 )
        {
        //Заново копируем текущие ошибки.
        all_projects_alarms_array[ driver_id ] = new alarm [ alarms_cnt ];
        for ( int i = 0; i < alarms_cnt; i++ )
            {
            all_projects_alarms_array[ driver_id ][ i ] = 
                alarms_vector[ driver_id ][ i ]; 
            }		
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int alarm_manager::clear_PAC_critical_errors( uchar driver_id )
    {
    vector < alarm >::iterator iter;
    char is_del_err;

    synch_all_projects_alarms[ driver_id ].WaitToWrite();
    do 
        {
        is_del_err = 0;
        for ( iter = alarms_vector[ driver_id ].begin(); 
            iter != alarms_vector[ driver_id ].end(); iter++ )
            {
            if ( strcmp( iter->group, ERRORS_GROUP_WAGO_CONNECT ) == 0 ||
                strcmp( iter->group, ERRORS_GROUP_PANEL_CONNECT ) == 0 ||
                strcmp( iter->group, ERRORS_GROUP_EMERGENCY_BUTTONS ) == 0 )
                {
                alarms_vector[ driver_id ].erase( iter );
                is_del_err = 1;
                break;
                }
            }
        } while ( is_del_err == 1 );
        id[ driver_id ]++;

        synch_project_alarms( driver_id );
        synch_all_projects_alarms[ driver_id ].Done();

        return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int alarm_manager::clear( uchar driver_id )
    {    
    synch_all_projects_alarms[ driver_id ].WaitToWrite();
    alarms_vector[ driver_id ].clear();
    synch_all_projects_alarms[ driver_id ].Done();

    return 0;
    }
//-----------------------------------------------------------------------------
int alarm_manager::clear_PAC_simple_devices_errors( uchar driver_id )
    {
    vector < alarm >::iterator iter;
    char is_del_err;

    synch_all_projects_alarms[ driver_id ].WaitToWrite();
    do 
        {
        is_del_err = 0;
        for ( iter = alarms_vector[ driver_id ].begin(); 
            iter != alarms_vector[ driver_id ].end(); iter++ )
            {
            if ( strcmp( iter->group, ERRORS_GROUP_SIMPLE_DEVICES ) == 0 ||
                strcmp( iter->group, ERRORS_GROUP_PANEL_CONNECT ) == 0 )
                {
                alarms_vector[ driver_id ].erase( iter );
                is_del_err = 1;
                break;
                }
            }
        } while ( is_del_err == 1 );
        id[ driver_id ]++;

        synch_project_alarms( driver_id );
        synch_all_projects_alarms[ driver_id ].Done();

        return 0;
    }
//-----------------------------------------------------------------------------
int alarm_manager::add_PAC_critical_errors( CRITICAL_ERRORS type,
                                           uchar project_id, 
                                           const char* PAC_name, 
                                           uint object_number,
                                           u_int priority )
    {
    alarm  alarm_new;
    char description[ 200 ];

    switch ( type )
        {
        case CE_WAGO:
            sprintf_s( description, 200, 
                "Контроллер \"%s\" - нет связи с узлом WAGO №[%d]!", 
                PAC_name, object_number );
            break;

        case CE_PANEL:
            sprintf_s( description, 200, 
                "Контроллер \"%s\" - нет связи с панелью №[%d]!", 
                PAC_name, object_number );
            break;

        case CE_EMERGENCY_BUTTON:
            sprintf_s( description, 200, 
                "Контроллер \"%s\" - нажата аварийная кнопка №[%d]!", 
                PAC_name, object_number );
            break;
        }           

    int len = strlen( description ) + 1; //Учитываем символ конца строки - 0.
    alarm_new.description = new char[ len ];
    strcpy_s( alarm_new.description, len, description );    

    alarm_new.type = AT_SPECIAL;
    alarm_new.enable = true;

    switch ( type )
        {
        case CE_WAGO:
            len = strlen( ERRORS_GROUP_WAGO_CONNECT ) + 1; 
            alarm_new.group = new char[ len ];
            strcpy_s( alarm_new.group, len, ERRORS_GROUP_WAGO_CONNECT );

            break;

        case CE_PANEL:
            len = strlen( ERRORS_GROUP_PANEL_CONNECT ) + 1; 
            alarm_new.group = new char[ len ];
            strcpy_s( alarm_new.group, len, ERRORS_GROUP_PANEL_CONNECT );
            break;

        case CE_EMERGENCY_BUTTON:
            len = strlen( ERRORS_GROUP_EMERGENCY_BUTTONS ) + 1; 
            alarm_new.group = new char[ len ];
            strcpy_s( alarm_new.group, len, ERRORS_GROUP_EMERGENCY_BUTTONS );
            break;
        } 
    alarm_new.priority = priority;

    alarm_new.id.object_type = type;
    alarm_new.id.object_alarm_number = 1;
    alarm_new.id.object_number = object_number; 

    alarm_new.inhibit = false;        
    alarm_new.state = AS_ALARM;
    alarm_new.suppress = false;

    alarm_new.driver_id = ( uchar ) project_id;

    add_alarm( alarm_new );   

    return 0;
    }
//-----------------------------------------------------------------------------
#endif // DRIVER

#ifdef PAC

auto_smart_ptr < dev_errors_manager > dev_errors_manager::instance;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
base_error::base_error(): error_state( AS_NORMAL )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
simple_error::simple_error( device* simple_device
                           ): base_error(),
                           simple_device( simple_device )                           
    {     
    }
//-----------------------------------------------------------------------------
simple_error::~simple_error()
    { 
    }
//-----------------------------------------------------------------------------
void simple_error::reset_errors_params()
    {
    simple_device->err_par[ 0 ] = 0;    
    }
//-----------------------------------------------------------------------------
int simple_error::save_as_Lua_str( char *str, bool &is_new_state )
    {
    str[ 0 ] = 0;

    // Проверка текущего состояния устройства.
    switch ( simple_device->get_state() ) 
        {
        case -1:        // Ecть ошибка.
            switch ( error_state ) 
                {
                case AS_ACCEPT:                    
                    break;

                case AS_ALARM:
                    break;

                case AS_NORMAL:
                case AS_RETURN:
                    error_state = AS_ALARM;
                    is_new_state = 1;
                    break;
                }            
            break;

        case 0:         // Нет ошибки.
        case 1:
            switch ( error_state ) 
                {
                case AS_NORMAL:
                    break;

                case AS_RETURN:
                    break;

                case AS_ACCEPT:
                    error_state = AS_NORMAL;
                    is_new_state = 1;
                    break;

                case AS_ALARM:
                    error_state = AS_RETURN;
                    is_new_state = 1;
                    break;
                }
            break;
        }
    // Проверка текущего состояния устройства.-!>

    if ( AS_ALARM == error_state ||
        AS_RETURN == error_state ) // Есть ошибка.
        {    	
        sprintf( str + strlen( str ), "\t%s\n", "{" );

        sprintf( str + strlen( str ), "\tdescription = \"%s\",\n",
            simple_device->get_name() );

        sprintf( str + strlen( str ), "priority    = %d%s", ALARM_CLASS_PRIORITY[ 3 ], "," );
        sprintf( str + strlen( str ), "state       = %d,\n", error_state );
        sprintf( str + strlen( str ), "suppress    = false,\n" );

        sprintf( str + strlen( str ), "},\n" );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void simple_error::print() const
    {
#ifdef DEBUG
    switch ( simple_device->get_type() )
        {
        case device::DT_V:
            Print( "VALVE" );
            break;

        case device::DT_M:
            Print( "MOTOR" );
            break;
        }
    Print( "[ %5u ]", simple_device->get_n() );
    Print( ", state[ %3d ]", error_state );
    Print( ", par[ %d ]\n", simple_device->err_par[ 0 ] );

#endif // DEBUG
    }
//-----------------------------------------------------------------------------
unsigned char simple_error::get_type() const
    {
    return simple_device->get_type();
    }
//-----------------------------------------------------------------------------
unsigned int simple_error::get_n() const
    {
    return simple_device->get_n();
    }
//-----------------------------------------------------------------------------
unsigned int simple_error::get_object_alarm_n() const
    {
    return SE_ERROR_CODE;
    }
//-----------------------------------------------------------------------------
int simple_error::set_cmd( int cmd )
    {
    int res = 0;

    switch ( cmd )
        {
        case BE_CMD_SUPPRESS:
            {            
            int current_state = simple_device->err_par[ 0 ];
            current_state |= device::DE_IS_SUPPRESS; 
            simple_device->err_par[ 0 ] = current_state;
            break;
            }

         case BE_CMD_UNSET_SUPPRESS:
             {
             int current_state = simple_device->err_par[ 0 ];
             current_state |= device::DE_IS_SUPPRESS; 
             current_state ^= device::DE_IS_SUPPRESS; 
             simple_device->err_par[ 0 ] = current_state;
             break;
             }

        case BE_CMD_ACCEPT:
            if ( AS_ALARM == error_state ||
                AS_RETURN == error_state )
                {
                if ( AS_RETURN == error_state )
                    {
                    error_state = AS_NORMAL;
                    }
                else
                    {
                    error_state = AS_ACCEPT;
                    }                
                }
            else
                {
#ifdef DEBUG
                Print( "simple_error::set_cmd(...) - error state = %d, \
                       trying to set to ACCEPT!\n", error_state );
#endif // DEBUG
                res = 1;
                }
            break;

        default:
            res = 2;
            break;
        }

#ifdef DEBUG
    Print( "simple_error::set_cmd(...) - cmd = %d\n", cmd );
    print();
#endif // DEBUG

    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
dev_errors_manager::dev_errors_manager(): errors_id( 0 )
    {      
    }
//-----------------------------------------------------------------------------
int dev_errors_manager::save_as_Lua_str( char *str, u_int_2 &id )
    {
    str[ 0 ] = 0;

    bool is_new_error_state = false;

    for ( u_int i = 0; i < s_errors_vector.size(); i++ )
        {
        s_errors_vector[ i ]->save_as_Lua_str( str + strlen( str ),
            is_new_error_state );  
        }

    if ( is_new_error_state )
        {
        errors_id++;        // Ошибки изменились.
        }

    id = errors_id; //Через параметр возвращаем состояние ошибок.

    return 0;
    }
//-----------------------------------------------------------------------------
int dev_errors_manager::add_error( base_error  *s_error )
    {
    s_errors_vector.push_back( s_error );
    return 0;
    }
//-----------------------------------------------------------------------------
void dev_errors_manager::print()
    {
#ifdef DEBUG
    Print( "dev_errors_manager\n" );

    for ( u_int i = 0; i < s_errors_vector.size(); i++ )
        {
        s_errors_vector[ i ]->print();
        }

#endif // DEBUG
    }
//-----------------------------------------------------------------------------
void dev_errors_manager::reset_errors_params()
    {
    for ( u_int i = 0; i < s_errors_vector.size(); i++ )
        {
        s_errors_vector[ i ]->reset_errors_params();
        }
    }
//-----------------------------------------------------------------------------
void dev_errors_manager::set_cmd( unsigned int cmd, unsigned int object_type, 
                                 unsigned int object_number, 
                                 unsigned int object_alarm_number )
    { 
    base_error *res = 0;

    // Поиск нужного устройства.
    for ( u_int i = 0; i < s_errors_vector.size(); i++ )
        {
        if ( s_errors_vector[ i ]->get_type() == object_type &&
            s_errors_vector[ i ]->get_n() == object_number &&
            s_errors_vector[ i ]->get_object_alarm_n() == object_alarm_number )
            {
            res = s_errors_vector[ i ];
            break;
            }
        }

    // Выполнение команды.
    if ( res )
        {
        int result = res->set_cmd( cmd );
        if ( 0 == result )
        	{
            errors_id++; // Cостояние ошибок изменилось.
        	}
        }
    else
        {
#ifdef DEBUG
        Print( "dev_errors_manager::set_cmd(...) - cmd = %u, object_type = %u,\
               object_number = %u, object_alarm_number = %u\n",
               cmd, object_type, object_number, object_alarm_number );
        Print( "Error object not found!\n" );
#endif // DEBUG
        }

#ifdef DEBUG
    //print();    
#endif // DEBUG

    }
//-----------------------------------------------------------------------------
dev_errors_manager::~dev_errors_manager()
    {
    for ( u_int i = 0; i < s_errors_vector.size(); i++ )
        {
        delete s_errors_vector.at( i );
        s_errors_vector.at( i ) = 0;
        }
    }
//-----------------------------------------------------------------------------
dev_errors_manager* dev_errors_manager::get_instance()
    {
    static int is_init = 0;
    if ( 0 == is_init )
        {
        instance = new dev_errors_manager();
        is_init = 1;
        }

    return instance;
    }
//-----------------------------------------------------------------------------
#endif // PAC
