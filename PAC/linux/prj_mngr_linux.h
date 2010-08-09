/// @file prj_mngr_linux.h
/// @brief Работа с загрузкой конфигурации проекта, инициализация объектов и 
/// т.д. для PAC с ОС linux.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Текущая версия:
/// @$Rev: 54 $.\n
/// @$Author: id@BMK $.\n
/// @$Date:: 2010-04-26 15:27:20#$.

#ifndef PRJ_MANAGER_LINUX_H
#define PRJ_MANAGER_LINUX_H

#include "prj_mngr.h"
#include "sys_linux.h"
//-----------------------------------------------------------------------------
/// @brief Инициализация проекта.
///
/// 
class project_manager_linux : public project_manager
    {
    public:
        project_manager_linux();
    };
//-----------------------------------------------------------------------------
#endif // PRJ_MANAGER_LINUX_H

