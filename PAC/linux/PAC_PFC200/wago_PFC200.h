/// @file wago_w750.h
/// @brief Работа с Wago для PAC WAGO 750-860.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG      - компиляция c выводом отладочной информации в консоль.
/// @c DEBUG_KBUS - компиляция c выводом отладочной информации работы KBUS в
/// консоль.
///
/// @par Текущая версия:
/// @$Rev: 588 $.\n
/// @$Author: id $.\n
/// @$Date:: 2013-01-21 08:47:30#$.

#ifndef WAGO_750_H
#define WAGO_750_H

#include "l_wago.h"

extern "C" {
#include <dal/adi_application_interface.h>
}
//-----------------------------------------------------------------------------
/// @brief Работа с модулями ввода/вывода Wago для PAC WAGO 750-860.
///
///
class wago_manager_PFC200 : public wago_manager_linux
    {
     public:
         
        wago_manager_PFC200();

        virtual ~wago_manager_PFC200();

        int read_inputs();
        
        int write_outputs();
        
    private:
        // vars for ADI-interface
        tDeviceInfo deviceList[ 10 ]; /// The list of devices given by the ADI.
        size_t nr_devices_found;      /// Number of devices found.
        size_t nr_kbus_found;         /// Position of the kbus in the list.
        tDeviceId kbus_device_id;     /// Device ID from the ADI.
        uint32_t task_id;             /// Task Id. 
    
        // process data
        uint8_t pd_in[ 4096 ];    /// Kbus input process data.
        uint8_t pd_out[ 4096 ];   /// Kbus output process data.
    
        ///< Pointer to the application interface.
        tApplicationDeviceInterface * adi;   
    };
//-----------------------------------------------------------------------------
#endif // WAGO_750_H
