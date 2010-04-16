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
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PAC_DEVICES_H
#define PAC_DEVICES_H

#include <limits.h>

#include <string.h>
#include <stdio.h>

#include "sys.h"
#include "wago.h"

#include "g_device.h"
//-----------------------------------------------------------------------------
/// @brief Шаблон класса, который используется для передачи состояния устройств,
/// которые представляются массивом некоторого типа.
//
/// Например: параметры, состояния и т.д.
template < class data_type > class i_array_device: public i_simple_device
    {
    public:
        /// @param n        - номер.
        /// @param new_name - имя.
        /// @param new_subdev_cnt - количество элементов в массиве.
        /// @param type     - тип. Для сохранения устройств и передачи на
        /// сервер.
        i_array_device( u_int_4 n, 
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

        /// @brief Сохранение устройства в байтовый поток.
        ///
        /// Для передачи устройства на сервер.
        ///
        /// @param buff [out] - указатель на байтовый буфер, куда будет
        /// произведено сохранение устройства.
        ///
        /// @return - количество сохраненных байт.
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

        /// @brief Сохранение состояния устройства в байтовый поток.
        ///
        /// Для передачи состояния устройства на сервер.
        ///
        /// @param buff [out] - указатель на байтовый буфер, куда будет
        /// произведено сохранение состояния устройства.
        ///
        /// @return - количество сохраненных байт.
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

        /// @brief Сохранение изменившегося состояния устройства в байтовый поток.
        ///
        /// Для передачи состояния устройства на сервер.
        ///
        /// @param buff [out] - указатель на байтовый буфер, куда будет
        /// произведено сохранение состояния устройства.
        ///
        /// @return - количество сохраненных байт.
        int  save_changed_state( char *buff )
            {
            // Данные группы (buff) в следующем виде:          
            //  2 байта - количество устройств, изменивших свое состояние;  (1)
            //  2 байта - номер устройства в массиве устройств.             (2)
            //  далее   - его измененное состояние                          (3)
            // Сохраняем переданное значение как предыдущее значение.       (4)
            //Изменяем размер ответа.                                       (5)
            //Увеличиваем на 1 количество устройств, изменивших свое
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

        /// @brief Получение значения элемента через индекс. 
        ///
        /// Это значение потом, например, может используется для сохранения 
        /// объекта в поток ( @ref save_state ).
        ///
        /// @param idx - индекс элемента.
        ///
        /// @return - значение элемента с индексом @ref idx.
        virtual data_type get_val( int idx ) = 0;

        /// @brief Вывод объекта в консоль.
        ///
        /// Для использования в отладочных целях.
        void print() const
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

        /// @brief Реализация-заглушка интерфейса класса @ref i_simple_device.
        u_int_4 get_n() const
            { 
            return 0; 
            } 

    protected:
        u_int_4         sub_dev_cnt;    ///< Количество подустройств.
        char            *name;          ///< Имя.
        char            type;           ///< Тип.
        u_int_4         n;              ///< Уникальный номер.

        data_type*      prev_val;       ///< Массив предыдущих значений.
    };
//-----------------------------------------------------------------------------
/// @brief 
//
///
class device : public i_simple_device
    {
    public:
        device(): number( 0 )
        {
        }

        enum DEVICE_TYPE
            {
            DT_V = 0, 
            DT_N,
            DT_M,
            DT_LS,     
            DT_TE,     
            DT_FE,     
            DT_FS,     
            DT_CTR,    
            DT_AO,     
            DT_LE,
            DT_FB,     
            DT_UPR,    
            DT_QE,    
            DT_AI,
            };

        enum DEVICE_SUB_TYPE
            {
            //DT_V = 0, 
            DST_V_1DO = 0,            
            DST_V_2DO,
            DST_V_1DO_1DI,
            DST_V_1DO_2DI,
            DST_V_2DO_2DI,
            DST_V_1DO_3DI, 
            DST_V_1DO_2DI_S,    // 2 FB на одно из состояний.
            DST_V_AS_MIX,       // Микспруф с AS интерфейсом.
            };

        virtual void on() = 0;
        virtual void off() = 0;
        virtual int  get_state() = 0;
        virtual int  set_state( int new_state ) = 0;

        virtual float get_value() = 0;
        virtual int   set_value( float new_value ) = 0;

        // Сохранение самого устройства в буфер.
        int save_device( char *buff  )
            {
            ( ( u_int_4* ) buff )[ 0 ] = number;
            return sizeof( u_int_4 );
            }
     
        void print() const
            {  
#ifdef DEBUG
            Print( "%5lu\n", ( unsigned long ) number );
#endif // DEBUG            
            }

        virtual int load( char *stream )
            {
            number = *( ( u_int_4* ) stream );
            return sizeof( u_int_4 );
            }

        //Возвращает номер устройства.
        u_int_4 get_n() const
            {
            return number;
            }

    protected:
        u_int_4 number;

        DEVICE_TYPE     type;
        DEVICE_SUB_TYPE sub_type;
    };

//-----------------------------------------------------------------------------
/// @brief
///
///
class char_state_device : public device
    {
    public:
         // Сохранение измененного состояния устройства в буфер.
        int save_changed_state( char *buff )
            {
            if ( prev_state != get_state() )
                {
                buff[ 0 ] = get_state();
                prev_state = get_state();
                return sizeof( char );
                }
            return 0;
            }

        // Сохранение состояния устройства в буфер.
        int save_state( char *buff  )
            {
            buff[ 0 ] = get_state();
            prev_state = get_state();
            return sizeof( char );
            }

    private:
        char prev_state;
    };
//-----------------------------------------------------------------------------
/// @brief 
///
/// 
class DI_device
    {
    public:
        virtual int get_state() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief 
//
/// 
class DO_device: public DI_device
    {
    public:
        virtual void on() = 0;
        virtual void off() = 0;

        virtual int set_state( int new_state ) = 0;
    };
//-----------------------------------------------------------------------------
/// @brief 
//
/// 
class AI_device
    {
    public:
        virtual float get_value() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief 
//
/// 
class AO_device: public AI_device
    {
    public:
        virtual int off() = 0;

        virtual float set_value( float new_value ) = 0;
    };
//-----------------------------------------------------------------------------
/// @brief 
//
/// 
class wago_device
    {
    public:
        wago_device(): DI_count( 0 ), DI_tables( 0 ), DI_offsets( 0 ),
            DO_count( 0 ), DO_tables( 0 ), DO_offsets( 0 ),
            AI_count( 0 ), AI_tables( 0 ), AI_offsets( 0 ),
            AO_count( 0 ), AO_tables( 0 ), AO_offsets( 0 )
            {
            }            

        virtual int load( char *stream )
            {
            //[ 0   ] - номер устройства.
            //[ 1   ] - 
            //[ 2   ] - 
            //[ 3   ] - 
            int pos = 0;

            //[ 4   ] - количество DI.
            //[ 5   ] - 
            //[ 6   ] - номер таблицы DI.
            //[ 7   ] - 
            //[ 8   ] - смещение в пределах таблицы.
            //[ ... ] -   
            //[ x1  ] - количество DO.
            //[ ... ] -   
            //[ x2  ] - количество AO.
            //[ ... ] -   
            //[ x3  ] - количество AI.
            //[ ... ] -            
            pos += load_table( stream + pos, DI_count, DI_tables, DI_offsets );
            pos += load_table( stream + pos, DO_count, DO_tables, DO_offsets );
            pos += load_table( stream + pos, AI_count, AI_tables, AI_offsets );
            pos += load_table( stream + pos, AO_count, AO_tables, AO_offsets );
            return pos;
            }

    protected:
        int get_DO( u_int index )
            {
            if ( index < DO_count && DO_tables && DO_offsets )
                {
                return wago_manager::get_instance()->get_DO( DO_tables[ index ],
                DO_offsets[ index ] );
                }

#ifdef DEBUG
                Print( "wago_device->get_DO(...) - error!\n" );
#endif // DEBUG

            return 0;
            }

        int set_DO( u_int index, char value )
            {
            return wago_manager::get_instance()->set_DO( DO_tables[ index ],
                DO_offsets[ index ], value );
            }

        int get_DI( u_int index )
            {
            return wago_manager::get_instance()->get_DI( DO_tables[ index ],
                DO_offsets[ index ] );
            }
        int set_DI( u_int index, char value )
            {
            return wago_manager::get_instance()->set_DI( DO_tables[ index ],
                DO_offsets[ index ], value );
            }

        float get_AO( u_int index )
            {
            return wago_manager::get_instance()->get_AO( DO_tables[ index ],
                DO_offsets[ index ] );
            }
        int   set_AO( u_int index, float value )
            {
            return wago_manager::get_instance()->set_AO( DO_tables[ index ],
                DO_offsets[ index ], value );
            }

        float get_AI( u_int index )
            {
            return wago_manager::get_instance()->get_AI( DO_tables[ index ],
                DO_offsets[ index ] );
            }
        int   set_AI( u_int index, float value )
            {
            return wago_manager::get_instance()->set_AI( DO_tables[ index ],
                DO_offsets[ index ], value );
            }

    private:
        u_int_2  DI_count;
        u_int_2* DI_tables;
        u_int_2* DI_offsets;

        u_int_2  DO_count;
        u_int_2* DO_tables;
        u_int_2* DO_offsets;

        u_int_2  AI_count;
        u_int_2* AI_tables;
        u_int_2* AI_offsets;

        u_int_2  AO_count;
        u_int_2* AO_tables;
        u_int_2* AO_offsets;

        int load_table( char *stream, u_int_2 &count, u_int_2 *tables,
            u_int_2 *offsets )
            {
            //[ 0   ] - количество DI (например).
            //[ 1   ] - 
            //[ 2   ] - номер таблицы DI №1.
            //[ 3   ] - 
            //[ 4   ] - смещение в пределах таблицы №1.
            //[ 5   ] -
            //[ 6   ] - номер таблицы DI №2.
            //[ 7   ] - 
            //[ 8   ] - смещение в пределах таблицы №2.
            //[ ... ] -  
            count = *( ( u_int_2 * ) stream );
            int pos = sizeof( u_int_2 );

            if ( count > 0 )
                {
                tables = new u_int_2[ count ];
                offsets = new u_int_2[ count ];
                for ( u_int i = 0; i < count; i++ )
                    {
                    tables[ i ] = *( ( u_int_2 * ) ( stream + pos ) );
                    pos += sizeof( u_int_2 );

                    offsets[ i ] = *( ( u_int_2 * ) ( stream + pos ) );
                    pos += sizeof( u_int_2 );
                    }
                }

            return pos;
            }
    };
//-----------------------------------------------------------------------------
/// @brief 
//
/// 
class DO_1 : public char_state_device,
        public DO_device,
        public wago_device
    {
    public:
        float get_value()
            {
            return get_state();
            }

        int set_value( float new_value )
            {
            return set_state( new_value );
            }
         
        int get_state()
            {
            return get_DO( DO_INDEX );
            }

        void on()
            {
            set_DO( DO_INDEX, 1 );
            }

        void off()
            {
            set_DO( DO_INDEX, 0 );
            }

        int set_state( int new_state )
            {
            return set_DO( DO_INDEX, new_state );
            }

        int parse_cmd( char *buff  )
            {
            set_state( buff[ 0 ] );
            return sizeof( char );
            }

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,
            };
    };
//-----------------------------------------------------------------------------
/// @brief 
//
/// 
class device_manager
    {
    public:
        int load_from_stream( char *stream );

        static DO_device* get_V( int number )
            {
            return 0;
            }
            
    private:
        device **project_devices;
    };
//-----------------------------------------------------------------------------

#define V device_manager::get_V


#endif // PAC_DEVICES_H
