#include "stdafx.h"

#include "plant.h"

#include <math.h>
//------------------------------------------------------------------------------
plant::plant( float h = 1 ):prev_obj_v1( 10 ),
    prev_control_v1( 0 ),
    prev_control_v2( 0 ),
    prev_control_v3( 0 ),
    prev_control_v4( 0 ),
    prev_control_v5( 0 ),
    current_obj_value( 0 ),
    k( 0.9f ), T( 400 ), h( h )
    {
    }
//------------------------------------------------------------------------------
float plant::get_new_out( float control_value )
    {
    float res = k * current_obj_value - 0.002f * prev_obj_v1 * prev_obj_v1 +
        0.9f * control_value + 0.1f * sin( prev_control_v1 );
    if ( res < 0 )
        {
        res = 0;
        }
    if ( res > 120 )
        {
        res = 120;
        }

    prev_obj_v1 = current_obj_value;
    current_obj_value = res;
    prev_control_v1 = control_value;

    return res;
    }
//------------------------------------------------------------------------------
float plant::get_current_out() const
    {
    return current_obj_value;
    }
//------------------------------------------------------------------------------