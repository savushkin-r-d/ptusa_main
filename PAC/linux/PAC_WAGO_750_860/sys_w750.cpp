#include "sys_w750.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NV_memory_manager_W750::NV_memory_manager_W750():NV_memory_manager()
    {
    PAC_NVRAM = new SRAM( "/dev/nvram", 32768, 0, 30 );
    PAC_EEPROM = new SRAM( "/dev/nvram", 32768, 31, 32767 );
    last_NVRAM_pos = PAC_NVRAM->get_available_start_pos();
    last_EEPROM_pos = PAC_EEPROM->get_available_start_pos();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
