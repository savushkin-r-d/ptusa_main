#if !defined WIN_OS && \
    !( defined LINUX_OS && defined PAC_PC ) && \
	!( defined LINUX_OS && defined PAC_PLCNEXT )
#error You must define OS!
#endif

#include "base_mem.h"
#include "log.h"

#include <chrono>

#ifdef LINUX_OS
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#endif // LINUX_OS

#ifdef WIN_OS
#include <windows.h>
#include <fileapi.h>
#include <io.h>
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
SRAM::SRAM( const std::filesystem::path& file_name, u_int size ) :
    file_path( file_name ),
    tmp_path( file_name.string() + ".tmp" ),
    total_size( size )
    {
    params_data = new std::byte[ total_size ];
    memset( params_data, 0, total_size );
    }
//-----------------------------------------------------------------------------
SRAM::~SRAM()
    {
    if ( params_data )
        {
        delete[] params_data;
        params_data = nullptr;
        }
    }
//-----------------------------------------------------------------------------
int SRAM::load_data()
    {
    if ( !std::filesystem::exists( file_path ) )
        {
        G_LOG->debug( "SRAM() - File (%s) not found.",
            file_path.string().c_str() );
        return 1;
        }
    else
        {
        G_LOG->debug( "SRAM() - File (%s) found, loading.",
            file_path.string().c_str() );
        auto f = fopen( file_path.string().c_str(), "r+b" );

        if ( f )
            {
            fseek( f, 0, SEEK_SET );
            auto res = fread( get_data(), sizeof( std::byte ), get_size(), f );
            fclose( f );
            f = nullptr;

            if ( res == 0 )
                {
                G_LOG->error( "Error reading device (%s) : %s.\n",
                    file_path.string().c_str(), strerror( errno ) );
                return 3;
                }
            }
        else
            {
            G_LOG->error(
                "SRAM() - ERROR: Can't open device (%s) : %s.\n",
                file_path.string().c_str(), strerror( errno ) );
            return 2;
            }
        }

    return 0;
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
    //    4. fsync( directory ) - для Linux

    if ( FILE* temp = fopen( tmp_path.string().c_str(), "w+b" ); !temp )
        {
        G_LOG->error( "SRAM() - ERROR: Can't open file (%s) : %s.\n",
            file_path.string().c_str(), strerror( errno ) );

        return 1;
        }
    else
        {
        fwrite( get_data(), sizeof( std::byte ), get_size(), temp );
        fflush( temp );

#ifdef WIN_OS
        auto fd = _fileno( temp );
        auto hFile = (HANDLE)_get_osfhandle( fd );

        if ( hFile != INVALID_HANDLE_VALUE )
            {
            if ( !FlushFileBuffers( hFile ) )
                {
                G_LOG->error(
                    "SRAM() - ERROR: FlushFileBuffers (%s) failed (%lu).",
                    file_path.string().c_str(), GetLastError() );
                return 2;
                }
            }
#else
        fsync( fileno( temp ) );
        if ( auto fd = open( tmp_path.parent_path().string().c_str(), O_RDONLY );
            fd != -1 )
            {
            fsync( fd );
            close( fd );
            }
#endif

        fclose( temp );
        temp = nullptr;

#ifdef WIN_OS
        MoveFileExA( tmp_path.string().c_str(), file_path.string().c_str(),
            MOVEFILE_REPLACE_EXISTING );
#else
        std::error_code ec;
        std::filesystem::rename( tmp_path, file_path, ec );
        if ( ec )
            {
            G_LOG->error( "SRAM() - ERROR: Can't rename (%s) to (%s) : %s.\n",
                tmp_path.string().c_str(), file_path.string().c_str(),
                ec.message().c_str() );
            return 3;
            }
#endif
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
