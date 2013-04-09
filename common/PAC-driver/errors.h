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

#if !( defined PAC_PC || defined PAC_WAGO_750_860 || defined UPAC_7186E || \
    defined UPAC_5000 ) && \
    !defined DRIVER
#error You must define type!
#endif

#if ( defined PAC_PC || defined PAC_WAGO_750_860 || defined UPAC_7186E || \
    defined UPAC_5000 ) && \
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

enum ALARM_TYPE
    {
    AT_DISCRETE,
    AT_VALUE,
    AT_DEVIATION,
    AT_RATE_OF_CHANGE,
    AT_SPECIAL,
    };

enum ALARM_CLASS_PRIORITY
    {
    P_FATAL         = 0,
    P_ERR_CONNECTION = 100,
    P_ALARM          = 250,
    P_MESSAGE        = 500,
    P_ANSWER         = 750,
    P_REMIND         = 1000,
    };

#endif // defined PAC || defined WIN32

//-----------------------------------------------------------------------------
#ifdef PAC
#include "param_ex.h"
#include "smart_ptr.h"
#include "PAC_dev.h"

#include "tech_def.h"

//-----------------------------------------------------------------------------
/// @brief Базовый класс с информацией об ошибке устройства.
///
class base_error
    {
    public:
        base_error();

        virtual ~base_error()
            {
            }

        /// @brief Сохранение ошибки в поток для передачи на сервер.
        ///
        /// @param stream       - поток байт.
        /// @param is_new_state - изменилось ли состояние ошибки
        ///                       (0 - нет, 1 - да).
        ///
        /// @return - количество записанных байт.
        virtual int save_as_Lua_str( char *str, bool &is_new_state ) = 0;

        /// @brief Отладочный вывод содержимого класса в консоль.
        virtual void print() const = 0;

        /// @brief Получение типа объекта.
        virtual unsigned char get_object_type() const = 0;

        /// @brief Получение номера объекта.
        virtual unsigned int get_object_n() const = 0;

        /// @brief Выполнение команды над ошибкой.
        virtual int set_cmd( int cmd, int object_alarm_number ) = 0;

        /// @brief Сброс параметров ошибки в значение по умолчанию (0).
        void reset_errors_params()
            {
            err_par[ P_PARAM_N ] = 0;
            }

    protected:
        saved_params_u_int_4 err_par;

        unsigned char error_state;    ///< Состояние ошибки.

        enum PARAMS  ///< Параметры ошибки, определяют номера битов.
            {
            P_PARAM_N = 1,	  //Номер параметра.

            P_IS_ENABLE = 1,  ///< Блокировка тревоги на этапе проектирования.
            P_IS_INHIBIT = 2, ///< Блокировка тревоги во время работы.
            P_IS_SUPPRESS = 4,///< Подавление тревоги клиентами.
            };

        enum COMMANDS                ///< Константы.
            {

            C_CMD_ACCEPT   = 100,    ///< Подтвердить ошибку.
            C_CMD_SUPPRESS = 200,    ///< Подавить ошибку.
            C_CMD_UNSET_SUPPRESS,    ///< Убрать подавление ошибки.
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

        /// @brief Сохранение ошибки в поток для передачи на сервер.
        ///
        /// @param stream       - поток байт.
        /// @param is_new_state - изменилось ли состояние ошибки
        ///                       (0 - нет, 1 - да).
        ///
        /// @return - количество записанных байт.
        int save_as_Lua_str( char *str, bool &is_new_state );

        /// @brief Отладочный вывод содержимого класса в консоль.
        void print() const;

        /// @brief Получение типа объекта.
        unsigned char get_object_type() const;

        /// @brief Получение номера объекта.
        unsigned int get_object_n() const;

        /// @brief Выполнение команды над ошибкой.
        int set_cmd( int cmd, int object_alarm_number );

    private:
        device          *simple_device; ///< Простое устройство.
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об ошибке сложного устройства (танк,
/// гребенка...).
///
/// У сложного устройства может быть несколько ошибок (сообщение, ответ, ...).
class tech_dev_error: public base_error
    {
    public:
        // Интерфейс base_error.
        tech_dev_error( tech_object* tech_dev ): tech_dev( tech_dev ),
            was_set_cmd( false )
            {
            }

        int save_as_Lua_str( char *str, bool &is_new_state )
            {
            str[ 0 ] = 0;
            static u_int prev_size = 0;

            if ( tech_dev->get_errors().size() != prev_size || was_set_cmd )
                {
                prev_size   = tech_dev->get_errors().size();
                was_set_cmd = false;

                is_new_state = true;
                }

            for ( u_int i = 0; i < tech_dev->get_errors().size(); i++ )
                {

                sprintf( str + strlen( str ), "\t%s\n", "{" );

                sprintf( str + strlen( str ), "\tdescription=\"%s\",\n",
                    tech_dev->get_errors()[ i ]->msg );
                sprintf( str + strlen( str ), "\tgroup=\"%s\",\n",
                    get_group( tech_dev->get_errors()[ i ]->type ) );

                sprintf( str + strlen( str ), "priority=%d%s",
                    get_priority( tech_dev->get_errors()[ i ]->type ), "," );

                sprintf( str + strlen( str ), "state=%d,\n", AS_ALARM );
                sprintf( str + strlen( str ), "type=%d,\n", AT_SPECIAL );

                sprintf( str + strlen( str ), "id_n=%d,\n",
                    tech_dev->get_number() );
                sprintf( str + strlen( str ), "id_object_alarm_number=%d,\n",
                    tech_dev->get_errors()[ i ]->n );
                sprintf( str + strlen( str ), "id_type=%d,\n", get_object_type() );

                sprintf( str + strlen( str ), "suppress=false\n" );

                sprintf( str + strlen( str ), "},\n" );
                }

            return 0;
            }

        void print() const
            {
#ifdef DEBUG
            Print( "%s\n",
                tech_dev->get_name_in_Lua() );
#endif // DEBUG
            }

        unsigned char get_object_type() const
            {
            return TE_TYPE + tech_dev->get_object_type();
            }

        unsigned int get_object_n() const
            {
            return tech_dev->get_number();
            }

        static const char* get_group( tech_object::ERR_MSG_TYPES err_type )
            {
            switch ( err_type )
                {
                case tech_object::ERR_CANT_ON:
                case tech_object::ERR_ON_WITH_ERRORS:
                    return "ответ";

                case tech_object::ERR_OFF:
                case tech_object::ERR_OFF_AND_ON:
                case tech_object::ERR_DURING_WORK:                
                    return "сообщение";

                case tech_object::ERR_ALARM:
                    return "тревога";
                }

            return "?";
            }

        static int get_priority( tech_object::ERR_MSG_TYPES err_type )
            {
            switch ( err_type )
                {
                case tech_object::ERR_CANT_ON:
                case tech_object::ERR_ON_WITH_ERRORS:
                    return P_ANSWER;

                case tech_object::ERR_OFF:
                case tech_object::ERR_OFF_AND_ON:
                case tech_object::ERR_DURING_WORK:
                    return P_MESSAGE;

                case tech_object::ERR_ALARM:
                    return P_ALARM;
                }

            return P_ALARM;
            }

        // Реализована следующая обработка любой команды - удаление сообщения из
        // вектора, для которого адресована команда.
        int set_cmd( int cmd, int object_alarm_number )
            {
            for ( u_int i = 0; i < tech_dev->get_errors().size(); i++ )
                {
                if( tech_dev->get_errors()[ i ]->n == object_alarm_number )
                    {
                    tech_dev->get_errors().erase(
                        tech_dev->get_errors().begin() + i );

                    was_set_cmd = true;
                    return 0;
                    }
                }
#ifdef DEBUG
            Print( "Object %d not found!\n", object_alarm_number );
#endif // DEBUG

            return 1;
            }

    private:
        tech_object* tech_dev; ///< Сложное устройство.

        enum TECH_DEV_ERROR_CONST   ///< Константы.
            {
            TE_TYPE = 100,          ///< Тип ошибки.
            };

        bool was_set_cmd;
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об всех ошибках простых устройств.
class dev_errors_manager
    {
    public:
        ~dev_errors_manager();

        //int is_any_error() const;

        /// @brief Сохранение всех ошибок в поток для передачи на сервер.
        ///
        /// @param stream - поток байт.
        ///
        /// @return < 0 - ошибка.
        /// @return   0 - ок.
        int save_as_Lua_str( char *str, u_int_2 &id );

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
        u_int_2 errors_id; // Cостояние ошибок.

        /// Единственный экземпляр класса.
        static auto_smart_ptr < dev_errors_manager > instance;

        /// @param s_errors_cnt - количество ошибок.
        dev_errors_manager();

        enum DEM_CONST
            {
            DEM_MAX_ERRORS_CNT = 29,
            };

        std::vector< base_error* > s_errors_vector;    ///< Массив ошибок.
    };
//-----------------------------------------------------------------------------
#define G_DEV_ERRORS_MANAGER dev_errors_manager::get_instance()
//-----------------------------------------------------------------------------
#endif // PAC

#endif //ERRORS_H
