#include "sys.h"

auto_smart_ptr < NV_memory_manager > NV_memory_manager::instance;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if ( defined LINUX || defined UCLINUX ) && defined DEBUG
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
//-----------------------------------------------------------------------------
int kbhit()
    {
    int tem = fcntl( 0, F_GETFL, 0 );
    fcntl( 0, F_SETFL, ( tem | O_NDELAY ) );
    char c = getchar();
    fcntl( 0, F_SETFL, tem );
    if ( c > 0 && c != 255 )
        {
        return 1;
        }

    return 0;
    }
//-----------------------------------------------------------------------------
void print_binary( u_int c )
    {
    if ( 0 == c )
        {
        printf( "0" );
        return;
        }

    u_int mask = 0x80000000;
    while( !( c & mask ) ) mask >>= 1;
    while ( mask )
        {
        printf( "%d", ( c & mask ) > 0 ? 1 : 0 );
        mask >>= 1;
        }
    }
//-----------------------------------------------------------------------------
int get_char()
    {
    struct termios oldt;
    struct termios newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
    }
#endif // ( defined LINUX || defined UCLINUX ) && defined DEBUG
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
            Print( "memory_range::write(...) - size[ %u ], incorrect params -> count[ %u ], start_pos[ %u ] \n", 
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
            Print( "memory_range::write(...) - size[ %u ], incorrect params -> count[ %u ], start_pos[ %u ] \n", 
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

    // ¬ыбор пам€ти, с которой будем работать.
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
            Print( "NV_memory_manager:get_memory_block(...) - incorrect memory type!\n" );
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
        Print( "NV_memory_manager:get_memory_block(...) - count [ %u ] + last memory position [ %u ] > available %s memory [ %u ], start position = %u, end position = %u\n",
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
    return instance;
    }
//-----------------------------------------------------------------------------
void NV_memory_manager::set_instance( NV_memory_manager* new_instance )
    {
    instance = new_instance;
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
