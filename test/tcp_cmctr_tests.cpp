#include "tcp_cmctr_tests.h"
#include "PAC_info.h"

using namespace ::testing;

#ifndef WIN_OS // For linux to deal with __stdcall.
#define __stdcall
#endif

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

int __stdcall fail_select_1( int, fd_set*, fd_set*, fd_set*, struct timeval* )
    {
    return -1;
    }


int __stdcall good_select( int, fd_set*, fd_set*, fd_set*, struct timeval* )
    {
    return 1;
    }

TEST( tcp_communicator, recvtimeout )
    {
    auto get_time_hook = subhook_new( reinterpret_cast<void*>( &get_time ),
        reinterpret_cast<void*>( &get_time_next_hour ), SUBHOOK_64BIT_OFFSET );
    subhook_install( get_time_hook );

    const auto SIZE = 100;
    u_char buff[ SIZE ] = { 0 };
    auto stat = stat_time();

    tcp_communicator::set_port( 30010, 30011 );
    tcp_communicator::init_instance( "Тест", "Test" );
    G_CMMCTR->evaluate();

    auto res = tcp_communicator::recvtimeout(
        tcp_communicator::get_master_socket(), buff, SIZE, 1, 0, "", "", &stat);
    EXPECT_EQ( res, -2 );
    //Проверяем, что время ожидания истекло.
    EXPECT_GE( stat.all_time, 1000 );
    EXPECT_GE( stat.cycles_cnt, 1 );
    EXPECT_GE( stat.max_iteration_cycle_time, 1000 );
    EXPECT_GE( stat.min_iteration_cycle_time, 1000 );

    stat.print_cycle_last_h--;
    // Устанавливаем в 0 время ожидания ответа, при превышении которого
    // будет уведомление.
    G_PAC_INFO()->par[ PAC_info::P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME ] = 0;
    res = tcp_communicator::recvtimeout(
        tcp_communicator::get_master_socket(), buff, SIZE, 1, 0, "", "", &stat );
    EXPECT_EQ( res, -2 );

    // Should fail - fail with select() returns -1.
    subhook_t select_m1_hook = subhook_new( reinterpret_cast<void*>( &select ),
        reinterpret_cast<void*>( fail_select_1 ), SUBHOOK_64BIT_OFFSET );
    subhook_install( select_m1_hook );    
    res = tcp_communicator::recvtimeout(
        tcp_communicator::get_master_socket(), buff, SIZE, 1, 0, "", "", &stat );
    EXPECT_EQ( res, -1 );
    subhook_remove( select_m1_hook );
    subhook_free( select_m1_hook );

    // Should fail - fail with recv() returns -1.
    subhook_t select_good_hook = subhook_new( reinterpret_cast<void*>( &select ),
        reinterpret_cast<void*>( good_select ), SUBHOOK_64BIT_OFFSET );
    subhook_install( select_good_hook );    
    res = tcp_communicator::recvtimeout(
        tcp_communicator::get_master_socket(), buff, SIZE, 1, 0, "", "", &stat );
    EXPECT_EQ( res, -1 );
    subhook_remove( select_good_hook );
    subhook_free( select_good_hook );

    
    subhook_remove( get_time_hook );
    subhook_free( get_time_hook );
    tcp_communicator::clear_instance();
    }
