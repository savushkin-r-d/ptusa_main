/// @file param_ex.h
/// @brief Содержит описания классов, которые реализуют работу с параметрами.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG - компиляция с выводом отладочной информации в консоль.@n@n
/// @c USE_SIMPLE_DEV_ERRORS   - компиляция с модулем ошибок устройств.@n
/// @c USE_NO_TANK_COMB_DEVICE - компиляция без объектов Танк и Гребенка. Для 
/// уменьшения размера программы при отсутствии таковых технологических
/// объектов.@n@n
/// @c I7186_E - компиляция для PAC I7186_E.@n
/// @c I7188_E - компиляция для PAC I7188_E.@n
/// @c I7188   - компиляция для PAC I7188.@n
/// @c W750    - компиляция для PAC W750.@n
/// 
/// @par Текущая версия:
/// @$Rev: $.\n
/// @$Author: $.\n
/// @$Date::                     $.

#ifndef PARAMS_EX_H
#define PARAMS_EX_H

#include <string.h>
#include "g_device.h"

#ifdef I7186_E
#include "plc_dev.h"
#endif // I7186_E

#ifdef W750
#include "PAC_dev.h"
#endif // I7186_E   

#include "sys.h"
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

    private:
        /// @brief Закрытый конструктор. 
        /// 
        /// Для вызова методов используется статический метод @ref get_instance.
        params_manager();

        /// Статический экземпляр класса для вызова методов.
        static params_manager *instance;

        /// Рабочий массив параметров.
        char params[ C_TOTAL_PARAMS_SIZE ];  

        /// Используется при создании экземпляра класса @ref parameters.
        int  last_idx; 

        /// Признак корректной загрузки параметров (верна контрольная сумма).
        int  loaded;

        unsigned int project_id;

        memory_range *params_mem;
        memory_range *CRC_mem;

        // Высчитывает контрольную сумму.
        unsigned int solve_CRC();

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
        /// @return - значение элемента с индексом @ref index. Если индекс 
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

        /// @brief Получение элемента через индекс.
        ///
        /// Для реализации интерфейса класса @ref array_device.
        ///
        /// @param index - индекс элемента.
        ///
        /// @return - значение элемента с индексом @ref index. Если индекс 
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
            int par_n = *( ( u_int_4* ) buff );
            ( *this )[ par_n ] = *( ( type* ) ( buff + sizeof( u_int_4 ) ) );

            return sizeof( u_int_4 ) + sizeof( type );
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
            if ( count > 0 && 0 == values )
                {
                values = new type[ count ];
                memset( values, 0, count * sizeof( type ) );
                }
            }

    protected:
        /// @brief Получение указателя на буфер для хранения значений
        /// параметров.
        ///
        /// @return - указатель на буфер для хранения значений параметров.
        type* get_values()
            {
            return values;
            }

        // Заглушка для обращения через индекс с выходом за диапазон.
        type            stub; 

        type            *values;
        unsigned int    count;
    };
//-----------------------------------------------------------------------------
class run_time_params_float: public parameters < float >, 
    array_device < float >
    {
    public:
        run_time_params_float( int count ):parameters < float >( count ),
            array_device < float >( 1, 
            "RT_PARAM_F", 
            count, 
            i_complex_device::ARRAY_DEV_FLOAT )
            {
            }

        float get_val( int idx )
            {
            return parameters< float >::get_val( idx );
            }

        int parse_cmd( char *buff )
            {
            int res = parameters< float >::parse_cmd( buff );
#ifdef DEBUG

#ifdef W750
            Print( "Set val work param float[ %u ] = %f\n",
                *( ( u_int_4* ) buff ), 
                *( ( float* ) ( buff + sizeof( u_int_4 ) ) ) );
#else
            Print( "Set val work param float[ %lu ] = %f\n",
                *( ( u_int_4* ) buff ), 
                *( ( float* ) ( buff + sizeof( u_int_4 ) ) ) );
#endif // W750

#endif //DEBUG
            return res;
            }

    };
//-----------------------------------------------------------------------------
class run_time_params_ulong: public parameters < u_int_4 >, 
    array_device < u_int_4 >
    {
    public:
        run_time_params_ulong( int count ) : parameters < u_int_4 >( count ),
            array_device < u_int_4 >( 1, "RT_PARAM_UL", count, 
            i_complex_device::ARRAY_DEV_ULONG )
            {
            }

        u_int_4 get_val( int idx )
            {
            return parameters< u_int_4 >::get_val( idx );
            }

        int parse_cmd( char *buff )
            {
            int res = parameters< u_int_4 >::parse_cmd( buff );
#ifdef DEBUG

#ifdef W750
            Print( "Set val work param ulong[ %u ] = %u\n",
                *( ( u_int_4* ) buff ), 
                *( ( u_int_4* ) ( buff + sizeof( u_int_4 ) ) ) );
#else
            Print( "Set val work param ulong[ %lu ] = %lu\n",
                *( ( u_int_4* ) buff ), 
                *( ( u_int_4* ) ( buff + sizeof( u_int_4 ) ) ) );
#endif // W750

#endif //DEBUG
            return res;
            }

    };
//-----------------------------------------------------------------------------
template < class type > class saved_params:
public parameters < type > 
    {
    public:
        saved_params( int count ) : parameters < type >( 
            count,
            ( type* ) params_manager::get_instance()->get_params_data( 
            count * sizeof( type ), start_pos ) )
            {
            }
        //---------------------------------------------------------------------
        int parse_cmd( char *buff )
            {
            int res = parameters< type >::parse_cmd( buff );

            u_int_4 idx = *( ( u_int_4* ) buff );

            params_manager::get_instance()->save( 
                start_pos + idx, sizeof( u_int_4* ) );

            return res;
            }
        //---------------------------------------------------------------------
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

    private:
        int start_pos;
    };
//-----------------------------------------------------------------------------
class saved_params_ulong: public saved_params < u_int_4 >, 
    array_device < u_int_4 >
    {
    public:
        saved_params_ulong( int count ) :
          saved_params < u_int_4 >( count ),
              
              array_device < u_int_4 >( 1, "S_PARAM_UL", count, 
              i_complex_device::ARRAY_DEV_ULONG )
              {
              }

          u_int_4 get_val( int idx )
              {
              return saved_params< u_int_4 >::get_val( idx );
              }

          int parse_cmd( char *buff )
              {
              int res = saved_params< u_int_4 >::parse_cmd( buff );
#ifdef DEBUG

#ifdef W750
              Print( "Set val saved param ulong[ %u ] = %u\n",
                  *( ( u_int_4* ) buff ), 
                  *( ( u_int_4* ) ( buff + sizeof( u_int_4 ) ) ) );
#else
              Print( "Set val saved param ulong[ %lu ] = %lu\n",
                  *( ( u_int_4* ) buff ), 
                  *( ( u_int_4* ) ( buff + sizeof( u_int_4 ) ) ) );
#endif // W750

#endif //DEBUG
              return res;
              }
    };
//-----------------------------------------------------------------------------
class saved_params_float: public saved_params < float >, 
    array_device < float >
    {
    public:
        saved_params_float( int count ):
          saved_params < float >( count ),
              array_device < float >( 1, "S_PARAM_F", count, 
              i_complex_device::ARRAY_DEV_FLOAT )
              {
              }

          float get_val( int idx )
              {
              return saved_params< float >::get_val( idx );
              }

          int parse_cmd( char *buff )
              {
              int res = saved_params< float >::parse_cmd( buff );
#ifdef DEBUG
              
#ifdef W750
              Print( "Set val saved param float[ %u ] = %f\n",
                  *( ( u_int_4* ) buff ), 
                  *( ( float* ) ( buff + sizeof( u_int_4 ) ) ) );
#else
              Print( "Set val saved param float[ %lu ] = %f\n",
                  *( ( u_int_4* ) buff ), 
                  *( ( float* ) ( buff + sizeof( u_int_4 ) ) ) );
#endif // W750
              
#endif //DEBUG
              return res;
              }
    };
//-----------------------------------------------------------------------------
class params_test
    {
    public:
        static int make_test();
    };
#endif // PARAMS_EX_H
