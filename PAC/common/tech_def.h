/// @file tech_def.h
/// @brief Реализована базовая функциональность следующих объектов, описывающих
/// технологический процесс - танк (технологическая емкость),
/// гребенка (совокупность клапанов) и т.д.
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

#ifndef TECH_DEFINITION_H
#define TECH_DEFINITION_H

#include <stdio.h>
#include <vector>
#include <locale>

#ifdef __BORLANDC__
extern "C" {
extern int snprintf(char *, size_t, const char *, /*args*/ ...);
    };
#endif // __BORLANDC__

#include "g_device.h"
#include "PAC_dev.h"

#include "tcp_cmctr.h"
#include "param_ex.h"

#include "mode_mngr.h"

#ifdef  __cplusplus
extern "C" {
#endif

#include    "lua.h"
#include    "lauxlib.h"
#include    "lualib.h"

#ifdef  __cplusplus
    };
#endif
#include    "tolua++.h"

//-----------------------------------------------------------------------------
/// @brief Работа с технологическим объектом.
///
/// Базовый класс для технологического объекта (танка, гребенки). Содержит
/// основные методы работы - работа с режимами и т.д.
/// Для информировании сервера (события, аварии, ...) используется следующий 
/// подход: имеется вектор из сообщений и методы работы с ним.
///
class tech_object: public i_Lua_save_device
    {
    public:
        /// @param name                     - название ("Гребенка" ...).
        /// @param number                   - номер.
        /// @param name_Lua                 - имя в Lua.
        /// @param states_count             - количество режимов.
        /// @param timers_count             - количество таймеров.
        /// @param par_float_count          - количество сохраняемых параметров типа float.
        /// @param runtime_par_float_count  - количество рабочих параметров типа float.
        /// @param par_uint_count           - количество сохраняемых параметров типа uint.
        /// @param runtime_par_uint_count   - количество рабочих параметров типа uint.
        tech_object( const char* name, u_int number, const char* name_Lua,
            u_int states_count,
            u_int timers_count,
            u_int par_float_count, u_int runtime_par_float_count,
            u_int par_uint_count, u_int runtime_par_uint_count );

        virtual ~tech_object();

        /// @brief Включение/выключение режима.
        ///
        /// @param mode      - режим.
        /// @param new_state - новое состояние режима.
        ///
        /// @return  0 - режим включен (отключен).
        /// @return  1 - режим ранее был включен (отключен).
        /// @return  3 - нет такого режима.
        int set_mode( u_int mode, int new_state );

        /// @brief Получение состояния режима.
        ///
        /// @param mode - режим.
        ///
        /// @return 1 - режим включен.
        /// @return 0 - режим не включен.
        int get_mode( u_int mode );

        int  evaluate();

        /// @brief Проверка возможности включения режима.
        ///
        /// Если данный метод возвращает 1, то тогда режим не включается.
        ///
        /// @param mode - режим.
        ///
        /// @return 1 - режим нельзя включить.
        /// @return 0 - режим можно включить.
        int check_on_mode( u_int mode, char* reason );

        /// @brief Инициализация режима.
        ///
        /// При инициализации режима выполнение нужных действий -
        /// включение маршрута, включение/выключение клапанов и т.д.
        ///
        /// @param mode - режим.
        void init_mode( u_int mode );

        /// @brief Проверка возможности выключения режима.
        ///
        /// @param mode - режим.
        ///
        /// @return 1 - режим нельзя выключить.
        /// @return 0 - режим можно выключить.
        int check_off_mode( u_int mode );

        /// @brief Завершение режима.
        ///
        /// При завершении режима выполнение нужных действий -
        /// выключение маршрута, включение/выключение клапанов и т.д.
        ///
        /// @param mode - режим.
        ///
        /// @return 0 - ок.
        int final_mode( u_int mode );

        /// @brief Инициализирует сохраняемые параметры значением 0.
        ///
        /// Данные нулевые значения сохраняются в энергонезависимой памяти.
        int init_params();

        /// @brief Инициализирует рабочие параметры значением 0.
        int init_runtime_params();

        /// @brief Выполнение команды.
        ///
        /// Здесь могут выполняться какие-либо действия (включаться/выключаться
        /// другие режимы).
        ///
        int exec_cmd( u_int cmd )
            {
#ifdef DEBUG
            Print ( "\'%.40s\' - exec command command = %2d\n",
                name, cmd );
#endif
            return 0;
            }

        int get_number() const
            {
            return number;
            }

        u_int get_modes_count() const
            {
            return modes_count;
            }

        saved_params_float      par_float;      ///< Сохраняемые параметры, тип float.
        run_time_params_float   rt_par_float;   ///< Рабочие параметры, тип float.
        saved_params_u_int_4    par_uint;       ///< Сохраняемые параметры, тип u_int.
        run_time_params_u_int_4 rt_par_uint;    ///< Рабочие параметры, тип u_int.

        timer_manager           timers;         ///< Таймеры объекта.

        // Lua implemented methods.
        int lua_exec_cmd( u_int cmd );

        int  lua_check_on_mode( u_int mode );
        void lua_init_mode( u_int mode );       
        int  lua_check_off_mode( u_int mode );
        int  lua_final_mode( u_int mode );
        int  lua_init_params();
        int  lua_init_runtime_params();
        // Lua implemented methods.

        
        // Работа с ошибками.

        /// @brief Проверка необходимости проверки устройств на ошибку обратной
        /// связи перед включением режима.
        ///
        /// @return 0 - не надо проверять.
        /// @return 1 - надо проверять.
        virtual int is_check_mode( int mode ) const;

        enum ERR_MSG_TYPES
            {
            ERR_CANT_ON,
            ERR_ON_WITH_ERRORS,
            ERR_OFF,
            ERR_OFF_AND_ON,
            ERR_DURING_WORK,
            ERR_SIMPLE,
            };

        struct  err_info
            {
            enum CONSTATS
                {
                MAX_STR_LENGTH = 150,
                };

            char          msg[ MAX_STR_LENGTH ];
            int           n;
            ERR_MSG_TYPES type;
            };

        int set_err_msg( const char *err_msg, int mode, int new_mode = 0, 
            ERR_MSG_TYPES type = ERR_CANT_ON );

        static const char* get_type( ERR_MSG_TYPES err_type );
        static int get_priority( ERR_MSG_TYPES err_type );

         std::vector< err_info* >& get_errors()
             {
             return errors;
             }
        // Работа с ошибками.

        /// @brief Отладочная печать объекта.
        void print() const
            {
            Print( "Object \'%.40s\' [%d]\n", name, number );
            modes_manager->print();
            }

        const char* get_name_in_Lua() const
            {
            static char tmp[ 100 ];
            snprintf( tmp, sizeof( tmp ), "%.40s",
                name_Lua );
            return tmp;
            }

        mode_manager* get_modes_manager()
            {
            return modes_manager;
            }

        int save_device( char *buff );

        int set_cmd( const char *prop, u_int idx, double val );

        int save_params_as_Lua_str( char* str );

        int set_param( int par_id, int index, double value );

    protected:
        u_int   number;         ///< Номер объекта.
        u_int_4 cmd;            ///< Хранение команды объекта.
        u_int   modes_count;    ///< Количество режимов.

        std::vector< u_int_4 >  state;  ///< Состояние объекта.

        run_time_params_u_int_4 modes_time;    ///< Время режимов, сек.

        enum CONSTANTS
            {
            C_MAX_NAME_LENGTH = 30,
            };
        char name[ C_MAX_NAME_LENGTH ];        ///< Имя объекта.
        char name_Lua[ C_MAX_NAME_LENGTH ];    ///< Имя объекта в Lua.


        smart_ptr< mode_manager > modes_manager; ///< Шаги режимов.

        enum PARAMS_ID
            {
            ID_PAR_FLOAT = 1,
            ID_RT_PAR_FLOAT,
            ID_PAR_UINT,
            ID_RT_PAR_UINT,            
            };

        // Работа с ошибками.
        std::vector< err_info* > errors;
        // Работа с ошибками.
    };
//-----------------------------------------------------------------------------
class tech_object_manager
    {
    public:
        /// @brief Инициализация сохраняемых параметров всех технологических
        /// объектов.
        int init_params();

        /// @brief Инициализация рабочих параметров всех технологических
        /// объектов.
        int init_runtime_params();

        /// @brief Получение единственного экземпляра класса.
        static tech_object_manager* get_instance();

        /// @brief Получение объекта с заданным активным режимом.
        int get_object_with_active_mode( u_int mode, u_int start_idx,
            u_int end_idx );

        /// @brief Получение объекта по номеру.
        tech_object* get_tech_objects( u_int idx );

        /// @brief Получение количества объектов.
        u_int get_count() const;

        /// @brief Выполнение итерации технологического процесса.
        void evaluate();

        /// @brief Инициализация объектов на основе скрипта описания Lua.
        int init_objects();

        ~tech_object_manager();

        /// @brief Добавление технологического объекта.
        void add_tech_object( tech_object* new_tech_object );

        int save_params_as_Lua_str( char* str );

#ifdef __BORLANDC__
#pragma option -w-inl
#endif // __BORLANDC__

        /// @brief Отладочная печать объекта.
        void print()
            {
            Print( "Technological objects manager [%d]:\n",
                tech_objects.size() );

            for ( u_int i = 0; i < tech_objects.size(); i++ )
                {
                tech_objects[ i ]->print();
                Print( "\n" );

#ifdef KEY_CONFIRM
                Print( "Press any key to continue..." );
                Getch();
                Print( "\n" );
#endif // KEY_CONFIRM
                }
            }       

#ifdef __BORLANDC__
#pragma option -w.inl
#endif // __BORLANDC__

    private:
        /// Единственный экземпляр класса.
        static auto_smart_ptr < tech_object_manager > instance;

        std::vector< tech_object* > tech_objects; ///< Технологические объекты.
    };
//-----------------------------------------------------------------------------
tech_object_manager* G_TECH_OBJECT_MNGR();
//-----------------------------------------------------------------------------
tech_object* G_TECH_OBJECTS( u_int idx );
//-----------------------------------------------------------------------------
#endif // TECH_DEFINITION_H
