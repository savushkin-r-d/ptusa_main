#include "stdafx.h"

#include "plant.h"

#include <math.h>
//------------------------------------------------------------------------------
plant::plant( float h = 1, int z = 3, float U = 0.5 ):previous_obj_v1( 10 ),
      current_obj_v( 10 ),
    K( 525.f ), T( .450f ), h( h ), z( z ), k1( 1 ), U( U )
    {
    for ( int i = 0; i < z + Z; i++ )
    	{
        prev_control.push_back( 0 );
    	}
    }
//------------------------------------------------------------------------------
float plant::get_new_out( float control_value )
    {
    control_value = U * control_value / 100;

    float k = K;

    float g = exp( -h / T );
    float A = k / ( 2 * h * h ) * ( ( 1 - g ) * ( 2 * T * T - T * h ) + 2 * h * h - 2 * T * h );
    float B = -k / ( h * h ) * ( ( 1 - g ) * ( 2 * T * T - h * h ) + h * h - 2 * T * h );
    float C = k / ( 2 * h * h ) * ( ( 1 - g ) * ( 2 * T * T + T * h ) - 2 * T * h );

    //A = 2;
   //B = 1;
    //C = 1;

    //Сдвиг управляющих сигналов.
        for ( int i = z + Z - 1; i > 0; i-- )
        {
        prev_control[ i ] = prev_control[ i - 1 ];
        } 
    prev_control[ 0 ] = control_value;

    float res = A * prev_control[ z ] + B * prev_control[ z + 1 ] +
        C * prev_control[ z + 2 ];
    res += k1 * g * previous_obj_v1; 
       

    if ( res < 0 )
        {
        res = 0;
        }
    if ( res > 120 )
        {
        res = 120;
        }

    previous_obj_v1 = current_obj_v;
    current_obj_v = res;   

    return res;
    }
//------------------------------------------------------------------------------
float plant::get_current_out() const
    {
    return current_obj_v;
    }
//------------------------------------------------------------------------------
float plant::get_current_control_v() const
    {
    return 100 * prev_control[ 0 ] / U;
    }
//-----------------------------------------------------------------------------
