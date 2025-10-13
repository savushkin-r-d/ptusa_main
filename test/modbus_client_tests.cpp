#include "modbus_client_tests.h"
#include <array>
#include <cstring>

using namespace ::testing;

class test_modbus_client : public modbus_client
    {
    public:
        test_modbus_client( unsigned int id, const char* ip ):modbus_client( id, ip )
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


// Вспомогательные функции для преобразования.
static std::array<unsigned char, 4> to_bytes( int_4 v )
    {
    std::array<unsigned char, 4> out{};
    std::memcpy( out.data(), &v, 4 );
    return out;
    }

static int_4 from_bytes( unsigned char b0, unsigned char b1,
    unsigned char b2, unsigned char b3 )
    {
    int_4 v = 0;
    unsigned char data[ 4 ] = { b0, b1, b2, b3 };
    std::memcpy( &v, data, 4 );
    return v;
    }

// Сдвиг между началом области чтения и записи.
// write_buff_start (13) - read_buff_start (9) = 4.
constexpr int kRWShift = 4;

// Записать 4 байта в "зону чтения" (read_buff_start + address*2)
// через set_byte с отрицательным смещением относительно write_buff_start.
static void preset_read_area( modbus_client& cli, unsigned int address,
    unsigned char a, unsigned char b, unsigned char c, unsigned char d )
    {
    const int base = static_cast<int>( address ) * 2 - kRWShift; // (9 + addr*2) - 13
    cli.set_byte( base + 0, a );
    cli.set_byte( base + 1, b );
    cli.set_byte( base + 2, c );
    cli.set_byte( base + 3, d );
    }

TEST( modbus_client, Set_AB_CD_WritesExpectedBytes )
    {
    modbus_client cli( 1, "127.0.0.1", 502, 50 );
    const unsigned int address = 5;
    const int_4 val = from_bytes( 0xAA, 0xBB, 0xCC, 0xDD );
    const auto bytes = to_bytes( val ); // [A, B, C, D]

    cli.set_int4_ab_cd( address, val );

    const unsigned baseOff = kRWShift + address * 2; // смещение для чтения write-области через get_byte
    EXPECT_EQ( cli.get_byte( baseOff + 0 ), bytes[ 0 ] ); // A
    EXPECT_EQ( cli.get_byte( baseOff + 1 ), bytes[ 1 ] ); // B
    EXPECT_EQ( cli.get_byte( baseOff + 2 ), bytes[ 2 ] ); // C
    EXPECT_EQ( cli.get_byte( baseOff + 3 ), bytes[ 3 ] ); // D
    }

TEST( modbus_client, Set_CD_AB_WritesExpectedBytes )
    {
    modbus_client cli( 1, "127.0.0.1", 502, 50 );
    const unsigned int address = 6;
    const int_4 val = from_bytes( 0x10, 0x20, 0x30, 0x40 );
    const auto bytes = to_bytes( val ); // [A=0x10, B=0x20, C=0x30, D=0x40]

    cli.set_int4_cd_ab( address, val );

    const unsigned baseOff = kRWShift + address * 2;
    EXPECT_EQ( cli.get_byte( baseOff + 0 ), bytes[ 2 ] ); // C
    EXPECT_EQ( cli.get_byte( baseOff + 1 ), bytes[ 3 ] ); // D
    EXPECT_EQ( cli.get_byte( baseOff + 2 ), bytes[ 0 ] ); // A
    EXPECT_EQ( cli.get_byte( baseOff + 3 ), bytes[ 1 ] ); // B
    }

TEST( modbus_client, Set_DC_BA_WritesExpectedBytes )
    {
    modbus_client cli( 1, "127.0.0.1", 502, 50 );
    const unsigned int address = 7;
    const int_4 val = from_bytes( 0x01, 0x02, 0x03, 0x04 );
    const auto bytes = to_bytes( val ); // [A=0x01, B=0x02, C=0x03, D=0x04]

    cli.set_int4_dc_ba( address, val );

    const unsigned baseOff = kRWShift + address * 2;
    EXPECT_EQ( cli.get_byte( baseOff + 0 ), bytes[ 3 ] ); // D
    EXPECT_EQ( cli.get_byte( baseOff + 1 ), bytes[ 2 ] ); // C
    EXPECT_EQ( cli.get_byte( baseOff + 2 ), bytes[ 1 ] ); // B
    EXPECT_EQ( cli.get_byte( baseOff + 3 ), bytes[ 0 ] ); // A
    }

TEST( modbus_client, set_int4 )
    {
    modbus_client cli( 1, "127.0.0.1", 502, 50 );
    const unsigned int address = 8;
    const int_4 val = from_bytes( 0xA1, 0xB2, 0xC3, 0xD4 );
    const auto bytes = to_bytes( val );

    cli.set_int4( address, val );

    const unsigned baseOff = kRWShift + address * 2;
    EXPECT_EQ( cli.get_byte( baseOff + 0 ), bytes[ 1 ] ); // B
    EXPECT_EQ( cli.get_byte( baseOff + 1 ), bytes[ 0 ] ); // A
    EXPECT_EQ( cli.get_byte( baseOff + 2 ), bytes[ 3 ] ); // D
    EXPECT_EQ( cli.get_byte( baseOff + 3 ), bytes[ 2 ] ); // C
    }

TEST( modbus_client, Get_AB_CD_ReadsExpectedValue )
    {
    modbus_client cli( 1, "127.0.0.1", 502, 50 );
    const unsigned int address = 9;

    // Заполняем область чтения: [A, B, C, D]
    const unsigned char A = 0x11, B = 0x22, C = 0x33, D = 0x44;
    preset_read_area( cli, address, A, B, C, D );

    const int_4 expected = from_bytes( A, B, C, D );
    EXPECT_EQ( cli.get_int4_ab_cd( address ), expected );
    }

TEST( modbus_client, Get_CD_AB_ReadsExpectedValue )
    {
    modbus_client cli( 1, "127.0.0.1", 502, 50 );
    const unsigned int address = 10;

    // Заполняем область чтения: [A, B, C, D]
    const unsigned char A = 0x55, B = 0x66, C = 0x77, D = 0x88;
    preset_read_area( cli, address, A, B, C, D );

    // Ожидаемое формирование: [C, D, A, B]
    const int_4 expected = from_bytes( C, D, A, B );
    EXPECT_EQ( cli.get_int4_cd_ab( address ), expected );
    }

TEST( modbus_client, Get_DC_BA_ReadsExpectedValue )
    {
    modbus_client cli( 1, "127.0.0.1", 502, 50 );
    const unsigned int address = 11;

    // Заполняем область чтения: [A, B, C, D]
    const unsigned char A = 0x9A, B = 0xBC, C = 0xDE, D = 0xF0;
    preset_read_area( cli, address, A, B, C, D );

    // Ожидаемое формирование: [D, C, B, A]
    const int_4 expected = from_bytes( D, C, B, A );
    EXPECT_EQ( cli.get_int4_dc_ba( address ), expected );
    }

TEST( modbus_client, get_int4 )
    {
    modbus_client cli( 1, "127.0.0.1", 502, 50 );
    const unsigned int address = 12;

    // Заполняем область чтения: [A, B, C, D]
    const unsigned char A = 0x0A, B = 0x1B, C = 0x2C, D = 0x3D;
    preset_read_area( cli, address, A, B, C, D );
        
    const int_4 expected = from_bytes( B, A, D, C );
    EXPECT_EQ( cli.get_int4( address ), expected );
    }

TEST( modbus_client, OutOfRangeAddressReturnsZero )
    {
    modbus_client cli( 1, "127.0.0.1", 502, 50 );
    // Для read: условие выхода address*2 > buff_size - read_buff_start - sizeof(int_4)
    // buff_size=256, read_buff_start=9, sizeof(int_4)=4 => порог 243 => address >= 122
    EXPECT_EQ( cli.get_int4_dc_ba( 122 ), 0 );
    }