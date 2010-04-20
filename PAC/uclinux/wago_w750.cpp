#include "wago_w750.h"

//-----------------------------------------------------------------------------
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

        int write_outputs()
            {
            return 0;
            }

        int get_DO( u_int table_n, u_int offset )
            {
            return 0;
            }
        int set_DO( u_int table_n, u_int offset, char value )
            {
            return 0;
            }

        int get_DI( u_int table_n, u_int offset )
            {
            return 0;
            }
        int set_DI( u_int table_n, u_int offset, char value )
            {
            return 0;
            }

        float get_AO( u_int table_n, u_int offset )
            {
            return 0;
            } 
        int   set_AO( u_int table_n, u_int offset, float value )
            {
            return 0;
            }

        float get_AI( u_int table_n, u_int offset )
            {
            return 0;
            }
        int   set_AI( u_int table_n, u_int offset, float value )
            {
            return 0;
            }
    };