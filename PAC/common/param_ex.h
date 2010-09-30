/// @file param_ex.h
/// @brief Содержит описания классов, которые реализуют работу с параметрами.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG - компиляция с выводом отладочной информации в консоль.@n
/// @c KEY_CONFIRM - переход к следующему отладочному сообщению по нажатии
/// клавиши.@n@n
/// @c USE_SIMPLE_DEV_ERRORS   - компиляция с модулем ошибок устройств.@n
/// @c USE_NO_TANK_COMB_DEVICE - компиляция без объектов Танк и Гребенка. Для 
/// уменьшения размера программы при отсутствии таковых технологических
/// объектов.
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PARAMS_EX_H
#define PARAMS_EX_H

#include <string.h>

#include "sys.h"
#ifdef LINUX_OS
#include "sys_linux.h"
#endif // LINUX_OS

#ifdef WIN_OS
#include "sys_win.h"
#endif // WIN_OS
#include "PAC_dev.h"
//-----------------------------------------------------------------------------
/// @brief Работа с параметрами. 
/// 
/// Служит для создания объектов для работы с параметрами. Проверяет 
/// корректность данных путем подсчета контрольной суммы.
class params_manager
    {
    public:
        enum CONSTANTS
            {
            C_TOTAL_PARAMS_SIZE = 700 * 4, ///< Общий размер памяти параметров.
            };

        /// @brief Возвращает единственный экземпляр класса для работы с 
        /// параметрами.
        ///
        /// @return - указатель на единственный объект класса @ref
        /// params_manager.
        static params_manager* get_instance();

        /// @brief Инициализация значений параметров.
        ///
        /// Считывание параметров из EEPROM в массив параметров, проверка 
        /// контрольной суммы. Контрольная сумма зависит от 
        /// параметра @ref C_TOTAL_PARAMS_SIZE.
        ///
        /// @param project_id - номер проекта. Для привязки параметров к 
        /// конкретной управляющей программе.
        ///
        /// @return 0 - ОК.
        /// @return 1 - Ошибка контрольной суммы. 
        int init( unsigned int project_id );

        /// @brief Окончательная инициализация значений параметров.
        ///
        /// Если при считывании параметров из EEPROM произошла ошибка (метод 
        /// @ref init), параметры инициализируются значениями по умолчанию 
        /// путем вызова заданных функций.
        ///
        /// @param auto_init_params - вызывать ли функцию init_params классов
        /// tank и comb.
        /// @param auto_init_work_params - вызывать ли функцию init_work_params 
        /// классов tank и comb.
        /// @param custom_init_params_function - пользовательская функция 
        /// инициализации параметров.
        void final_init( int auto_init_params = 1, 
            int auto_init_work_params = 1,
            void ( *custom_init_params_function )() = 0 );

        /// @brief Запись параметров в EEPROM.
        ///
        /// Запись параметров из массива параметров в EEPROM.
        ///
        /// @param start_pos - номер индекса, с которого начать запись
        /// параметров (для записи только одного параметра).
        /// @param count - количество записываемых байт.
        void save( int start_pos = 0, int count = 0 );

        /// @brief Сброс контрольной суммы (для инициализации значениями по 
        /// умолчанию).
        void reset_CRC();

        /// @brief Получение указателя на блок данных параметров.
        ///
        /// @param size      - размер блока данных в байтах.
        /// @param [out] start_pos - стартовый индекс в глобальном массиве 
        /// параметров. Необходим для дальнейшей записи параметров в память.
        ///
        /// @return 0 - ОК.
        /// @return 1 - Ошибка контрольной суммы. 
        char* get_params_data( int size, int &start_pos );

        ~params_manager();

    private:
        /// @brief Закрытый конструктор. 
        /// 
        /// Для вызова методов используется статический метод @ref get_instance.
        params_manager();

        /// Статический экземпляр класса для вызова методов.
        static auto_smart_ptr< params_manager > instance;

        /// Рабочий массив параметров.
        char params[ C_TOTAL_PARAMS_SIZE ];  

        /// Номер последнего выделенного параметра. Используется при создании
        /// экземпляра класса @ref parameters.
        u_int last_idx;

        /// Признак корректной загрузки параметров (достоверность контрольной
        /// суммы).
        int loaded;

        u_int project_id;   ///< Номер проекта (для уникальности параметров).

        memory_range *params_mem; ///< Память параметров.
        memory_range *CRC_mem;    ///< Память контрольной суммы.

        // Высчитывание контрольной суммы.
        u_int solve_CRC();

        /// @brief Проверка контрольной суммы.
        ///
        /// Рассчет контрольной суммы и сравнение ее со значением, 
        /// хранящемся в NVRAM (2 первых байта NVRAM).
        ///
        /// @return 0 - ОК.
        /// @return 1 - Ошибка контрольной суммы. 
        int check_CRC();

        /// @brief Рассчет контрольной суммы и запись ее в NVRAM.
        void make_CRC();
    };
//-----------------------------------------------------------------------------
/// @brief Работа с массивом параметров. 
/// 
/// Служит для создания конкретных типов параметров. Реализованы операции
/// доступа через индекс ( [] ).
template < class type > class parameters 
    {
    public:      
        /// @brief Получение элемента через операцию индексирования.
        ///
        /// @param index - индекс элемента.
        ///
        /// @return - значение элемента с заданным индексом. Если индекс 
        /// выходит за диапазон, возвращается значение заглушки - поля @ref
        /// stub ( значение 0 ).
        type& operator[] ( unsigned int index )
            {
            if ( index < count )
                {
                return values[ index ];
                }
#ifdef DEBUG
            else
                {
                Print( "parameters[] - error: index[ %u ] > count [ %u ]\n",
                    index, count );
                }
#endif // DEBUG

            stub = 0;
            return stub;
            }

        int save( u_int idx, type val )
            {
            this->operator []( idx ) = val;
            return 0;
            }

        /// @brief Получение элемента через индекс.
        ///
        /// Для реализации интерфейса класса @ref array_device.
        ///
        /// @param idx - индекс элемента.
        ///
        /// @return - значение элемента с заданным индексом. Если индекс 
        /// выходит за диапазон, возвращается значение заглушки - поля @ref
        /// stub ( значение 0 ).
        type get_val( int idx )
            {
            return this->operator []( idx );
            }

        /// @brief Выполнение команды.
        ///
        /// Для реализации интерфейса класса @ref array_device.
        ///
        /// @param buff - символьный поток с командой.
        ///
        /// @return - количество обработанных байт команды.
        int parse_cmd( char *buff )
            {
            u_int_4 par_n;
            memcpy( &par_n, buff, sizeof( par_n ) );
            buff += sizeof( par_n );

            memcpy( &( ( *this )[ par_n ] ), buff, sizeof( type ) );

            return sizeof( par_n ) + sizeof( type );
            }

        /// @brief Сбрасывает все значения параметров в 0.
        virtual void reset_to_0()
            {
            for ( u_int i = 0; i < count; i++ )
                {
                values[ i ] = 0;
                }
            }

        /// @brief Получение количества параметров.
        ///
        /// @return - количество параметров.
        u_int get_count()
            {
            return count;
            }

        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        /// @param value - указатель на буфер для хранения значений параметров.
        parameters( int count, type *value = 0 ): count( count ),
            values( value )
            {
#ifdef DEBUG
            if ( 0 == count )
                {
                Print( "parameters(...) - error: count = 0!\n" );
                }
#endif // DEBUG
            is_delete = 0;
            if ( count > 0 && 0 == values )
                {
                is_delete = 1;
                values = new type[ count ];
                memset( values, 0, count * sizeof( type ) );
                }
            }

        virtual ~parameters()
            {
            if ( 1 == is_delete && count > 0 && values > 0 )
                {
                delete [] values;
                values = 0;
                count = 0;
                }
            }

    protected:
        char is_delete; ///< Признак удаления буфера при удалении объекта.

        /// @brief Получение указателя на буфер для хранения значений
        /// параметров.
        ///
        /// @return - указатель на буфер для хранения значений параметров.
        type* get_values()
            {
            return values;
            }

        /// Заглушка для обращения через индекс с выходом за диапазон.
        type         stub; 

        unsigned int count;     ///< Количество элементов.
        type         *values;   ///< Указатель на массив значений элементов.
    };
//-----------------------------------------------------------------------------
/// @brief Работа с параметрами времени выполнения типа float.
///
/// Данные параметры передаются на сервер через соответствующие теги.
class run_time_params_float: public parameters < float >,
    public array_device < float >
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        run_time_params_float( int count ):parameters < float >( count ),
            array_device < float >( 1,
            "RT_PARAM_F", 
            count, 
            i_complex_device::ARRAY_DEV_FLOAT )
            {
            }

        /// @brief Реализация интерфейса класса @ref array_device.
        float get_val( int idx )
            {
            return parameters< float >::get_val( idx );
            }

        /// @brief Реализация интерфейса класса @ref array_device.
        int parse_cmd( char *buff )
            {
            int res = parameters< float >::parse_cmd( buff );
#ifdef DEBUG
            float   val;
            u_int_4 idx;
            memcpy( &idx, buff, sizeof( idx ) );
            buff += sizeof( idx );
            memcpy( &val, buff, sizeof( val ) );

            Print( "Set val work param float[ %2u ] = %f\n", idx, val );
#endif // DEBUG
            return res;
            }

    };
//-----------------------------------------------------------------------------
/// @brief Работа с параметрами времени выполнения типа @ref u_int_4.
///
/// Данные параметры передаются на сервер через соответствующие теги.
class run_time_params_u_int_4: public parameters < u_int_4 >,
    public array_device < u_int_4 >
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        /// @param name  - имя параметров.
        run_time_params_u_int_4( int count,
            const char* name = "RT_PARAM_UL" ) : parameters < u_int_4 >( count ),
            array_device < u_int_4 >( 1, name, count,
            i_complex_device::ARRAY_DEV_ULONG )
            {
            }

        virtual ~run_time_params_u_int_4()
            {
            }

        /// @brief Реализация интерфейса класса @ref array_device.
        u_int_4 get_val( int idx )
            {
            return parameters< u_int_4 >::get_val( idx );
            }

        /// @brief Реализация интерфейса класса @ref array_device.
        int parse_cmd( char *buff )
            {
            int res = parameters< u_int_4 >::parse_cmd( buff );
#ifdef DEBUG
            u_int_4 val;
            u_int_4 idx;
            memcpy( &idx, buff, sizeof( idx ) );
            buff += sizeof( idx );
            memcpy( &val, buff, sizeof( val ) );

            Print( "Set val work param u_int_4[ %2u ] = %u\n", idx, val );
#endif // DEBUG
            return res;
            }
    };
//-----------------------------------------------------------------------------
/// @brief Работа с сохраняемыми параметрами.
///
/// Данные параметры передаются на сервер через соответствующие теги. При
/// перезагрузке PAC их значения сохраняются.
template < class type > class saved_params:
public parameters < type > 
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        saved_params( int count ) : parameters < type >(
            count,
            ( type* ) params_manager::get_instance()->get_params_data( 
            count * sizeof( type ), start_pos ) )
            {
            }

        virtual ~saved_params()
            {
            }

        /// @brief Реализация интерфейса класса @ref array_device.
        int parse_cmd( char *buff )
            {
            int res = parameters< type >::parse_cmd( buff );

            u_int_4 idx;
            memcpy( &idx, buff, sizeof( idx ) );

            params_manager::get_instance()->save( 
                start_pos + idx * sizeof( type ), sizeof( type ) );

            return res;
            }

        /// @brief Сохранение значения параметра в энергонезависимой памяти.
        ///
        /// Операция доступа через индекс сохраняет значение параметра только
        /// в буфере, так что для сохранения его в энергонезависимой памяти надо
        /// использовать данный метод.
        int save( u_int idx, type value )
            {
            int res = 1;
            if ( idx < parameters< type >::get_count() )
                {
                parameters< type >::get_values()[ idx ] = value;

                params_manager::get_instance()->save( 
                    start_pos + idx * sizeof( type ), sizeof( type ) );
                res = 0;
                }
#ifdef DEBUG
            else
                {
                Print( "parameters:save - index[ %u ] > count [ %u ]\n",
                    idx, parameters< type >::get_count() );
                }
#endif // DEBUG
            return res;
            }

        /// @brief Сохранение значения всех параметов в энергонезависимой памяти.
        ///
        /// Операция доступа через индекс сохраняет значение параметра только
        /// в буфере, так что для сохранения его в энергонезависимой памяти надо
        /// использовать данный метод.
        int save_all()
            {
            params_manager::get_instance()->save(
                start_pos,  parameters< type >::get_count() * sizeof( type ) );

            return 0;
            }

        void reset_to_0()
            {
            for ( u_int i = 0; i <  parameters< type >::get_count(); i++ )
                {
                parameters< type >::get_values()[ i ] = 0;
                }

            params_manager::get_instance()->save(
                start_pos, sizeof( type ) * parameters< type >::get_count() );
            }

    private:
        /// Индекс начала значений в общем массиве, для сохранения значения
        /// параметра в энергонезависимой памяти (@ref save).
        int start_pos;
    };
//-----------------------------------------------------------------------------
/// @brief Работа с сохраняемыми параметрами типа @ref u_int_4.
class saved_params_u_int_4: public saved_params < u_int_4 >,
    public array_device < u_int_4 >
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        saved_params_u_int_4( int count ) :
          saved_params < u_int_4 >( count ),              
              array_device < u_int_4 >( 1, "S_PARAM_UL", count,
              i_complex_device::ARRAY_DEV_ULONG )
              {
              }

          /// @brief Реализация интерфейса класса @ref array_device.
          u_int_4 get_val( int idx )
              {
              return saved_params< u_int_4 >::get_val( idx );
              }

          /// @brief Реализация интерфейса класса @ref array_device.
          int parse_cmd( char *buff )
              {
              int res = saved_params< u_int_4 >::parse_cmd( buff );
#ifdef DEBUG
              u_int_4  val;
              u_int_4 idx;
              memcpy( &idx, buff, sizeof( idx ) );
              buff += sizeof( idx );
              memcpy( &val, buff, sizeof( val ) );

              Print( "Set val saved param u-int_4[ %2u ] = %u\n", idx, val );
#endif // DEBUG

              return res;
              }
    };
//-----------------------------------------------------------------------------
/// @brief Работа с сохраняемыми параметрами типа float.
class saved_params_float: public saved_params < float >, 
    public array_device < float >
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        saved_params_float( int count ):
          saved_params < float >( count ),
              array_device < float >( 1, "S_PARAM_F", count, 
              i_complex_device::ARRAY_DEV_FLOAT )
              {
              }

          virtual ~saved_params_float()
              {
              }

          /// @brief Реализация интерфейса класса @ref array_device.
          float get_val( int idx )
              {
              return saved_params< float >::get_val( idx );
              }

          /// @brief Реализация интерфейса класса @ref array_device.
          int parse_cmd( char *buff )
              {
              int res = saved_params< float >::parse_cmd( buff );
#ifdef DEBUG
              float   val;
              u_int_4 idx;
              memcpy( &idx, buff, sizeof( idx ) );
              buff += sizeof( idx );
              memcpy( &val, buff, sizeof( val ) );

              Print( "Set val saved param float[ %2u ] = %f\n", idx, val );
#endif //DEBUG
              return res;
              }
    };
//-----------------------------------------------------------------------------
/// @brief Класс для тестирования классов работы с параметрами.
class params_test
    {
    public:
        /// @brief Тестирование классов работы с параметрами.
        ///
        /// @return 0 - Ок.
        /// @return 1 - Ошибка.
        static int make_test();
    };
//-----------------------------------------------------------------------------
#endif // PARAMS_EX_H
