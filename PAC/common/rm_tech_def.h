/// @file rm_tech_def.h
/// @brief ����������� ���������������� ��������������� �������� ��� ����������
/// �������� (����� ������-����������).
///
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
/// @c DEBUG   - ���������� c ������� ���������� ���������� � �������.
///
/// @par ������� ������:
/// @$Rev: 678 $.\n
/// @$Author: id $.\n
/// @$Date:: 2013-05-29 16:44:39#$.

#ifndef RM_TECH_DEFINITION_H
#define RM_TECH_DEFINITION_H

#include <vector>

#include "s_types.h"
#include "PAC_err.h"

//-----------------------------------------------------------------------------
/// @brief ������ � ��������������� ��������.
///
/// ������� ����� ��� ���������������� ������� (�����, ��������). ��������
/// �������� ������ ������ - ������ � �������� � �.�.
/// ��� �������������� ������� (�������, ������, ...) ������������ ���������
/// ������: ������� ������ �� ��������� � ������ ������ � ���.
///
class rm_tech_object
    {
    public:
        /// @brief ���������/���������� ������.
        ///
        /// @param mode      - �����.
        /// @param new_state - ����� ��������� ������.
        ///
        /// @return  0 - ����� ������� (��������).
        /// @return  1 - ����� ����� ��� ������� (��������).
        /// @return  3 - ��� ������ ������.
        int set_mode( u_int mode, int new_state, char* err_info );

        /// @brief �������� ����������� ��������� ������.
        ///
        /// ���� ������ ����� ���������� 1, �� ����� ����� �� ����������.
        ///
        /// @param mode - �����.
        ///
        /// @return 1 - ����� ������ ��������.
        /// @return 0 - ����� ����� ��������.
        int check_on_mode( u_int mode, char* err_info );

        private:
            /// @brief ��� �������.
            std::string name;
    };
//-----------------------------------------------------------------------------
class i_rm_cmmctr
    {
    public:
        i_rm_cmmctr( char* name, char* IP_address, int remote_PAC_id ):
            name( name ), IP_address( IP_address ), remote_PAC_id( remote_PAC_id )
            {
            }

        virtual ~i_rm_cmmctr()
            {
            }

        /// @brief ������� �������� ������� PAC.
        virtual int send_2_PAC( int service_n, const char *cmd_str,
            int length  ) = 0;

        /// @brief ����� ������� �� PAC.
        virtual int evaluate() = 0;

        void print() const
            {
            Print( "\"%s\" - \"%s\" (%d)\n",
                name.c_str(), IP_address.c_str(), remote_PAC_id );
            }

    protected:
        char* get_out_data( unsigned int &cnt )
            {
            cnt = answer_size;
            return Lua_str;
            }

        enum PARAMS
            {
            P_MAX_BUFFER_SIZE = 20*1024,
            };

        enum SERVICE_IDS
            {
            PAC_CMMCTR_SERVICE_ID = 1,
            };

        /// @brief ����� ��� ������ ������� � ������������.
        char in_buff[ P_MAX_BUFFER_SIZE ];

        /// @brief ��������� � ������ �� ������ ������.
        char *Lua_str;

        std::string name;
        std::string IP_address;

        // @brief ����� ���������� PAC.
        int remote_PAC_id;

        // @brief ������ ������ ���������� PAC.
        u_int answer_size;
    };
#endif // RM_TECH_DEFINITION_H
