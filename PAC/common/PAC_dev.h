/// @file PAC_dev.h
/// @brief Классы, которые реализуют функции передачи 
/// состояния устройств PAC на сервер. Также реализованы классы для работы с 
/// устройствами ( клапана, насосы,... ). Классы используются ТОЛЬКО в 
/// контроллере ( PAC ).
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG - компиляция c выводом отладочной информации в консоль.
/// @c DEBUG_NO_WAGO_MODULES - простые устройства работают без модулей
/// wago (состояния хранят в себе).
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PAC_DEVICES_H
#define PAC_DEVICES_H

#include <string.h>
#include <stdio.h>

#include <vector>

#include "sys.h"
#include "wago.h"
#include "g_device.h"

class saved_params_u_int_4;

//for levels
#if defined LEVEL_REVERSE
#define LIS     1
#define LNO     0
#else
#define LIS     0
#define LNO     1
#endif

#define OFF     0
#define ON      1

//-----------------------------------------------------------------------------
/// @brief Шаблон класса, который используется для передачи состояния устройств,
/// которые представляются массивом некоторого типа.
//
/// Например: параметры, состояния и т.д.
template < class data_type > class array_device: public i_simple_device
    {
    public:
        /// @param n        - номер.
        /// @param new_name - имя.
        /// @param new_subdev_cnt - количество элементов в массиве.
        /// @param type     - тип. Для сохранения устройств и передачи на
        /// сервер.
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

        virtual ~array_device()
            {
            if ( name )
                {
                delete [] name;
                name = 0;
                }
            if ( prev_val )
                {
                delete [] prev_val;
                prev_val = 0;
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
            memcpy( buff + idx, &n, sizeof( n ) );              //(2)
            idx += sizeof( n );
            buff[ idx++ ] = strlen( name );                     //(3)
            strcpy( buff + idx, name );                         //(4)
            idx += strlen( name ) + 1;
            memcpy( buff + idx, &sub_dev_cnt, sizeof( sub_dev_cnt ) );  //(5)
            idx += sizeof( sub_dev_cnt );

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
            u_int_2 answer_size = 0;
            memcpy( buff + answer_size, &n, sizeof( n ) );                    //(1)
            answer_size += sizeof( n );
            memcpy( buff + answer_size, &sub_dev_cnt, sizeof( sub_dev_cnt ) );//(2)
            answer_size += sizeof( sub_dev_cnt );

            for ( u_int_4 i = 0; i < sub_dev_cnt; i++ )
                {
                data_type val = get_val( i );
                memcpy( buff + answer_size, &val, sizeof( val ) );

                prev_val[ i ] = val;
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
        /// @return - значение элемента с заданным индексом.
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
//-----------------------------------------------------------------------------
//ОБЩЕЕ ОПИСАНИЕ.
//  Служит для представления состояния сложных объектов (танк, гребенка).
template < class data_type > class device_state:
public array_device < data_type >
    {
    public:
        enum OBJECT_TYPE
            {
            T_TECH_OBJECT = 1,
            T_PID,
            T_CNTR,
            T_MNGR,
            T_PACK_DEVICE,
            T_F_DEVICE,
            T_MSA,

            T_PATH_COMB,   //Гребенка маршрутов.
            };

        device_state( u_int_4 n,
            const char *new_name,
            u_int_2 new_subdev_cnt,
            char type,
            u_int_4 *state,
            void *owner_object,
            char owner_type ):
        array_device < data_type >( n,
            new_name,
            new_subdev_cnt,
            type ),                                       
            state( state ),
            owner_object( owner_object ),
            owner_type( owner_type )
            {
            }

        virtual ~device_state()
            {
            }

    protected:
        u_int_4     *state;
        void        *owner_object;
        char        owner_type;
    };
//-----------------------------------------------------------------------------
//ОБЩЕЕ ОПИСАНИЕ.
//  Служит для представления состояния сложных объектов (танк, гребенка).
//  Каждый режим передается через 1 байт.
class single_state: public device_state < char >
    {
    public:
        single_state( const char *name, int n, u_int_4 *state,
            void *owner_object, char owner_type, int size ):
        device_state < char >( n, name, size,
            i_complex_device::ARRAY_DEV_BYTE,
            state,
            owner_object,
            owner_type )
            {
            }

        virtual ~single_state()
            {            
            }

        char get_val( int idx );
        int  parse_cmd( char *buff  );
    };
//-----------------------------------------------------------------------------
//ОБЩЕЕ ОПИСАНИЕ.
//  Служит для представления состояния сложных объектов (танк, гребенка).
//  Каждые 32 режима передаются как одно слово (4 байта).
class complex_state: public device_state < u_int_4 >
    {
    public:
        complex_state( const char *name, int n, u_int_4 *state,
            void *owner_object, char owner_type, int size = 1 );

        virtual ~complex_state()
            {
            }

        u_int_4 get_val( int idx );
        int     parse_cmd( char *buff  );
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//ОБЩЕЕ ОПИСАНИЕ.
//  Шаблон класса, который используется для передачи строк.
class string_device: public i_simple_device
    {
    protected:
        char    *name;          // Имя.
        u_int_4 n;              // Уникальный номер.
        char    *str;           // Строка.
        int     max_str_len;    // Максимальная длина строки, для set_value().

    public:
        string_device( u_int_4 n, const char *new_name, char* str, int max_str_len );

        int  save_device( char *buff );
        int  save_state( char *buff );
        int  save_changed_state( char *buff );

        void    print() const;
        u_int_4 get_n() const;

        int     parse_cmd( char *buff  );
    };
//-----------------------------------------------------------------------------
//ОБЩЕЕ ОПИСАНИЕ.
//  Состояние переменной с определенным именем.
class var_state:public i_simple_device
    {    
    public:
        var_state( const char *new_name, char type,
            u_int_4 &var ):var( var ),
            type( type )
            {            
            strlcpy( name, new_name, C_MAX_NAME_LENGTH );
            }

        int parse_cmd( char *buff  )
            {
            var = ( ( u_int_4* ) ( buff + 4 ) )[ 0 ];
            return 4 + 4;
            }

        int save_device( char *buff )
            {
            // Данные группы (buff) в следующем виде:
            //    1 байт  - тип;                              (1)
            //    4 байта - номер;                            (2)
            //    1 байт  - длина имени группы устройства;    (3)
            //    х байт  - имя группы устройства;            (4)
            //    4 байта - количество подустройств;          (5)
            u_int_2 idx = 0;

            buff[ idx++ ] = type;                             //(1)                            
            ( ( u_int_4* ) ( buff + idx ) )[ 0 ] = 1;         //(2)
            idx += 4;
            buff[ idx++ ] = strlen( name );                   //(3)              
            strcpy( buff + idx, name );                       //(4)
            idx += strlen( name ) + 1;
            ( ( u_int_4* ) ( buff + idx ) )[ 0 ] = 1;         //(5)
            idx += 4;           

            return idx;
            }

        int save_state( char *buff )
            {
            // Данные группы (buff) в следующем виде:
            //  4 байта - номер устройства;           (1)
            //  4 байта - количество подустройств;    (2)
            //  4 байта - данные переменной.          (3)  

            ( ( u_int_4* ) buff )[ 0 ] = 1;           //(1)
            ( ( u_int_4* ) buff )[ 1 ] = 1;           //(2)

            ( ( u_int_4* ) buff )[ 2 ] = var;         //(3)
            return 4 + 4 + 4;
            }

        int save_changed_state( char *buff )
            {
            return save_state( buff );
            }

        void print() const
            {
            char tmp_str[ 100 ];    
            sprintf( tmp_str, "\"%s\" ", name );

            print_str( tmp_str );
            }

        u_int_4 get_n() const
            {
            return 1;
            }

        int load_state( char *buff )
            {
            buff++;
            return 0;
            }

        int load_changed_state( char *buff )
            {
            buff++;
            return 0;
            }

        int load_device( char *buff )
            {
            buff++;
            return 0;
            }

        u_int_4 get_idx()
            {
            return 1;
            }

        void set_idx( u_int_4 new_idx )
            {
            new_idx++;
            }

    private:
        enum CONSTANTS
            {
            C_MAX_NAME_LENGTH = 20,
            };

        u_int_4 &var;
        char    type;
        char    name[ C_MAX_NAME_LENGTH ];          //Имя.
    };
//-----------------------------------------------------------------------------
/// @brief Интерфейс счетчика.
class i_counter
    {
    public:
        /// @brief Приостановка работы счетчика.
        virtual void pause() = 0;

        /// @brief Возобновление работы счетчика.
        virtual void start() = 0;

        /// @brief Сброс счетчика и остановка счета.
        /// 
        /// После сброса для продолжения работы необходимо вызвать @ref start().
        virtual void reset() = 0;

        /// @brief Сброс счетчика и продолжение счета.
        void restart();

        /// @brief Получение значения счетчика.
        virtual u_int get_quantity() = 0;

        virtual ~i_counter()
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе дискретного входа.
///
/// Обратная связь, предельный уровень и т.д. являются примерами таких
/// устройств.
class i_DI_device
    {
    public:
        i_DI_device();

        /// @brief Получение текущего состояния устройства.
        ///
        /// @return - текущее состояние устройства в виде целого числа.
        virtual int get_state_now() = 0;

        /// @brief Получение отфильтрованного состояния устройства.
        ///
        /// Устройство меняет свое состояние, если дискретный вход находится в
        /// в данном состоянии больше заданного интервала времени (Задание
        /// интервала - смотри @ref set_change_time).
        ///
        /// @return - состояние устройства в виде целого числа.
        virtual int get_state();

        /// @brief Установка времени ожидания изменения состояния.
        ///
        /// @param time - время ожидания изменения состояния.
        void set_change_time( u_int time );

        /// @brief Установка состояния.
        ///
        /// Данный метод используется для задания состояния устройства перед
        /// его проверкой.
        ///
        /// @param new_state - новое состояние.
        void set_state( int new_state );

    protected:
        u_int last_check_time;///< Время последней проверки состояния.
        int   state;          ///< Предыдущее состояние, для исключения дребезга.
        u_int dt;             ///< Интервал установления состояния, мсек.
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе дискретного выхода.
///
/// Клапан, мешалка и т.д. являются примерами таких устройств.
class i_DO_device: public i_DI_device
    {
    public:
        /// @brief Включение устройства.
        ///
        /// Установка устройства в активное состояние. Для клапана это означает
        /// его активирование, то есть если он нормально закрытый - открытие.
        virtual void on() = 0;

        /// @brief Выключение устройства.
        ///
        /// Установка устройства в пассивное состояние. Для клапана это означает
        /// его деактивирование, то есть если он нормально закрытый - закрытие.
        virtual void off() = 0;

        /// @brief Установка нового состояния устройства.
        ///
        /// @param new_state - новое состояние объекта.
        virtual void set_state( int new_state ) = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на на основе аналогового входа.
///
/// Температура, расход и т.д. являются примерами таких устройств.
class i_AI_device
    {
    public:
        /// @brief Получение текущего состояния устройства.
        ///
        /// @return - текущее состояние устройства в виде дробного числа.
        virtual float get_value() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе аналогового выхода.
///
/// Аналоговый канал управления и т.д. являются примерами таких устройств.
class i_AO_device: public i_AI_device
    {
    public:
        /// @brief Выключение устройства.
        ///
        /// Установка устройства в пассивное состояние. Для клапана это означает
        /// его деактивирование, то есть если он нормально закрытый - закрытие.
        virtual void off() = 0;

        /// @brief Установка текущего состояния устройства.
        ///
        /// @param new_value - новое состояние устройства.
        virtual void set_value( float new_value ) = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Класс универсального простого устройства, который используется в 
/// режимах.
class device : public i_simple_device,    
    public i_AO_device,    
    public i_DO_device
    {
    public:
        //-Ошибки.
        saved_params_u_int_4 *err_par;

        enum DEV_ERROR_PARAMS  ///< Параметры ошибки, определяют номера битов.
            {
            DE_IS_ENABLE = 1,  ///< Блокировка тревоги на этапе проектирования.    
            DE_IS_INHIBIT = 2, ///< Блокировка тревоги во время работы.    
            DE_IS_SUPPRESS = 4,///< Подавление тревоги клиентами.
            };

        const char * get_name() const
            {
            return "";
            }
        //-Ошибки.!->

        enum CONSTANTS
            {
            C_DEVICE_TYPE_CNT = 14,     ///< Количество типов устройств.
            };

        static const char DEV_NAMES[][ 5 ];
        static const char DEV_TYPES[];

        /// Типы устройств.
        enum DEVICE_TYPE
            {
            DT_NONE = -1,      ///< Тип не определен.

            DT_V = 0,   ///< Клапан. 
            DT_N,       ///< Насос.
            DT_M,       ///< Мешалка.
            DT_LS,      ///< Уровень (есть/нет).
            DT_TE,      ///< Температура.
            DT_FE,      ///< Расход (значение).
            DT_FS,      ///< Расход (есть/нет).
            DT_CTR,     ///< Счетчик.
            DT_AO,      ///< Аналоговый выход.
            DT_LE,      ///< Уровень (значение).
            DT_FB,      ///< Обратная связь.
            DT_UPR,     ///< Канал управления.
            DT_QE,      ///< Концентрация.
            DT_AI,      ///< Аналоговый вход.
            };

        /// Подтипы устройств.
        enum DEVICE_SUB_TYPE
            {
            DST_NONE = -1,      ///< Подтип не определен.

            //DT_V = 0, 
            DST_V_DO_1 = 1,     ///< Клапан с одним каналом управления.
            DST_V_DO_2,         ///< Клапан с двумя каналами управления.
            DST_V_DO_1_DI_1,    ///< Клапан с одним каналом управления и одной обратной связью.
            DST_V_DO_1_DI_2,    ///< Клапан с одним каналом управления и двумя обратными связями.
            DST_V_DO_2_DI_2,    ///< Клапан с двумя каналами управления и двумя обратными связями.
            DST_V_MIXPROOF,     ///< Клапан микспруф.

            DST_V_1DO_3DI,      ///< Клапан с одним каналом управления и тремя обратными связями.
            DST_V_1DO_2DI_S,    ///< Клапан с одним каналом управления и двумя обратными связями на одно из состояний.
            DST_V_AS_MIXPROOF,       ///< Клапан с двумя каналами управления и двумя обратными связями с AS интерфейсом (микспруф).
            };

        device( int number, 
            device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type ): number( number ),
            type( type ),
            sub_type( sub_type )            
            { 
            }

        virtual ~device()
            {
            }

        /// @brief Выключение устройства.
        ///
        /// Установка устройства в пассивное состояние. Для клапана это означает
        /// его деактивирование, то есть если он нормально закрытый - закрытие.
        virtual void off() = 0;

        /// @brief Сохранение самого устройства в буфер.
        ///
        /// @param buff - буфер для сохранения устройства.
        ///
        /// @return -  количество записанных байт.
        int save_device( char *buff );

        /// @brief Вывод объекта в консоль.
        ///
        /// Для использования в отладочных целях.
        void print() const;

        /// @brief Получение номера устройства.
        ///
        /// @return -  номер устройства.
        u_int_4 get_n() const;

        /// @brief Получение типа устройства.
        int get_type() const
            {
            return type;
            }

        /// @brief Получение подтипа устройства.
        int get_sub_type() const
            {
            return sub_type;
            }

    protected:
        u_int_4 number;             ///< Номер устройства.

        DEVICE_TYPE     type;       ///< Тип устройства.
        DEVICE_SUB_TYPE sub_type;   ///< Подтип устройства.
    }; 
//-----------------------------------------------------------------------------
/// @brief Устройство, для хранения состояния которого необходим один байт.
///
/// Это клапаны, насосы, мешалки, обратные связи и т.д.
class char_state_device
    {
    public:
        /// @brief Сохранение измененного состояния устройства в буфер.
        ///
        /// @param buff [ out ] - адрес буфера, куда будут записываться данные.
        ///
        /// @return >= 0 - количество записанных байт.
        int save_changed_state( char *buff );

        /// @brief Сохранение состояния устройства в буфер.
        ///
        /// @param buff [ out ] - адрес буфера, куда будут записываться данные.
        ///        
        /// @return >= 0 - количество записанных байт.
        int save_state( char *buff  );

        /// @brief Получение состояния устройства.
        virtual int get_state() = 0;

    private:
        char prev_state;    ///< Предыдущее состояние устройства.
    };
//-----------------------------------------------------------------------------
/// @brief Устройство, для хранения состояния которого необходимо без знаковое
/// целое размером 4 байта.
///
/// Например счетчик.
class u_int_4_state_device
    {
    public:
        /// @brief Сохранение измененного состояния устройства в буфер.
        ///
        /// @param buff [ out ] - адрес буфера, куда будут записываться данные.
        ///
        /// @return >= 0 - количество записанных байт.
        virtual int save_changed_state( char *buff );

        /// @brief Сохранение состояния устройства в буфер.
        ///
        /// @param buff [ out ] - адрес буфера, куда будут записываться данные.
        ///        
        /// @return >= 0 - количество записанных байт.
        virtual int save_state( char *buff );

        /// @brief Получение состояния устройства.
        virtual u_int_4 get_u_int_4_state() = 0;

    private:
        u_int_4 prev_state;    ///< Предыдущее состояние устройства.
    };
//-----------------------------------------------------------------------------
/// @brief Устройство, для хранения состояния которого необходимо дробное число
/// размером 4 байта.
///
/// Например температура.
class float_state_device
    {
    public:
        /// @brief Сохранение измененного состояния устройства в буфер.
        ///
        /// @param buff [ out ] - адрес буфера, куда будут записываться данные.
        ///
        /// @return >= 0 - количество записанных байт.
        virtual int save_changed_state( char *buff );

        /// @brief Сохранение состояния устройства в буфер.
        ///
        /// @param buff [ out ] - адрес буфера, куда будут записываться данные.
        ///        
        /// @return >= 0 - количество записанных байт.
        virtual int save_state( char *buff );

        /// @brief Получение состояния устройства.
        virtual float get_value() = 0;

    private:
        float prev_state;    ///< Предыдущее состояние устройства.
    };
//-----------------------------------------------------------------------------
/// @brief Виртуальное устройство.
///
/// Необходимо для возвращения результата поиска устройства с несуществующим
/// номером. Методы данного класса ничего не делают. 
class dev_stub : public device,
    public i_counter
    {
    public:
        dev_stub(): device( 0, DT_NONE, DST_NONE )
            {
            }

        int     save_state( char *buff );        
        int     save_changed_state( char *buff );
        int     save_device( char *buff );

        u_int_4 get_n() const;                
        void    print() const;                

        float   get_value();
        void    set_value( float new_value );

        void    on();                
        void    off();                
        void    set_state( int new_state );
        int     get_state_now();

        int     parse_cmd( char *buff );

        void    pause();
        void    start();
        void    reset();        
        u_int   get_quantity();
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с дискретными входами/выходами.
///
/// Базовый класс для различных дискретных устройств.
class digital_device : public device,
    public char_state_device,
    public wago_device
    {
    public:
        digital_device( int number, device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type ): device( number, type, sub_type )
#ifdef DEBUG_NO_WAGO_MODULES
            , state( 0 )
#endif // DEBUG_NO_WAGO_MODULES
            {
            }

        virtual ~digital_device()
            {
            }

        float   get_value();
        void    set_value( float new_value );
        void    set_state( int new_state );  
        int     parse_cmd( char *buff );                        
        void    print() const;

        int save_changed_state( char *buff );
        int save_state( char *buff );                

#ifdef DEBUG_NO_WAGO_MODULES
        /// @brief Получение мгновенного состояния объекта.        
        ///        
        /// @return - мгновенное состояние объекта.
        int  get_state_now();

        void on();
        void off();
#endif // DEBUG_NO_WAGO_MODULES

        int get_state();

    protected:
        enum CONSTANTS
            {
            C_SWITCH_TIME = 3, ///< Время переключения устройства, сек.
            };

    private:
#ifdef DEBUG_NO_WAGO_MODULES
        char state;  ///< Состояние устройства.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с аналоговыми входами/выходами.
///
/// Базовый класс для различных аналоговых устройств.
class analog_device : public device,
    public float_state_device,
    public wago_device
    {
    public:
        analog_device( int number, 
            device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type ): device( number, type, sub_type )
#ifdef DEBUG_NO_WAGO_MODULES
            ,value( 0 )
#endif  // DEBUG_NO_WAGO_MODULES
            {
            }

        void  set_state( int new_state );
        int   get_state_now();
        int   parse_cmd( char *buff );
        
        void  print() const;
        void  on();        
        void  off();

#ifdef DEBUG_NO_WAGO_MODULES
        float get_value();
        void set_value( float new_value );

#else  // DEBUG_NO_WAGO_MODULES

        float get_value() = 0;

#endif // DEBUG_NO_WAGO_MODULES

        int save_changed_state( char *buff );
        int save_state( char *buff );

#ifdef DEBUG_NO_WAGO_MODULES
    private:
        float value;    ///< Состояние устройства.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным выходом.
///
/// Это может быть клапан, насос, канал управления...
class DO_1 : public digital_device
    {
    public:
        DO_1( int number, device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type ): digital_device( number, type, sub_type )
            {
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        int  get_state_now();
        void on();
        void off();

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< Индекс канала дискретного выхода.
            };
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с двумя дискретными выходами.
///
/// Это может быть клапан, насос...
class DO_2 : public digital_device
    {
    public:
        DO_2( int number, device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type ): digital_device( number, type, sub_type )
            {
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        int  get_state_now();
        void on();
        void off();        

    private:
        enum CONSTANTS
            {
            DO_INDEX_1 = 0, ///< Индекс канала дискретного выхода №1.
            DO_INDEX_2,     ///< Индекс канала дискретного выхода №2.
            };
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным выходом и одним дискретным входом.
///
/// Это может быть клапан, насос...
class DO_1_DI_1 : public digital_device
    {
    public:
        DO_1_DI_1( int number, device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type ): digital_device( number, type, sub_type )
            {
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        int  get_state_now();
        void on();
        void off();

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,           ///< Индекс канала дискретного выхода.
            DI_INDEX = 0,           ///< Индекс канала дискретного входа.

            PAR_FB_STATE = 0,       ///< Индекс параметра учета обратной связи.
            };

        u_long start_switch_time;  ///< Время начала переключения клапана.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным выходом и двумя дискретными входами.
///
/// Это может быть клапан, насос...
class DO_1_DI_2 : public digital_device
    {
    public:
        DO_1_DI_2( int number, device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type ): digital_device( number, type, sub_type )
            {
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        int  get_state_now();
        void on();
        void off();

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,           ///< Индекс канала дискретного выхода.

            DI_INDEX_1 = 0,         ///< Индекс №1 канала дискретного входа.
            DI_INDEX_2,             ///< Индекс №2 канала дискретного входа.
            };

        u_long start_switch_time;   ///< Время начала переключения клапана.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с двумя дискретными выходами и двумя дискретными входами.
///
/// Это может быть клапан, насос...
class DO_2_DI_2 : public digital_device
    {
    public:
        DO_2_DI_2( int number, device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type ): digital_device( number, type, sub_type )
            {
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        int  get_state_now();
        void on();
        void off();

    private:
        enum CONSTANTS
            {
            DO_INDEX_1 = 0,         ///< Индекс канала дискретного выхода №1.
            DO_INDEX_2,             ///< Индекс канала дискретного выхода №2.

            DI_INDEX_1 = 0,         ///< Индекс №1 канала дискретного входа.
            DI_INDEX_2,             ///< Индекс №2 канала дискретного входа.
            };

        u_long start_switch_time;   ///< Время начала переключения клапана.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Клапан mixproof.
class valve_mix_proof : public digital_device
    {
    public:
        valve_mix_proof( u_int number ): digital_device( number, DT_V, DST_V_MIXPROOF )
            {
            }

        enum STATES
            {
            ST_CLOSE = 0,   ///< Закрыт.
            ST_OPEN,        ///< Открыт.
            ST_UPPER_SEAT,  ///< Открыть верхнее седло.
            ST_LOW_SEAT,    ///< Открыть нижнее седло.
            };

        void open_upper_seat();
        void open_low_seat();

#ifndef DEBUG_NO_WAGO_MODULES
        int  get_state_now();
        void on();
        void off();
        void set_state( int new_state );

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< Индекс канала дискретного выхода.
            DO_INDEX_U,     ///< Индекс канала дискретного выхода верхнего седла.
            DO_INDEX_L,     ///< Индекс канала дискретного выхода нижнего седла.

            DI_INDEX_U = 0, ///< Индекс канала дискретного входа верхнего седла.
            DI_INDEX_L,     ///< Индекс канала дискретного входа нижнего седла.
            };

        u_long start_switch_time;   ///< Время начала переключения клапана.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Клапан AS-mixproof.
class valve_AS_mix_proof : public digital_device
    {
    public:
        valve_AS_mix_proof( u_int number );

        enum STATES
            {
            ST_CLOSE = 0,   ///< Закрыт.
            ST_OPEN,        ///< Открыт.
            ST_UPPER_SEAT,  ///< Открыть верхнее седло.
            ST_LOW_SEAT,    ///< Открыть нижнее седло.
            };

        void open_upper_seat();
        void open_low_seat();

#ifndef DEBUG_NO_WAGO_MODULES
        int  get_state_now();
        void on();
        void off();
        void set_state( int new_state );

    private:
        enum CONSTANTS
            {
            };

        u_long start_switch_time;   ///< Время начала переключения клапана.
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым входом.
///
/// Это может быть температура, расход (величина)...
class AI_1 : public analog_device
    {
    public:
        AI_1( u_int number, 
            device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type ): analog_device( number, type, sub_type )
            {
            }
#ifndef DEBUG_NO_WAGO_MODULES
    public:
        float get_value();
        void   set_value( float new_value );

        /// @brief Получение максимального значения выхода устройства.
        virtual float get_max_val() = 0;

        /// @brief Получение минимального значения выхода устройства.
        virtual float get_min_val() = 0;

    protected:
        enum CONSTANTS
            {
            AI_INDEX = 0,   ///< Индекс канала аналогового входа.
            };
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Температура.
class temperature_e : public AI_1
    {
    public:
        temperature_e( u_int number ): AI_1( number, DT_TE, DST_NONE )
            {
            }

        float get_max_val();
        float get_min_val();
    };
//-----------------------------------------------------------------------------
/// @brief Текущий уровень.
class level_e : public AI_1
    {
    public:
        level_e( u_int number ): AI_1( number, DT_LE, DST_NONE )
            {
            }

        float get_max_val();
        float get_min_val();
    };
//-----------------------------------------------------------------------------
/// @brief Текущий поток.
class flow_e : public AI_1
    {
    public:
        flow_e( u_int number ): AI_1( number, DT_FE, DST_NONE )
            {
            }

        float get_max_val();
        float get_min_val();

    private:
        enum CONSTANTS
            {
            C_MIN_PAR_NUMBER = 0,   ///< Индекс параметра минимального значения.
            C_MAX_PAR_NUMBER,       ///< Индекс параметра максимального значения.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Концентрация.
class concentration_e : public AI_1
    {
    public:
        concentration_e( u_int number ): AI_1( number, DT_QE, DST_NONE )
            {
            }

        float get_max_val();
        float get_min_val();

    private:
        enum CONSTANTS
            {
            C_MIN_PAR_NUMBER = 0,   ///< Индекс параметра минимального значения.
            C_MAX_PAR_NUMBER,       ///< Индекс параметра максимального значения.
            };                      
    };
//-----------------------------------------------------------------------------
/// @brief Устройство аналогового выхода 4-20 мА.
class analog_input_4_20 : public AI_1
    {
    public:
        analog_input_4_20( u_int number ): AI_1( number, DT_AI, DST_NONE )
            {
            }

        float get_max_val();
        float get_min_val();
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым выходом.
///
/// Это может быть управляемый клапан...
class AO_1 : public analog_device
    {
    public:
        AO_1( u_int number, 
            device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type ): analog_device( number, type, sub_type )
            {
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        /// @brief Получение максимального значения выхода устройства.
        virtual float get_max_val() = 0;

        /// @brief Получение минимального значения входа устройства.
        virtual float get_min_val() = 0;

        float get_value();
        void  set_value( float new_value );

    private:
        enum CONSTANTS
            {
            AO_INDEX = 0,   ///< Индекс канала аналогового выхода.
            };
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым входом.
///
/// Это может быть управляемый клапан...
class AO_0_100 : public AO_1
    {
    public:
        AO_0_100( u_int number ) : AO_1( number, DT_AO, DST_NONE )
            {
            }

        float get_max_val();
        float get_min_val();

    private:
        enum CONSTANTS
            {
            C_AO_MIN_VALUE = 0,     ///< Индекс параметра минимального значения.
            C_AO_MAX_VALUE = 100,   ///< Индекс параметра максимального значения.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным входом.
///
/// Это может быть обратная связь, расход (есть/нет)...
class DI_1 : public digital_device
    {
    public:
        DI_1( u_int number, 
            device::DEVICE_TYPE type, 
            device::DEVICE_SUB_TYPE sub_type, u_int dt = 0 ):
        digital_device( number, type, sub_type )
            {
            set_change_time( dt );
            }

#ifndef DEBUG_NO_WAGO_MODULES
    public:
        int  get_state_now();
        void on();
        void off();

    private:
        enum CONSTANTS
            {
            DI_INDEX = 0,   ///< Индекс канала дискретного входа.
            };
#endif // DEBUG_NO_WAGO_MODULES
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним каналом управления.
class valve_DO_1 : public DO_1
    {
    public:
        valve_DO_1( u_int number ) : DO_1( number, DT_V, DST_V_DO_1 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с двумя каналами управления.
class valve_DO_2 : public DO_2
    {
    public:
        valve_DO_2( u_int number ): DO_2( number, DT_V, DST_V_DO_2 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним каналом управления и одной обратной связью.
class valve_DO_1_DI_1 : public DO_1_DI_1
    {
    public:
        valve_DO_1_DI_1( u_int number ): DO_1_DI_1( number, DT_V, DST_V_DO_1_DI_1 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним каналом управления и двумя обратными связями.
class valve_DO_1_DI_2 : public DO_1_DI_2
    {
    public:
        valve_DO_1_DI_2( u_int number ): DO_1_DI_2( number, DT_V, DST_V_DO_1_DI_2 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с двумя каналами управления и двумя обратными связями.
class valve_DO_2_DI_2 : public DO_2_DI_2
    {
    public:
        valve_DO_2_DI_2( u_int number ): DO_2_DI_2( number, DT_V, DST_V_DO_2_DI_2 )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Насос.
class pump : public DO_1_DI_1
    {
    public:
        pump( u_int number ): DO_1_DI_1( number, DT_N, DST_NONE )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Мешалка.
class mixer : public DO_1_DI_1
    {
    public:
        mixer( u_int number ): DO_1_DI_1( number, DT_M, DST_NONE )
            {
            }
    };
//-----------------------------------------------------------------------------
/// @brief Датчик сигнализатора уровня.
class level_s : public DI_1
    {
    public:
        level_s( u_int number, u_int dt = 1000 ): DI_1( number, DT_LS, DST_NONE )
            {
            set_change_time( dt );
            }
    };
//-----------------------------------------------------------------------------
/// @brief Датчик сигнализатора расхода.
class flow_s : public DI_1
    {
    public:
        flow_s( u_int number ): DI_1( number, DT_FS, DST_NONE )
            {            
            }
    };
//-----------------------------------------------------------------------------
/// @brief Датчик обратной связи.
class feedback : public DI_1
    {
    public:
        feedback( u_int number ): DI_1( number, DT_FB, DST_NONE )
            {            
            }
    };
//-----------------------------------------------------------------------------
/// @brief Сигнал управления
class control_s : public DO_1
    {
    public:
        control_s( u_int number ): DO_1( number, DT_UPR, DST_NONE )
            {            
            }
    };
//-----------------------------------------------------------------------------
/// @brief Счетчик.
class counter : public device,
    public i_counter,
    public u_int_4_state_device,
    public wago_device
    {
    public:
        counter( u_int number ): device( number, DT_CTR, DST_NONE ), 
            value( 0 ),
            last_read_value( 0 )
            {            
            }

        virtual ~counter()
            {            
            }

        float get_value();
        void  set_value( float new_value );
        int   get_state_now();
        void  on();
        void  off();
        void  set_state( int new_state );
        int   parse_cmd( char *buff  );
        void  print() const;

        u_int_4 get_u_int_4_state();

        int save_changed_state( char *buff );
        int save_state( char *buff );

        void  pause();
        void  start();
        void  reset();
        u_int get_quantity();

    private:
        enum STATES
            {
            S_STOP,
            S_WORK,
            S_PAUSE,
            };

        enum CONSTANTS
            {
            AI_INDEX = 0,       ///< Индекс канала аналогового входа.

            MAX_VAL = 65535,    ///< Максимальное значение счетчика.
            };

        u_int value;
        u_int last_read_value;

        STATES state;
    };
//-----------------------------------------------------------------------------
/// @brief Менеджер устройств.
///
/// Содержит информацию обо всех устройствах проекта.
class device_manager
    {
    public:
        device_manager();

        ~device_manager();

        /// @brief Получение устройства по его номеру.        
        device* get_device( device::DEVICE_TYPE dev_type, u_int dev_number );

        /// @brief Получение клапана по его номеру.
        i_DO_device* get_V( int number );

        /// @brief Получение насоса по номеру.
        i_DO_device* get_N( int number );

        /// @brief Получение мешалки по номеру.
        i_DO_device* get_M( int number );

        /// @brief Получение уровня по номеру.
        i_DI_device* get_LS( int number );

        /// @brief Получение расхода по номеру.
        i_DI_device* get_FS( int number );

        /// @brief Получение аналогового входа по номеру.
        i_AI_device* get_AI( int number );

        /// @brief Получение аналогового выхода по номеру.
        i_AO_device* get_AO( int number );

        /// @brief Получение счетчика по номеру.
        i_counter* get_CTR( int number );

        /// @brief Получение температуры по номеру.
        i_AI_device* get_TE( int number );

        /// @brief Получение текущего расхода по номеру.
        i_AI_device* get_FE( int number );

        /// @brief Получение текущего уровня по номеру.
        i_AI_device* get_LE( int number );

        /// @brief Получение обратной связи по номеру.
        i_DI_device* get_FB( int number );

        /// @brief Получение управляющего канала по номеру.
        i_DO_device* get_UPR( int number );

        /// @brief Получение текущей концентрации по номеру.
        i_AI_device* get_QE( int number );

        /// @brief Получение единственного экземпляра класса.
        static device_manager* get_instance();

        /// @brief Отладочная печать объекта в консоль.
        void print() const;

        /// @brief Установка единственного экземпляра класса.
        static void set_instance( device_manager* new_instance );

        /// @brief Получение всех простых устройств, для передачи на сервер.
        complex_device * get_communication_device()
            {
            return devices;
            }

        /// @brief Получение фиктивного устройства.
        dev_stub* get_stub()
            {
            return &stub;
            }

    protected:
        /// Все простые устройства, для передачи на сервер.
        complex_device *devices;

        dev_stub stub;  ///< Устройство-заглушка, фиктивное устройство. 

        struct range    ///< Диапазон устройств одного типа. 
            {
            int start_pos;
            int end_pos;
            };

        /// Диапазоны устройств всех типов.
        range dev_types_ranges[ device::C_DEVICE_TYPE_CNT ];
        
        /// @brief Получение индекса устройства по его номеру.        
        int get_device_n( device::DEVICE_TYPE dev_type, u_int dev_number );

        std::vector< device* > project_devices; ///< Все устройства.

        /// @brief Единственный экземпляр класса.
        static auto_smart_ptr < device_manager > instance;

    public:
        // @brief Установка числа устройств.
        //
        // Вызывается из Lua.
        wago_device* add_device( int dev_type, int dev_sub_type, 
            u_int number, char * descr );

        void complete_init()
            {
            devices = new complex_device( 0, "GLB", device::C_DEVICE_TYPE_CNT, 0 );

            for ( int i = 0; i < device::C_DEVICE_TYPE_CNT; i++ )
                {
                int dev_cnt = dev_types_ranges[ i ].end_pos -
                    dev_types_ranges[ i ].start_pos + 1;

                if ( dev_types_ranges[ i ].start_pos == -1 )
                    {
                    dev_cnt = 0;
                    }

                devices->sub_dev[ i ] =
                    new complex_device( 0, device::DEV_NAMES[ i ], dev_cnt,
                    device::DEV_TYPES[ i ] );

                if ( dev_cnt )
                    {
                    int pos = 0;
                    for ( int j = dev_types_ranges[ i ].start_pos;
                        j <= dev_types_ranges[ i ].end_pos; j++ )
                        {
                        ( ( complex_device* ) ( devices->sub_dev[ i ] ) )->sub_dev[ pos++ ] =
                            project_devices[ j ];
                        }
                    }
                }
            }
    };
//-----------------------------------------------------------------------------
/// @brief таймер.
///
/// Предоставляет функциональность таймера.
class timer
    {
    public:
        enum STATE
            {
            S_STOP = 0, ///< Не работает.
            S_WORK,     ///< Работает.
            S_PAUSE,    ///< Пауза.
            };

        /// @brief Сохранение состояния устройства в буфер.
        ///
        /// @param buff [ out ] - адрес буфера, куда будут записываться данные.
        ///        
        /// @return >= 0 - количество записанных байт.
        int save( char *buff );

        /// @brief Считывание состояния устройства из буфера.
        ///
        /// @param buff [ out ] - адрес буфера, откуда будут считываться данные.
        ///        
        /// @return >= 0 - количество считанных байт.
        int load( char *buff );

        /// @brief Получение размера буфера для сохранения состояния устройства.
        ///
        /// @return >= 0 - размер буфера, байт.
        int get_saved_size() const;

        timer();

        /// @brief Запуск таймера.
        void start();

        /// @brief Сброс таймера.
        void reset();

        /// @brief Пауза таймера.
        void pause();

        /// @brief Проверка исхода времени таймера.
        ///
        /// @return 1 - время вышло.
        /// @return 0 - время не вышло.
        bool is_time_up() const;

        /// @brief Получение времени работы таймера.
        ///
        /// @return - время работы таймера.
        u_long  get_work_time() const;

        /// @brief Установка задания таймера.
        ///
        /// @param new_countdown_time - задание.
        void set_countdown_time( u_long new_countdown_time );

        /// @brief Получение задания таймера.
        ///
        /// @return - задание таймера.
        u_long  get_countdown_time() const;

        /// @brief Получение состояния таймера.
        ///
        /// @return - состояние таймера.
        STATE get_state() const;

    private:
        u_long  last_time;  ///< Время, когда таймер был запущен/остановлен.
        u_long  work_time;  ///< Время работы таймера.

        STATE   state;           ///< Состояние.
        u_long  countdown_time;  ///< Задание таймера.
    };
//-----------------------------------------------------------------------------
/// @brief таймер.
///
/// Реализация группы таймеров.
class timer_manager
    {
    public:
        /// @param timers_count - количество таймеров в группе.
        timer_manager( u_int timers_count );

        ~timer_manager();

        /// @brief Безопасное получение таймера по индексу.
        ///
        /// @param index - индекс таймера.
        ///
        /// @return - таймер с нужным индексом, заглушка - в случае выхода за 
        /// диапазон.
        timer& operator[] ( unsigned int index );

    private:
        u_int   timers_cnt; ///< Количество таймеров.
        timer   *timers;    ///< Таймеры.

        timer   stub;       ///< Заглушка.
    };
//-----------------------------------------------------------------------------
///@brief Получение менеджера устройств.
///
///@return Менеджер устройств проекта.
device_manager* G_DEVICE_MANAGER();
//-----------------------------------------------------------------------------
/// @brief Получение клапана по номеру.
///
/// @param number - номер клапана.
/// @return - клапан с заданным номером. Если нет такого клапана, возвращается
/// заглушка (@ref dev_stub).
i_DO_device* V( int number );
//-----------------------------------------------------------------------------
/// @brief Получение насоса по номеру.
///
/// @param number - номер насоса.
/// @return - насос с заданным номером. Если нет такого насоса, возвращается
/// заглушка (@ref dev_stub).
i_DO_device* N( int number );
//-----------------------------------------------------------------------------
/// @brief Получение мешалки по номеру.
///
/// @param number - номер мешалки.
/// @return - мешалка с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_device* M( int number );
//-----------------------------------------------------------------------------
/// @brief Получение сигнального уровня по номеру.
///
/// @param number - номер сигнального уровня.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* LS( int number );
//-----------------------------------------------------------------------------
/// @brief Получение сигнального расхода по номеру.
///
/// @param number - номер сигнального расхода.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* FS( int number );
//-----------------------------------------------------------------------------
/// @brief Получение аналогового входа по номеру.
///
/// @param number - номер аналогового входа.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* AI( int number );
//-----------------------------------------------------------------------------
/// @brief Получение аналогового выхода по номеру.
///
/// @param number - номер аналогового выхода.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AO_device* AO( int number );
//-----------------------------------------------------------------------------
/// @brief Получение счетчика по номеру.
///
/// @param number - номер счетчика.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_counter* CTR( int number );
//-----------------------------------------------------------------------------
/// @brief Получение температуры по номеру.
///
/// @param number - номер температуры.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* TE( int number );
//-----------------------------------------------------------------------------
/// @brief Получение текущего расхода по номеру.
///
/// @param number - номер текущего расхода.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* FE( int number );
//-----------------------------------------------------------------------------
/// @brief Получение текущего уровня по номеру.
///
/// @param number - номер текущего уровня.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* LE( int number );
//-----------------------------------------------------------------------------
/// @brief Получение обратной связи по номеру.
///
/// @param number - номер обратной связи.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DI_device* FB( int number );
//-----------------------------------------------------------------------------
/// @brief Получение канала управления по номеру.
///
/// @param number - номер канала управления.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_DO_device* UPR( int number );
//-----------------------------------------------------------------------------
/// @brief Получение текущей концентрации по номеру.
///
/// @param number - номер текущей концентрации.
/// @return - устройство с заданным номером. Если нет такого устройства,
/// возвращается заглушка (@ref dev_stub).
i_AI_device* QE( int number );
//-----------------------------------------------------------------------------
/// @brief Получение насоса по номеру.
///
/// @return - виртуальное устройство.
dev_stub* STUB();
//-----------------------------------------------------------------------------
#endif // PAC_DEVICES_H
