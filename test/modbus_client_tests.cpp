#include "modbus_client_tests.h"

using namespace ::testing;

class test_modbus_client : public modbus_client
    {
    public:
        test_modbus_client( unsigned int id, char* ip ):modbus_client( id, ip )
            {
            }

        auto get_tcp_client()
            {
            return tcpclient;
            }

        void test_init_frame( unsigned int address, unsigned int value,
            unsigned int function_code )
            {
            init_frame( address, value, function_code );
            }
    };

TEST( modbus_client, init_frame )
    {
    test_modbus_client m_client( 1, "127.0.0.1" );

    const auto ADDRESS = 100;
    const auto VALUE = 10;
    const auto FUNCTION_CODE = 15;

    m_client.test_init_frame( ADDRESS, VALUE, FUNCTION_CODE );
    ASSERT_EQ( m_client.get_tcp_client()->buff[ 7 ], FUNCTION_CODE );
    ASSERT_EQ( m_client.get_tcp_client()->buff[ 9 ], ADDRESS );
    ASSERT_EQ( m_client.get_tcp_client()->buff[ 11 ], VALUE );    
    }
