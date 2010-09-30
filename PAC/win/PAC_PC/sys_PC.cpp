#include "sys_PC.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NV_memory_manager_PC::NV_memory_manager_PC():NV_memory_manager()
    {
    // FIXME Реализовать создание файла при его отсутствии.
    PAC_NVRAM  = new SRAM( "./nvram.txt", 32768, 0, 30 );
    PAC_EEPROM = new SRAM( "./nvram.txt", 32768, 31, 32767 );
    last_NVRAM_pos  = PAC_NVRAM->get_available_start_pos();
    last_EEPROM_pos = PAC_EEPROM->get_available_start_pos();
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
