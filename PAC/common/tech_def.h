/// @file tech_def.h
/// @brief ����������� ������� ���������������� ��������� ��������, �����������
/// ��������������� ������� - ���� (��������������� �������),
/// �������� (������������ ��������) � �.�.
///
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
///
/// @par ������� ������:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef TECH_DEFINITION_H
#define TECH_DEFINITION_H

#include <stdio.h>
#include <vector>
#include <locale>

#include "i_tech_def.h"

#include "g_device.h"
#include "PAC_dev.h"

#include "tcp_cmctr.h"
#include "param_ex.h"

#include "mode_mngr.h"

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
/// @brief ������ � ��������������� ��������.
///
/// ������� ����� ��� ���������������� ������� (�����, ��������). ��������
/// �������� ������ ������ - ������ � �������� � �.�.
/// ��� �������������� ������� (�������, ������, ...) ������������ ���������
/// ������: ������� ������ �� ��������� � ������ ������ � ���.
///
class tech_object: public i_tech_object, public i_Lua_save_device
    {
    public:
        /// @param name                     - �������� ("��������", ...).
        /// @param number                   - �����.
        /// @param type                     - ���.
        /// @param name_Lua                 - ��� � Lua.
        /// @param states_count             - ���������� �������.
        /// @param timers_count             - ���������� ��������.
        /// @param par_float_count          - ���������� ����������� ���������� ���� float.
        /// @param runtime_par_float_count  - ���������� ������� ���������� ���� float.
        /// @param par_uint_count           - ���������� ����������� ���������� ���� uint.
        /// @param runtime_par_uint_count   - ���������� ������� ���������� ���� uint.
        tech_object( const char* name, u_int number, u_int type, const char* name_Lua,
            u_int states_count,
            u_int timers_count,
            u_int par_float_count, u_int runtime_par_float_count,
            u_int par_uint_count, u_int runtime_par_uint_count );

        virtual ~tech_object();

        /// @brief ���������/���������� ������.
        ///
        /// @param mode      - �����.
        /// @param new_state - ����� ��������� ������.
        ///
        /// @return  0 - ����� ������� (��������).
        /// @return  1 - ����� ����� ��� ������� (��������).
        /// @return  3 - ��� ������ ������.
        int set_mode( u_int mode, int new_state );

        /// @brief ��������� ��������� ������.
        ///
        /// @param mode - �����.
        ///
        /// @return 1 - ����� �������.
        /// @return 0 - ����� �� �������.
        int get_mode( u_int mode );

        /// @brief ��������� ��������� ��������.
        ///
        /// @param mode - �����.
        ///
        /// @return ... - ����� � ...
        /// @return 2 - ����� � �����.
        /// @return 1 - ����� �������.
        /// @return 0 - ����� �� �������.
        int get_operation_state( u_int mode );

        virtual int  evaluate();

        /// @brief �������� ����������� ��������� ������.
        ///
        /// ���� ������ ����� ���������� 1, �� ����� ����� �� ����������.
        ///
        /// @param mode - �����.
        ///
        /// @return 1 - ����� ������ ��������.
        /// @return 0 - ����� ����� ��������.
        int check_on_mode( u_int mode, char* reason );

        /// @brief �������� ����������� ���������� ������.
        ///
        /// @param mode - �����.
        ///
        /// @return 1 - ����� ������ ���������.
        /// @return 0 - ����� ����� ���������.
        int check_off_mode( u_int mode );

        /// @brief ���������� ������.
        ///
        /// ��� ���������� ������ ���������� ������ �������� -
        /// ���������� ��������, ���������/���������� �������� � �.�.
        ///
        /// @param mode - �����.
        ///
        /// @return 0 - ��.
        int final_mode( u_int mode );

        /// @brief �������������� ����������� ��������� ��������� 0.
        ///
        /// ������ ������� �������� ����������� � ����������������� ������.
        int init_params();

        /// @brief �������������� ������� ��������� ��������� 0.
        int init_runtime_params();

        /// @brief ���������� �������.
        ///
        /// ����� ����� ����������� �����-���� �������� (����������/�����������
        /// ������ ������).
        ///
        int exec_cmd( u_int cmd )
            {
            if ( G_DEBUG ) 
                {
                printf ( "\'%.40s\' - exec command command = %2d\n",
                    name, cmd );
                }
            return 0;
            }

        /// @brief ������ ���������� ������������� �������.
        ///
        bool is_idle() const;

        /// @brief ������� �� ���� �� ���� ������ ����� ���������������� �������.
        bool is_any_important_mode();

        int get_number() const
            {
            return number;
            }

        u_int get_modes_count() const
            {
            return operations_count;
            }

        /// @brief �������� �� ����������� ��������� ��������.
        ///
        int check_operation_on( u_int operation_n, bool show_error = true );

        saved_params_float      par_float;      ///< ����������� ���������, ��� float.
        run_time_params_float   rt_par_float;   ///< ������� ���������, ��� float.
        saved_params_u_int_4    par_uint;       ///< ����������� ���������, ��� u_int.
        run_time_params_u_int_4 rt_par_uint;    ///< ������� ���������, ��� u_int.

        timer_manager           timers;         ///< ������� �������.

        // Lua implemented methods.
        int lua_exec_cmd( u_int cmd );

        int  lua_check_on_mode( u_int mode, bool show_error = true );
        void lua_init_mode( u_int mode );
        
        int  lua_check_off_mode( u_int mode );
        int  lua_final_mode( u_int mode );
        int  lua_init_params();
        int  lua_init_runtime_params();

        int lua_on_start( u_int mode );
        int lua_on_pause( u_int mode );
        int lua_on_stop( u_int mode );
        // Lua implemented methods.


        // ������ � ��������.

        /// @brief �������� ������������� �������� ��������� �� ������ ��������
        /// ����� ����� ���������� ������.  ���������� �� Lua.
        ///
        /// @return 0 - �� ���� ���������.
        /// @return 1 - ���� ���������.
        virtual int is_check_mode( int mode ) const;

        enum ERR_MSG_TYPES
            {
            ERR_CANT_ON,
            ERR_ON_WITH_ERRORS,
            ERR_OFF,
            ERR_OFF_AND_ON,
            ERR_DURING_WORK,
            ERR_ALARM,

            ERR_TO_FAIL_STATE,
            ERR_CANT_ON_2_OPER, //��� �������� ����������� ��������.
            ERR_CANT_ON_2_OBJ, //��� �������� ����������� �������� ������� �������.
            };

        struct  err_info
            {
            enum CONSTATS
                {
                MAX_STR_LENGTH = 150,
                };

            char          msg[ MAX_STR_LENGTH ];
            int           n;
            ERR_MSG_TYPES type;
            };

        int set_err_msg( const char *err_msg, int mode, int new_mode = 0,
            ERR_MSG_TYPES type = ERR_CANT_ON );

        static const char* get_type( ERR_MSG_TYPES err_type );
        static int get_priority( ERR_MSG_TYPES err_type );

         std::vector< err_info* >& get_errors()
             {
             return errors;
             }

         int get_object_type()
             {
             return type;
             }
        // ������ � ��������.

        /// @brief ���������� ������ �������.
        void print() const
            {
            printf( "Object \'%.40s\' [%d], type %d\n", name, number, type );
            printf( "par_float[%d] par_uint[%d] rt_par_float[%d] rt_par_uint[%d]\n",
                par_float.get_count(), par_uint.get_count(),
                rt_par_float.get_count(), rt_par_uint.get_count() );
            printf( "timers[%d]\n", timers.get_count() );

            par_float.print();
            par_uint.print();
            rt_par_float.print();
            rt_par_uint.print();

            operations_manager->print();
            }

        const char* get_name_in_Lua() const
            {
            static char tmp[ 100 ];
            sprintf( tmp, "%.40s", name_Lua );
            return tmp;
            }

        const char* get_name() const
            {
            return name;
            }

        //��������� ���������� ��� ����. ��� ������� �������� ���������� 
        //��������� par_uint, ��� ������ �������� ������ ��� ����������
        //par_float. 
        float get_step_param( u_int idx ) const
            {
            if ( idx >= 1 )
                {
                if ( par_uint.get_count() >= idx )
                    {
                    return (float)par_uint[ idx ];
                    }
                else
                    {
                    if ( par_float.get_count() >= idx )
                        {
                        return par_float[ idx ];
                        }
                    }
                }
                
            return -1.;
            }

        const saved_params_float* get_params() const
            {
            return &par_float;
            }

        operation_manager* get_modes_manager()
            {
            return operations_manager;
            }

        virtual int save_device( char *buff );

        virtual int set_cmd( const char *prop, u_int idx, double val );

        int save_params_as_Lua_str( char* str );

        int set_param( int par_id, int index, double value );

#ifdef RM_PAC
        int rm_save_device_state( char *buff )
            {
            return 0;
            }
#endif // RM_PAC

        /// @brief ��������� ����������������� ������ �������, ���������� � 1.
        ///
        /// @param idx - ���������������� �����, >= 1.
        void set_serial_idx( u_int idx );

    protected:
        u_int serial_idx;           ///< ���������������� ������ ������� (� 1).

        u_int   number;             ///< ����� �������.
        u_int   type;               ///< ��� ������� (��� ������).
        u_int_4 cmd;                ///< �������� ������� �������.
        u_int   operations_count;   ///< ���������� ��������.

        std::vector< u_int_4 >  state;  ///< ��������� ������� (�������).
        run_time_params_u_int_4 modes_time;    ///< ����� ��������, ���.

        std::vector< u_int_4 >  available;     ///< ����������� ��������.

        enum CONSTANTS
            {
            C_MAX_NAME_LENGTH = 50,
            };
        char name[ C_MAX_NAME_LENGTH + 1 ];        ///< ��� �������.
        char name_Lua[ C_MAX_NAME_LENGTH + 1 ];    ///< ��� ������� � Lua.

        smart_ptr< operation_manager > operations_manager; ///< ���� �������.

        enum PARAMS_ID
            {
            ID_PAR_FLOAT = 1,
            ID_RT_PAR_FLOAT,
            ID_PAR_UINT,
            ID_RT_PAR_UINT,
            };

        // ������ � ��������.
        enum ERRORS_CONSTANTS
            {
            E_MAX_ERRORS_SIZE = 10,
            };

        std::vector< err_info* > errors;
        // ������ � ��������.

        //�������� �� ����������� ��������.
        void check_availability( u_int operation_n );

        int set_extra_step( u_int operation, int step, int cmd );
    };
//-----------------------------------------------------------------------------
class tech_object_manager
    {
    public:
        /// @brief ������������� ����������� ���������� ���� ���������������
        /// ��������.
        int init_params();

        /// @brief ������������� ������� ���������� ���� ���������������
        /// ��������.
        int init_runtime_params();

        /// @brief ��������� ������������� ���������� ������.
        static tech_object_manager* get_instance();

        /// @brief ��������� ������� � �������� �������� �������.
        int get_object_with_active_mode( u_int mode, u_int start_idx,
            u_int end_idx );

        /// @brief ��������� ������� �� ������.
        tech_object* get_tech_objects( u_int idx );

        /// @brief ��������� ���������� ��������.
        u_int get_count() const;

        /// @brief ���������� �������� ���������������� ��������.
        void evaluate();

        /// @brief ������������� �������� �� ������ ������� �������� Lua.
        int init_objects();

        ~tech_object_manager();

        /// @brief ���������� ���������������� �������.
        void add_tech_object( tech_object* new_tech_object );

        int save_params_as_Lua_str( char* str );

        /// @brief ������� �� ���� �� ���� ������ ����� ���������������� �������.
        bool is_any_important_mode()
            {
            for ( u_int i = 0; i < tech_objects.size(); i++ )
                {
                if ( tech_objects[ i ]->is_any_important_mode() )
                	{
                    return true;
                	}
                }

            return false;
            }

#ifdef __BORLANDC__
#pragma option -w-inl
#endif // __BORLANDC__

        /// @brief ���������� ������ �������.
        void print()
            {
            printf( "Technological objects manager [%d]:\n",
                tech_objects.size() );

            for ( u_int i = 0; i < tech_objects.size(); i++ )
                {
                tech_objects[ i ]->print();

#ifdef KEY_CONFIRM
                printf( "Press any key to continue..." );
                Getch();
                printf( "\n" );
#endif // KEY_CONFIRM
                }
            }

#ifdef __BORLANDC__
#pragma option -w.inl
#endif // __BORLANDC__

    private:
        tech_object_manager();

        /// ������������ ��������� ������.
        static auto_smart_ptr < tech_object_manager > instance;

        std::vector< tech_object* > tech_objects; ///< ��������������� �������.

        tech_object* stub;
    };
//-----------------------------------------------------------------------------
tech_object_manager* G_TECH_OBJECT_MNGR();
//-----------------------------------------------------------------------------
tech_object* G_TECH_OBJECTS( u_int idx );
//-----------------------------------------------------------------------------
#endif // TECH_DEFINITION_H
