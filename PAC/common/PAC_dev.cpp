#include "PAC_dev.h"
//-----------------------------------------------------------------------------
template < class data_type > array_device< data_type >::array_device( 
    u_int_4 n, 
    char *new_name, 
    u_int_2 new_subdev_cnt, 
    char type ):
sub_dev_cnt( new_subdev_cnt ), 
type( type ), 
n( n )
    {
    name = new char[ strlen( new_name ) + 1 ];
    strcpy( name, new_name );

    prev_val = new data_type[ sub_dev_cnt ];
    for ( int i = 0; i < sub_dev_cnt; i++ )
        {
        prev_val[ i ] = 0;
        }              
    }
//-----------------------------------------------------------------------------
// Данные группы (buff) в следующем виде:
//  4 байта - номер устройства;                       (1)
//  4 байта - количество подустройств;                (2)
//  далее   - данные каждого подустройства.
template < class data_type > int array_device< data_type >::save_state( 
    char *buff )
    {
    ( ( u_int_4* ) buff )[ 0 ] = n;                   //(1)
    ( ( u_int_4* ) buff )[ 1 ] = sub_dev_cnt;	      //(2)
    u_int_2 answer_size = 8;

    for ( u_int_4 i = 0; i < sub_dev_cnt; i++ )
        {
        //( ( data_type* ) ( buff + 8 + i*sizeof( data_type ) ) )[ 0 ] = 
        //    get_val( i );

        ( ( data_type* ) ( buff + 8 ) )[ 0 ] = 
            get_val( i );
        buff += sizeof( data_type );
        prev_val[ i ] = get_val( i );
        answer_size += sizeof( data_type );      
        }      

    return answer_size;
    } 
//-----------------------------------------------------------------------------
// Данные группы (buff) в следующем виде:          
//  2 байта - количество устройств, измененных свое состоние; (1)
//  2 байта - номер устройства в массиве устройств.           (2)
//  далее   - его измененое состояние                         (3)
// Сохраняем переданное значение как предыдущее значение.     (4)
//Изменяем размер ответа.                                     (5)
//Увеличиваем на 1 количество устройств, изменивщих свое
//состояние.                                                  (6)
//Если нет устройств, изменивших свое состояние, возвращаем 0. (7)
template < class data_type > int array_device< data_type >::
save_changed_state( char *buff )
    {              
    u_int_2 *changed_dev_cnt = ( u_int_2* ) ( buff );	      //1
    *changed_dev_cnt = 0;
    u_int_2 answer_size = 2;

    for ( u_int_2 i = 0; i < sub_dev_cnt; i++ )
        {
        if ( get_val( i ) != prev_val[ i ] )
            {
            ( ( u_int_2* ) ( buff + answer_size ) )[ 0 ] = i; //2
            answer_size += 2;
            ( ( data_type* ) ( buff + answer_size ) )[ 0 ] =  //3
                get_val( i );
            prev_val[ i ] = get_val( i );                     //4
            answer_size += sizeof( data_type );               //5
            ( *changed_dev_cnt )++;                           //6
            }
        }
    if ( 2 == answer_size )                                   //7
        {
        return 0;
        }

    return answer_size;              
    } 
//-----------------------------------------------------------------------------
// Данные группы (buff) в следующем виде:
//    1 байт  - тип;                                  (1)
//    4 байта - номер;                                (2)
//    1 байт  - длина имени группы устройства;        (3)
//    х байт  - имя группы устройства;                (4)
//    4 байта - количество подустройств;              (5)
template < class data_type > int array_device< data_type >::
save_device( char *buff )
    {
    u_int_2 idx = 0;

    buff[ idx++ ] = type;                             //(1)                            
    ( ( u_int_4* ) ( buff + idx ) )[ 0 ] = n;         //(2)
    idx += 4;
    buff[ idx++ ] = strlen( name );                   //(3)              
    strcpy( buff + idx, name );                       //(4)
    idx += strlen( name ) + 1;
    ( ( u_int_4* ) ( buff + idx ) )[ 0 ] = sub_dev_cnt;//(5)
    idx += 4;           

    return idx;
    }
//-----------------------------------------------------------------------------
template < class data_type > void array_device< data_type >::print() const 
    {
    char tmp_str[ 100 ];    
    //sprintf( tmp_str, "\"%s\", \t[ %3lu ]", name, sub_dev_cnt );
    if ( strlen( name ) < 8 ) 
        {
        sprintf( tmp_str, "\"%s\", \t\t[ %3lu ]", name, sub_dev_cnt );
        }
    else sprintf( tmp_str, "\"%s\", \t[ %3lu ]", name, sub_dev_cnt );

    print_str( tmp_str );
    }
//-----------------------------------------------------------------------------
template < class data_type > u_int_4 array_device< data_type >::get_n() const
    { 
    return 0; 
    } 
//-----------------------------------------------------------------------------
template < class data_type > int array_device< data_type >::
load_state( char *buff ) 
    { 
    buff++;
    return 0; 
    } 
//-----------------------------------------------------------------------------
template < class data_type > int array_device< data_type >::load_changed_state( 
    char *buff ) 
    { 
    buff++;
    return 0; 
    } 
//-----------------------------------------------------------------------------
template < class data_type > int array_device< data_type >::load_device( 
    char *buff )  
    { 
    buff++;
    return 0; 
    } 
//-----------------------------------------------------------------------------
template < class data_type > int array_device< data_type >::parse_cmd( 
    char *buff  )   
    {      
    buff++;
    return 0; 
    } 
//-----------------------------------------------------------------------------
template < class data_type > u_int_4 array_device< data_type >::get_idx()
    {
    return 0;
    }
//-----------------------------------------------------------------------------
template < class data_type > void array_device< data_type >::set_idx( 
    u_int_4 new_idx )
    {  
    new_idx++;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
