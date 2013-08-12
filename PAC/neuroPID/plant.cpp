#include "stdafx.h"

#include "plant.h"

#include <math.h>
//---------------------------------------------------------------------------------------
plant::plant():prev_obj_value( 0 ),
    prev_control_value( 0 ),
    current_obj_value( 0 ),
    k( 0.9 )
    {
    }
//---------------------------------------------------------------------------------------
double plant::get_new_out( double control_value )
    {
    double res = k * current_obj_value - 0.002 * prev_obj_value * prev_obj_value +
        0.9 * control_value + 0.1 * sin( prev_control_value );
    if ( res < 0 )
        {
        res = 0;
        }
    if ( res > 120 )
        {
        res = 120;
        }

    prev_obj_value = current_obj_value;
    current_obj_value = res;
    prev_control_value = control_value;

    return res;
    }
//---------------------------------------------------------------------------------------
double plant::get_current_out() const
    {
    return current_obj_value;
    }
//---------------------------------------------------------------------------------------