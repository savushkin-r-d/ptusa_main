/// @file wago_w750.h
/// @brief Р Р°Р±РѕС‚Р° СЃ Wago РґР»СЏ PAC WAGO 750-860.
///
/// @author  РРІР°РЅСЋРє Р”РјРёС‚СЂРёР№ РЎРµСЂРіРµРµРІРёС‡.
///
/// @par РћРїРёСЃР°РЅРёРµ РґРёСЂРµРєС‚РёРІ РїСЂРµРїСЂРѕС†РµСЃСЃРѕСЂР°:
/// @c DEBUG_KBUS - РєРѕРјРїРёР»СЏС†РёСЏ c РІС‹РІРѕРґРѕРј РѕС‚Р»Р°РґРѕС‡РЅРѕР№ РёРЅС„РѕСЂРјР°С†РёРё СЂР°Р±РѕС‚С‹ KBUS РІ
/// РєРѕРЅСЃРѕР»СЊ.
///
/// @par РўРµРєСѓС‰Р°СЏ РІРµСЂСЃРёСЏ:
/// @$Rev: 588 $.\n
/// @$Author: id $.\n
/// @$Date:: 2013-01-21 08:47:30#$.

#ifndef WAGO_750_H
#define WAGO_750_H

#include "l_bus_coupler_io.h"

extern "C" {
#include <dal/adi_application_interface.h>
}
//-----------------------------------------------------------------------------
/// @brief Р Р°Р±РѕС‚Р° СЃ РјРѕРґСѓР»СЏРјРё РІРІРѕРґР°/РІС‹РІРѕРґР° Wago РґР»СЏ PAC WAGO 750-860.
///
///
class io_manager_PFC200 : public io_manager_linux
    {
     public:

        io_manager_PFC200();

        virtual ~io_manager_PFC200();

        int read_inputs();

        int write_outputs();

    private:
        // vars for ADI-interface
        tDeviceInfo devList[ 10 ]; /// The list of devices given by the ADI.
        size_t nr_devices_found;   /// Number of devices found.
        size_t nr_kbus_found;      /// Position of the kbus in the list.
        tDeviceId kbus_device_id;  /// Device ID from the ADI.
        uint32_t task_id;          /// Task Id.

        // process data
        uint8_t pd_in[ 4096 ];     /// Kbus input process data.
        uint8_t pd_out[ 4096 ];    /// Kbus output process data.

        ///< Pointer to the application interface.
        tApplicationDeviceInterface * adi;
    };
//-----------------------------------------------------------------------------
#endif // WAGO_750_H
