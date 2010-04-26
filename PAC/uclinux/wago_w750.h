#include "wago.h"

/// @brief
///
///
class wago_manager_w750 : public wago_manager
    {
    public:
        int read_inputs()
            {
            return 0;
            }

        virtual int write_outputs()
            {
            return 0;
            }
    };
