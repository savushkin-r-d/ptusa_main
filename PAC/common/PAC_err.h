/// @file PAC_err.h
/// @brief Содержит описания классов, которые используются для организации 
/// работы с критическими ошибками PAC.
/// 
/// Класс @ref PAC_critical_errors_manager, служащий для организации работы с 
/// ошибками простых устройств, содержит всю необходимую информацию. Для сбора 
/// всех ошибок служит структура @ref PAC_critical_errors_manager::critical_error. 
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c PAC_ERRORS_H - включение данного файла в компиляцию только один раз.@n
/// 
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PAC_ERRORS_H
#define PAC_ERRORS_H

#include <string.h>
#include <vector>

#include "errors.h"
#include "smart_ptr.h"
#include "dtime.h"
#include "led.h"

//-----------------------------------------------------------------------------


//0, 2, n - нет связи с панелью номер n.
//0, 3, n - нет связи с Modbus устройством номер n.
//0, 4, 4 - нет связи с контролирующим PAC.
//0, 5, 5 - нет связи с сервером.
//
//1, 1 - ошибка работы с COM-портом WAGO. 
//      1 - CRC error
//
//13 - ошибки времени работы:
//  1, n  - нажата аварийная кнопка с номером обратной связи n. 
//-----------------------------------------------------------------------------
class PAC_critical_errors_manager
    {
    public:
        enum GE_CONST
            {
            GE_ERROR_SIZE = 3,      ///< Размер одной ошибки, байт.
            };

        PAC_critical_errors_manager();        

        void show_errors();
        void set_global_error( ALARM_CLASS eclass, ALARM_SUBCLASS p1, 
            unsigned long param );
        void reset_global_error( ALARM_CLASS eclass, ALARM_SUBCLASS p1, 
            unsigned long param );

        int save_as_Lua_str( char *str, u_int_2 &id );

        static PAC_critical_errors_manager * get_instance();

        u_int get_id() const
            {
            return errors_id;
            }

    private:
        static auto_smart_ptr < PAC_critical_errors_manager > instance;

        struct critical_error
            {
            int             err_class;     ///< Класс ошибки.
            unsigned int    err_sub_class; ///< Подкласс ошибки.
            unsigned int    param;         ///< Параметр ошибки.

            critical_error( int err_class = 0, u_int err_sub_class = 0,
                u_int param = 0 );
            };

        std::vector< critical_error >  errors;

        u_int_2 errors_id;
        int     global_ok;
    };
//-----------------------------------------------------------------------------
#endif // PAC_ERRORS_H
