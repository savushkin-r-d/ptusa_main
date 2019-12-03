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

#include "string"

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

        /// @brief ��������� ����� � ������ Lua.
        int init_path( const char* path );

        /// @brief ��������� ����� � ��������� ������ Lua.
        int init_sys_path( const char* sys_path );

        std::string sys_path = "";   //���� � ��������� �������� Lua.
        std::string path = "";       //���� � ����������� ������ �������� Lua.

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
