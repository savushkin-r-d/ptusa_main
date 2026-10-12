#pragma once

#include <cstdint>
#include <cstring>
#include <stdexcept>

/// Доступ к буферам AXLModule с длиной, проверенной по PDI 0x0037.
/// В libAxiobus 11.2 m_processDataLength не инициализировано, поэтому
/// getInputProcessData/setOutputProcessData могут копировать ноль байт.
/// Указатели на защищённые члены позволяют использовать настоящий объект
/// базового класса без приведения его к несуществующему наследнику.
template<class Module>
class axiobus_process_data : public Module
    {
    public:
        static void read( const Module& module, unsigned char* data,
            unsigned int bytes, unsigned int module_bytes )
            {
            auto member = &axiobus_process_data::m_inOffset;
            auto source = reinterpret_cast<const unsigned char*>(
                module.*member );
            check( source, data, bytes, module_bytes );
            if ( bytes ) std::memcpy( data, source, bytes );
            }

        static void write( Module& module, const unsigned char* data,
            unsigned int bytes, unsigned int module_bytes )
            {
            auto member = &axiobus_process_data::m_outOffset;
            auto target = reinterpret_cast<unsigned char*>( module.*member );
            check( data, target, bytes, module_bytes );
            if ( bytes ) std::memcpy( target, data, bytes );
            }

    private:
        static void check( const void* source, const void* target,
            unsigned int bytes, unsigned int module_bytes )
            {
            if ( module_bytes > 1024 || bytes > module_bytes )
                {
                throw std::runtime_error(
                    "размер данных превышает длину модуля из PDI 0x0037" );
                }
            if ( bytes && ( !source || !target ) )
                {
                throw std::runtime_error(
                    "недоступен буфер данных модуля Axiobus" );
                }
            }
    };
