/// @file prj_mngr_w750.h
/// @brief Работа с загрузкой конфигурации проекта, инициализация объектов и 
/// т.д. для PAC WAGO 750-860.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Текущая версия:
/// @$Rev: 54 $.\n
/// @$Author: id@BMK $.\n
/// @$Date:: 2010-04-26 15:27:20#$.

#ifndef PRJ_MANAGER_W750_H
#define PRJ_MANAGER_W750_H

#include "prj_mngr.h"
#include "sys_w750.h"
//-----------------------------------------------------------------------------
/// @brief Инициализация проекта.
///
/// 
class project_manager_w750 : public project_manager
    {
    public:
        project_manager_w750();
    };
//-----------------------------------------------------------------------------
#endif // PRJ_MANAGER_W750_H

