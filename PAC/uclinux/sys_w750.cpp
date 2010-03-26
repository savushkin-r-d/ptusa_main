#include "sys_w750.h"
#include "fcntl.h"
#include "stdio.h"
//-----------------------------------------------------------------------------
SRAM::SRAM( u_int total_size, 
           u_int available_start_pos, 
           u_int available_end_pos ): NV_memory( total_size, 
           available_start_pos,      
           available_end_pos )
    {
    if( ( file = open( "/dev/nvram" ) ) < 0 )  
        {  
        printf( "SRAM() - ERROR: Can't open device 
            (\"/dev/nvram\")\n" );  
        close( file );  
        file = 0;
        } 
    } 
//-----------------------------------------------------------------------------
SRAM::read( void *buff, u_int count, u_int start_pos )
    {
    if ( file )
    	{
        lseek( file, get_available_start_pos() + start_pos, SEEK_SET );
        return read( file, buff, count );
    	}

    return 0;
    }
//-----------------------------------------------------------------------------
SRAM::write( void *buff, u_int count, u_int start_pos )
    {
    if ( file )
        {
        lseek( file, get_available_start_pos() + start_pos, SEEK_SET );
        return write( file, buff, count );
        }

    return 0;
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