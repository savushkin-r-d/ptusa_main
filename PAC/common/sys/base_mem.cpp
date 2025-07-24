#if !defined WIN_OS && \
    !( defined LINUX_OS && defined PAC_PC ) && \
    !( defined LINUX_OS && defined PAC_WAGO_750_860 ) && \
    !( defined LINUX_OS && defined PAC_WAGO_PFC200 ) && \
	!( defined LINUX_OS && defined PAC_PLCNEXT )
#error You must define OS!
#endif

#include "base_mem.h"

#ifdef WIN_OS
#include "w_mem.h"
#endif // WIN_OS

#if defined LINUX_OS && defined PAC_PC
#include "l_mem.h"
#endif // LINUX_OS

#if defined LINUX_OS && defined PAC_WAGO_750_860
#include "l_mem.h"
#endif // LINUX_OS

#if defined LINUX_OS && defined PAC_WAGO_PFC200
#include "mem_PFC200.h"
#endif // LINUX_OS

#if defined LINUX_OS && defined PAC_PLCNEXT
#ifdef PAC_PLCNEXT_ALONE
#include "l_mem.h"
#else
#include "mem_PLCnext.h"
#endif
#endif // LINUX_OS

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
            if ( G_DEBUG )
                {
                printf( "memory_range::write(...) - size[ %u ], incorrect params -> "
                    "count[ %u ], start_pos[ %u ] \n",
                    size, count, start_pos );
                }
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
            if ( G_DEBUG )
                {
                printf( "memory_range::write(...) - size[ %u ], incorrect params -> "
                    "count[ %u ], start_pos[ %u ] \n",
                    size, count, start_pos );
                }
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
    const int EEPROM_SIZE = 32768;
    const int NVRAM_SIZE = 32;

#ifdef WIN_OS
    PAC_NVRAM  = new SRAM( "./nvram.txt", EEPROM_SIZE, 0, NVRAM_SIZE - 1 );
    PAC_EEPROM = new SRAM( "./nvram.txt", EEPROM_SIZE, NVRAM_SIZE, EEPROM_SIZE - 1 );
#endif // WIN_OS

#if defined LINUX_OS && defined PAC_PC
    PAC_NVRAM  = new SRAM( "./nvram.txt", EEPROM_SIZE, 0, NVRAM_SIZE - 1 );
    PAC_EEPROM = new SRAM( "./nvram.txt", EEPROM_SIZE, NVRAM_SIZE, EEPROM_SIZE - 1 );
#endif

#if defined LINUX_OS && defined PAC_WAGO_750_860
    PAC_NVRAM  = new SRAM( "/dev/nvram", EEPROM_SIZE, 0, NVRAM_SIZE - 1 );
    PAC_EEPROM = new SRAM( "/dev/nvram", EEPROM_SIZE, NVRAM_SIZE, EEPROM_SIZE - 1 );
#endif

#if defined LINUX_OS && defined PAC_WAGO_PFC200
    PAC_NVRAM  = new eeprom_PFC200( EEPROM_SIZE, 0, NVRAM_SIZE - 1 );
    PAC_EEPROM = new eeprom_PFC200( EEPROM_SIZE, NVRAM_SIZE, EEPROM_SIZE - 1 );
#endif

#if defined LINUX_OS && defined PAC_PLCNEXT
#ifdef PAC_PLCNEXT_ALONE
    PAC_NVRAM  = new SRAM( "./nvram.txt", EEPROM_SIZE, 0, NVRAM_SIZE - 1 );
    PAC_EEPROM = new SRAM( "./nvram.txt", EEPROM_SIZE, NVRAM_SIZE, EEPROM_SIZE - 1 );
#else
    PAC_NVRAM  = new eeprom_PLCnext( EEPROM_SIZE, 0, NVRAM_SIZE - 1 );
    PAC_EEPROM = new eeprom_PLCnext( EEPROM_SIZE, NVRAM_SIZE, EEPROM_SIZE - 1 );
#endif
#endif

    last_NVRAM_pos  = PAC_NVRAM->get_available_start_pos();
    last_EEPROM_pos = PAC_EEPROM->get_available_start_pos();
    }
//-----------------------------------------------------------------------------
void NV_memory_manager::init_ex( void * par )
    {
#if defined LINUX_OS && defined PAC_PLCNEXT
    PAC_NVRAM->init( par );
    PAC_EEPROM->init( par );
#endif
    }
//-----------------------------------------------------------------------------
memory_range* NV_memory_manager::get_memory_block( MEMORY_TYPE m_type,
    u_int count )
    {
    NV_memory *memory = 0;
    u_int     *last_mem_pos = 0;
    char      mem_name[ 10 ];

    // Выбор памяти, с которой будем работать.
    switch ( m_type )
        {
    case MT_NVRAM:
        memory = PAC_NVRAM;
        last_mem_pos = &last_NVRAM_pos;
        if ( G_DEBUG )
            {
            strcpy( mem_name, "NVRAM" );
            }
        break;

    case MT_EEPROM:
        memory = PAC_EEPROM;
        last_mem_pos = &last_EEPROM_pos;
        if ( G_DEBUG )
            {
            strcpy( mem_name, "EEPROM" );
            }
        break;

    default:
        if ( G_DEBUG )
            {
            printf( "NV_memory_manager:get_memory_block(...) - incorrect memory "
                "type!\n" );            
            }

        return new memory_range( 0, 0, 0 );
        }

    if ( 0 == memory )
        {
        if ( G_DEBUG )
            {
            printf( "NV_memory_manager:get_memory_block(...) - memory = NULL!\n" );        
            }
        return new memory_range( 0, 0, 0 );
        }

    if ( *last_mem_pos + count >
        memory->get_available_end_pos() )
        {
        if ( G_DEBUG )
            {
            printf( "NV_memory_manager:get_memory_block(...) - count [ %u ] + last "
                "memory position [ %u ] > available %s memory [ %u ], start "
                "position = %u, end position = %u\n",
                count,
                *last_mem_pos,
                mem_name,
                memory->get_size(),
                memory->get_available_start_pos(),
                memory->get_available_end_pos() );        
            }
        return new memory_range( 0, 0, 0 );
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
