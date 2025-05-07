#include "tcp_cmctr_tests.h"

using namespace ::testing;

TEST( tcp_communicator, evaluate )
    {    
    tcp_communicator::set_port( 30000, 30001 );
    tcp_communicator::init_instance( "Тест", "Test" );    

    EXPECT_EQ( 0, G_CMMCTR->evaluate() );

#ifdef LINUX_OS
    linux_tcp_client cl( "127.0.0.1", G_CMMCTR->get_port(), 1, 1 );
#else
    win_tcp_client cl( "127.0.0.1", G_CMMCTR->get_port(), 1, 1 );
    cl.InitLib();
#endif

    EXPECT_EQ( tcp_client::ASYNCCONNECTSTATE::ACS_DISCONNECTED,
        cl.get_connected_state() );
    cl.reconnectTimeout = 0;
    auto cnt = 0;
    auto r = cl.AsyncConnect();
    while ( r != tcp_client::ASYNCCONNECTSTATE::ACS_CONNECTED && cnt < 1000 )
        {
        sleep_ms( 1 );
        r = cl.AsyncConnect();
        cnt++;
        }
    ASSERT_EQ( tcp_client::ASYNCCONNECTSTATE::ACS_CONNECTED,
        cl.get_connected_state() );    

    EXPECT_EQ( 0, G_CMMCTR->evaluate() );
    //Должен быть получен ответ на подключение.
    auto size = cl.AsyncRecive();
    cnt = 0;
    while ( size <= 0 && cnt < 1000 )
        {
        sleep_ms( 1 );
        size = cl.AsyncRecive();
        cnt++;
        }
    ASSERT_GT( size, 0 );
    cl.buff[ size ] = '\0';
    EXPECT_STREQ( cl.buff, "PAC accept" );

    cl.AsyncSend( 10 );
    G_CMMCTR->evaluate();

    cl.Disconnect();
    EXPECT_EQ( 0, G_CMMCTR->evaluate() );


#ifdef LINUX_OS
    linux_tcp_client modbus_cl( "127.0.0.1", G_CMMCTR->get_modbus_port(), 1, 1 );
#else
    win_tcp_client modbus_cl( "127.0.0.1", G_CMMCTR->get_modbus_port(), 1, 1 );
    modbus_cl.InitLib();
#endif

    ASSERT_EQ( tcp_client::ASYNCCONNECTSTATE::ACS_DISCONNECTED,
        modbus_cl.get_connected_state() );
    modbus_cl.reconnectTimeout = 0;
    cnt = 0;
    r = modbus_cl.AsyncConnect();;
    while ( r != tcp_client::ASYNCCONNECTSTATE::ACS_CONNECTED && cnt < 1000 )
        {
        sleep_ms( 1 );
        r = modbus_cl.AsyncConnect();
        cnt++;
        }
    EXPECT_EQ( tcp_client::ASYNCCONNECTSTATE::ACS_CONNECTED,
        modbus_cl.get_connected_state() );

    EXPECT_EQ( 0, G_CMMCTR->evaluate() );
    sleep_ms( 1 );
    //Для модбас клиента ответа на подключение не будет.
    size = modbus_cl.AsyncRecive();
    ASSERT_EQ( size, 0 );

    modbus_cl.Disconnect();
    EXPECT_EQ( 0, G_CMMCTR->evaluate() );

    tcp_communicator::clear_instance();
    }

TEST( tcp_communicator, checkBuff )
    {
    int s = socket( AF_INET, SOCK_STREAM, 0 );

    auto res = tcp_communicator::checkBuff( s );
#ifdef LINUX_OS
    EXPECT_TRUE( res );
#else
    EXPECT_FALSE( res );
#endif

    res = tcp_communicator::checkBuff( -10 );
    EXPECT_FALSE( res );
    }

TEST( tcp_communicator, recvtimeout )
    {
    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_time_next_hour ), SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );

    const auto SIZE = 100;
    u_char buff[ SIZE ] = { 0 };
    auto stat = stat_time();

    tcp_communicator::set_port( 30000, 30001 );
    tcp_communicator::init_instance( "Тест", "Test" );

    auto s = socket( AF_INET, SOCK_STREAM, 0 );

    auto res = tcp_communicator::recvtimeout( s, buff, SIZE, 1, 0, "", "", &stat );
    EXPECT_EQ( res, -2 );
    //Проверяем, что время ожидания истекло.
    EXPECT_GE( stat.all_time, 1000 );
    EXPECT_GE( stat.cycles_cnt, 1 );
    EXPECT_GE( stat.max_iteration_cycle_time, 1000 );
    EXPECT_GE( stat.min_iteration_cycle_time, 1000 );

    subhook_remove( get_time_hook );
    tcp_communicator::clear_instance();
    }
