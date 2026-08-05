#include "params_ex_tests.h"

using namespace ::testing;


TEST( params_manager, evaluate )
    {
    auto pm = params_manager::get_instance();
    pm->init( 0x12345678 );
    pm->par->save( 1, 0xDEADBEEF );
    EXPECT_EQ( 0xDEADBEEF, pm->par[ 0 ][ params_manager::P_IS_RESET_PARAMS ] );

    pm->save();
    pm->evaluate();
    }

TEST( params_manager, get_params_data )
    {
    auto pm = params_manager::get_instance();
    int start_pos;

    // Запрос большего количества данных, чем есть в памяти. Результат должен
    // быть `nullptr`.
    auto data = pm->get_params_data(
        static_cast<int>( params_manager::CONSTANTS::C_TOTAL_PARAMS_SIZE ) + 1,
        start_pos );
    EXPECT_EQ( data, nullptr );
    }
