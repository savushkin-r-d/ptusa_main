#include "dtime_test.h"

uint32_t mock_get_millisec()
    {
    return 0;
    }

TEST( sys, get_delta_millisec )
    {
    auto hook_get_millisec = subhook_new( reinterpret_cast<void*>( &get_millisec ),
        reinterpret_cast<void*>( &mock_get_millisec ), SUBHOOK_64BIT_OFFSET );
    auto res = subhook_install( hook_get_millisec );
    ASSERT_EQ( res, 0 );

    auto dt = get_delta_millisec( UINT32_MAX );
    EXPECT_EQ( dt, 1 );

    dt = get_delta_millisec( 1 );
    EXPECT_EQ( dt, UINT32_MAX );

    subhook_remove( hook_get_millisec );
    subhook_free( hook_get_millisec );
    }
