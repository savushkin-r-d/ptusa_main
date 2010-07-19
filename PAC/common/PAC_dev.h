/// @file PAC_dev.h
/// @brief Классы, которые реализуют функции передачи 
/// состояния устройств PAC на сервер. Также реализованы классы для работы с 
/// устройствами ( клапана, насосы,... ). Классы используются ТОЛЬКО в 
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

#include <string.h>
#include <stdio.h>

#include <string>

#include "sys.h"
#include "wago.h"

#include "g_device.h"

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
            T_POST,
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

        u_int_4 get_val( int idx );
        int     parse_cmd( char *buff  );
        //void    print() const;
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
/// @brief Интерфейс счетчика.
class i_counter
    {
    public:
        /// @brief Приостановка работы счетчика.
        virtual void pause() = 0;

        /// @brief Возобновление работы счетчика.
        virtual void start() = 0;

        /// @brief Сброс счетчика.
        /// 
        /// После сброса для продолжения работы необходимо вызвать @ref start().
        virtual void reset() = 0;

        /// @brief Получение значения счетчика.
        virtual u_int get_quantity() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с дискретным входом.
///
/// Устройства типа обратной связи, уровня и т.д. реализуют данный интерфейс.
class i_DI_device
    {
    public:
        /// @brief Получение текущего состояния устройства.
        ///
        /// @return - текущее состояние устройства в виде целого числа.
        virtual int get_state() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с дискретным выходом.
///
/// Устройства типа клапана, мешалки и т.д. реализуют данный интерфейс.
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
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        virtual int set_state( int new_state ) = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с аналоговым входом.
///
/// Устройства типа температуры, расхода и т.д. реализуют данный интерфейс.
class i_AI_device
    {
    public:
        /// @brief Получение текущего состояния устройства.
        ///
        /// @return - текущее состояние устройства в виде дробного числа.
        virtual float get_value() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с аналоговым выходом.
///
/// Устройства типа аналогового каналы управления и т.д. реализуют данный
/// интерфейс.
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
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        virtual int set_value( float new_value ) = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Класс универсального простого устройства, который используется в 
/// режимах.
class device : public i_simple_device,    
    public i_AO_device,    
    public i_DO_device
    {
    public:
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
            };

        device();

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

        /// @brief Загрузка самого устройства из буфера.
        ///
        /// @param cfg_file - дескриптор открытого текстового файла с описанием
        ///   устройства.
        ///
        /// @return - 0 - ок.
        virtual int load( file *cfg_file );

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
        int     save_state( char *buff );        
        int     save_changed_state( char *buff );
        int     save_device( char *buff );

        u_int_4 get_n() const;                
        void    print() const;                

        float   get_value();
        int     set_value( float new_value );                
                        
        void    on();                
        void    off();                
        int     set_state( int new_state );
        int     get_state();
                
        int     parse_cmd( char *buff );
               
        int     load( file *cfg_file ); 

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
        float   get_value();
        int     set_value( float new_value );
        int     set_state( int new_state );  
        int     parse_cmd( char *buff );                     
        int     load( file *cfg_file );      
        void    print() const;

        int save_changed_state( char *buff );
        int save_state( char *buff );        
        int get_state() = 0;

    protected:
        enum CONSTANTS
            {
            C_SWITCH_TIME = 5, ///< Время переключения клапана, сек.
            };
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
        float   get_value() = 0;
        int     set_state( int new_state );
        int     get_state();
        int     parse_cmd( char *buff );
        int     load( file *cfg_file );
        void    print() const;
        void    on();        
        void    off();

        int save_changed_state( char *buff );
        int save_state( char *buff );
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным выходом.
///
/// Это может быть клапан, насос, канал управления...
class DO_1 : public digital_device
    {
    public:
        int  get_state();
        void on();
        void off();

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,   ///< Индекс канала дискретного выхода.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с двумя дискретными выходами.
///
/// Это может быть клапан, насос...
class DO_2 : public digital_device
    {
    public:
        int  get_state();
        void on();
        void off();        

    private:
        enum CONSTANTS
            {
            DO_INDEX_1 = 0, ///< Индекс канала дискретного выхода №1.
            DO_INDEX_2,     ///< Индекс канала дискретного выхода №2.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным выходом и одним дискретным входом.
///
/// Это может быть клапан, насос...
class DO_1_DI_1 : public digital_device
    {
    public:
        int  get_state();
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
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным выходом и двумя дискретными входами.
///
/// Это может быть клапан, насос...
class DO_1_DI_2 : public digital_device
    {
    public:
        int  get_state();
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
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с двумя дискретными выходами и двумя дискретными входами.
///
/// Это может быть клапан, насос...
class DO_2_DI_2 : public digital_device
    {
    public:
        int  get_state();
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
    };
//-----------------------------------------------------------------------------
/// @brief Клапан mixproof.
class valve_mix_proof : public digital_device
    {
    public:
        int  get_state();
        void on();
        void open_upper_seat();
        void open_low_seat();
        void off();

        enum STATES
            {
            ST_CLOSE = 0,   ///< Закрыт.
            ST_OPEN,        ///< Открыт.
            ST_UPPER_SEAT,  ///< Открыть верхнее седло.
            ST_LOW_SEAT,    ///< Открыть нижнее седло.
            };

        int set_state( int new_state );

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
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым входом.
///
/// Это может быть температура, расход (величина)...
class AI_1 : public analog_device
    {
    public:
        float get_value();
        int   set_value( float new_value );

        /// @brief Получение максимального значения выхода устройства.
        virtual float get_max_val() = 0;

        /// @brief Получение минимального значения выхода устройства.
        virtual float get_min_val() = 0;

    protected:
        enum CONSTANTS
            {
            AI_INDEX = 0,   ///< Индекс канала аналогового входа.
            };
    };
//---------------------------------l--------------------------------------------
/// @brief Температура.
class temperature_e : public AI_1
    {
    public:
        float get_max_val();
        float get_min_val();

        float get_value();
        int   set_value( float new_value );
    };
//-----------------------------------------------------------------------------
/// @brief Текущий уровень.
class level_e : public AI_1
    {
    public:
        float get_max_val();
        float get_min_val();
    };
//-----------------------------------------------------------------------------
/// @brief Текущий поток.
class flow_e : public AI_1
    {
    public:
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
        /// @brief Получение максимального значения выхода устройства.
        virtual float get_max_val() = 0;

        /// @brief Получение минимального значения входа устройства.
        virtual float get_min_val() = 0;

        float get_value();
        int   set_value( float new_value );

    private:
        enum CONSTANTS
            {
            AO_INDEX = 0,   ///< Индекс канала аналогового выхода.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым входом.
///
/// Это может быть управляемый клапан...
class AO_0_100 : public AO_1
    {
    public:
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
        int  get_state();
        void on();
        void off();

    private:
        enum CONSTANTS
            {
            DI_INDEX = 0,   ///< Индекс канала дискретного входа.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним каналом управления.
class valve_DO_1 : public DO_1
    {
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с двумя каналами управления.
class valve_DO_2 : public DO_2
    {
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним каналом управления и одной обратной связью.
class valve_DO_1_DI_1 : public DO_1_DI_1
    {
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с одним каналом управления и двумя обратными связями.
class valve_DO_1_DI_2 : public DO_1_DI_2
    {
    };
//-----------------------------------------------------------------------------
/// @brief Клапан с двумя каналами управления и двумя обратными связями.
class valve_DO_2_DI_2 : public DO_2_DI_2
    {
    };
//-----------------------------------------------------------------------------
/// @brief Насос.
class pump : public DO_1_DI_1
    {
    };
//-----------------------------------------------------------------------------
/// @brief Мешалка.
class mixer : public DO_1_DI_1
    {
    };
//-----------------------------------------------------------------------------
/// @brief Датчик сигнализатора уровня.
class level_s : public DI_1
    {
    };
//-----------------------------------------------------------------------------
/// @brief Датчик сигнализатора расхода.
class flow_s : public DI_1
    {
    };
//-----------------------------------------------------------------------------
/// @brief Датчик обратной связи.
class feedback : public DI_1
    {
    };
//-----------------------------------------------------------------------------
/// @brief Сигнал управления
class control_s : public DO_1
    {
    };
//-----------------------------------------------------------------------------
/// @brief Счетчик.
class counter : public device,
    public i_counter,
    public u_int_4_state_device,
    public wago_device
    {
    public:        
        float get_value();
        int   set_value( float new_value );
        int   get_state();
        void  on();
        void  off();
        int   set_state( int new_state );
        int   parse_cmd( char *buff  );
        int   load( file *cfg_file );
        void  print() const;

        u_int_4 get_u_int_4_state();

        int save_changed_state( char *buff );
        int save_state( char *buff );

        void  pause();
        void  start();
        void  reset();
        u_int get_quantity();

    private:
        enum CONSTANTS
            {
            AI_INDEX = 0, ///< Индекс канала аналогового входа.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Менеджер устройств.
///
/// Содержит информацию обо всех устройствах проекта.
class device_manager
    {
    public:
        device_manager();

        /// @brief Загрузка из файла конфигурации.
        int load_from_cfg_file( file *cfg_file );

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

    public:
        dev_stub stub;  ///< Устройство-заглушка, фиктивное устройство. 

        struct range    ///< Диапазон устройств одного типа. 
            {
            int start_pos;
            int end_pos;
            };

        /// Диапазоны устройств всех типов.
        range dev_types_ranges[ device::C_DEVICE_TYPE_CNT ];

        /// @brief Получение устройства по его номеру.        
        device* get_device( device::DEVICE_TYPE dev_type, u_int dev_number,
            char const * dev_name );

        /// @brief Получение индекса устройства по его номеру.        
        int get_device_n( device::DEVICE_TYPE dev_type, u_int dev_number );

        int    devices_count;               ///< Количество устройств.
        device **project_devices;           ///< Все устройства.

        static device_manager* instance;    ///< Единственный экземпляр класса.
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

        int save( char *buff );

        int load( char *buff );

        int get_saved_size() const;

        timer();

        void    start();

        void    reset();

        void    pause();

        bool    is_time_up() const;

        u_long  get_work_time() const;

        void    set_countdown_time( u_long new_countdown_time );

        u_long  get_countdown_time() const;

        STATE   get_state() const;

    private:
        u_long  last_time;  ///< Время, когда таймер был запущен/остановлен.
        u_long  work_time;  ///< Время работы таймера.

        STATE   state;           ///< Состояние.
        u_long  countdown_time;  ///< Задание таймера.
    };
//-----------------------------------------------------------------------------
/// @brief таймер.
///
/// Предоставляет группу таймеров.
class timer_manager
    {
    public:
        timer_manager( u_int timers_count ): timers_cnt( timers_count ),
            timers( 0 )
        {
        if ( timers_cnt )
            {
            timers = new timer[ timers_cnt ];
            }   
        }

        ~timer_manager()
            {
            if ( timers )
                {
                delete [] timers;
                timers = 0;
                }
            }

    timer& operator[] ( unsigned int index )
        {
        if ( index < timers_cnt )
                {
                return timers[ index ];
                }
#ifdef DEBUG
            else
                {
                Print( "timer_manager[] - error: index[ %u ] > count [ %u ]\n",
                    index, timers_cnt );
                }
#endif // DEBUG

            return stub;
        }

    private:
        u_int   timers_cnt;
        timer   *timers;

        timer   stub;
    };
//-----------------------------------------------------------------------------
#define G_DEVICE_MANAGER device_manager::get_instance()
//-----------------------------------------------------------------------------
#define V   G_DEVICE_MANAGER->get_V   ///< Получение клапана по номеру.
#define N   G_DEVICE_MANAGER->get_N   ///< Получение насоса по номеру.
#define M   G_DEVICE_MANAGER->get_M   ///< Получение мешалки по номеру.
#define LS  G_DEVICE_MANAGER->get_LS  ///< Получение сигнального уровня по номеру.
#define FS  G_DEVICE_MANAGER->get_FS  ///< Получение сигнального расхода по номеру.
#define AI  G_DEVICE_MANAGER->get_AI  ///< Получение аналогового входа по номеру.
#define AO  G_DEVICE_MANAGER->get_AO  ///< Получение аналогового выхода по номеру.
#define CTR G_DEVICE_MANAGER->get_CTR ///< Получение счетчика по номеру.
#define TE  G_DEVICE_MANAGER->get_TE  ///< Получение температуры по номеру.
#define FE  G_DEVICE_MANAGER->get_FE  ///< Получение текущего расхода по номеру.
#define LE  G_DEVICE_MANAGER->get_LE  ///< Получение текущего уровня по номеру.
#define FB  G_DEVICE_MANAGER->get_FB  ///< Получение обратной связи по номеру.
#define UPR G_DEVICE_MANAGER->get_UPR ///< Получение канала управления по номеру.
#define QE  G_DEVICE_MANAGER->get_QE  ///< Получение текущей концентрации по номеру.
//-----------------------------------------------------------------------------
#endif // PAC_DEVICES_H
