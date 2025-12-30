#include "dtime_test.h"

uint32_t get_delta_millisec_test( uint32_t time1 )
    {
    uint32_t now = 0;
    // Unsigned integer subtraction in C++ handles wraparound correctly via
    // modular arithmetic.
    return now - time1;
    }

TEST( sys, get_delta_millisec )
    {
    auto dt = get_delta_millisec( 0 );
    EXPECT_GT( dt, 0 );

    auto now = get_millisec();
    dt = get_delta_millisec( now );
    // Сразу же измеряем пройденное время - должно быть меньше 100 мс.
    EXPECT_LE( dt, 100 );

    dt = get_delta_millisec_test( UINT32_MAX );
    EXPECT_EQ( dt, 1 );

    dt = get_delta_millisec_test( 1 );
    EXPECT_EQ( dt, UINT32_MAX );
    }
