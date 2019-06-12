/// @file mem_PLCnext.h
/// @brief �������� �������� �������, ������� ��������� ���������� ����� PAC
/// �� ������ � �������.
///
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
///
/// @par ������� ������:
/// @$Rev: 220 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-15 16:58:56#$.

#ifndef MEM_PLCNEXT_H
#define MEM_PLCNEXT_H

#include "base_mem.h"

#include <cstdint>
//-----------------------------------------------------------------------------
/// @brief ������ � ����������������� ��� (Static Memory).
///
/// ����� ����������� �� ���������� ������ ������/������ - 1 �������.
class eeprom_PLCnext: public NV_memory
    {
    friend class NV_memory_manager;

    public:
        virtual ~eeprom_PLCnext();

        void init( void * NV_ram_data )
            {
            this->NV_ram_data = ( std::uint8_t * ) NV_ram_data;
            }

    private:
        eeprom_PLCnext( int total_size, int available_start_pos,
            int available_end_pos );

        /// @brief ����� ���������� @ref i_memory.
        int read( void *buff, u_int count, u_int start_pos );

        /// @brief ����� ���������� @ref i_memory.
        int write( void *buff, u_int count, u_int start_pos );

        std::uint8_t * NV_ram_data; 	// Pointer to begin of NVRAM
    };
//-----------------------------------------------------------------------------
#endif // MEM_PLCNEXT_H
