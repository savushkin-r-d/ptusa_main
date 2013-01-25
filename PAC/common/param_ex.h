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

#include <math.h>
#include <string.h>

#include "base_mem.h"
#include "g_device.h"

#ifdef __BORLANDC__
#pragma option -w-inl
#pragma option -w-ccc
#pragma option -w-rch
#endif // __BORLANDC__

class saved_params_u_int_4;
//-----------------------------------------------------------------------------
/// @brief Работа с параметрами.
///
/// Служит для создания объектов для работы с параметрами. Проверяется
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

        enum PARAMS
            {
            P_COUNT = 1,

            P_IS_RESET_PARAMS = 1,
            };

        saved_params_u_int_4 *par;

        int save_params_as_Lua_str( char* str );

        int restore_params_from_server_backup( char *backup_str );

        // Высчитывание контрольной суммы.
        u_int solve_CRC();

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

template < class type, bool is_float > class parameters
    {
    private:
        char name[ 20 ];

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
            if ( index <= count && index > 0 )
                {
                return values[ index - 1 ];
                }
#ifdef DEBUG
            else
                {
                Print( "parameters[] - error: index[ %u ] > count [ %u ]\n",
                    index, count );
                }
#endif // DEBUG

            return stub;
            }

        /// @brief Получение элемента через операцию индексирования.
        ///
        /// @param index - индекс элемента.
        ///
        /// @return - значение элемента с заданным индексом. Если индекс
        /// выходит за диапазон, возвращается значение заглушки - поля @ref
        /// stub ( значение 0 ).
        const type& operator[] ( unsigned int index ) const
            {
            if ( index <= count && index > 0 )
                {
                return values[ index - 1 ];
                }
#ifdef DEBUG
            else
                {
                Print( "parameters[] - error: index[ %u ] > count [ %u ]\n",
                    index, count );
                }
#endif // DEBUG
                        
            return stub;
            }
        /// @brief Получение элемента через индекс.
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

        /// @brief Установка всех параметров в нулевое значение.
        virtual void reset_to_0()
            {
            for ( u_int i = 1; i <= count; i++ )
                {
                values[ i - 1 ] = 0;
                }
            }

        /// @brief Получение количества параметров.
        ///
        /// @return - количество параметров.
        u_int get_count() const
            {
            return count;
            }

        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        /// @param name  - имя объекта.
        /// @param value - указатель на буфер для хранения значений параметров.
        parameters( int count, const char *name, type *value = 0 ): count( count ),
            values( value )
            {
            stub = 0;

            strncpy( this->name, name, sizeof( this->name ) );

#ifdef DEBUG_IDE
            if ( 0 == count )
                {
                Print( "parameters(...) - error: count = 0!\n" );
                }
            //debug_break;
#endif // DEBUG_IDE
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

        void print() const
            {
            printf( "param %d\n", count );
            }

        int save_device_ex( char *buff, const char *prefix, const char *new_name )
            {
            sprintf( buff, "%s%s = \n", prefix, new_name );
            save_dev( buff + strlen( buff ), prefix );

            return 0;
            }
                
        int save_device( char *buff, const char *prefix )
            {
            sprintf( buff, "%s%s = \n", prefix, name );            
            save_dev( buff + strlen( buff ), prefix );

            return strlen( buff );
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

    private:
        int save_dev( char *buff, const char *prefix )
            {
            sprintf( buff, "%s\t{\n", prefix );
            int answer_size = strlen( buff );

            sprintf( buff + answer_size, "%s\t", prefix );
            answer_size += strlen( buff + answer_size );

            for ( u_int i = 1; i <= count; i++ )
                {
                if ( is_float )
                    {
                    float val  = ( float ) get_val( i );
                    if ( 0. == val )
                        {
                        sprintf( buff + answer_size, "0, " );
                        }
                    else
                        {
                        double tmp;
                        if ( modf( val, &tmp ) == 0 )
                            {
                            sprintf( buff + answer_size, "%d, ", ( int ) val );
                            }
                        else
                            {
                            sprintf( buff + answer_size, "%.2f, ", val );
                            }
                        }
                    }
                else
                    {
                    sprintf( buff + answer_size, "%u, ", ( u_int ) get_val( i ) );
                    }

                answer_size += strlen( buff + answer_size );
                }

            sprintf( buff + answer_size, "\n%s\t},\n", prefix );
            answer_size += strlen( buff + answer_size );
            return answer_size;
            }

    };
//-----------------------------------------------------------------------------
/// @brief Работа с параметрами времени выполнения типа float.
///
/// Данные параметры передаются на сервер через соответствующие теги.
class run_time_params_float: public parameters < float, true >
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        run_time_params_float( int count ):parameters < float, true >( count, "RT_PAR_F" )
            {
            }

    protected:
        
        float get_val( int idx )
            {
            return parameters< float, true >::get_val( idx );
            }
    };
//-----------------------------------------------------------------------------
/// @brief Работа с параметрами времени выполнения типа @ref u_int_4.
///
/// Данные параметры передаются на сервер через соответствующие теги.
class run_time_params_u_int_4: public parameters < u_int_4, false >
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        /// @param name  - имя параметров.
        run_time_params_u_int_4( int count,
            const char *name = "RT_PARAM_UI" ) : parameters < u_int_4, false >( count, name )           
            {
            }

        virtual ~run_time_params_u_int_4()
            {
            }

    protected:        
        u_int_4 get_val( int idx )
            {
            return parameters< u_int_4, false >::get_val( idx );
            }
    };
//-----------------------------------------------------------------------------
/// @brief Работа с сохраняемыми параметрами.
///
/// Данные параметры передаются на сервер через соответствующие теги. При
/// перезагрузке PAC их значения сохраняются.
template < class type, bool is_float > class saved_params:
public parameters < type, is_float >
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        /// @param name  - имя объекта.
        saved_params( int count, const char *name ) : parameters < type, is_float >(
            count, name,
            ( type* ) params_manager::get_instance()->get_params_data( 
            count * sizeof( type ), start_pos ) )
            {
            }

        virtual ~saved_params()
            {
            }

        //template < class type, bool is_float > class proxy_data
        //    {
        //    public:
        //        proxy_data( type value, int idx, saved_params * par
        //            ):value( value ), idx( idx ), par( par )
        //            {
        //            }

        //        type &value;

        //        int idx;

        //        saved_params *par;


        //    operator type ()
        //        {
        //        return value;
        //        }

        //    type operator =( type value )
        //        {
        //        if ( par )
        //            {                    
        //            par->save( idx, value );
        //            }                

        //        return value;
        //        }
        //    };

////        /// @brief Получение элемента через операцию индексирования.
////        ///
////        /// @param index - индекс элемента.
////        ///
////        /// @return - значение элемента с заданным индексом. Если индекс
////        /// выходит за диапазон, возвращается значение заглушки - поля @ref
////        /// stub ( значение 0 ).
////        proxy_data< type, is_float > operator[] ( unsigned int index )        
////            {
////            if ( index < count )
////                {
////                return proxy_data< type, is_float >( values[ index ], index, this );
////                }
////#ifdef DEBUG
////            else
////                {
////                Print( "parameters[] - error: index[ %u ] > count [ %u ]\n",
////                    index, count );
////                }
////#endif // DEBUG
////
////            stub = 0;
////            return proxy_data< type, is_float >( stub, -1, 0 );
////            }

        /// @brief Сохранение значения параметра в энергонезависимой памяти.
        ///
        /// Операция доступа через индекс сохраняет значение параметра только
        /// в буфере, так что для сохранения его в энергонезависимой памяти надо
        /// использовать данный метод.
        type save( u_int idx, type value )
            {
            if ( idx < parameters< type, is_float >::get_count() )
                {
                parameters< type, is_float >::get_values()[ idx ] = value;

                params_manager::get_instance()->save( 
                    start_pos + idx * sizeof( type ), sizeof( type ) );
                }
#ifdef DEBUG
            else
                {
                Print( "parameters:save - index[ %u ] > count [ %u ]\n",
                    idx, parameters< type, is_float >::get_count() );
                }
#endif // DEBUG
            return value;
            }

        /// @brief Сохранение значения всех параметров в энергонезависимой памяти.
        ///
        /// Операция доступа через индекс сохраняет значение параметра только
        /// в буфере, так что для сохранения его в энергонезависимой памяти надо
        /// использовать данный метод.
        int save_all()
            {
            params_manager::get_instance()->save(
                start_pos,  parameters< type, is_float >::get_count() * sizeof( type ) );

            return 0;
            }

        /// @brief Установка всех параметров в нулевое значение и сохранение
        /// в энергонезависимой памяти.
        void reset_to_0()
            {
            for ( u_int i = 0; i <  parameters< type, is_float >::get_count(); i++ )
                {
                parameters< type, is_float >::get_values()[ i ] = 0;
                }

            params_manager::get_instance()->save(
                start_pos, sizeof( type ) * parameters< type, is_float >::get_count() );
            }

        void print() const
            {
            //printf( "saved param %d\n", count );
            }

    private:
        /// Индекс начала значений в общем массиве, для сохранения значения
        /// параметра в энергонезависимой памяти (@ref save).
        int start_pos;
    };
//-----------------------------------------------------------------------------
/// @brief Работа с сохраняемыми параметрами типа @ref u_int_4.
class saved_params_u_int_4: public saved_params < u_int_4, false >
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        saved_params_u_int_4( int count ) :
          saved_params < u_int_4, false >( count, "S_PAR_UI" )
              {
              }

          virtual ~saved_params_u_int_4()
              {
              }

    protected:        
        u_int_4 get_val( int idx )
            {
            return saved_params< u_int_4, false >::get_val( idx );
            }
    };
//-----------------------------------------------------------------------------
/// @brief Работа с сохраняемыми параметрами типа float.
class saved_params_float: public saved_params < float, true >     
    {
    public:
        /// @brief Конструктор.
        ///
        /// @param count - количество параметров.
        saved_params_float( int count ):
          saved_params < float, true >( count, "S_PAR_F" )
              {
              }

          virtual ~saved_params_float()
              {
              }

    protected:        
        float get_val( int idx )
            {
            return saved_params< float, true >::get_val( idx );
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
#ifdef __BORLANDC__
#pragma option -w.inl
#pragma option -w.ccc
#pragma option -w.rch
#endif // __BORLANDC__

#endif // PARAMS_EX_H
