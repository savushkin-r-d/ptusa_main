/// @file prj_mngr.h
/// @brief Работа с загрузкой конфигурации проекта, инициализация объектов и 
/// т.д.
/// 
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Текущая версия:
/// @$Rev: 54 $.\n
/// @$Author: id@BMK $.\n
/// @$Date:: 2010-04-26 15:27:20#$.

#ifndef PRJ_MANAGER_H
#define PRJ_MANAGER_H

#include "sys.h"
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
        int proc_main_params( int argc, char *argv[] );

        /// @brief Загрузка конфигурации проекта.
        /// 
        /// @param file_name - имя конфигурационного файла проекта.
        int load_configuration( const char *file_name );

        /// @brief Получение единственного экземпляра класса.
        static project_manager* get_instance();

        /// @brief Установка единственного экземпляра класса.
        static void set_instance( project_manager* new_instance );

    protected:
        file *cfg_file;     ///< Конфигурационный файл.

    private:
        static project_manager* instance;   ///< Единственный экземпляр класса.
    };
//-----------------------------------------------------------------------------
#define G_PROJECT_MANAGER project_manager::get_instance()
//-----------------------------------------------------------------------------
#endif // PRJ_MANAGER_H
