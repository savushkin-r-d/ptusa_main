/// @file base_mem.h
/// @brief �������� �������� �������, ������� ��������� ���������� ����� PAC -
/// ��������� ������ - �� ����������� ������.
///
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
///
/// @par ������� ������:
/// @$Rev: 220 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-15 16:58:56#$.

#ifndef MEM_H
#define MEM_H

#include "console.h"
#include "s_types.h"

#include "smart_ptr.h"
//-----------------------------------------------------------------------------
/// @brief ��������� ������� � ������.
class i_memory
    {
    public:
        virtual ~i_memory()
            {
            }

        /// @brief ������ ������� ����.
        ///
        /// @param buf       - ����� ������, ���� ����� ����������� ������.
        /// @param count     - ���������� ����������� ����.
        /// @param start_pos - ��������� �����.
        ///
        /// @return <  0 - ������.
        /// @return >= 0 - ���������� ��������� ����.
        virtual int read( void *buf, u_int count, u_int start_pos ) = 0;

        /// @brief ������ ������� ����.
        ///
        /// @param buf       - ����� ������, ���� ����� ������������ ������.
        /// @param count     - ���������� ����������� ����.
        /// @param start_pos - ��������� �����.
        ///
        /// @return <  0 - ������.
        /// @return >= 0 - ���������� ���������� ����.
        virtual int write( void *buf, u_int count, u_int start_pos ) = 0;

        /// @brief ��������� ������� ������ � ������.
        ///
        /// @return - ������ ������ � ������.
        virtual u_int get_size() const = 0;
    };
//-----------------------------------------------------------------------------
///\russian
/// @brief ������ �������� � ����������������� ���. ���������� �� ����������
/// ���������� ������.
///\endif

class NV_memory : public i_memory
    {

    public:
        /// @param total_size           - ����� ������ ������.
        /// @param available_start_pos  - ��������� ��������� �����.
        /// @param available_end_pos    - �������� ��������� �����.
        NV_memory( u_int total_size, u_int available_start_pos,
            u_int available_end_pos );

        /// @brief ����� ���������� @ref i_memory.
        u_int get_size() const
            {
            return total_size;
            }

        /// @brief ��������� ���������� ���������� ��� ������ ������ ������.
        ///
        /// @return - ��������� ��������� ��� ������ ����� ������.
        u_int get_available_start_pos() const
            {
            return available_start_pos;
            }

        /// @brief ��������� ��������� ���������� ��� ������ ������ ������.
        ///
        /// @return - �������� ��������� ��� ������ ����� ������.
        u_int get_available_end_pos() const
            {
            return available_end_pos;
            }

        virtual ~NV_memory()
            {
            }

        virtual void init( void * NV_ram_data ) {}

    private:

        u_int total_size;           ///< ����� ������ ������.

        /// @brief ��������� ��������� �����.
        /// @details ��� �������� ����������������� ��������� �������.
        u_int available_start_pos;

        /// @brief �������� ��������� �����.
        /// @details ��� �������� ����������������� ��������� �������.
        u_int available_end_pos;
    };
//-----------------------------------------------------------------------------
/// @brief ������ � ������ ������.
class memory_range: public i_memory
    {
    friend class NV_memory_manager;

    public:
        /// @brief ����� ���������� @ref i_memory.
        u_int get_size() const
            {
            return size;
            }

        virtual ~memory_range()
            {
            }

        /// @brief ����� ���������� @ref i_memory.
        int read( void *buf, u_int count, u_int start_pos );

        /// @brief ����� ���������� @ref i_memory.
        int write( void *buf, u_int count, u_int start_pos );

    private:
        i_memory    *memory;    ///< ��������� �� ������ ������.
        u_int       start_pos;  ///< ��������� �����.
        u_int       size;       ///< ������ ����� ������ � ������.

        /// @brief �������� �����������.
        ///
        /// �������� �������� �������������� ����� ����� get_memory_block
        /// ������ @ref NV_memory_manager.
        memory_range( i_memory *memory, u_int start_pos, u_int size );

        /// @brief �������� �� ������������ ����������.
        ///
        /// @return - ��������� ��������
        ///    0 - ��.
        ///    1 - ������.
        int check_params( u_int count, u_int start_pos );
    };
//-----------------------------------------------------------------------------
/// @brief ������ � �������� �������� �������. ������������ ���������� ��
/// ���������� ���������� �������.
class file
    {
    public:
        virtual ~file()
            {
            }

        /// @brief �������� �����.
        ///
        /// @param file_name - ��� �����.
        ///
        /// @return - ���������
        ///    0   - ��.
        ///    0 < - ������.
        virtual int file_open( const char *file_name ) = 0;

        /// @brief ������ �� ����� ����� ������.
        ///
        /// @param buffer - �����, ���� ��������� ���������.
        /// @param count - ���������� ����������� ����.
        ///
        /// @return - ���������� ��������� ����.
        virtual int file_read( void *buffer, int count ) = 0;

        /// @brief ������ �� ����� ������.
        ///
        /// @return - ��������� �� ������ ������.
        virtual char* fget_line() = 0;

        /// @brief ������ �� ����� ������ ��� ��������� ������� ��������� �
        /// �����.
        ///
        /// @return - ��������� �� ������ ������.
        virtual char* pfget_line() = 0;

        /// @brief �������� �����.
        ///
        /// @return - ���������
        ///    0   - ��.
        ///    0 < - ������.
        virtual void file_close() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief ������ � ����������������� ���. ���������� �� �����������
/// ������������� ������.
class NV_memory_manager
    {
    public:
        enum MEMORY_TYPE ///< ���� ����������������� ������.
            {
            MT_NVRAM,
            MT_EEPROM
            };

        /// @brief ��������� ������� ��� ������ � ������ ������.
        ///
        /// @param m_type - ��� ������, ������ ����� ��������� ������.
        /// @param count  - ������ � ������.
        ///
        /// @return =  0 - ������.
        /// @return >  0 - ��������� �� ������ ����������� ����� ������.
        virtual memory_range* get_memory_block( MEMORY_TYPE m_type,
            u_int count );

        /// @brief ��������� ������������� ���������� ������ ��� ������ �
        /// �����������.
        ///
        /// @return - ��������� �� ������������ ������ ������ @ref
        /// NV_memory_manager.
        static NV_memory_manager* get_instance();

        virtual ~NV_memory_manager();

        void init_ex( void * par );

    protected:
        /// ����������� ��������� ������ ��� ������ �������.
        static auto_smart_ptr < NV_memory_manager > instance;

        NV_memory_manager();

        /// @brief ������ � ����������������� ��� (non-volatile random access
        /// memory).
        ///
        /// ������ ������ �������� �� ���������, �� ����� ����������� ��
        /// ���������� ������ ������/������.
        NV_memory *PAC_NVRAM;

        /// @brief ������ � ����������������� ��� (Electrically Erasable
        /// Programmable Read-Only Memory).
        ///
        /// ����� ����������� �� ���������� ������ ������/������ - 1 �������.
        NV_memory *PAC_EEPROM;

        u_int last_NVRAM_pos;  ///< ������ �������� ���������� �������� NVRAM.
        u_int last_EEPROM_pos; ///< ������ �������� ���������� �������� EEPROM.
    };
//-----------------------------------------------------------------------------
#endif // MEM_H
