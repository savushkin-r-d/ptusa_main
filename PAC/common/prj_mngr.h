/// @file prj_mngr.h
/// @brief Работа с загрузкой конфигурации проекта, инициализация объектов и 
/// т.д.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PRJ_MANAGER_H
#define PRJ_MANAGER_H

#include "smart_ptr.h"
#include "base_mem.h"

//-----------------------------------------------------------------------------
/// @brief Инициализация проекта.
///
/// 
class project_manager
    {
    public:        
        /// @brief Обработка параметров командной строки.
        /// 
        /// @param argc - количество параметров.
        /// @param argv - массив параметров.
        int proc_main_params( int argc, const char *argv[] );

        /// @brief Загрузка системной конфигурации проекта на основе скрипта.
        /// 
        /// Системная конфигурация была предварительно загружена из файла
        /// wago.main.plua.
        int lua_load_configuration();

        /// @brief Получение единственного экземпляра класса.
        static project_manager* get_instance();

        virtual ~project_manager();

    protected:
        file *cfg_file;     ///< Конфигурационный файл.

    private:
        /// @brief Единственный экземпляр класса.
        static auto_smart_ptr < project_manager > instance;
    };
//-----------------------------------------------------------------------------
#define G_PROJECT_MANAGER project_manager::get_instance()
//-----------------------------------------------------------------------------
#endif // PRJ_MANAGER_H
