/// @file linux\PAC_PC\wago_PC.h
/// @brief ������ � Wago ��� PC.
/// 
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
/// @c DEBUG   - ���������� c ������� ���������� ���������� � �������.
/// 
/// @par ������� ������:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef IO_PC_H
#define IO_PC_H

#include "bus_coupler_io.h"

//-----------------------------------------------------------------------------
/// @brief ������ � �������� �����/������ Wago ��� PC.
///
///
class io_manager_PC : public io_manager
    {
    public:
        io_manager_PC();

        virtual ~io_manager_PC();

        int read_inputs();
        int write_outputs();
    };
//-----------------------------------------------------------------------------
#endif // IO_PC_H


