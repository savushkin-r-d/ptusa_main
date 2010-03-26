/// @file param_ex.h
/// @brief Содержит описания классов, которые реализуют работу с параметрами.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c USE_SIMPLE_DEV_ERRORS   - компиляция с модулем ошибок устройств.@n
/// @c USE_NO_TANK_COMB_DEVICE - компиляция без объектов Танк и Гребенка.
/// 
/// @par Текущая версия:
/// @$Rev: $.\n
/// @$Author: $.\n
/// @$Date::                    #$.

#ifndef PARAMS_EX_H
#define PARAMS_EX_H

#include <string.h>

#include "g_device.h"
#include "plc_dev.h"

#ifdef USE_SIMPLE_DEV_ERRORS
#include "errors.h"

extern dev_errors_manager *g_dev_errors_manager; 
#endif // USE_SIMPLE_DEV_ERRORS

#ifndef USE_NO_TANK_COMB_DEVICE
#include "priem.h"

extern TTank            **g_tanks;
extern TMyComb          **g_combs;
#endif //USE_NO_TANK_COMB_DEVICE
//-----------------------------------------------------------------------------
#include "sys.h"
#include "sys_7186.h"
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
template < class type > class parameters
    {
    public:       
        type& operator[] ( unsigned int index );

        type get_val( int idx );

        int parse_cmd( char *buff );

        u_int get_count() const;
                
        parameters( int count, type *value = 0 );

    protected:
        type* get_values();

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
        run_time_params_float( int count );

        float get_val( int idx ) const;

        int parse_cmd( char *buff  );
    };
//-----------------------------------------------------------------------------
class run_time_params_ulong: public parameters < u_int_4 >, 
    array_device < u_int_4 >
    {
    public:
        run_time_params_ulong( int count );

        u_int_4 get_val( int idx );

        int parse_cmd( char *buff  );
    };
//-----------------------------------------------------------------------------
template < class type > class saved_params:
public parameters < type > 
    {
    public:
        saved_params( int count );

        int parse_cmd( char *buff  );

        int save( u_int idx, type value );

    private:
        int start_pos;
    };
//-----------------------------------------------------------------------------
class saved_params_ulong: public saved_params < u_int_4 >, 
    array_device < u_int_4 >
    {
    public:
        saved_params_ulong( int count );

        u_int_4 get_val( int idx );

        int parse_cmd( char *buff  );
    };
//-----------------------------------------------------------------------------
class saved_params_float: public saved_params < float >, 
    array_device < float >
    {
    public:
        saved_params_float( int count );

        float get_val( int idx );

        int parse_cmd( char *buff  );
    };
//-----------------------------------------------------------------------------
class params_test
    {
    public:
        static int make_test();
    };
#endif // PARAMS_EX_H