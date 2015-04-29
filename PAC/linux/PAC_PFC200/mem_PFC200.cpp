
#include "mem_PFC200.h"

#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include <nvram.h>
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
eeprom_PFC200::eeprom_PFC200(int total_size, int available_start_pos,
    int available_end_pos) : NV_memory( total_size, available_start_pos,
    available_end_pos ), NV_ram_data( 0 )
    {
    if ( nvram_init() < 0 )
        {
        fprintf( stderr, "NVRAM init failed\n" );
        exit( EXIT_FAILURE );
        }

#ifdef DEBUG
    int nvram_size = nvram_get_size();
    printf( "Total EEPROM size %d, mem size %d\n",
        nvram_size, total_size );
#endif

    NV_ram_data = ( char* ) nvram_get_mapping();  // get begin of NVRAM;
    }
//-----------------------------------------------------------------------------
eeprom_PFC200::~eeprom_PFC200()
    {
    nvram_close();
    }
//-----------------------------------------------------------------------------
int eeprom_PFC200::read( void *buff, u_int count, u_int start_pos )
    {
    if ( 0 == NV_ram_data ) return 0;

    memcpy( buff, NV_ram_data + get_available_start_pos() + start_pos,
        count );

    return 0;
    }
//-----------------------------------------------------------------------------
int eeprom_PFC200::write( void *buff, u_int count, u_int start_pos )
    {
    if ( 0 == NV_ram_data ) return 0;

    memcpy( NV_ram_data + get_available_start_pos() + start_pos,
        buff, count );

    return 0;
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
