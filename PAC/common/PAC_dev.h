/// @file PAC_dev.h
/// @brief Содержит описания классов, которые реализуют функции передачи 
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

        /// Типы устройств.
        enum DEVICE_TYPE
            {
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
            DST_NONE = 0,       ///< Подтип неопределен.

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
        int save_device( char *buff  );

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

        int get_type() const
            {
            return type;
            }

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
        /// @brief Реализация интерфейса @ref i_save_device.
        int save_changed_state( char *buff );

        /// @brief Реализация интерфейса @ref i_save_device.
        int save_state( char *buff  );

        /// @brief .
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
        /// @brief Реализация интерфейса @ref i_save_device.
        virtual int save_changed_state( char *buff );

        /// @brief Реализация интерфейса @ref i_save_device.
        virtual int save_state( char *buff );

        /// @brief Для сохранения состояния устройства.                
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
        /// @brief Реализация интерфейса @ref i_save_device.
        virtual int save_changed_state( char *buff );

        /// @brief Реализация интерфейса @ref i_save_device.
        virtual int save_state( char *buff );

        /// @brief .
        virtual float get_value() = 0;

    private:
        float prev_state;    ///< Предыдущее состояние устройства.
    };
//-----------------------------------------------------------------------------
/// @brief Устройство на основе модулей ввода/вывода WAGO. 
/// 
/// В общем случае у устройства может быть один или несколько каналов
/// ввода/вывода (дискретных или аналоговых).
class wago_device
    {
    public:
        static char debug_mode;

        wago_device();

        /// @brief Загрузка самого устройства из буфера.
        ///
        /// @param cfg_file - дескриптор открытого текстового файла с описанием
        ///   устройства.
        ///
        /// @return -  количество считанных байт.
        virtual int load( file *cfg_file );

    protected:
        enum CONSTANTS
            {
            C_MAX_ANALOG_CHANNEL_VALUE = 65535,
            };

        /// @brief Получение состояния канала дискретного выхода.
        ///
        /// @param index - индекс канала в таблице дискретных выходных каналов 
        /// устройства.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        int get_DO( u_int index );

        /// @brief Установка состояния канала дискретного выхода.
        ///
        /// @param index - индекс канала в таблице дискретных выходных каналов 
        /// устройства.
        /// @param value - новое состояние канала.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        int set_DO( u_int index, char value );

        /// @brief Получение состояния канала дискретного входа.
        ///
        /// @param index - индекс канала в таблице дискретных входных каналов 
        /// устройства.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        int get_DI( u_int index );

        /// @brief Установка состояния канала дискретного входа.
        ///
        /// @param index - индекс канала в таблице дискретных входных каналов 
        /// устройства.
        /// @param value - новое состояние канала.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        int set_DI( u_int index, char value );

        /// @brief Получение состояния канала аналогового выхода.
        ///
        /// @param index - индекс канала в таблице аналоговых выходных каналов 
        /// устройства.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        u_int get_AO( u_int index );

        /// @brief Установка состояния канала аналогового выхода.
        ///
        /// @param index - индекс канала в таблице аналоговых выходных каналов 
        /// устройства.
        /// @param value - новое состояние канала.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        int set_AO( u_int index, u_int value );

        /// @brief Получение состояния канала аналогового входа.
        ///
        /// @param index - индекс канала в таблице аналоговых входных каналов 
        /// устройства.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        u_int get_AI( u_int index );

        /// @brief Установка состояния канала аналогового входа.
        ///
        /// @param index - индекс канала в таблице аналоговых входных каналов 
        /// устройства.
        /// @param value - новое состояние канала.
        ///
        /// @return -  0 - Ок.
        /// @return - >0 - ошибка.
        int   set_AI( u_int index, u_int value );

        float get_par( u_int index );

        virtual void print() const;

    private:
        /// @brief Группа каналов ввода/вывода устройства.
        struct IO_channels
            {
            enum CHANNEL_TYPE
                {
                CT_DI = 1,
                CT_DO,
                CT_AI,
                CT_AO,
                };

            u_int  count;   ///< Количество каналов.
            u_int* tables;  ///< Массив таблиц.
            u_int* offsets; ///< Массив смещений в пределах таблиц.

            u_int  **int_read_values;    ///< Массив значений для чтения.
            u_int  **int_write_values;   ///< Массив значений для записи.
            u_char **char_read_values;   ///< Массив значений для чтения.
            u_char **char_write_values;  ///< Массив значений для записи.

            IO_channels( CHANNEL_TYPE type ) : count( 0 ), tables( 0 ),
                offsets( 0 ), 
                int_read_values( 0 ), int_write_values( 0 ),
                char_read_values( 0 ), char_write_values( 0 ),
                type( type )
                {
                }

            CHANNEL_TYPE type;
            };

        IO_channels DI_channels;    ///< Каналы дискретного входа.
        IO_channels DO_channels;    ///< Каналы дискретного выхода.
        IO_channels AI_channels;    ///< Каналы аналогового входа.
        IO_channels AO_channels;    ///< Каналы аналогового выхода.

        u_int   params_count;
        float   *params;

        void print_table( const char *str, const IO_channels &channels ) const;

        /// @brief Загрузка информации о группе каналов ввода/вывода из 
        /// байтового потока.
        /// 
        /// @param stream - буфер для считывания устройства.
        /// @param [out] channels - группа, в которая считывается.
        ///
        /// @return -  количество считанных байт.
        int load_table_from_string( char *str, IO_channels &channels );
    };
//-----------------------------------------------------------------------------
/// @brief Виртуальное устройство.
///
/// Необходимо для возвращения результата поиска устройства с несуществующим
/// номером. Методы данного класса ничего не делают. 
class dev_stub : public device
    {
    public:
        int save_state( char *buff )
            {
            return 0;
            }
        int save_changed_state( char *buff )
            {
            return 0;
            }
        int save_device( char *buff )
            {
            return 0;
            }
        u_int_4 get_n() const
            {
            return 0;
            }
        void print() const
            {
#ifdef DEBUG
            Print( "virtual device" );
#endif // DEBUG
            }

        float get_value();

        int set_value( float new_value );

        int get_state();

        void on();

        void off();

        int set_state( int new_state );

        int parse_cmd( char *buff );

        int load( file *cfg_file );
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с дискретными входами/выходами.
///
/// Базовый клас для различных дискретных устройств.
class digital_device : public device,
    public char_state_device,
    public wago_device
    {
    public:
        float get_value();

        int set_value( float new_value );

        int set_state( int new_state );

        int parse_cmd( char *buff );

        int load( file *cfg_file );

        void print() const;

        /// @brief Реализация интерфейса @ref i_save_device.
        int save_changed_state( char *buff )
            {
            return char_state_device::save_changed_state( buff );
            }

        /// @brief Реализация интерфейса @ref i_save_device.
        int save_state( char *buff )
            {
            return char_state_device::save_state( buff );
            }

        /// @brief .
        int get_state() = 0;

    protected:
        enum CONSTANTS
            {
            C_SWITCH_TIME = 5000,
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с аналоговыми входами/выходами.
///
/// Базовый клас для различных аналоговых устройств.
class analog_device : public device,
    public float_state_device,
    public wago_device
    {
    public:
        float get_value() = 0;

        int set_state( int new_state )
            {
            return set_value( new_state );
            }

        int get_state()
            {
            return ( int ) get_value();
            }

        int parse_cmd( char *buff )
            {
            set_value( *( ( float* ) buff ) );
            return sizeof( float );
            }

        int load( file *cfg_file )
            {
            device::load( cfg_file );
            wago_device::load( cfg_file );

            return 0;
            }

        void print() const
            {
            device::print();
            wago_device::print();
            }

        void on()
            {
            }

        void off()
            {
            set_value( 0 );
            }

        /// @brief Реализация интерфейса @ref i_save_device.
        int save_changed_state( char *buff )
            {
            return float_state_device::save_changed_state( buff );
            }

        /// @brief Реализация интерфейса @ref i_save_device.
        int save_state( char *buff )
            {
            return float_state_device::save_state( buff );
            }
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным выходом.
///
/// Это может быть клапан, насос, канал управления...
class DO_1 : public digital_device
    {
    public:
        int get_state();

        void on();

        void off();

        int set_state( int new_state );

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с двумя дискретными выходами.
///
/// Это может быть клапан, насос...
class DO_2 : public digital_device
    {
    public:
        int get_state();

        void on();

        void off();

        int set_state( int new_state );

    private:
        enum CONSTANTS
            {
            DO_INDEX_1 = 0,
            DO_INDEX_2,
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным выходом и одним дискретным входом.
///
/// Это может быть клапан, насос...
class DO_1_DI_1 : public digital_device
    {
    public:

        int get_state();

        void on();

        void off();

        int set_state( int new_state );

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,
            DI_INDEX = 0,

            PAR_FB_STATE = 0,
            };

        u_long switch_time;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным выходом и двумя дискретными входами.
///
/// Это может быть клапан, насос...
class DO_1_DI_2 : public digital_device
    {
    public:
        int get_state();

        void on();

        void off();

        int set_state( int new_state );

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,

            DI_INDEX_1 = 0,
            DI_INDEX_2,
            };

        u_long switch_time;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с двумя дискретными выходами и двумя дискретными входами.
///
/// Это может быть клапан, насос...
class DO_2_DI_2 : public digital_device
    {
    public:
        int get_state();

        void on();

        void off();

        int set_state( int new_state );

    private:
        enum CONSTANTS
            {
            DO_INDEX_1 = 0,
            DO_INDEX_2,

            DI_INDEX_1 = 0,
            DI_INDEX_2,
            };

        u_long switch_time;
    };
//-----------------------------------------------------------------------------
/// @brief Клапан mixproof.
class mix_proof : public digital_device
    {
    public:

        int get_state();

        void on();

        void open_upper_seat();

        void open_low_seat();

        void off();

        enum STATES
            {
            ST_CLOSE = 0,
            ST_OPEN,
            ST_UPPER_SEAT,
            ST_LOW_SEAT,
            };

        int set_state( STATES new_state );

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,
            DO_INDEX_U,
            DO_INDEX_L,

            DI_INDEX_U = 0,
            DI_INDEX_L,
            };

        u_long switch_time;
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым входом.
///
/// Это может быть температура, расход (величина)...
class AI_1 : public analog_device
    {
    public:
        float get_value();

        int set_value( float new_value );

        virtual float get_max_val() = 0;
        virtual float get_min_val() = 0;

    protected:
        enum CONSTANTS
            {
            AI_INDEX = 0,
            };
    };
//---------------------------------l--------------------------------------------
/// @brief Устройство ...
class temperature_e : public AI_1
    {
    public:
        float get_max_val();
        float get_min_val();

        float get_value();

        int set_value( float new_value );
    };
//-----------------------------------------------------------------------------
/// @brief Устройство ...
class level_e : public AI_1
    {
    public:
        float get_max_val();
        float get_min_val();
    };
//-----------------------------------------------------------------------------
/// @brief Устройство ...
class flow_e : public AI_1
    {
    public:
        float get_max_val();
        float get_min_val();

    private:
        enum CONSTANTS
            {
            C_MIN_PAR_NUMBER = 0,
            C_MAX_PAR_NUMBER,
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство ...
class concentration_e : public AI_1
    {
    public:
        float get_max_val();
        float get_min_val();

    private:
        enum CONSTANTS
            {
            C_MIN_PAR_NUMBER = 0,
            C_MAX_PAR_NUMBER,
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство ...
class analog_input_4_20 : public AI_1
    {
    public:
        float get_max_val();
        float get_min_val();
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним аналоговым входом.
///
/// Это может быть управляемый клапан...
class AO_1 : public analog_device
    {
    public:
        float get_value();

        int set_value( float new_value );

    private:
        enum CONSTANTS
            {
            AO_INDEX = 0,

            C_AO_MIN_VALUE = 0,
            C_AO_MAX_VALUE = 100,
            };
    };
//-----------------------------------------------------------------------------
/// @brief Устройство с одним дискретным входом.
///
/// Это может быть обратная связь, расход (есть/нет)...
class DI_1 : public digital_device
    {
    public:
        int get_state();

        void on();

        void off();

    private:
        enum CONSTANTS
            {
            DI_INDEX = 0,
            };
    };
//-----------------------------------------------------------------------------
/// @brief Счетчик.
///
class counter : public device,
    public u_int_4_state_device,
    public wago_device
    {
    public:        
        float get_value();

        int set_value( float new_value );

        int get_state();

        void on();

        void off();

        int set_state( int new_state );

        int parse_cmd( char *buff  );

        int load( file *cfg_file );

        void print() const;

        u_int_4 get_u_int_4_state();

        /// @brief Реализация интерфейса @ref i_save_device.
        int save_changed_state( char *buff )
            {
            return u_int_4_state_device::save_changed_state( buff );
            }

        /// @brief Реализация интерфейса @ref i_save_device.
        int save_state( char *buff )
            {
            return u_int_4_state_device::save_state( buff );
            }

    private:
        enum CONSTANTS
            {
            AI_INDEX = 0,
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

        int load_from_cfg_file( file *cfg_file );

        i_DO_device* get_V( int number );

        i_DO_device* get_N( int number );

        i_DO_device* get_M( int number );

        i_DI_device* get_LS( int number );

        i_DI_device* get_FS( int number );

        i_AI_device* get_AI( int number );

        i_AO_device* get_AO( int number );

        counter* get_CTR( int number );

        i_AI_device* get_TE( int number );

        i_AI_device* get_FE( int number );

        i_AI_device* get_LE( int number );

        i_DI_device* get_FB( int number );

        i_DO_device* get_UPR( int number );

        i_AI_device* get_QE( int number );

        static device_manager* get_instance();

        void print() const;

        static void set_instance( device_manager* new_instance );

    private:
        dev_stub stub;

        struct range
            {
            int start_pos;
            int end_pos;
            };

        range dev_types_ranges[ device::C_DEVICE_TYPE_CNT ];

        /// @brief Получение устройства по его номеру.
        ///
        device* get_device( device::DEVICE_TYPE dev_type, u_int dev_number,
            char const * dev_name );

        /// @brief Получение индееекса устройства по его номеру.
        ///
        int get_device_n( device::DEVICE_TYPE dev_type, u_int dev_number );

        int    devices_count;
        device **project_devices;

        static device_manager* instance;
    };
//-----------------------------------------------------------------------------
// Получение соответствующего устройства по его номеру.
#define V   device_manager::get_instance()->get_V
#define N   device_manager::get_instance()->get_N
#define M   device_manager::get_instance()->get_M
#define LS  device_manager::get_instance()->get_LS
#define FS  device_manager::get_instance()->get_FS
#define AI  device_manager::get_instance()->get_AI
#define AO  device_manager::get_instance()->get_AO
#define CTR device_manager::get_instance()->get_CTR
#define TE  device_manager::get_instance()->get_TE
#define FE  device_manager::get_instance()->get_FE
#define LE  device_manager::get_instance()->get_LE
#define FB  device_manager::get_instance()->get_FB
#define UPR device_manager::get_instance()->get_UPR
#define QE  device_manager::get_instance()->get_QE
//-----------------------------------------------------------------------------
#endif // PAC_DEVICES_H
