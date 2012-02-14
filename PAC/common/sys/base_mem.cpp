#if !defined WIN_OS && !( defined LINUX_OS && defined PAC_PC ) && \
    !( defined LINUX_OS && defined PAC_WAGO_750_860 ) && \
    !( defined MINIOS7 && defined UPAC_7186E ) && \
    !( defined MINIOS7 && defined UPAC_5000 )
#error You must define OS!
#endif 

#include "base_mem.h"

#ifdef WIN_OS
#include "w_mem.h"
#endif // WIN_OS

#ifdef LINUX_OS
#include "l_mem.h"
#endif // LINUX_OS

#ifdef MINIOS7
#include "mos7_mem.h"
#endif // MINIOS7

auto_smart_ptr < NV_memory_manager > NV_memory_manager::instance;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NV_memory::NV_memory( u_int total_size,
    u_int available_start_pos,
    u_int available_end_pos ) : total_size( total_size ),
    available_start_pos( available_start_pos ),
    available_end_pos( available_end_pos )
    {
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
memory_range::memory_range( i_memory *memory, u_int start_pos, 
    u_int size ) : memory( memory ), 
    start_pos( start_pos ),
    size( size )
    {
    }
//-----------------------------------------------------------------------------
int memory_range::read( void *buf, u_int count, u_int start_pos )
    {
    if ( memory )
        {
        if ( check_params( count, start_pos ) != 0 )
            {
#ifdef DEBUG
            Print( "memory_range::write(...) - size[ %u ], incorrect params -> "
                "count[ %u ], start_pos[ %u ] \n", 
                size, count, start_pos );
#endif // DEBUG
            return 0;
            }

        return memory->read( buf, count, this->start_pos + start_pos );
        } 

    return 0;
    }
//-----------------------------------------------------------------------------
int memory_range::write( void *buf, u_int count, u_int start_pos )
    {
    if ( memory )
        {
        if ( check_params( count, start_pos ) != 0 )
            {
#ifdef DEBUG
            Print( "memory_range::write(...) - size[ %u ], incorrect params -> "
                "count[ %u ], start_pos[ %u ] \n", 
                size, count, start_pos );
#endif // DEBUG
            return 0;
            }

        return memory->write( buf, count, this->start_pos + start_pos );
        } 

    return 0;
    }
//-----------------------------------------------------------------------------
int memory_range::check_params( u_int count, u_int start_pos )
    {
    if ( size < start_pos + count )
        {
        return 1;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NV_memory_manager::NV_memory_manager() : PAC_NVRAM( 0 ),
    PAC_EEPROM( 0 ),
    last_NVRAM_pos( 0 ),
    last_EEPROM_pos( 0 )
    {
#ifdef WIN_OS
    // FIXME Реализовать создание файла при его отсутствии.
    PAC_NVRAM  = new SRAM( "./nvram.txt", 31, 0, 30 );
    PAC_EEPROM = new SRAM( "./nvram.txt", 32737, 31, 32767 ); 
#endif // WIN_OS

#if defined LINUX_OS && defined PAC_PC
    // FIXME Реализовать создание файла при его отсутствии.
    PAC_NVRAM  = new SRAM( "./nvram.txt", 32768, 0, 30 );
    PAC_EEPROM = new SRAM( "./nvram.txt", 32768, 31, 32767 );   
#endif

#if defined LINUX_OS && defined PAC_WAGO_750_860
    PAC_NVRAM  = new SRAM( "/dev/nvram", 32768, 0, 30 );
    PAC_EEPROM = new SRAM( "/dev/nvram", 32768, 31, 32767 );
#endif

#if defined MINIOS7 && defined UPAC_7186E
    PAC_NVRAM = new NVRAM_7186();
    PAC_EEPROM = new EEPROM_7186();
#endif

    last_NVRAM_pos  = PAC_NVRAM->get_available_start_pos();
    last_EEPROM_pos = PAC_EEPROM->get_available_start_pos();
    }
//-----------------------------------------------------------------------------
memory_range* NV_memory_manager::get_memory_block( MEMORY_TYPE m_type,
    u_int count )
    {
    NV_memory    *memory = 0;
    u_int        *last_mem_pos = 0; 
#ifdef DEBUG
    char      mem_name[ 10 ];
#endif // DEBUG

    // Выбор памяти, с которой будем работать.
    switch ( m_type )
        {
    case MT_NVRAM:
        memory = PAC_NVRAM;
        last_mem_pos = &last_NVRAM_pos;
#ifdef DEBUG
        strcpy( mem_name, "NVRAM" );
#endif // DEBUG
        break;

    case MT_EEPROM:
        memory = PAC_EEPROM;
        last_mem_pos = &last_EEPROM_pos;
#ifdef DEBUG
        strcpy( mem_name, "EEPROM" );
#endif // DEBUG
        break;

    default:
#ifdef DEBUG
        Print( "NV_memory_manager:get_memory_block(...) - incorrect memory "
            "type!\n" );
        get_char();
#endif // DEBUG

        return new memory_range( 0, 0, 0 );
        }

    if ( 0 == memory )
        {
#ifdef DEBUG
        Print( "NV_memory_manager:get_memory_block(...) - memory = NULL!\n" );
        get_char();
#endif // DEBUG
        new memory_range( 0, 0, 0 );
        }

    if ( *last_mem_pos + count > 
        memory->get_available_end_pos() )
        {
#ifdef DEBUG
        Print( "NV_memory_manager:get_memory_block(...) - count [ %u ] + last "
            "memory position [ %u ] > available %s memory [ %u ], start "
            "position = %u, end position = %u\n",
            count, 
            *last_mem_pos,
            mem_name,
            memory->get_size(),            
            memory->get_available_start_pos(),
            memory->get_available_end_pos() );
        get_char();
#endif // DEBUG
        new memory_range( 0, 0, 0 );
        }

    memory_range *res = new memory_range( memory, *last_mem_pos, count );
    *last_mem_pos += count;

    return res;
    }
//-----------------------------------------------------------------------------
NV_memory_manager* NV_memory_manager::get_instance()
    {
    if ( instance.is_null() )
        {
        instance = new NV_memory_manager();
        }

    return instance;
    }
//-----------------------------------------------------------------------------
NV_memory_manager::~NV_memory_manager()
    {
    if ( PAC_NVRAM )
        {
        delete PAC_NVRAM;
        PAC_NVRAM = 0;
        }
    if ( PAC_EEPROM )
        {
        delete PAC_EEPROM;
        PAC_EEPROM = 0;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
