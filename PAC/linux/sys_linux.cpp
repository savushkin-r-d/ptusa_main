#include "sys_linux.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>


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
SRAM::SRAM( const char *file_name,
           u_int total_size,
           u_int available_start_pos,
           u_int available_end_pos ): NV_memory( total_size,
           available_start_pos,
           available_end_pos )
    {
    if ( strlen( file_name ) > C_MAX_FILE_NAME - 1 )
        {
        printf( "SRAM:: error - max file name length (%s).\n",
            file_name );
        }
    else
        {
        strcpy( this->file_name, file_name );

        if ( 0 == file )
            {
            if( ( file = open( file_name, O_RDWR ) ) < 0 )
                {
#ifdef DEBUG
                printf( "SRAM() - ERROR: Can't open device (%s) : %s.\n",
                   file_name, strerror( errno ) );
#endif // DEBUG
                file = 0;
                }
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
            printf( "Error reading device (%s) : %s.\n",
                file_name, strerror( errno )  );
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
            printf( "Error writing device (%s) : %s.\n",
                file_name, strerror( errno )  );
            }
#endif // DEBUG
        }

    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
data_file::data_file() : f( 0 )
    {
    }
//-----------------------------------------------------------------------------
int data_file::file_open( const char* file_name )
    {
    f = open( file_name, O_RDONLY );
    return f;
    }
//-----------------------------------------------------------------------------
int data_file::file_read( void *buffer, int count )
    {
    int res = 0;
    if ( f > 0 )
        {
        res = read( f, buffer, count );
        }

    return res;
    }
//-----------------------------------------------------------------------------
char* data_file::fget_line()
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
char* data_file::pfget_line()
    {
    fget_line();
    int str_len = strlen( buf );
    lseek( f, -str_len, SEEK_CUR );
    return buf;
    }
//-----------------------------------------------------------------------------
void data_file::file_close()
    {
    if ( f > 0 )
        {
        close( f );
        f = 0;
        }
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
u_long get_millisec()
    {
    static timeval start_tv;
    static char is_init = 0;
    if ( 0 == is_init )
        {
        gettimeofday( &start_tv, NULL );
        is_init = 1;
        }

    timeval tv;
    gettimeofday( &tv, NULL );

    u_long now = 1000UL * ( tv.tv_sec - start_tv.tv_sec ) +
            ( tv.tv_usec - start_tv.tv_usec ) / 1000;

    return now;
    }
//-----------------------------------------------------------------------------
u_long get_sec()
    {
    timeval tv;
    gettimeofday( &tv, NULL );

    return tv.tv_sec;
    }
//-----------------------------------------------------------------------------
u_long get_delta_millisec( u_long time1 )
    {
    u_long now = get_millisec();

    return now >= time1 ? now - time1 : ULONG_MAX - time1 + now;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

