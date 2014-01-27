/// @file rm_manager.h
/// @brief Реализована функциональность менеджера работы с удаленными
/// контроллерами.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG   - компиляция c выводом отладочной информации в консоль.
///
/// @par Текущая версия:
/// @$Rev: 678 $.\n
/// @$Author: id $.\n
/// @$Date:: 2013-05-29 16:44:39#$.

#ifndef RM_MANAGER_H
#define RM_MANAGER_H

#if !defined WIN_OS && !defined LINUX_OS
#error You must define OS!
#endif

#include <vector>

#include "s_types.h"

#include "rm_tech_def.h"
//-----------------------------------------------------------------------------
class rm_manager
    {
    public:
        /// @brief Обмен данными от PAC.
        int evaluate();

        /// <summary>
        /// Добавление удаленного PAC для управления.
        /// </summary>
        /// <param name="name">Имя удаленного PAC.</param>
        /// <param name="IP_address">IP-адрес удаленного PAC.</param>
        /// <returns></returns>
        void add_rm_cmmctr( char* name, char* IP_address, int remote_PAC_id );

        static rm_manager* get_instance()
            {
            return instance;
            }

        void print() const;

    private:
        rm_manager(): cmmctrs( std::vector< i_rm_cmmctr* >() )
            {                      
            }

        static rm_manager* instance;

        std::vector< i_rm_cmmctr* > cmmctrs;
    };
//-----------------------------------------------------------------------------
rm_manager* G_RM_MANAGER();

#endif // RM_MANAGER_H
