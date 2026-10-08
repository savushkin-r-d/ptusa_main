#pragma once

#include <cstddef>
#include <memory>

/// @brief Доступ к данным локальной шины без зависимости от SDK PLCnext.
class local_bus_driver
    {
    public:
        struct module_info
            {
            unsigned int article;
            unsigned int bytes;
            bool missing;
            };

        virtual ~local_bus_driver() = default;
        virtual bool initialize() = 0;
        virtual const char* get_error_text() const { return ""; }
        virtual bool ready() = 0;
        virtual size_t module_count() = 0;
        virtual module_info module( size_t index ) = 0;
        virtual bool read_inputs() = 0;
        virtual bool write_outputs() = 0;
        virtual void read_module( size_t index, unsigned char* data,
            unsigned int bytes ) = 0;
        virtual void write_module( size_t index, unsigned char* data,
            unsigned int bytes ) = 0;
    };

/// @brief На неподдерживаемой платформе возвращает nullptr.
std::unique_ptr<local_bus_driver> make_local_bus_driver();

/// @brief Длина процессных данных из записи идентификации PDI 0x0037.
bool decode_local_bus_process_data_length( const unsigned char* record,
    size_t size, unsigned int& bytes );
