#include "tcp_cmctr_tests.h"

using namespace ::testing;

TEST( tcp_communicator, evaluate )
    {
    G_CMMCTR->init_instance( "Тест", "Test" );
    G_CMMCTR->set_port( 30000, 30001 );
    EXPECT_EQ( 0, G_CMMCTR->evaluate() );

#ifdef LINUX_OS
    linux_tcp_client cl( "127.0.0.1", G_CMMCTR->get_port(), 1, 1);
#else
    win_tcp_client cl( "127.0.0.1", G_CMMCTR->get_port(), 1, 1 );
    cl.InitLib();
#endif

    EXPECT_EQ( tcp_client::ASYNCCONNECTSTATE::ACS_DISCONNECTED,
        cl.get_connected_state() );

    sleep_ms( 1 );
    cl.reconnectTimeout = 0;
    cl.checkConnection();    
    ASSERT_EQ( tcp_client::ASYNCCONNECTSTATE::ACS_CONNECTED,
        cl.get_connected_state() );    

    EXPECT_EQ( 0, G_CMMCTR->evaluate() );
    //Должен быть получен ответ на подключение.
    sleep_ms( 10 );
    auto size = cl.AsyncRecive();
    auto cnt = 0;
    while ( size <= 0 && cnt < 1000 )
        {
        sleep_ms( 1 );
        size = cl.AsyncRecive();
        cnt++;
        }
    ASSERT_GT( size, 0 );
    cl.buff[ size ] = '\0';
    EXPECT_STREQ( cl.buff, "PAC accept");

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
    sleep_ms( 1 );
    modbus_cl.checkConnection();
    EXPECT_EQ( tcp_client::ASYNCCONNECTSTATE::ACS_CONNECTED,
        modbus_cl.get_connected_state() );

    EXPECT_EQ( 0, G_CMMCTR->evaluate() );
    sleep_ms( 1 );
    //Для модбас клиента ответа на подключение не будет.
    size = modbus_cl.AsyncRecive();
    ASSERT_EQ( size, 0 );

    modbus_cl.Disconnect();
    EXPECT_EQ( 0, G_CMMCTR->evaluate() );
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
