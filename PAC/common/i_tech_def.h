/// @file i_tech_def.h
/// @brief ����������� ��������� ��������� ��������, ����������� ���������������
/// ������� - ���� (��������������� �������), �������� (������������ ��������)
/// � �.�.
///
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
///
/// @par ������� ������:
/// @$Rev: 868 $.\n
/// @$Author: id $.\n
/// @$Date:: 2013-12-26 10:29:25#$.
#ifndef I_TECH_H
#define I_TECH_H

#include "s_types.h"
#include "param_ex.h"
//-----------------------------------------------------------------------------
/// @brief ������ � ��������������� ��������.
///
/// ������� ������������ ����� ��� ���������������� ������� (�����, ��������). 
/// �������� �������� ������ ������ - ������ � �������� � �.�.
///
class i_tech_object
    {
#ifndef __GNUC__
#pragma region ������� ���������� ������ � ������ ����������� �������.
#endif

    public:
        static const char* get_prefix()
            {
            return white_spaces;
            }

    protected:
        inline static char white_spaces[ 256 ] = "";

#ifndef __GNUC__
#pragma endregion
#endif

    public:
    /// @brief ���������/���������� ������.
    ///
    /// @param mode      - �����.
    /// @param new_state - ����� ��������� ������.
    ///
    /// @return  0 - ����� ������� (��������).
    /// @return  1 - ����� ����� ��� ������� (��������).
    /// @return  3 - ��� ������ ������.
    virtual int set_mode( u_int mode, int new_state ) = 0;

    virtual const char* get_name() const = 0;

    virtual float get_step_param( u_int idx ) const = 0;

    virtual const saved_params_float* get_params() const = 0;
    };

#endif // I_TECH_H
