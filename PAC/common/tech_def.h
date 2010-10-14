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
/// @$Rev: 96 $.\n
/// @$Author: id@BMK $.\n
/// @$Date:: 2010-07-09 15:57:50#$.

#ifndef TECH_DEFINITION_H
#define TECH_DEFINITION_H

#include <stdio.h>
#include <vector>

#include "sys.h"
#include "g_device.h"
#include "PAC_dev.h"
#include "PAC_err.h"

#include "tcp_cmctr.h"
#include "param_ex.h"

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
class tech_object
    {
    public:
        /// @param name                     - название (tank, comb).
        /// @param number                   - номер.
        /// @param states_count             - количество режимов.
        /// @param timers_count             - количество таймеров.
        /// @param par_float_count          - количество сохраняемых параметров типа float.
        /// @param runtime_par_float_count  - количество рабочих параметров типа float.
        /// @param par_uint_count           - количество сохраняемых параметров типа uint.
        /// @param runtime_par_uint_count   - количество рабочих параметров типа uint.
        tech_object( const char* name = "tech_object", u_int number = 0,
            u_int states_count = 1,
            u_int timers_count = 3,
            u_int par_float_count = 5, u_int runtime_par_float_count = 5,
            u_int par_uint_count = 5, u_int runtime_par_uint_count = 5 );

        virtual ~tech_object();

        /// @brief Включение/выключение режима.
        ///
        /// @param mode      - режим.
        /// @param new_state - новое состояние режима.
        int set_mode( u_int mode, int new_state );

        /// @brief Получение состояния режима.
        ///
        /// @param mode - режим.
        ///
        /// @return 1 - режим включен.
        /// @return 0 - режим не включен.
        int get_mode( u_int mode );

        /// @brief Проверка возможности включения режима.
        ///
        /// Если данный метод возвращает 1, то тогда режим не включается.
        ///
        /// @param mode - режим.
        ///
        /// @return 1 - режим нельзя включить.
        /// @return 0 - режим можно включить.
        int check_on_mode( u_int mode );

        /// @brief Выполнение команды.
        ///
        /// Здесь могут выполняться какие-либо действия (включаться/выключаться
        /// другие режимы).
        ///
        int exec_cmd( u_int cmd )
            {
#ifdef DEBUG
            Print ( "Exec command %s[ %2d ] command = %2d\n",
                com_dev->get_name(), number, cmd );
#endif
            return 0;
            }

        /// @brief Инициализация режима.
        ///
        /// При инициализации режима выполнение нужных действий -
        /// включение маршрута, включение/выключение клапанов и т.д.
        ///
        /// @param mode - режим.
        void init_mode( u_int mode );

        /// @brief Выполнение включенных режимов.
        ///
        /// При активном режиме выполнение нужных действий -
        /// обновление маршрута, включение/выключение клапанов и т.д.
        ///
        /// @return 0 - ок.
        int evaluate();

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

        complex_device* get_complex_dev()
            {
            return com_dev;
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

        //--Lua implemented methods.
        int lua_exec_cmd( u_int cmd );

        int  lua_check_on_mode( u_int mode );
        void lua_init_mode( u_int mode );
        int  lua_evaluate();
        int  lua_check_off_mode( u_int mode );
        int  lua_final_mode( u_int mode );
        int  lua_init_params();
        int  lua_init_runtime_params();
        //--Lua implemented methods.--!>

    protected:
        smart_ptr< complex_device > com_dev; ///< Связь с сервером.

        u_int   number;         ///< Номер объекта.
        u_int_4 cmd;            ///< Хранение команды объекта.
        u_int   modes_count;    ///< Количество режимов.

        std::vector< u_int_4 >  state;  ///< Состояние объекта.

        std::vector< u_int_4 >  mode_start_time;    ///< Время начала режима.
        run_time_params_u_int_4 mode_time;          ///< Время режимов, сек.

        enum CONSTANTS
            {
            C_MAX_NAME_LENGTH = 30,
            };
        char name[ C_MAX_NAME_LENGTH ];    ///< Имя объекта.
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

        /// @brief Установка единственного экземпляра класса.
        static int set_instance( tech_object_manager* new_instance );

        /// @brief Получение объекта с заданным активным режимом.
        int get_object_with_active_mode( u_int mode, u_int start_idx,
            u_int end_idx );

        /// @brief Получение объекта по номеру.
        tech_object* get_tech_objects( u_int idx );

        /// @brief Получение количества объектов.
        u_int get_count() const;

        /// @brief Выполнение итерации технологического процесса.
        void evaluate();

        int init_objects();

        ~tech_object_manager();

        /// @brief Добавление технологического объекта.
        void add_tech_object( tech_object* new_tech_object );

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
