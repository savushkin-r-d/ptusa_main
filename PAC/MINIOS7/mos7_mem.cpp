#include "mos7_mem.h"
#ifdef UPAC_5000
#include "5000\upac5000.h"
#endif

#ifdef UPAC_7186E
#include "7186e.h"
#endif
//-----------------------------------------------------------------------------
EEPROM_7186::EEPROM_7186():NV_memory( 16383, 8192, 16127 )
    {
    }
//-----------------------------------------------------------------------------
int EEPROM_7186::read( void *buff, u_int count, u_int start_pos )
    {
    int res = EE_MultiRead_L( start_pos, count, ( char* ) buff );

#ifdef DEBUG
    if ( res != 0 )
    	{
        Print( "EEPROM_7186::read() - error! \n" );
    	}
#endif // DEBUG

    if ( 0 == res  ) 
        {
        res = count; // Возвращаем количество считанных байт.
        }
    else
        {
        res = 0;     // Возвращаем 0 в случае ошибки чтения.
        }

    return res;
    }
//-----------------------------------------------------------------------------
int EEPROM_7186::write( void *buff, u_int count, u_int start_pos )
    {
    EnableEEP();
    int res = EE_MultiWrite_L( start_pos, count, ( char* ) buff );

#ifdef DEBUG
    if ( res != 0 )
        {
        Print( "EEPROM_7186::write() - error! \n" );
        }
#endif // DEBUG

    if ( 0 == res  ) 
        {
        res = count; // Возвращаем количество записанных байт.
        }
    else
        {
        res = 0;     // Возвращаем 0 в случае ошибки чтения.
        }
    ProtectEEP();

    return res;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NVRAM_7186::NVRAM_7186():NV_memory( 31, 0, 30 )
    {   
    }
//-----------------------------------------------------------------------------
int NVRAM_7186::read( void *buff, u_int count, u_int start_pos )
    {
    for ( int i = 0; i < count; i++ )
        {
        int res = ReadNVRAM( start_pos + i );
        if ( res >= 0 )
            {
            ( ( char* ) buff )[ i ] = res;
            }
        else
            {
#ifdef DEBUG
            Print( "NVRAM_7186::read() - error! \n" );
#endif // DEBUG
            return i;
            }                    
        }
    return count;
    }
//-----------------------------------------------------------------------------
int NVRAM_7186::write( void *buff, u_int count, u_int start_pos )
    {
    for ( int i = 0; i < count; i++ )
        {
        int res = WriteNVRAM( start_pos + i, ( ( char* ) buff )[ i ] );

        if ( res != 0 )
            {
#ifdef DEBUG
            Print( "NVRAM_7186::write() - error! \n" );
#endif // DEBUG
            return i;
            }
        }

    return count;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

