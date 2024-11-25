#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <subhook.h>
#include <vector>
#include <limits.h>

unsigned long subhook_for_get_delta_millisec( unsigned long );

unsigned long get_delta_millisec( unsigned long time1 );

const auto GTESTS_DELTA_MILLISEC_SUBHOOK = subhook_new(
    reinterpret_cast<void*>( &get_delta_millisec ),
    reinterpret_cast<void*>( &subhook_for_get_delta_millisec ),
    SUBHOOK_64BIT_OFFSET );

class DeltaMilliSecSubHooker 
    {
    inline static unsigned long millitime;

public:

    static unsigned long get_millisec()
        {
        return millitime;
        }

    static void set_millisec( unsigned long millisec )
        {
        millitime = millisec;
        subhook_install( GTESTS_DELTA_MILLISEC_SUBHOOK );
        }

    static void set_default_time() 
        {
        millitime = 1UL;
        subhook_remove( GTESTS_DELTA_MILLISEC_SUBHOOK );
        }

    explicit DeltaMilliSecSubHooker( unsigned long millisec ) 
        {
        DeltaMilliSecSubHooker::set_millisec( millisec );
        }
    };
