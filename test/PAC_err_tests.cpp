#include "PAC_err_tests.h"

using namespace ::testing;

TEST( PAC_critical_errors_manager, set_global_error )
    {
    auto mngr = PAC_critical_errors_manager::get_instance();
    EXPECT_EQ( false, mngr->is_any_error() );

    mngr->set_global_error( PAC_critical_errors_manager::AC_UNKNOWN,
        PAC_critical_errors_manager::AS_UNKNOWN, 1 );

    EXPECT_EQ( true, mngr->is_any_error() );
    mngr->reset_global_error( PAC_critical_errors_manager::AC_UNKNOWN,
        PAC_critical_errors_manager::AS_UNKNOWN, 1 );
    EXPECT_EQ( false, mngr->is_any_error() );
    }
