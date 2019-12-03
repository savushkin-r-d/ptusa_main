/// @file prj_mngr.h
/// @brief ������ � ��������� ������������ �������, ������������� �������� � 
/// �.�.
/// 
/// @author  ������ ������� ���������.
///
/// @par ������� ������:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PRJ_MANAGER_H
#define PRJ_MANAGER_H

#include "smart_ptr.h"
#include "base_mem.h"

//-----------------------------------------------------------------------------
/// @brief ������������� �������.
///
/// 
class project_manager
    {
    public:        
        /// @brief ��������� ���������� ��������� ������.
        /// 
        /// @param argc - ���������� ����������.
        /// @param argv - ������ ����������.
        int proc_main_params( int argc, const char *argv[] );

        /// @brief �������� ��������� ������������ ������� �� ������ �������.
        /// 
        /// ��������� ������������ ���� �������������� ��������� �� �����
        /// wago.main.plua.
        int lua_load_configuration();

        /// @brief ��������� ������������� ���������� ������.
        static project_manager* get_instance();

        virtual ~project_manager();

        char* sys_path = 0;   //���� � ��������� �������� Lua.
        char* path = 0;       //���� � ����������� ������ �������� Lua.

    protected:
        file *cfg_file;     ///< ���������������� ����.

    private:
        /// @brief ������������ ��������� ������.
        static auto_smart_ptr < project_manager > instance;
    };
//-----------------------------------------------------------------------------
#define G_PROJECT_MANAGER project_manager::get_instance()
//-----------------------------------------------------------------------------
#endif // PRJ_MANAGER_H
