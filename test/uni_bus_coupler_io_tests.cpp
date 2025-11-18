#include "uni_bus_coupler_io_tests.h"

using namespace ::testing;

#ifndef WIN_OS // For linux to deal with __stdcall.
#define __stdcall
#endif

int __stdcall fail_socket( int, int, int )
    {
    return -1;
    }

int __stdcall fail_setsockopt( int, int, int, const char*, int )
    {
    return 1;
    }

#ifdef WIN_OS
int __stdcall fail_ioctlsocket( int, long, long )
    {
    return 1;
    }
#else
int fail_fcntl( int, int )
    {
    return 1;
    }
#endif

int __stdcall fail_select_m1( int, fd_set*, fd_set*, fd_set*, struct timeval* )
    {
    return -1;
    }

int __stdcall fail_select_0( int, fd_set*, fd_set*, fd_set*, struct timeval* )
    {
    return 0;
    }

int __stdcall success_select( int, fd_set*, fd_set*, fd_set*, struct timeval* )
    {
    return 1;
    }

int __stdcall fail_getsockopt( int, int, int, char*, int* )
    {
    return -1;
    }

int __stdcall success_getsockopt( int, int, int, char*, int* )
    {
    return 0;
    }

TEST( uni_io_manager, net_init )
    {
    uni_io_manager mngr;

    // Should fail - node is null.
    auto res = mngr.net_init( nullptr );
    EXPECT_EQ( res, 1 );

    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1000, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );

    subhook_t socket_hook = subhook_new( reinterpret_cast<void*>( socket ),
        reinterpret_cast<void*>( fail_socket ), SUBHOOK_64BIT_OFFSET );
    subhook_install( socket_hook );
    // Should fail - fail with create socket.
    EXPECT_EQ( 3, mngr.net_init( &node ) );
    subhook_remove( socket_hook );

    subhook_t setsockopt_hook = subhook_new( reinterpret_cast<void*>( setsockopt ),
        reinterpret_cast<void*>( fail_setsockopt ), SUBHOOK_64BIT_OFFSET );
    subhook_install( setsockopt_hook );
    // Should fail - fail with setsockopt().
    EXPECT_EQ( 4, mngr.net_init( &node ) );
    subhook_remove( setsockopt_hook );

#ifdef WIN_OS
    subhook_t ioctlsocket_hook = subhook_new( reinterpret_cast<void*>( ioctlsocket ),
        reinterpret_cast<void*>( fail_ioctlsocket ), SUBHOOK_64BIT_OFFSET );
    subhook_install( ioctlsocket_hook );
    // Should fail - fail with fcntl().
    EXPECT_EQ( 5, mngr.net_init( &node ) );
    subhook_remove( ioctlsocket_hook );
#else
    subhook_t fcntl_hook = subhook_new( reinterpret_cast<void*>( fcntl ),
        reinterpret_cast<void*>( fail_fcntl ), SUBHOOK_64BIT_OFFSET );
    subhook_install( fcntl_hook );
    // Should fail - fail with fcntl().
    EXPECT_EQ( 5, mngr.net_init( &node ) );
    subhook_remove( fcntl_hook );
#endif

    subhook_t select_m1_hook = subhook_new( reinterpret_cast<void*>( select ),
        reinterpret_cast<void*>( fail_select_m1 ), SUBHOOK_64BIT_OFFSET );
    subhook_install( select_m1_hook );
    // Should fail - fail with select() - 1.
    EXPECT_EQ( 6, mngr.net_init( &node ) );
    subhook_remove( select_m1_hook );
    subhook_t select_0_hook = subhook_new( reinterpret_cast<void*>( select ),
        reinterpret_cast<void*>( fail_select_0 ), SUBHOOK_64BIT_OFFSET );
    subhook_install( select_0_hook );
    // Should fail - fail with select() - 0.
    EXPECT_EQ( 6, mngr.net_init( &node ) );
    subhook_remove( select_0_hook );

    subhook_t getsockopt_1_hook = subhook_new( reinterpret_cast<void*>( getsockopt ),
        reinterpret_cast<void*>( fail_getsockopt ), SUBHOOK_64BIT_OFFSET );
    subhook_install( getsockopt_1_hook );
    subhook_t select_1_hook = subhook_new( reinterpret_cast<void*>( select ),
        reinterpret_cast<void*>( success_select ), SUBHOOK_64BIT_OFFSET );
    subhook_install( select_1_hook );
    // Should fail - fail with getsockopt() - 1.
    EXPECT_EQ( 7, mngr.net_init( &node ) );
    subhook_remove( getsockopt_1_hook );
    subhook_remove( select_1_hook );

    subhook_install( select_1_hook );
    subhook_t getsockopt_0_hook = subhook_new( reinterpret_cast<void*>( getsockopt ),
        reinterpret_cast<void*>( success_getsockopt ), SUBHOOK_64BIT_OFFSET );
    subhook_install( getsockopt_0_hook );
    // Should OK.
    EXPECT_EQ( 0, mngr.net_init( &node ) );
    subhook_remove( select_1_hook );
    subhook_remove( getsockopt_0_hook );

    mngr.disconnect( &node );

    subhook_free( socket_hook );
    subhook_free( setsockopt_hook );
#ifdef WIN_OS
    subhook_free( ioctlsocket_hook );
#else
    subhook_free( fcntl_hook );
#endif    
    subhook_free( select_m1_hook );
    subhook_free( select_0_hook );
    subhook_free( getsockopt_1_hook );
    subhook_free( select_1_hook );
    subhook_free( getsockopt_0_hook );
    }

class test_uni_io_manager : public uni_io_manager
    {
    public:
        int e_communicate( io_node* node, int bytes_to_send,
            int bytes_to_receive ) override
            {
            if ( is_set_function_code )
                {
                buff[ 7 ] = function_code;
                }
            return res;
            }

        void set_result_to_error()
            {
            res = 1;
            }

        void set_result_to_ok()
            {
            res = 0;
            }

        void set_function_code( u_char f_code )
            {
            function_code = f_code;
            is_set_function_code = true;
            }

        void reset_function_code()
            {
            is_set_function_code = false;
            }

    private:
        int res = 0;
        u_char function_code;
        bool is_set_function_code = false;
    };

TEST( uni_io_manager, read_inputs )
    {
    test_uni_io_manager mngr;
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );

    // Should not fail - nodes count is 0.
    auto res = mngr.read_inputs();
    EXPECT_EQ( res, 0 );

    mngr.init( 4 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    mngr.add_node( 1, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        2, "127.0.0.1", "A200", 1, 1, 1, 1, 1, 1 );
    mngr.get_node( 1 )->is_active = false;
    mngr.add_node( 2, io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        3, "127.0.0.1", "A300", 1, 1, 1, 1, 1, 1 );
    mngr.add_node( 3, io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        4, "127.0.0.1", "A400", 1, 1, 1, 1, 1, 1 );
    mngr.get_node( 3 )->is_active = false;

    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_fixed_time ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );

    testing::internal::CaptureStdout();

    mngr.reset_function_code();
    mngr.set_result_to_ok();
    res = mngr.read_inputs();
    EXPECT_EQ( res, 1 );
    const std::string EXP_TIME = "2025-03-12 00.00.00 ";
    const std::string expectedOutput =
        EXP_TIME + ANSI_COLOR_GREEN +
        R"(ERROR  (3) -> Read DI:bus coupler returned error. "A300":"127.0.0.1" )"
        "(received code=2, expected=2, received size=0, expected=1).\n" +
        ANSI_COLOR_RESET +
        EXP_TIME + ANSI_COLOR_RED +
        R"(ERROR  (3) -> Read AI:bus coupler returned error. "A100":"127.0.0.1" )"
        "(received code=4, expected=4, received size=31, expected=2).\n" +
        ANSI_COLOR_RESET
        ;
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, expectedOutput );


    subhook_remove( get_time_hook );
    subhook_free( get_time_hook );
    io_manager::replace_instance( prev_mngr );
    }

TEST( uni_io_manager, write_outputs )
    {
    test_uni_io_manager mngr;
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );

    // Should not fail - nodes count is 0.
    auto res = mngr.write_outputs();
    EXPECT_EQ( res, 0 );

    mngr.init( 4 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 6*16, 6*16, 6, 6, 1, 1 );
    // Just for testing add set of modules.
    mngr.init_node_AO( 0, 0, 1027843, 0 );
    mngr.init_node_AO( 0, 1, 2688093, 1 );
    mngr.init_node_AO( 0, 2, 2688527, 2 );
    mngr.init_node_AO( 0, 3, 2688527, 3 );
    mngr.init_node_AO( 0, 4, 2688666, 4 );
    mngr.init_node_AO( 0, 5, 10, 5 );

    mngr.add_node( 1, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        2, "127.0.0.1", "A200", 1, 1, 1, 1, 1, 1 );
    mngr.get_node( 1 )->is_active = false;
    mngr.add_node( 2, io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        3, "127.0.0.1", "A300", 1, 1, 2, 4, 1, 1 );
    mngr.init_node_AO( 2, 0, 638, 0 );
    mngr.init_node_AO( 2, 1, 10, 2 );
    mngr.add_node( 3, io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        4, "127.0.0.1", "A400", 1, 1, 1, 1, 1, 1 );
    mngr.get_node( 3 )->is_active = false;

    // Должна быть успешная запись.
    res = mngr.write_outputs();
    EXPECT_EQ( res, 0 ); 

    // Должна быть неуспешная запись (e_communicate() возвращает 1 - 
    // неуспешная сетевая операция). Сообщений не должно быть.
    std::string expectedOutput = "";
    testing::internal::CaptureStdout();
    mngr.set_result_to_error();
    res = mngr.write_outputs();
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( res, 1 );
    EXPECT_EQ( output, expectedOutput );

    // Должна быть неуспешная запись (ошибки на уровне данных). Сообщения должны быть.
    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_fixed_time ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );
    const std::string EXP_TIME = "2025-03-12 00.00.00 ";
    expectedOutput =
        EXP_TIME +
#if defined LINUX_OS
        "\x1B[31m" +
#endif
        R"(ERROR  (3) -> Write DO:bus coupler returned error. "A300":"127.0.0.1" )"
        "(received code=1, expected=15, received size=0, expected=1).\n" +
#if defined LINUX_OS
        "\x1B[0m" +
#endif
        EXP_TIME +
#if defined LINUX_OS        
        "\x1B[31m" +
#endif
        R"(ERROR  (3) -> Write AO:bus coupler returned error. "A100":"127.0.0.1" )"
        "(received code=1, expected=16, received size=35, expected=6).\n"
#if defined LINUX_OS
        + "\x1B[0m"
#endif
        ;
    testing::internal::CaptureStdout();
    mngr.set_function_code( 1 );
    mngr.set_result_to_ok();
    res = mngr.write_outputs();
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( res, 1 );
    EXPECT_EQ( output, expectedOutput );


    subhook_remove( get_time_hook );
    subhook_free( get_time_hook );
    io_manager::replace_instance( prev_mngr );
    }

TEST( uni_io_manager, read_write_data )
    {
    test_uni_io_manager mngr;
    io_manager* prev_mngr = io_manager::replace_instance( &mngr );

    mngr.init( 2 );
    mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );
    mngr.add_node( 1, io_manager::io_node::TYPES::WAGO_750_XXX_ETHERNET,
        2, "127.0.0.1", "A200", 1, 1, 1, 1, 1, 1 );

    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_fixed_time ),
        SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );

    mngr.set_result_to_error();
    auto res = mngr.read_inputs();
    auto temp_node = mngr.get_node( 0 );
    EXPECT_TRUE( temp_node->read_io_error_flag );
    EXPECT_EQ( res, 1 );

    // Так ранее попытка чтения состояния узлов была неуспешной,
    // записи выходов не будет.
    res = mngr.write_outputs();
    temp_node = mngr.get_node( 1 );
    EXPECT_TRUE( temp_node->read_io_error_flag );
    EXPECT_EQ( res, 1 );


    subhook_remove( get_time_hook );
    subhook_free( get_time_hook );
    io_manager::replace_instance( prev_mngr );
    }

TEST( uni_io_manager, e_communicate )
    {
    uni_io_manager mngr;
    io_manager::io_node node( io_manager::io_node::TYPES::PHOENIX_BK_ETH,
        1000, "127.0.0.1", "A100", 1, 1, 1, 1, 1, 1 );

    auto res = mngr.e_communicate( &node, 1, 1 );
    EXPECT_NE( res, 0 );
    }


class MockUniIoManager : public uni_io_manager
    {
    public:
        MOCK_METHOD( void, add_err_to_log, ( const char*, const char*,
            const char*, int, int, int, int ), ( const, override ) );
        // Переопределяем e_communicate, чтобы не было реального обмена
        MOCK_METHOD( int, e_communicate, ( io_node*, int, int ), ( override ) );
    };

class UniBusCouplerIoTest : public ::testing::Test
    {
    protected:
        void SetUp() override 
            {
            mngr.init( 1 );
            mngr.add_node( 0, io_manager::io_node::TYPES::PHOENIX_BK_ETH,
                1, "127.0.0.1", "A100", 6 * 16, 6 * 16, 6, 6, 1, 1 );
            node = mngr.get_node( 0 );
            // Just for testing add set of modules.
            mngr.init_node_AO( 0, 0, 1027843, 0 );
            mngr.init_node_AO( 0, 1, 2688093, 1 );
            mngr.init_node_AO( 0, 2, 2688527, 2 );
            mngr.init_node_AO( 0, 3, 2688527, 3 );
            mngr.init_node_AO( 0, 4, 2688666, 4 );
            mngr.init_node_AO( 0, 5, 10, 5 );
            };

        io_manager::io_node* node;
        NiceMock<MockUniIoManager> mngr;
    };

TEST_F( UniBusCouplerIoTest, write_outputs_PHOENIX_BK_ETH )
    {    
    //Проверяем обработку неуспешной записи выходов (сетевая ошибка).
    ON_CALL( mngr, e_communicate( _, _, _ ) ).WillByDefault(
        []( const io_manager::io_node*, int, int )
        {
        return -1;
        } );
    EXPECT_CALL( mngr, add_err_to_log( _, _, _, _, _, _, _ ) )
        .Times( 0 );

    auto res = mngr.write_outputs();
    // Проверяем, что флаг не изменился (остался false).
    EXPECT_FALSE( node->flag_error_write_message );
    EXPECT_EQ( res, 1 );

    //Проверяем обработку получения ошибки при записи выходов.
    ON_CALL( mngr, e_communicate( _, _, _ ) ).WillByDefault(
        [this]( const io_manager::io_node*, int, int )
        {
        mngr.buff[ 7 ] = 0x11; // Любое значение, кроме 0x10.
        return 0;
        } );   
    uni_io_manager real_mngr;
    EXPECT_CALL( mngr, add_err_to_log( ::testing::StrEq( "Write AO" ),
        _, _, 0x11, 0x10, 35, 6 ) )
        .WillRepeatedly( Invoke( &real_mngr, &uni_io_manager::add_err_to_log ) );    
    testing::internal::CaptureStdout();
    res = mngr.write_outputs();
    auto output = testing::internal::GetCapturedStdout();
    // Проверяем, что флаг изменился (стал true).
    EXPECT_TRUE( node->flag_error_write_message );
    EXPECT_EQ( res, 1 );
    EXPECT_NE( output, "" ); //Здесь должно быть сообщение.

    testing::internal::CaptureStdout();
    res = mngr.write_outputs();
    EXPECT_EQ( res, 1 );
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ( output, "" ); //Здесь уже не должно быть сообщения.
    }
