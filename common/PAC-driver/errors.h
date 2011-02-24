/// @file errors.h
/// @brief Содержит описания классов и структур, которые используются в 
///  драйвере на сервере для организации информирования об ошибка.
///
/// Класс @ref base_error, служащий для организации работы с ошибкой, содержит   
/// всю необходимую информацию. Для хранения всех ошибок служит класс
/// @ref dev_errors_manager. 
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c PAC   - компиляция для контроллера.@n
/// @c WIN32 - компиляция для Windows.
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef ERRORS_H
#define ERRORS_H

#ifdef DRIVER
typedef unsigned char uchar;

#include <vector>
#include "SWMRG.h"

#include "PAC_err.h"
#endif // DRIVER
//-----------------------------------------------------------------------------
#if ( defined PAC || defined PAC_PC ) || defined WIN32
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
#endif // defined PAC || defined WIN32
//-----------------------------------------------------------------------------
#ifdef DRIVER

//  Глобальный идентификатор тревоги.
struct alarm_id
    {
    int object_type;
    int object_number;
    int object_alarm_number;
    };

#pragma pack( push, 8 ) //Выравнивание полей структур по 8 байт.
//-----------------------------------------------------------------------------
struct alarm_params 
    {
    double  param1;
    double  param2;
    double  param3;
    double  param4;
    double  param5;
    double  param6;
    double  param7;
    double  param8;
    double  param9;
    double  param10;
    };
//-----------------------------------------------------------------------------
struct alarm
    {
    alarm_params params;

    //  Определяет тип тревоги:
    //     atDiscrete      - дискретная ( true/false )
    //     atValue         - контроля значения ( Lo/LoLo, Hi/HiHi )
    //     atDeviation     - отклонения ( MinValue/MajValue )
    //     atRateOfChange  - изменения скорости ( speed )
    //     atSpecial       - специальная
    ALARM_TYPE type;

    //  Описание тревоги.
    char *description;

    //  Блокировка тревоги на этапе
    //  проектирования.
    uchar enable;

    //  Определяет принадлежность тревоги
    //  какой либо группе тревог.
    char *group;

    //  Блокировка тревоги во время работы.
    uchar  inhibit;

    //  Приоритет тревоги ( 0 - 999 )
    //  Тип тревоги:
    //     0       - системные
    //     1-249   - критические
    //     250-499 - важные
    //     500-749 - маловажные
    //     750-999 - информационные
    int priority;

    //  Состояние тревоги:
    //     asNormal    - тревоги нет
    //     asAlarm     - тревога есть
    //     asReturn    - контролируемое значение
    //                вернулось в нормальное
    //                состояние
    //     asAccept    - тревога подтверждена
    ALARM_STATE state;

    //  Подавление тревоги клиентами.
    uchar  suppress;

    //  Глобальный идентификатор тревоги:
    alarm_id id;

    // id драйвера.
    uchar driver_id;

    bool operator == ( const alarm &alarm2 )
        {
        return !strcmp( this->description, alarm2.description ); 
        }

    bool operator < ( const alarm &alarm2 ) const
        {
        return strcmp( this->description, alarm2.description ) < 0 ? 1 : 0;
        }

    alarm( alarm const& copy );

    alarm & operator = ( const alarm & copy );

    int load_from_stream_as_simple_error( char *stream );

    alarm();

    ~alarm();

    void accept()
        {
        state = AS_ACCEPT;
        }
    };
//-----------------------------------------------------------------------------
struct all_alarm
    {
    int     cnt;
    alarm   *alarms; 
    int     id;
    };
//-----------------------------------------------------------------------------
struct error_cmd
    {
    int cmd; 
    int object_type;
    int object_number;
    int object_alarm_number;
    };
//-----------------------------------------------------------------------------
#pragma pack( pop )
//-----------------------------------------------------------------------------
class alarm_manager
    {
    public:

        enum AM_CONST
            {
            AM_MAX_COUNT = 256,
            };

        enum CRITICAL_ERRORS
            {
            CE_WAGO = 1,
            CE_PANEL,
            CE_EMERGENCY_BUTTON,
            };

        alarm_manager();
        ~alarm_manager();

        int add_no_PAC_connection_error( const char *description, 
            uchar project_id );
        int remove_no_PAC_connection_error( const char *description,
            uchar driver_id );

        int get_alarms( uchar driver_id, all_alarm &project_alarms );

        int add_PAC_critical_errors( CRITICAL_ERRORS type, uchar project_id, 
            const char* PAC_name, unsigned int param_number, u_int priority );

        int clear_PAC_critical_errors( uchar driver_id );

        int clear_PAC_simple_devices_errors( uchar driver_id );

        int add_alarm( alarm &new_alarm );

#ifndef MONITORING_DRIVER
    private:
#endif // MONITORING_DRIVER
        //1 - код уникальности ошибок.
        //2 - вектор ошибок проектов.   
        //3 - массив ошибок для передачи серверу.
        //4 - массив объектов синхронизации работы с ошибками.
        //5 - имя группы ошибок связи с модулями WAGO.
        unsigned int id[ AM_MAX_COUNT ];                        //1              
        std::vector< alarm > alarms_vector[ AM_MAX_COUNT ];     //2
        alarm   *all_projects_alarms_array[ AM_MAX_COUNT ];     //3
        CSWMRG  synch_all_projects_alarms[ AM_MAX_COUNT ];      //4
        
        static const char *ERRORS_GROUP_WAGO_CONNECT;           //5
        static const char *ERRORS_GROUP_SIMPLE_DEVICES;         //6
        static const char *ERRORS_GROUP_PANEL_CONNECT;          //7
        static const char *ERRORS_GROUP_EMERGENCY_BUTTONS;      //8

        int remove_alarm( alarm &r_alarm );
        int clear( uchar driver_id );

        //Синхронизация ошибок, находящихся в векторе alarms_vector и массиве 
        //all_projects_alarms_array.
        int synch_project_alarms( uchar driver_id );
    };
#endif // DRIVER

#if defined PAC || defined PAC_PC

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
#endif // PAC || PAC_PC

#endif //ERRORS_H
