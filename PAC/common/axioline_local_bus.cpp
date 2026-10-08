#include "axioline_local_bus.h"

bool decode_local_bus_process_data_length( const unsigned char* record,
    size_t size, unsigned int& bytes )
    {
    bytes = 0;
    if ( !record || size < 8 ) return false;
    auto length = static_cast<unsigned int>( record[2] ) * 256 + record[3];
    if ( length > 1024 ) return false;
    bytes = length;
    return true;
    }

#ifdef PTUSA_AXIOBUS
#include "Axiobus/Axiobus.h"
#include <cerrno>
#include <cstdio>
#include <cstring>

namespace
    {
    class axiobus_driver final : public local_bus_driver
        {
        public:
            bool initialize() override
                {
                if ( bus && bus->isInitialized() && metadata_ready )
                    {
                    error_text[0] = 0;
                    return true;
                    }
                if ( !check_access( "/dev/axio_xfer0", false ) ||
                    !check_access( "/tmp/axiopdi", true ) ) return false;
                if ( !bus )
                    {
                    bus = std::make_unique<PLCnext::Axiobus>(
                        PLCnext::Axiobus::DIRECT,
                        PLCnext::Axiobus::EXPLICIT );
                    }
                else if ( !bus->isInitialized() )
                    {
                    bus->initialize();
                    }
                if ( !bus->isInitialized() )
                    {
                    std::snprintf( error_text, sizeof( error_text ),
                        "Axiobus init_error=%u", bus->getInitializationError() );
                    return false;
                    }
                process_bytes.clear();
                for ( auto module : bus->getModules() )
                    {
                    std::vector<unsigned char> record;
                    auto result = module->pdiRead( 0, 0x0037, 0, record );
                    unsigned int bytes = 0;
                    if ( !result || !decode_local_bus_process_data_length(
                        record.data(), record.size(), bytes ) )
                        {
                        std::snprintf( error_text, sizeof( error_text ),
                            "PDI 0x0037 slot=%u error=%04X size=%zu",
                            module->getSlotNumber(), result.ErrorCode,
                            record.size() );
                        return false;
                        }
                    process_bytes.push_back( bytes );
                    }
                metadata_ready = true;
                error_text[0] = 0;
                return true;
                }

            const char* get_error_text() const override { return error_text; }

            bool ready() override
                {
                auto diag = bus->getDiagnosticsInfo();
                // RUNNING, ACTIVE, BUS_READY; ошибки шины и контроллера.
                bool operational = !diag.driverError &&
                    ( diag.status & 0x00E0 ) == 0x00E0 &&
                    ( diag.status & 0x010C ) == 0;
                if ( !operational )
                    {
                    std::snprintf( error_text, sizeof( error_text ),
                        "Axiobus status=%04X param1=%04X param2=%04X "
                        "driver_error=%d", diag.status, diag.param1,
                        diag.param2, diag.driverError );
                    }
                return operational;
                }

            size_t module_count() override
                {
                return bus->getModules().size();
                }

            module_info module( size_t index ) override
                {
                auto module = bus->getModules().at( index );
                return { module->getOrderNumber(),
                    process_bytes.at( index ), module->isMissing() };
                }

            bool read_inputs() override { return bus->readInputs(); }
            bool write_outputs() override { return bus->writeOutputs(); }

            void read_module( size_t index, unsigned char* data,
                unsigned int bytes ) override
                {
                bus->getModules().at( index )->getInputProcessData(
                    data, 0, 0, bytes );
                }

            void write_module( size_t index, unsigned char* data,
                unsigned int bytes ) override
                {
                bus->getModules().at( index )->setOutputProcessData(
                    data, 0, 0, bytes );
                }

        private:
            bool check_access( const char* path, bool optional )
                {
                struct stat info;
                if ( stat( path, &info ) != 0 )
                    {
                    if ( optional && errno == ENOENT ) return true;
                    }
                else if ( access( path, R_OK | W_OK ) == 0 ) return true;
                auto code = errno;
                std::snprintf( error_text, sizeof( error_text ),
                    "%s: %s (%d)", path, std::strerror( code ), code );
                return false;
                }

            std::unique_ptr<PLCnext::Axiobus> bus;
            std::vector<unsigned int> process_bytes;
            bool metadata_ready = false;
            char error_text[256] = {};
        };
    }
#endif

std::unique_ptr<local_bus_driver> make_local_bus_driver()
    {
#ifdef PTUSA_AXIOBUS
    return std::make_unique<axiobus_driver>();
#else
    return nullptr;
#endif
    }
