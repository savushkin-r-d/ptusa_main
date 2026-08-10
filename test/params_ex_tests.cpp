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

TEST( params_manager, reserve_params_region )
    {
    auto pm = params_manager::get_instance();
    int start_pos;

    // Запрос большего количества параметров, чем есть в памяти. Результат
    // должен быть `nullptr`.
    auto data = pm->reserve_params_region(
        static_cast<int>( params_manager::CONSTANTS::C_TOTAL_PARAMS_SIZE ) + 1,
        start_pos );
    EXPECT_EQ( data, nullptr );
    }


TEST( memory_range, get_memory_block )
    {
    auto mm = NV_memory_manager::get_instance();

    // Запрос большего количества памяти, чем есть в NVRAM. Результат должен
    // быть объект с размером 0.
    auto bad_mem = mm->get_memory_block(
        NV_memory_manager::MEMORY_TYPE::MT_NVRAM, 100 );
    EXPECT_EQ( 0, bad_mem->get_size() );

    std::array<std::byte, 10> buff{};
    // Попытка прочитать для объекта, у которого не выделено памяти. Результат
    // должен быть 2.
    auto res = bad_mem->read( buff.data(), 10 );
    EXPECT_EQ( 2, res );

    // Попытка записать для объекта, у которого не выделено памяти. Результат
    // должен быть 1.
    res = bad_mem->safe_save( buff.data() );
    EXPECT_EQ( 1, res );

    // Попытка заполнить нулями для объекта, у которого не выделено памяти.
    // Результат должен быть 1.
    res = bad_mem->zero_fill();
    EXPECT_EQ( 1, res );


    auto good_mem = mm->get_memory_block(
        NV_memory_manager::MEMORY_TYPE::MT_NVRAM, 1 );
    EXPECT_EQ( 1, good_mem->get_size() );

    // Попытка прочитать больше памяти, чем выделено. Результат должен быть 1.
    res = good_mem->read( buff.data(), 100 );
    EXPECT_EQ( 1, res );
    }

namespace
    {
    FILE* bad_fopen( [[maybe_unused]] const char* filename,
        [[maybe_unused]] const char* mode )
        {
        return nullptr;
        }

    int bad_fread( [[maybe_unused]] void* ptr,
        [[maybe_unused]] size_t size,
        [[maybe_unused]] size_t count,
        [[maybe_unused]] FILE* stream )
        {
        return 0;
        }
    }

TEST( SRAM, constructor )
    {
    auto fopen_hook = subhook_new( reinterpret_cast<void*>( &fopen ),
        reinterpret_cast<void*>( &bad_fopen ), SUBHOOK_64BIT_OFFSET );
    subhook_install( fopen_hook );

    SRAM bad_sram( "test_sram1.bin", 1024 );
    EXPECT_EQ( 2, bad_sram.read( nullptr, 10, 5 ) );

    subhook_remove( fopen_hook );
    }

TEST( SRAM, read )
    {
    auto fread_hook = subhook_new( reinterpret_cast<void*>( &fread ),
        reinterpret_cast<void*>( &bad_fread ), SUBHOOK_64BIT_OFFSET );
    subhook_install( fread_hook );

    SRAM good_sram( "test_sram2.bin", 1024 );
    EXPECT_EQ( 1, good_sram.read( nullptr, 10, 5 ) );

    subhook_remove( fread_hook );
    }

TEST( SRAM, safe_save )
    {
    SRAM good_sram( "test_sram2.bin", 1024 );

    auto fopen_hook = subhook_new( reinterpret_cast<void*>( &fopen ),
        reinterpret_cast<void*>( &bad_fopen ), SUBHOOK_64BIT_OFFSET );
    subhook_install( fopen_hook );

    std::array<std::byte, 10> buff{};
    EXPECT_EQ( 1, good_sram.safe_save( buff.data() ) );

    subhook_remove( fopen_hook );
    }
