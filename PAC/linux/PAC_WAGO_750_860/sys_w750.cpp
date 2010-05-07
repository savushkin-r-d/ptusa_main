#include "sys_w750.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

NV_memory_manager* NV_memory_manager::instance = 0;
int SRAM::file = 0;
//-----------------------------------------------------------------------------
int read_file( int file, void *buff, u_int count )
    {
    return read( file, buff, count );
    }
//-----------------------------------------------------------------------------
int write_file( int file, void *buff, u_int count )
    {
    return write( file, buff, count );
    }
//-----------------------------------------------------------------------------
SRAM::SRAM( u_int total_size, 
           u_int available_start_pos, 
           u_int available_end_pos ): NV_memory( total_size, 
           available_start_pos,      
           available_end_pos )
    {
    if ( 0 == file )
        {
        if( ( file = open( "/dev/nvram", O_RDWR ) ) < 0 )
            {
#ifdef DEBUG
            perror( "SRAM() - ERROR: Can't open device (\"/dev/nvram\")" );
#endif // DEBUG

            close( file );
            file = 0;
            }

        }
    } 
//-----------------------------------------------------------------------------
SRAM::~SRAM()
    {
    if ( file )
        {
        close( file );
        file = 0;
        }
    }
//-----------------------------------------------------------------------------
int SRAM::read( void *buff, u_int count, u_int start_pos )
    {
    int res = 0;

    if ( file )
        {
        lseek( file, get_available_start_pos() + start_pos, SEEK_SET );
        res = read_file( file, buff, count );

#ifdef DEBUG
        if ( res <= 0 )
            {
            perror( "Error reading device (\"/dev/nvram\")" );
            }
#endif // DEBUG

        }

    return res;
    }
//-----------------------------------------------------------------------------
int SRAM::write( void *buff, u_int count, u_int start_pos )
    {
    int res = 0;

    if ( file )
        {
        lseek( file, get_available_start_pos() + start_pos, SEEK_SET );
        res = write_file( file, buff, count );

#ifdef DEBUG
        if ( res <= 0 )
            {
            perror( "Error writing device (\"/dev/nvram\")" );
            }
#endif // DEBUG
        }

    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NV_memory_manager_W750::NV_memory_manager_W750():NV_memory_manager()
    {
    PAC_NVRAM = new SRAM( 32768, 0, 30 );
    PAC_EEPROM = new SRAM( 32768, 31, 32767 );
    last_NVRAM_pos = PAC_NVRAM->get_available_start_pos();
    last_EEPROM_pos = PAC_EEPROM->get_available_start_pos();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
file_w750::file_w750() : f( 0 )
    {
    }
//-----------------------------------------------------------------------------
int file_w750::fopen( const char* file_name )
    {
    f = open( file_name, O_RDONLY );
    return f;
    }
//-----------------------------------------------------------------------------
int file_w750::fread( void *buffer, int count )
    {
    int res = 0;
    if ( f > 0 )
        {
        res = read( f, buffer, count );
        }

    return res;
    }
//-----------------------------------------------------------------------------
char* file_w750::fget_line()
    {
    buf[ 0 ] = 0;

    char *tmp_buff = buf;
    int pos = 1;
    int res = read( f, tmp_buff, 1 );
    if ( res != 1 )
        {
#ifdef DEBUG
        Print( "Error reading file - can\'t read!\n" );
#endif // DEBUG
        return buf;
        }

    while ( tmp_buff[ 0 ] != '\n' )
        {
        tmp_buff++;
        res = read( f, tmp_buff, 1 );

        if ( res != 1 )
            {
#ifdef DEBUG
            Print( "Error reading file - can\'t read more!\n" );
#endif // DEBUG
            break;
            }

        pos++;
        if ( pos >= C_MAX_BUFFER_SIZE )
            {
#ifdef DEBUG
            Print( "Error reading file - line is too long!\n" );
#endif // DEBUG
            break;
            }
        }

    buf[ pos ] = 0;
    return buf;
    }
//-----------------------------------------------------------------------------
char* file_w750::pfget_line()
    {
    fget_line();
    int str_len = strlen( buf );
    lseek( f, -str_len, SEEK_CUR );
    return buf;
    }
//-----------------------------------------------------------------------------
void file_w750::fclose()
    {
    if ( f > 0 )
        {
        close( f );
        f = 0;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
u_long get_ms()
    {    
    timeb timebuffer;
    ftime( &timebuffer );
    return timebuffer.time * 1000 + timebuffer.millitm;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

