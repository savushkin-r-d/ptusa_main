#include <string.h>
#include <stdio.h>

#include "g_device.h"

#ifdef DRIVER
#include "bug_log.h"
#include "pc_device.h"

#pragma warning( disable : 4996 ) //sprintf to sprintf_s
#endif // DRIVER

#ifdef PAC
#include "PAC_dev.h"

auto_smart_ptr < device_communicator > device_communicator::instance;
u_int_4 device_communicator::dev_cnt;
i_complex_device *device_communicator::dev[ device_communicator::C_MAX_COMLEX_DEVICES ];

#endif // PAC

#ifdef DRIVER
u_int_2 G_PROTOCOL_VERSION = 3;
#else 

#ifdef USE_SIMPLE_DEV_ERRORS
u_int_2 G_PROTOCOL_VERSION = 3;
#else
u_int_2 G_PROTOCOL_VERSION = 2;
#endif // USE_SIMPLE_DEV_ERRORS

#endif // DRIVER

//-----------------------------------------------------------------------------
void print_str( char *err_str, char is_need_CR )
    {    
#ifdef DRIVER
    bug_log::add_msg( "System", "", err_str );
#else
    Print( "%s", err_str  );
    if ( is_need_CR )
        {
        Print( "\n" );
        }
#endif // DRIVER
    }
//-----------------------------------------------------------------------------
complex_device::complex_device(): sub_dev( 0 ),
        n( 0 ),
        sub_dev_cnt( 0 ),
        type( 0 )        
    {      
#ifdef DRIVER
    strcpy_s( name, MAX_NAME_LENGTH - 1, "?" );    
#else
    strcpy( name, "?" );   
#endif
    }
//-----------------------------------------------------------------------------
complex_device::complex_device( u_int_4 n, const char *new_name,
                               u_int_2 new_subdev_cnt, char type ): n( n ),
        sub_dev_cnt( new_subdev_cnt ),
        type( type )
    {           
#ifdef DRIVER
    strcpy_s( name, MAX_NAME_LENGTH - 1, new_name );    
#else
    strcpy( name, new_name );   
#endif

    if ( sub_dev_cnt ) 
        {
        sub_dev = new i_simple_device*[ sub_dev_cnt ];
        for ( u_int i = 0; i < sub_dev_cnt; i++ ) sub_dev[ i ] = 0;
        }
    else sub_dev = 0;
    }
//-----------------------------------------------------------------------------
#ifdef DRIVER
complex_device::complex_device( u_int_4 n,
                               char *new_name, 
                               u_int_2 new_subdev_cnt, 
                               char type, 
                               i_complex_device *owner_compl_dev ): type( type ), 
                               n( n ), 
                               sub_dev_cnt( new_subdev_cnt )
    {
    name = new char[ MAX_NAME_LENGTH ];
    strcpy_s( name, MAX_NAME_LENGTH - 1, new_name );    

    if ( sub_dev_cnt ) sub_dev = new i_simple_device*[ sub_dev_cnt ];
    else sub_dev = 0;

    compl_dev = owner_compl_dev;
    }
//-----------------------------------------------------------------------------
#endif // DRIVER
complex_device::~complex_device()
    {
    if ( sub_dev_cnt )
        {
        delete [] sub_dev;
        sub_dev = 0;
        sub_dev_cnt = 0;
        }    
    }
//-----------------------------------------------------------------------------
char complex_device::get_type() const
    {
    return type;
    }
//-----------------------------------------------------------------------------
u_int_4 complex_device::get_n() const
    {
    return n;
    }
//-----------------------------------------------------------------------------
const char* complex_device::get_name() const
    {
    return name;
    }
//-----------------------------------------------------------------------------
u_int_4 complex_device::get_subdev_quantity() const
    {
    return sub_dev_cnt;
    }
//-----------------------------------------------------------------------------
i_save_device*  complex_device::get_save_dev( u_int_4 idx ) const
    {
#if defined DEBUG && defined DRIVER
    if ( idx >= sub_dev_cnt )
        {
        _DebugBreak();
        }
#endif // DEBUG

    return sub_dev[ idx ];
    }
//-----------------------------------------------------------------------------
// Данные группы (buff) в следующем виде:
//    1 байт  - тип;                                    (1)
//            0 - сложное устройство;
//            1 - группа устройств с размерностью данных 1 байт;
//            2 - группа устройств с размерностью данных 4 байта;
//    4 байта - номер;                                  (2)
//    1 байт  - длина имени группы устройства;          (3)
//    х байт  - имя группы устройства;                  (4)
//    4 байта - количество подустройств;                (5)
//    далее   - данные подустройств.
int  complex_device::save_device( char *buff )
    {
    int idx = 0;

    buff[ 0 ] = get_type();                             //(1)
    buff++;
    idx++;
    memcpy( buff, &n, sizeof( n ) );                    //(2)
    buff += sizeof( n );
    idx += sizeof( n );
    buff[ 0 ] = ( unsigned char ) strlen( get_name() ); //(3)
    buff++;
    idx++;
#ifndef DRIVER
    strcpy( buff, get_name() );                         //(4)
#else
    strcpy_s( buff, 255, get_name() );                  //(4)
#endif
    buff += strlen( get_name() ) + 1;
    idx += strlen( get_name() ) + 1;
    memcpy( buff, &sub_dev_cnt, sizeof( sub_dev_cnt ) );//(5)
    buff += sizeof( sub_dev_cnt );
    idx += sizeof( sub_dev_cnt );

    for ( u_int_4 i = 0; i < get_subdev_quantity(); i++ )
        {
        u_int_4 size = get_save_dev( i )->save_device( buff );
        buff += size;
        idx += size;        
        }
    return idx;
    }
//-----------------------------------------------------------------------------
// Данные группы (buff) в следующем виде:
//  4 байта - номер устройства;                         (1)
//  4 байта - количество подустройств;                  (2)
//  далее   - данные каждого подустройства.
int  complex_device::save_state( char *buff )
    {
    memcpy( buff, &n, sizeof( n ) );                                    //(1)
    unsigned int answer_size = sizeof( n );
    memcpy( buff + answer_size, &sub_dev_cnt, sizeof( sub_dev_cnt ) );  //(2)
    answer_size += sizeof( sub_dev_cnt );

    for ( unsigned int i = 0; i < get_subdev_quantity(); i++ )
        {
        answer_size += get_save_dev( i )->save_state( buff + answer_size );
        }
    return answer_size;
    }
//-----------------------------------------------------------------------------
int  complex_device::set_type( char new_type )
    {
    type = new_type;
    return 0;
    }
//-----------------------------------------------------------------------------
int  complex_device::set_n( u_int_4 new_n )
    {
    n = new_n;
    return 0;
    }
//-----------------------------------------------------------------------------
int  complex_device::set_name( char * new_name )
    {
    if ( name )         
        {
#ifdef DRIVER
        strcpy_s( name, MAX_NAME_LENGTH - 1, new_name );    
#else
        strcpy( name, new_name );   
#endif
        }
    return 0;
    }
//-----------------------------------------------------------------------------
int complex_device::set_subdev_quantity( u_int_4 new_dev_cnt )
    {
    sub_dev_cnt = new_dev_cnt;
    sub_dev = new i_simple_device*[ new_dev_cnt ];

    return 0;
    }
//-----------------------------------------------------------------------------
#ifdef DRIVER        
i_load_device* complex_device::get_load_dev( u_int_4 idx )
    {
#if defined DEBUG 
    if ( idx >= sub_dev_cnt )
        {
        _DebugBreak();
        }
#endif // DRIVER

    return sub_dev[ idx ];
    }        
#endif // DRIVER                
//-----------------------------------------------------------------------------
// Данные группы (buff) в следующем виде:
//    1 байт  - тип;                                    (1)
//            0 - сложное устройство;
//            1 - группа устройств с размерностью данных 1 байт;
//            2 - группа устройств с размерностью данных 4 байта;
//    4 байта - номер;                                  (2)
//    1 байт  - длина имени группы устройства;          (3)
//    х байт  - имя группы устройства;                  (4)
//    4 байта - количество подустройств;                (5)
//    далее   - данные подустройств.
int  complex_device::load_device( char *buff )
    {
#ifdef DRIVER
    int idx = 0;
    set_type( buff++[ 0 ] );                            //(1)
    idx++;
    set_n( ( ( u_int_4* ) buff )[ 0 ] );                //(2)
    buff += 4;
    idx += 4;
    uchar name_len = buff++[ 0 ];                       //(3)
    idx++;

    if ( buff[ name_len ] != 0 )
        {            
        sprintf_s( bug_log::msg, bug_log::msg_size, 
            "i_complex_device::load_device( char *buff ) - неверное имя устройства!" );
        //bug_log::println_msg();
        bug_log::add_warning_msg( "Driver", "" );
        return -1;
        }

    char *new_name = new char[ name_len + 1 ];

    strcpy_s( new_name, MAX_NAME_LENGTH, buff );
    buff += name_len + 1;
    idx += name_len + 1;
    set_name( new_name );

    u_int_4 subdev_quantity = ( ( u_int_4* ) buff )[ 0 ];
    set_subdev_quantity( subdev_quantity );             //(5)
    buff += 4;
    idx += 4;

    int res = 0;
    for ( u_int_4 i = 0; i < subdev_quantity; i++ )
        {        
        switch ( type )
            {
            case COMPLEX_DEV:
                sub_dev[ i ] = new complex_device( i + 1, "?", 0, GROUP_DEV_BYTE, this );
                res = get_load_dev( i )->load_device( buff );
                if ( res < 0 )
                    {
                    return res;
                    }

                buff += res;
                idx += res; 

                sub_dev[ i ]->set_idx( i );
                break;

            case GROUP_DEV_BYTE:
            case GROUP_DEV_LONG:
            case GROUP_DEV_FLOAT:
                try
                    {
                    sub_dev[ i ] = new virtual_device( this );
                    }
                catch ( std::bad_alloc exception )
                    {                        
                    bug_log::add_warning_msg( "Driver", 
                        "complex_device::load_device - не удалось выделить память для создания объекта!" );
                    return -2;
                    }                

                res = get_load_dev( i )->load_device( buff );                
                if ( res < 0 )
                    {
                    return res;
                    }

                buff += res;
                idx += res;    

                sub_dev[ i ]->set_idx( i );
                break;

            case ARRAY_DEV_BYTE:
            case ARRAY_DEV_FLOAT:
            case ARRAY_DEV_LONG:
            case ARRAY_DEV_UINT:
            case ARRAY_DEV_INT: 
            case ARRAY_DEV_STR:
                sub_dev[ i ] = new virtual_device( this, i + 1 );
                sub_dev[ i ]->set_idx( i );
                break;

            default:                    
                sprintf_s( bug_log::msg, bug_log::msg_size, 
                    "i_complex_device::load_device( char *buff ) - неверный тип устройства = %d!",
                    type );
                bug_log::add_warning_msg( "Driver", "" );
#ifdef _DEBUG
                _DebugBreak();
#endif
                return -1;
            }   

        //sprintf_s( bug_log::msg, bug_log::msg_size, "complex_device - load_device \"%s\"[%d] of %d", 
        //    new_name, i, subdev_quantity );
        //bug_log::println_msg();
        //Sleep( 100 );

        }
    return idx;
#else
    buff++;
    return 0;
#endif
    }
//-----------------------------------------------------------------------------
// Данные группы (buff) в следующем виде:
//  4 байта - номер устройства;                         (1)
//  4 байта - количество подустройств;                  (2)
//  далее   - данные каждого подустройства.
int  complex_device::load_state( char *buff  )
    {
#ifdef DRIVER
    u_int_4 rec_id = ( ( u_int_4* ) buff )[ 0 ];        //(1) 
    u_int_4 rec_groups_cnt = ( ( u_int_4* ) buff )[ 1 ];//(2) 
    buff += 8;
    u_int_4 idx = 8;

    if ( rec_id != get_n() ||
        rec_groups_cnt != get_subdev_quantity() )
        {
        char err_str[ 600 ];
        sprintf( err_str, "i_complex_device::load_state( char *buff ) - Error! rec_id = %lu, rec_groups_cnt=%lu, get_n() = %lu, get_subdev_quantity() = %lu!\n", 
            rec_id, rec_groups_cnt, get_n(), get_subdev_quantity() );
        print_str( err_str );
        return -1;
        }

    //if ( !get_subdev_quantity() ) return idx;

    int res = 0;
    for ( unsigned int i = 0; i < get_subdev_quantity(); i++ )
        {      
        res = get_load_dev( i )->load_state( buff );
        if ( res < 0 )
            {
            return res;
            }
        buff += res;
        idx += res;
        }

    return idx;

#else
    buff++;
    return 0;
#endif // DRIVER     
    }    
//-----------------------------------------------------------------------------
void complex_device::print() const
    {
    static char indent[ 40 ] = "";
    static int start_pos = 0;
    memset( indent + start_pos, ' ', 4 );
    start_pos += 4;
    indent[ start_pos ] = 0;

    char tmp_str[ 200 ];  

    if ( start_pos > 4 )
        {
        sprintf( tmp_str, "\"%s\"[ %3lu ], \ttype = %d", name,
            ( unsigned long int  ) sub_dev_cnt, type );
        }
    else
        {
        sprintf( tmp_str, "\"%s%lu\"[ %3lu ], \ttype = %d", 
                name, ( unsigned long int  ) get_n(),
                ( unsigned long int  ) sub_dev_cnt, type );
        }

    print_str( tmp_str );  



    for ( unsigned int i = 0; i < sub_dev_cnt; i++ ) 
        {
        sprintf( tmp_str, "%s[ %3d ] ", indent, i );
        print_str( tmp_str, 0 );
        sub_dev[ i ]->print();
        }

    start_pos -= 4;
    indent[ start_pos ] = 0;    
    }
//-----------------------------------------------------------------------------
#ifdef DRIVER 
i_simple_device* complex_device::get_sub_dev( u_int_4 id ) const
    {
    //-Бинарный поиск.
    int l = 0;
    int u = sub_dev_cnt - 1;
    int i;
    while ( l <= u ) 
        {
        i = ( l + u ) / 2;

        if ( sub_dev[ i ]->get_n() == id  )
            {
            return sub_dev[ i ];
            }

        if ( id > sub_dev[ i ]->get_n() ) 
            {
            l = i + 1;
            } 
        else 
            {
            u = i - 1;
            }
        }
    //-Бинарный поиск.-!>

    return 0;
    }
//-----------------------------------------------------------------------------
i_complex_device* complex_device::get_sub_complex_dev( char *sub_dev_name ) const
    {
    if ( !sub_dev ) return 0;
    if ( GROUP_DEV_BYTE == type || GROUP_DEV_FLOAT == type ) return 0;

    for ( unsigned int i = 0; i < sub_dev_cnt; i++ ) 
        {
        if ( strcmp( ( ( i_complex_device* )sub_dev[ i ] )->get_name(), 
            sub_dev_name ) == 0 )
            {
            return ( i_complex_device* ) sub_dev[ i ];
            }
        }

    return 0;
    }
#endif // DRIVER
//-----------------------------------------------------------------------------
//Выполнение команды, хранящейся в буфере.
//Структура данных buff:
//    4 байта - индекс устройства в массиве устройств;
//    х байт  - данные команды устройства.
int complex_device::parse_cmd( char *buff  ) 
    {
    u_int_4 dev_n;
    memcpy( &dev_n, buff, sizeof( dev_n ) );

#if defined DEBUG && defined DRIVER
    if ( dev_n >= sub_dev_cnt )
        {
        _DebugBreak();
        }
#endif // DEBUG

    return sizeof( dev_n ) + sub_dev[ dev_n ]->parse_cmd( buff + sizeof( dev_n ) );
    }
//-----------------------------------------------------------------------------
u_int_4 complex_device::get_idx()
    {
    return idx;
    }
//-----------------------------------------------------------------------------
void complex_device::set_idx( u_int_4 new_idx )
    {
    idx = new_idx;
    }
//-----------------------------------------------------------------------------
// Данные группы (buff) в следующем виде:
//  2 байта - количества устройств;                                 (1)
//  2 байта - индекс устройства в массиве устройств;                (2)
//  далее   - данные каждого подустройства.                         (3)
int complex_device::load_changed_state( char *buff )
    {
#ifdef DRIVER 
    u_int_2 rec_groups_cnt = ( ( u_int_2* ) buff )[ 0 ];            //1 
    buff += 2;
    int idx = 2;

    if ( !get_subdev_quantity() ) return -1;

    int res = 0;
    for ( u_int_2 i = 0; i < rec_groups_cnt; i++ )
        {      
        u_int_2 dev_idx = ( ( u_int_2* ) buff )[ 0 ];               //2
        buff += 2;
        idx += 2;

        res = get_load_dev( dev_idx )->load_changed_state( buff );  //3
        if ( res < 0 )
            {
            return -1;
            }        
        buff += res;
        idx += res;
        }

    return idx;

#else
    buff++;
    return 0;
#endif // DRIVER
    }
//-----------------------------------------------------------------------------
// Данные группы (buff) в следующем виде:          
//  2 байта - количество устройств, измененных свое состоние;   (1)
//  2 байта - номер устройства в массиве устройств.             (2)
//  далее   - его измененые данные                              (3)
//Изменяем размер ответа.                                       (4)
//Увеличиваем на 1 количество устройств, изменивщих свое
//состояние.                                                    (5)
//Если не устройств, изменивших свое состояние, возвращаем 0.   (6)
int complex_device::save_changed_state( char *buff )
    {
    u_int_2 *changed_sub_dev_cnt = ( u_int_2* ) buff;           //1
    *changed_sub_dev_cnt = 0;
    int answer_size = 2;

    for ( u_int_2 i = 0; i < get_subdev_quantity(); i++ )
        {
        ( ( u_int_2* ) ( buff + answer_size ) )[ 0 ] = i;       //2        
        int res = get_save_dev( i )->save_changed_state( 
            buff + answer_size + 2 );                           //3
        if ( res ) 
            {            
            answer_size = answer_size + 2 + res;                //4
            ( *changed_sub_dev_cnt )++;                         //5
            }
        }
    if ( 2 == answer_size )                                     //6             
        {
        return 0;
        }

    return answer_size;    
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
device_communicator::device_communicator()
    {  
    dev_cnt = 0;
    
#ifdef PAC
    for ( int i = 0; i < C_MAX_COMLEX_DEVICES; i++ )
        {
        dev[ i ] = 0;
        }
#endif // PAC
    }
//-----------------------------------------------------------------------------
device_communicator::~device_communicator()
    {
    }
//-----------------------------------------------------------------------------
#ifdef DRIVER        
i_complex_device* device_communicator::get_group( char* dev_name, u_int_4 dev_n, 
                                                 char* name ) const
    {
    if ( !dev ) return 0;

    for ( unsigned int i = 0; i < dev_cnt; i++ ) 
        {
        if ( strcmp( dev[ i ]->get_name(), dev_name ) == 0 && 
            dev[ i ]->get_n() == dev_n )
            {
            return ( ( complex_device* ) dev[ i ] )->get_sub_complex_dev( name );
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int device_communicator::load_state( char *buff  )
    {
    //Структура полученных от контроллера данных:
    // 4 байта - количество устройств;
    // потом идут данные каждой группы.

    //int rec_groups_cnt = ( ( u_int_4* ) buff )[ 0 ]; 
    buff += 4;

    if ( !dev ) return -1;
    int res = 0;

    for ( unsigned int i = 0; i < dev_cnt; i++ )
        {       
        res = dev[ i ]->load_state( buff );  
        if ( res < 0 )
            {
            return res;
            }
        buff += res;
        }

    return 0;
    }        
//-----------------------------------------------------------------------------
int device_communicator::load_device( char *buff  ) 
    {
    //Структура полученных от контроллера данных:
    // 4 байта - количество групп устройств;
    // потом идут данные каждой группы.

    dev_cnt = ( ( u_int_4* ) buff )[ 0 ]; 
    buff += 4;
    if ( dev )
        {        
        bug_log::add_msg( "Driver", "", "Удалены предыдущие устройства PAC!" );
        delete[] dev;
        dev = 0;
        }
    try
        {
        dev = new i_complex_device*[ dev_cnt ];  
        }
    catch ( std::bad_alloc exception )
        {                        
        bug_log::add_warning_msg( "Driver", "",
            "device_communicator::load_device(...) - не удалось выделить память!" );
        return -2;
        }

    int res = 0;
    for ( unsigned int i = 0; i < dev_cnt; i++ )
        {

        try
            {
            dev[ i ] = new complex_device(); 
            }
        catch ( std::bad_alloc exception )
            {                                    
            bug_log::add_warning_msg( "Driver", "",
                "device_communicator::load_device(...) - не удалось выделить память!" );
            return -2;
            }

        dev[ i ]->set_idx( i ); 

        res = dev[ i ]->load_device( buff );
        if ( res < 0 )
            {
            return res;
            }

        buff += res;
        }

    return 0;
    }        
#endif // DRIVER                
//-----------------------------------------------------------------------------
void device_communicator::print() const
    {  
    char tmp_str[ 200 ];

    sprintf( tmp_str, "\nDevice communicator. Dev count = %u.", dev_cnt );
    print_str( tmp_str );

    if ( !dev_cnt ) return;

    for ( unsigned int i = 0; i < dev_cnt; i++ ) 
        {
        sprintf( tmp_str, "[ %3d ] ", i );
        print_str( tmp_str, 0 );
        dev[ i ]->print();
        } 
    sprintf( tmp_str, "Done." );
    print_str( tmp_str );
    }
//-----------------------------------------------------------------------------
#ifdef DRIVER        
int device_communicator::load_changed_state( char *buff )
    {
    // Структура полученных от контроллера данных:
    //2 байта - количество устройств;                           (1)
    //Для каждого устройства:                                   (2)
    //потом идут номер устройства в массиве устройств,          (3)
    //далее данные устройства.                                  (4)                               

    u_int_2 rec_dev_cnt = ( ( u_int_2* ) buff )[ 0 ];           //1 
    buff += sizeof( rec_dev_cnt );

    if ( !dev ) return -1;
    int res = 0;

    for ( u_int_2 i = 0; i < rec_dev_cnt; i++ )                 //2
        {     
        u_int_2 device_idx = ( ( u_int_2* ) buff )[ 0 ];        //3
        if ( device_idx > dev_cnt)
            {
#ifdef WIN32
            sprintf_s( bug_log::msg, bug_log::msg_size,
                "device_communicator::load_changed_state(...) - номер устройства [ %d ] больше количества устройств [ %d ]!",
                device_idx, dev_cnt );
            bug_log::add_warning_msg( "Driver", "" );
#endif //WIN32

#ifdef _DEBUG
            _DebugBreak();
#endif        
            return -1;
            }

        buff += sizeof( device_idx );
        res = dev[ device_idx ]->load_changed_state( buff );    //4       
        if ( res < 0 )
            {
            return res;
            }
        buff += res;            
        }

    return 0;
    }        
#endif // DRIVER                
//-----------------------------------------------------------------------------
#ifdef PAC

#ifdef USE_SIMPLE_DEV_ERRORS
#include "errors.h"

extern dev_errors_manager *g_dev_errors_manager; 
#endif // USE_SIMPLE_DEV_ERRORS

//#define DEBUG_DEV_CMCTR

long device_communicator::write_devices_states_service( long len, 
                                                       u_char *data,
                                                       u_char *outdata )
    {
    if ( len < 1 ) return 0;

    u_int answer_size = 0;
    
#ifdef DEBUG_DEV_CMCTR
    u_long start_time = get_millisec();
#endif // DEBUG_DEV_CMCTR             

    u_int param_size = 0;
    static u_int_2 g_devices_request_id = 0;

    switch ( data[ 0 ] )
        {
        case GET_INFO_ON_CONNECT:            
            param_size = sizeof( G_PROTOCOL_VERSION );
            memcpy( outdata, &G_PROTOCOL_VERSION, param_size ); //-Версия протокола.

            outdata     += param_size;
            answer_size += param_size;

            strcpy( ( char* ) outdata, 
                tcp_communicator::get_instance()->get_host_name() );            
            answer_size += strlen( 
                tcp_communicator::get_instance()->get_host_name() ) + 1;

#ifdef DEBUG_DEV_CMCTR
            Print( "G_PROTOCOL_VERSION = %u, host =[%s]\n", G_PROTOCOL_VERSION,
                tcp_communicator::get_instance()->get_host_name() );
#endif // DEBUG_DEV_CMCTR
            return answer_size;

        case GET_DEVICES:
            {            
            param_size = sizeof( g_devices_request_id );
            if ( 0 == g_devices_request_id )
                {
                memcpy( &g_devices_request_id, data + 1, param_size );
                }

            memcpy( outdata, &g_devices_request_id, param_size );
            answer_size += param_size;

            param_size = sizeof( dev_cnt );
            memcpy( outdata + answer_size, &dev_cnt, param_size );
            answer_size += param_size;

            for ( u_int i = 0; i < dev_cnt; i++ )
                {
                answer_size += dev[ i ]->save_device( ( char* ) outdata + 
                    answer_size );
                }      
#ifdef DEBUG_DEV_CMCTR
            Print( "Devices size = %u, g_devices_request_id = %u\n",
                answer_size, 
                g_devices_request_id );

            Print( "Operation time = %lu\n", get_delta_millisec( start_time ) );
#endif // DEBUG_DEV_CMCTR
            return answer_size;
            }

        case GET_DEVICES_STATES:
            {            
            param_size = sizeof( g_devices_request_id );
            memcpy( outdata, &g_devices_request_id, param_size );
            answer_size += param_size;

            param_size = sizeof( dev_cnt );
            memcpy( outdata + answer_size, &dev_cnt, param_size );
            answer_size += param_size;

            for ( u_int i = 0; i < dev_cnt; i++ )
                {
                answer_size += dev[ i ]->save_state( ( char* ) outdata +
                    answer_size );
                }
#ifdef DEBUG_DEV_CMCTR
            Print( "Devices states size = %u, g_devices_request_id = %d\n",
                answer_size, g_devices_request_id );

            Print( "Operation time = %lu\n", get_delta_millisec( start_time ) );
#endif // DEBUG_DEV_CMCTR
            return answer_size;
            }

        case GET_DEVICES_CHANGED_STATES:
            {
            param_size = sizeof( g_devices_request_id );
            memcpy( outdata, &g_devices_request_id, param_size );
            answer_size += param_size;

            u_int_2 changed_device_cnt = 0;

            param_size = sizeof( changed_device_cnt );
            memcpy( outdata + answer_size, &changed_device_cnt, param_size );
            answer_size += param_size;

            for ( u_int i = 0; i < dev_cnt; i++ )
                {
                u_int_2 res = dev[ i ]->save_changed_state( ( char* ) outdata +
                    answer_size + sizeof( i ) );
                if ( res )
                    {
                    param_size = sizeof( i );
                    memcpy( outdata + answer_size, &i, param_size );
                    answer_size += res + param_size;
                    changed_device_cnt++;
                    }                
                }

            param_size = sizeof( changed_device_cnt );
            memcpy( outdata + sizeof( g_devices_request_id ),
                &changed_device_cnt, param_size );

#ifdef DEBUG_DEV_CMCTR
            Print( "\nChanged states size = %d\n", answer_size );

            Print( "Operation time = %lu\n", get_delta_millisec( start_time ) );
#endif // DEBUG_DEV_CMCTR
            return answer_size;
            }

        case EXEC_DEVICE_CMD:
            {
#ifdef DEBUG_DEV_CMCTR
            Print( "\nEXEC_DEVICE_CMD\n" );
            u_long par = 0;
            param_size = sizeof( par );
            memcpy( &par, data + 1, param_size );
            Print( "unsigned long buff[1] - %lu; ", par );
            memcpy( &par, data + 1 + param_size, param_size );
            Print( "[2] - %lu; ", par );
            memcpy( &par, data + 1 + 2 * param_size, param_size );
            Print( "[3] - %lu; ", par );
            memcpy( &par, data + 1 + 3 * param_size, param_size );
            Print( "[4] - %lu\n", par );

            float par_float = 0;
            param_size = sizeof( par_float );
            memcpy( &par_float, data + 1, param_size );
            Print( "float         buff[1] - %f; ", par_float );
            memcpy( &par_float, data + 1 + param_size, param_size );
            Print( "[2] - %f; ", par_float );
            memcpy( &par_float, data + 1 + 2 * param_size, param_size );
            Print( "[3] - %f; ", par_float );
            memcpy( &par_float, data + 1 + 3 * param_size, param_size );
            Print( "[4] - %f\n", par_float );
#endif // DEBUG_DEV_CMCTR

            u_int_4 dev_n = 0;
            param_size = sizeof( dev_n );
            memcpy( &dev_n, data + 1 , param_size );
            dev[ dev_n ]->parse_cmd( ( char* ) data + 5 );

#ifdef DEBUG_DEV_CMCTR
            Print( "Operation time = %lu\n", get_delta_millisec( start_time ) );
#endif // DEBUG_DEV_CMCTR
            
            outdata[ 0 ] = 0;
            outdata[ 1 ] = 0; //Возвращаем 0.
            answer_size = 2;
            return answer_size;
            }

        case GET_PAC_ERRORS:
            outdata[ 0 ] = 0;
            outdata[ 1 ] = 0; //Возвращаем 0.
            answer_size = 2;
#ifdef DEBUG_DEV_CMCTR
            Print( "GET_PAC_ERRORS\n" );
#endif

#if defined I7186_E
            if ( g_pac_critical_errors )
                {
                answer_size = g_pac_critical_errors->save_to_stream( outdata );
                }
#endif // defined I7186_E

#ifdef DEBUG_DEV_CMCTR
            Print( "Critical errors count = %d, answer size = %d\n",
                outdata[ 2 ], 
                answer_size );
#endif // DEBUG_DEV_CMCTR

#ifdef USE_SIMPLE_DEV_ERRORS
            answer_size += g_dev_errors_manager->save_to_stream( outdata + 
                answer_size );
#ifdef DEBUG_DEV_CMCTR
            Print( "Simple devices errors count = %d, answer size = %d\n", 
                ( outdata + answer_size )[ 4 ], answer_size );
#endif // DEBUG_DEV_CMCTR
#endif // USE_SIMPLE_DEV_ERRORS

            return answer_size;

#ifdef USE_SIMPLE_DEV_ERRORS
        case SET_PAC_ERROR_CMD:            
            unsigned int count = *( ( u_int_2* ) ( data + 1 ) ); 

            unsigned int *uint_cmd = ( unsigned int* ) ( data + 1 + 2 );
            for ( i = 0; i < count; i++ )
                {
#ifdef DEBUG_DEV_CMCTR
                Print( "SET_PAC_ERROR_CMD" );
                Print( "cmd = %u, object_type = %u, object_number = %u, \
                       object_alarm_number = %u\n", uint_cmd[ 0 ],
                       uint_cmd[ 1 ], uint_cmd[ 2 ], uint_cmd[ 3 ]   );
#endif // DEBUG_DEV_CMCTR

                g_dev_errors_manager->set_cmd( uint_cmd[ 0 ], uint_cmd[ 1 ],
                    uint_cmd[ 2 ], uint_cmd[ 3 ] );

                uint_cmd += 4;
                }

            ( ( u_int_2* ) ( outdata + answer_size ) )[ 0 ] = 0; //Возвращаем 0.
            answer_size += 2;

            return answer_size;
#endif // USE_SIMPLE_DEV_ERRORS
        }

    return answer_size;
    }                  
//-----------------------------------------------------------------------------
int device_communicator::add_device( i_complex_device *device )
    {
    if ( dev_cnt < C_MAX_COMLEX_DEVICES )
        {
        dev[ dev_cnt++ ] = device;
        }
    else
        {
#ifdef DEBUG
        Print( "device_communicator::add_device() - Error : max devices is used!\n" );
#endif // DEBUG
        return 1;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
#endif //PAC
