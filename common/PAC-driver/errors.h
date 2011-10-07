/// @file errors.h
/// @brief Содержит описания перечислений, которые используются в 
///  драйвере на сервере для организации информирования об ошибка.
///
/// Класс @ref base_error, служащий для организации работы с ошибкой, содержит   
/// всю необходимую информацию. Для хранения всех ошибок служит класс
/// @ref dev_errors_manager. 
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c PAC    - компиляция для контроллера.@n
/// @c DRIVER - компиляция для драйвера (ОС Windows).
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef ERRORS_H
#define ERRORS_H

#if !( defined PAC_PC || defined PAC_WAGO_750_860 || defined UPAC_7186E ) && \
    !defined DRIVER
#error You must define type!
#endif

#if ( defined PAC_PC || defined PAC_WAGO_750_860 || defined UPAC_7186E ) && \
    !defined PAC
#define PAC
#endif

//-----------------------------------------------------------------------------
#if defined PAC || defined DRIVER
enum ALARM_STATE
    {
    AS_NORMAL,
    AS_ALARM,
    AS_RETURN, 
    AS_ACCEPT, 
    };

enum PRIORITY_TYPE
    {
    PT_SYSTEM,
    PT_CRITICAL,
    PT_IMPOTENT,
    PT_UNIMPOTENT,
    P_INFORMATIONAL,    
    };

enum ALARM_TYPE
    {
    AT_DISCRETE,
    AT_VALUE,
    AT_DEVIATION,
    AT_RATE_OF_CHANGE,
    AT_SPECIAL,
    };


enum OBJECT_TYPE      ///< Тип объекта, для которого возникла тревога.
    {
    OT_UNKNOWN,
    OT_PAC,           ///< PAC.
    };

enum ALARM_CLASS      ///< Класс тревоги.
    {
    AC_UNKNOWN,
    AC_NO_CONNECTION, ///< Ошибка связи.

    AC_COM_DRIVER,    ///< Ошибка работы с COM-портом.
    AC_RUNTIME_ERROR, ///< Ошибки во время работы.
    };

enum ALARM_SUBCLASS         ///< Подкласс тревоги.
    {
    //AC_NO_CONNECTION,     ///< Ошибка связи.
    AS_WAGO = 1,            ///< Ошибки модулей WAGO.
    AS_PANEL,               ///< Ошибки панелей EasyView.
    AS_MODBUS_DEVICE,       ///< Ошибки устройства, опрашиваемого по Modbus.

    AS_EASYSERVER = 5,      ///< Ошибки EasyServer.

    //AC_RUNTIME_ERROR,     ///< Ошибки во время работы.
    AS_EMERGENCY_BUTTON = 1,///< Нажата аварийная кнопка.
    };

#endif // defined PAC || defined WIN32

extern const char *ALARM_CLASS_STR[ 4 ];
extern const char *ALARM_SUBCLASS_STR[ 4 ][ 5 ];
extern const int ALARM_CLASS_PRIORITY[ 4 ];

//-----------------------------------------------------------------------------
#ifdef PAC

#include "errors.h"
#include "param_ex.h"
//#include "sys.h"
#include "smart_ptr.h"
#include "PAC_dev.h"

//-----------------------------------------------------------------------------
/// @brief Базовый класс с информацией об ошибке устройства.
///
class base_error
    {
    public:
        base_error();

        /// @brief Сброс параметров ошибки в значение по умолчанию (0).
        virtual void reset_errors_params() = 0;

        /// @brief Сохранение ошибки в поток для передачи на сервер.
        ///        
        /// @param stream       - поток байт.
        /// @param is_new_state - изменилось ли состояние ошибки 
        ///                       (0 - нет, 1 - да).
        ///
        /// @return - количество записанных байт.        
        virtual int save_to_stream( u_char *stream, char &is_new_state ) = 0;

        /// @brief Отладочный вывод содержимого класса в консоль.
        virtual void print() const = 0;

        /// @brief Получение типа объекта.
        virtual unsigned char get_type() const = 0;

        /// @brief Получение номера объекта.
        virtual unsigned int get_n() const = 0;

        /// @brief Получение номера ошибки объекта.
        virtual unsigned int get_object_alarm_n() const = 0;

        /// @brief Выполнение команды над ошибкой.
        virtual int set_cmd( int cmd ) = 0;

    protected:
        unsigned char error_state;    ///< Cостояние ошибки.

        enum BASE_ERRORS_CMD          ///< Константы.
            {            
            BE_CMD_ACCEPT   = 100,    ///< Подтвердить ошибку.
            BE_CMD_SUPPRESS = 200,    ///< Подавить ошибку.
            BE_CMD_UNSET_SUPPRESS,    ///< Убрать подавление ошибки.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об ошибке простого устройства (клапан,
/// насос...).
///
/// У простого устройства может быть только одна ошибка (ошибка обратной
/// связи).
class simple_error: public base_error
    {
    public:
        simple_error( device* simple_device = 0 );
        virtual ~simple_error();

        /// @brief Сброс параметров ошибки в значение по умолчанию (0).
        void reset_errors_params();

        /// @brief Сохранение ошибки в поток для передачи на сервер.
        ///        
        /// @param stream       - поток байт.
        /// @param is_new_state - изменилось ли состояние ошибки 
        ///                       (0 - нет, 1 - да).
        ///
        /// @return - количество записанных байт.        
        int save_to_stream( u_char *stream, char &is_new_state );

        /// @brief Отладочный вывод содержимого класса в консоль.
        void print() const;

        /// @brief Получение типа объекта.
        unsigned char get_type() const;

        /// @brief Получение номера объекта.
        unsigned int get_n() const;

        /// @brief Получение номера ошибки объекта.
        unsigned int get_object_alarm_n() const;

        /// @brief Выполнение команды над ошибкой.
        int set_cmd( int cmd );

    private:
        device          *simple_device; ///< Простое устройство.

        enum SYMPLE_ERROR_CONST     ///< Константы.
            {
            SE_ERROR_CODE = 1,      ///< Код ошибки - нет обратной связи.
            SE_PRIORITY = 300,      ///< Приоритет ошибки простого устройства.
            };
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об всех ошибках простых устройств.
class dev_errors_manager
    {
    public:
        ~dev_errors_manager();
        
        int is_any_error() const;
            
        /// @brief Сохранение всех ошибок в поток для передачи на сервер.
        ///        
        /// @param stream - поток байт.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int save_to_stream( u_char *stream );

        /// @brief Добавление ошибки в массив ошибок.
        ///        
        /// @param s_error - добавляемая ошибка.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int add_error( base_error *s_error );

        /// @brief Сброс параметров всех ошибок в значение по умолчанию (0).
        void reset_errors_params();

        /// @brief Отладочный вывод содержимого класса в консоль.
        void print();

        /// @brief Изменение параметров ошибки.
        void set_cmd( unsigned int cmd, unsigned int object_type, 
            unsigned int object_number, unsigned int object_alarm_number );

        /// @brief Получение единственного экземпляра класса.
        static dev_errors_manager* get_instance();

    private:

        /// Единственный экземпляр класса.
        static auto_smart_ptr < dev_errors_manager > instance;

        /// @param s_errors_cnt - количество ошибок.
        dev_errors_manager();

        enum DEM_CONST
            {
            DEM_MAX_ERRORS_CNT = 29,
            };

        u_int_2 errors_id;

        std::vector< base_error* > s_errors_vector;    ///< Массив ошибок.

        u_char is_any_err;  //< Признак наличия ошибок простых устройств.
    };
//-----------------------------------------------------------------------------
#define G_DEV_ERRORS_MANAGER dev_errors_manager::get_instance()
//-----------------------------------------------------------------------------
#endif // PAC 

#endif //ERRORS_H
