#if !defined WIN_OS && \
    !( defined LINUX_OS && defined PAC_PC ) && \
    !( defined LINUX_OS && defined PAC_WAGO_750_860 ) && \
    !( defined LINUX_OS && defined PAC_WAGO_PFC200 ) && \
	!( defined LINUX_OS && defined PAC_PLCNEXT )
#error You must define OS!
#endif

#include "base_mem.h"
#include "log.h"

#include <chrono>
#ifdef LINUX_OS
#include <unistd.h>

#include <cstdio>
#include <cerrno>
#endif // LINUX_OS

#if defined LINUX_OS && defined PAC_WAGO_PFC200
#include "mem_PFC200.h"
#endif // LINUX_OS

#include <vector>

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
int memory_range::read( std::byte *buf, u_int count, u_int start_pos/*= 0*/ )
    {
    if ( memory )
        {
        if ( check_params( count, start_pos ) != 0 )
            {
            G_LOG->debug( "memory_range::read(...) - size[ %u ], incorrect "
                "params -> count[ %u ], start_pos[ %u ] \n",
                size, count, start_pos );
            return 0;
            }

        return memory->read( buf, count, this->start_pos + start_pos );
        }

    return 0;
    }
//-----------------------------------------------------------------------------
int memory_range::safe_save( const std::byte* buff )
    {
    if ( memory )
        {
        return memory->safe_save( buff );
        }
    return 0;
    }
//-----------------------------------------------------------------------------
int memory_range::zero_fill()
    {
    if ( memory )
        {
        return memory->zero_fill();
        }

    return 1;
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
    const int EEPROM_SIZE
#ifdef PTUSA_TEST
        = 10 * 32'768;
#else
        = 32'768;
#endif

    const int NVRAM_SIZE = 32;

#if defined WIN_OS || ( defined LINUX_OS && ( defined PAC_PC || defined PAC_PLCNEXT ) )
    PAC_NVRAM  = new SRAM( "./nvram.bin", NVRAM_SIZE, 0, NVRAM_SIZE - 1 );
    PAC_EEPROM = new SRAM( "./eeprom.bin", EEPROM_SIZE, 0, EEPROM_SIZE - 1 );
#endif

#if defined LINUX_OS && defined PAC_WAGO_750_860
    PAC_NVRAM  = new SRAM( "/dev/nvram", EEPROM_SIZE, 0, NVRAM_SIZE - 1 );
    PAC_EEPROM = new SRAM( "/dev/nvram", EEPROM_SIZE, NVRAM_SIZE, EEPROM_SIZE - 1 );
#endif

#if defined LINUX_OS && defined PAC_WAGO_PFC200
    PAC_NVRAM  = new eeprom_PFC200( EEPROM_SIZE, 0, NVRAM_SIZE - 1 );
    PAC_EEPROM = new eeprom_PFC200( EEPROM_SIZE, NVRAM_SIZE, EEPROM_SIZE - 1 );
#endif

    last_NVRAM_pos  = PAC_NVRAM->get_available_start_pos();
    last_EEPROM_pos = PAC_EEPROM->get_available_start_pos();
    }
//-----------------------------------------------------------------------------
memory_range* NV_memory_manager::get_memory_block( MEMORY_TYPE m_type,
    u_int count )
    {
    NV_memory *memory = 0;
    u_int     *last_mem_pos = 0;
    const char* mem_name = "";

    // Выбор памяти, с которой будем работать.
    switch ( m_type )
        {
    case MT_NVRAM:
        memory = PAC_NVRAM;
        last_mem_pos = &last_NVRAM_pos;
        if ( G_DEBUG )
            {
            mem_name = "NVRAM";
            }
        break;

    case MT_EEPROM:
        memory = PAC_EEPROM;
        last_mem_pos = &last_EEPROM_pos;
        if ( G_DEBUG )
            {
            mem_name = "EEPROM";
            }
        break;

    default:
        if ( G_DEBUG )
            {
            G_LOG->debug( "NV_memory_manager:get_memory_block(...) - "
                "incorrect memory type!\n" );
            }

        return new memory_range( 0, 0, 0 );
        }

    if ( nullptr == memory )
        {
        if ( G_DEBUG )
            {
            G_LOG->debug( "NV_memory_manager:get_memory_block(...) - "
                "memory == nullptr!\n" );
            }
        return new memory_range( 0, 0, 0 );
        }

    if ( *last_mem_pos + count >
        memory->get_available_end_pos() )
        {
        if ( G_DEBUG )
            {
            G_LOG->debug( "NV_memory_manager:get_memory_block(...) - count "
                "[ %u ] + last memory position [ %u ] > available %s memory "
                "[ %u ], start position = %u, end position = %u\n",
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
SRAM::SRAM( const std::filesystem::path& file_name,
    u_int total_size,
    u_int available_start_pos,
    u_int available_end_pos ) : NV_memory( total_size,
        available_start_pos,
        available_end_pos ),
    file_path( file_name ),
    tmp_path( file_name.string() + ".tmp" )
    {
    if ( ( file = fopen( file_path.string().c_str(), "r+b" ) ) == nullptr )
        {
        //Пытаемся создать файл
        file = fopen( file_path.string().c_str(), "w+b" );
        if ( file )
            {
            fseek( file, total_size, SEEK_SET );
            char tmp = 0;
            fwrite( &tmp, sizeof( tmp ), 1, file );
            fflush( file );
            }
        else
            {
            if ( G_DEBUG )
                {
                G_LOG->error(
                    "SRAM() - ERROR: Can't open device (%s) : %s.\n",
                    file_path.string().c_str(), strerror( errno ) );
                }
            file = nullptr;
            }
        }
    }
//-----------------------------------------------------------------------------
SRAM::~SRAM()
    {
    if ( file )
        {
        fclose( file );
        file = nullptr;
        }
    }
//-----------------------------------------------------------------------------
int SRAM::read( std::byte* buff, u_int count, u_int start_pos )
    {
    int res = 0;

    if ( file )
        {
        fseek( file, get_available_start_pos() + start_pos, SEEK_SET );
        res = fread( buff, sizeof( char ), count, file );

        if ( G_DEBUG )
            {
            if ( res <= 0 )
                {
                G_LOG->error( "Error reading device (%s) : %s.\n",
                    file_path.string().c_str(), strerror( errno ) );
                }
            }

        }

    return res;
    }
//-----------------------------------------------------------------------------
int SRAM::zero_fill()
    {
    if ( !file ) return 1;

    std::vector<std::byte> zeros( get_size(), std::byte{ 0 } );
    fseek( file, 0, SEEK_SET );
    fwrite( zeros.data(), sizeof( std::byte ), zeros.size(), file );
    fflush( file );
    return 0;
    }
//-----------------------------------------------------------------------------
int SRAM::safe_save( const std::byte* buff )
    {
    std::chrono::high_resolution_clock::time_point start;
    if ( G_DEBUG )
        {
        start = std::chrono::high_resolution_clock::now();
        }

    // Схема атомарного сохранения:
    //    1. записать данные во временный файл
    //    2. fsync( temp )
    //    3. rename( temp, target )
    //    4. fsync( directory ) ( опционально )

    if ( FILE* temp = fopen( tmp_path.string().c_str(), "w+b" ); !temp )
        {
        G_LOG->error( "SRAM() - ERROR: Can't open device (%s) : %s.\n",
            file_path.string().c_str(), strerror( errno ) );

        return 1;
        }
    else
        {
        fclose( file );

        fwrite( buff, sizeof( char ), get_size(), temp );
        fflush( temp );
        fclose( temp );
        temp = nullptr;

#ifdef WIN_OS
        MoveFileExA( tmp_path.string().c_str(), file_path.string().c_str(),
            MOVEFILE_REPLACE_EXISTING );
#else
        std::filesystem::rename( tmp_path, file_path );
#endif
        file = fopen( file_path.string().c_str(), "r+b" );

        if ( G_DEBUG )
            {
            auto end = std::chrono::high_resolution_clock::now();
            const auto duration = std::chrono::duration_cast<
                std::chrono::microseconds>( end - start ).count();
            G_LOG->debug( "SRAM::safe_save() - write time: %lld us (%s).",
                static_cast<long long>( duration ),
                file_path.string().c_str() );
            }
        }

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
