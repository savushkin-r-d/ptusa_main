/// @file mem_PLCnext.h
/// @brief Содержит описания классов, которые реализуют аппаратную часть PAC
/// по работе с памятью.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev: 220 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-15 16:58:56#$.

#ifndef MEM_PLCNEXT_H
#define MEM_PLCNEXT_H

#include <cstdint>

#include "base_mem.h"
//-----------------------------------------------------------------------------
/// @brief Работа с энергонезависимой ОЗУ (Static Memory).
///
/// Имеет ограничения на количество циклов записи/чтения - 1 миллион.
class eeprom_PLCnext : public NV_memory {
  friend class NV_memory_manager;

 public:
  virtual ~eeprom_PLCnext();

  void init(void *NV_ram_data) {
    this->NV_ram_data = (std::uint8_t *)NV_ram_data;
  }

 private:
  eeprom_PLCnext(int total_size, int available_start_pos,
                 int available_end_pos);

  /// @brief Метод интерфейса @ref i_memory.
  int read(void *buff, u_int count, u_int start_pos);

  /// @brief Метод интерфейса @ref i_memory.
  int write(void *buff, u_int count, u_int start_pos);

  std::uint8_t *NV_ram_data;  // Pointer to begin of NVRAM
};
//-----------------------------------------------------------------------------
#endif  // MEM_PLCNEXT_H
