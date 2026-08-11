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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
SRAM::SRAM( const std::filesystem::path& file_name, u_int size ) :
    file_path( file_name ),
    tmp_path( file_name.string() + ".tmp" ),
    total_size( size )
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
            G_LOG->error(
                "SRAM() - ERROR: Can't open device (%s) : %s.\n",
                file_path.string().c_str(), strerror( errno ) );
            file = nullptr;
            }
        }

    params_data = new std::byte[ total_size ];
    memset( params_data, 0, total_size );
    }
//-----------------------------------------------------------------------------
SRAM::~SRAM()
    {
    if ( file )
        {
        fclose( file );
        file = nullptr;
        }

    if ( params_data )
        {
        delete[] params_data;
        params_data = nullptr;
        }
    }
//-----------------------------------------------------------------------------
int SRAM::load()
    {
    if ( file )
        {
        fseek( file, 0, SEEK_SET );

        if ( auto res = fread(
            get_data(), sizeof( std::byte ), get_size(), file ); res == 0 )
            {
            G_LOG->error( "Error reading device (%s) : %s.\n",
                file_path.string().c_str(), strerror( errno ) );
            return 1;
            }

        return 0;
        }

    return 2;
    }
//-----------------------------------------------------------------------------
int SRAM::safe_save()
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

        fwrite( get_data(), sizeof( std::byte ), get_size(), temp );
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
std::byte* SRAM::get_data()
    {
    return params_data;
    }
//-----------------------------------------------------------------------------
void SRAM::zero_fill()
    {
    if ( params_data )
        {
        memset( params_data, 0, total_size );
        }
    }
//-----------------------------------------------------------------------------
u_int SRAM::get_size() const
    {
    return total_size;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
