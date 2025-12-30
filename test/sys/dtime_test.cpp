#include "dtime_test.h"

uint32_t mock_get_millisec()
    {
    return 0;
    }

TEST( sys, get_delta_millisec )
    {
    auto hook_get_millisec = subhook_new( (void*)get_millisec,
        (void*)mock_get_millisec, SUBHOOK_64BIT_OFFSET );
    subhook_install( hook_get_millisec );

    auto res = get_delta_millisec( UINT32_MAX );
    ASSERT_EQ( res, 1 );

    res = get_delta_millisec( 1 );
    ASSERT_EQ( res, UINT32_MAX );

    subhook_remove( hook_get_millisec );
    subhook_free( hook_get_millisec );
    }
