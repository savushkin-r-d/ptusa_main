/// @file PAC_dev.h
/// @brief Содержит описания классов, которые реализуют функции передачи 
/// состояния устройств PAC на сервер. Классы используются ТОЛЬКО в 
/// контроллере ( PAC ).
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG   - компиляция c выводом отладочной информации в консоль.
/// 
/// @par Текущая версия:
/// @$Rev: $.\n
/// @$Author: $.\n
/// @$Date::                     $.

#ifndef PAC_DEVICES_H
#define PAC_DEVICES_H

#include <string.h>
#include <stdio.h>
#include "g_device.h"

//-----------------------------------------------------------------------------
/// @brief Шаблон класса, который используется для передачи состояния устройств,
/// которые представляются массивом некоторого типа.
//
/// Например: параметры, состояния и т.д.
template < class data_type > class array_device: public i_simple_device
    {
    public:        
        array_device( u_int_4 n, 
            const char *new_name,
            u_int_2 new_subdev_cnt,
            char type ): sub_dev_cnt( new_subdev_cnt ), 
            type( type ), 
            n( n )
            {
            name = new char[ strlen( new_name ) + 1 ];
            strcpy( name, new_name );

            prev_val = new data_type[ sub_dev_cnt ];
            for ( unsigned int i = 0; i < sub_dev_cnt; i++ )
                {
                prev_val[ i ] = 0;
                }              
            }

        int  save_device( char *buff )
            {
            // Данные группы (buff) в следующем виде:
            //    1 байт  - тип;                                (1)
            //    4 байта - номер;                              (2)
            //    1 байт  - длина имени группы устройства;      (3)
            //    х байт  - имя группы устройства;              (4)
            //    4 байта - количество подустройств;            (5)

            u_int_2 idx = 0;

            buff[ idx++ ] = type;                               //(1)
            ( ( u_int_4* ) ( buff + idx ) )[ 0 ] = n;           //(2)
            idx += 4;
            buff[ idx++ ] = strlen( name );                     //(3)
            strcpy( buff + idx, name );                         //(4)
            idx += strlen( name ) + 1;
            ( ( u_int_4* ) ( buff + idx ) )[ 0 ] = sub_dev_cnt; //(5)
            idx += 4;           

            return idx;

            }

        int  save_state( char *buff )
            {
            // Данные группы (buff) в следующем виде:
            //  4 байта - номер устройства;                     (1)
            //  4 байта - количество подустройств;              (2)
            //  далее   - данные каждого подустройства.
            ( ( u_int_4* ) buff )[ 0 ] = n;                     //(1)
            ( ( u_int_4* ) buff )[ 1 ] = sub_dev_cnt;           //(2)
            u_int_2 answer_size = 8;

            for ( u_int_4 i = 0; i < sub_dev_cnt; i++ )
                {
                ( ( data_type* ) ( buff + 8 ) )[ 0 ] = 
                    get_val( i );
                buff += sizeof( data_type );
                prev_val[ i ] = get_val( i );
                answer_size += sizeof( data_type );      
                }      

            return answer_size;

            }
        int  save_changed_state( char *buff )
            {
            // Данные группы (buff) в следующем виде:          
            //  2 байта - количество устройств, измененных свое состоние;   (1)
            //  2 байта - номер устройства в массиве устройств.             (2)
            //  далее   - его измененое состояние                           (3)
            // Сохраняем переданное значение как предыдущее значение.       (4)
            //Изменяем размер ответа.                                       (5)
            //Увеличиваем на 1 количество устройств, изменивщих свое
            //состояние.                                                    (6)
            //Если нет устройств, изменивших свое состояние, возвращаем 0.  (7)
            u_int_2 *changed_dev_cnt = ( u_int_2* ) ( buff );               //1
            *changed_dev_cnt = 0;
            u_int_2 answer_size = 2;

            for ( u_int_2 i = 0; i < sub_dev_cnt; i++ )
                {
                if ( get_val( i ) != prev_val[ i ] )
                    {
                    ( ( u_int_2* ) ( buff + answer_size ) )[ 0 ] = i;       //2
                    answer_size += 2;
                    ( ( data_type* ) ( buff + answer_size ) )[ 0 ] =        //3
                        get_val( i );
                    prev_val[ i ] = get_val( i );                           //4
                    answer_size += sizeof( data_type );                     //5
                    ( *changed_dev_cnt )++;                                 //6
                    }
                }
            if ( 2 == answer_size )                                         //7
                {
                return 0;
                }

            return answer_size;              

            }

        virtual data_type get_val( int idx ) = 0;

        void    print() const
            {
            char tmp_str[ 100 ];    

            if ( strlen( name ) < 8 ) 
                {
                sprintf( tmp_str, "\"%s\", \t\t[ %3lu ]", 
                    name, ( unsigned long int ) sub_dev_cnt );
                }
            else sprintf( tmp_str, "\"%s\", \t[ %3lu ]", 
                    name, ( unsigned long int ) sub_dev_cnt );

            print_str( tmp_str );
            }

        u_int_4 get_n() const
            { 
            return 0; 
            } 

        int load_state( char *buff )
            { 
            buff++; // Чтобы не было warning'а.
            return 0; 
            } 

        int load_changed_state( char *buff )
            { 
            buff++; // Чтобы не было warning'а.
            return 0; 
            } 

        int load_device( char *buff )
            { 
            buff++; // Чтобы не было warning'а.
            return 0; 
            } 

        int     parse_cmd( char *buff  )
            {      
            buff++; // Чтобы не было warning'а.
            return 0; 
            } 

        u_int_4 get_idx()
            {
            return 0;
            }

        void set_idx( u_int_4 new_idx )
            {  
            new_idx++; // Чтобы не было warning'а.
            }

    protected:
        u_int_4         sub_dev_cnt;    ///< Количество подустройств.
        char            *name;          ///< Имя.
        char            type;           ///< Тип.
        u_int_4         n;              ///< Уникальный номер.

        data_type*      prev_val;       ///< Массив предыдущих значений.
    };
//-----------------------------------------------------------------------------

#endif // PAC_DEVICES_H

