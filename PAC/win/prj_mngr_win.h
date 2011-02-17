/// @file prj_mngr_linux.h
/// @brief Работа с загрузкой конфигурации проекта, инициализация объектов и 
/// т.д. для PAC с ОС linux.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PRJ_MANAGER_WIN_H
#define PRJ_MANAGER_WIN_H

#include "prj_mngr.h"
#include "sys_win.h"
//-----------------------------------------------------------------------------
/// @brief Инициализация проекта.
///
/// 
class project_manager_win : public project_manager
    {
    public:
        project_manager_win();
    };
//-----------------------------------------------------------------------------
#endif // PRJ_MANAGER_LINUX_H

