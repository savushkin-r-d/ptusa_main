
#include "mem_PLCnext.h"

#include <stdio.h>
#include <stdlib.h>

#include "log.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
eeprom_PLCnext::eeprom_PLCnext(int total_size, int available_start_pos,
    int available_end_pos ) : NV_memory( total_size,
    available_start_pos, available_end_pos ), NV_ram_data( 0 )
    {
    }
//-----------------------------------------------------------------------------
eeprom_PLCnext::~eeprom_PLCnext()
    {
    }
//-----------------------------------------------------------------------------
int eeprom_PLCnext::read( void *buff, u_int count, u_int start_pos )
    {
    if ( 0 == NV_ram_data ) return 0;

    memcpy( buff, NV_ram_data + get_available_start_pos() + start_pos,
        count );

    return 0;
    }
//-----------------------------------------------------------------------------
int eeprom_PLCnext::write( void *buff, u_int count, u_int start_pos )
    {
    if ( 0 == NV_ram_data ) return 0;

    memcpy( NV_ram_data + get_available_start_pos() + start_pos,
        buff, count );

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
