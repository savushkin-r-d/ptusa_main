#ifndef WAGO_750_H
#define WAGO_750_H

#include "wago.h"

#include "kbusapi.h"

/// @brief
///
///
class wago_manager_w750 : public wago_manager
    {
    public:
        wago_manager_w750();

        virtual ~wago_manager_w750();

        int read_inputs();

        virtual int write_outputs();
    };

#endif // WAGO_750_H

