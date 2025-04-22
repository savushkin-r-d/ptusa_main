#pragma once
#include "../includes.h"

#include "device/lifebit.h"

class MockDevice : public device
    {
    public:
        MockDevice() : device( "MockDevice1", DEVICE_TYPE::DT_LIFE_DEVICE,
            DEVICE_SUB_TYPE::DST_LIFEBIT, 0 ) {}

        MOCK_METHOD( int, get_state, ( ), ( override ) );
    };

class LifebitTest : public ::testing::Test
    {
    protected:
        lifebit* lb_dev;
        MockDevice* mock_di_device;

        void SetUp() override 
            {
            mock_di_device = new MockDevice();

            lb_dev = new lifebit( "TestDevice" );
            // Устанавливаем mock устройство.
            lb_dev->set_property( "DI", mock_di_device );
            }

        void TearDown() override
            {
            delete lb_dev;
            lb_dev = nullptr;

            delete mock_di_device;
            mock_di_device = nullptr;
            }
    };

TEST_F( LifebitTest, EvaluateIO_NoDiDevice_NoAction ) 
    {
    EXPECT_EQ( lb_dev->get_state(), 0 );   // Состояние должно быть нулевое.
    
    lb_dev->evaluate_io();
    EXPECT_EQ( lb_dev->get_state(), 0 );   // Состояние должно быть нулевое.

    lb_dev->set_property( "DI", nullptr ); // Убираем устройство.
    lb_dev->evaluate_io();
    EXPECT_EQ( lb_dev->get_state(), 0 );   // Состояние должно быть нулевое.
    }

TEST_F( LifebitTest, EvaluateIO_StateChanged_DeviceActivated ) 
    {
    EXPECT_CALL( *mock_di_device, get_state() )
        .WillOnce( ::testing::Return( 1 ) ); // Состояние сигнала изменилось.

    lb_dev->evaluate_io();
    EXPECT_EQ( lb_dev->get_state(), 1 );     // Устройство должно активироваться.
    }

TEST_F( LifebitTest, EvaluateIO_TimerExceeded_DeviceDeactivated )
    {
    EXPECT_CALL( *mock_di_device, get_state() )
        .WillOnce( ::testing::Return( 1 ) ); // Состояние изменилось.

    lb_dev->evaluate_io();                
    EXPECT_EQ( lb_dev->get_state(), 1 );     // Устройство должно активироваться.

    // Эмулируем превышение времени.
    // Устанавливаем DT = 100 мс.
    lb_dev->set_par( static_cast<u_int>( lifebit::PARAM::DT ), 0, 100 );
    EXPECT_CALL( *mock_di_device, get_state() )
        .WillRepeatedly( ::testing::Return( 1 ) ); // Состояние не меняется.

    // Эмулируем время. Устанавливаем время так, чтобы таймер истек.
    lb_dev->start_time -= 200;
    lb_dev->evaluate_io();
    EXPECT_EQ( lb_dev->get_state(), 0 ); // Устройство должно деактивироваться
    }

TEST_F( LifebitTest, save_device )
    {
    // Создаем std::array для записи
    std::array<char, 256> buffer = { 0 };

    // Вызываем метод save_device
    auto bytes_written = lb_dev->save_device( buffer.data() );

    // Проверяем, что метод вернул корректное количество записанных байт.
    EXPECT_GT( bytes_written, 0 );

    EXPECT_STREQ( "TestDevice={M=0, ST=0, V=0, DT=0},\n", buffer.data() );
    }

TEST_F( LifebitTest, SetStringProperty_SetsDIDevice )
    {
    lb_dev->set_property( "DI", nullptr );
    // Проверяем, что изначально di_device равен nullptr
    EXPECT_EQ( lb_dev->di_device, nullptr );

    // Устанавливаем свойство "DI" с именем устройства
    const char* device_name = "MockDevice1";
    G_DEVICE_MANAGER()->add_device( mock_di_device,
        device::DEVICE_TYPE::DT_LIFE_DEVICE );
    lb_dev->set_string_property( "DI", device_name );

    // Проверяем, что di_device был установлен
    EXPECT_EQ( lb_dev->di_device, mock_di_device );

    G_DEVICE_MANAGER()->remove_device( mock_di_device->get_serial_n() );
    }

TEST_F( LifebitTest, SetStringProperty_InvalidField_NoAction )
    {
    // Устанавливаем несуществующее поле
    const char* invalid_field = "INVALID";
    lb_dev->set_string_property( invalid_field, "SomeValue" );

    // Проверяем, что di_device не изменился
    EXPECT_EQ( lb_dev->di_device, mock_di_device );
    }

TEST_F( LifebitTest, GetNameInLua_ReturnsCorrectName )
    {
    const char* expected_name = "TestDevice";
    const char* lua_name = lb_dev->get_name_in_Lua();
    ASSERT_STREQ( lua_name, expected_name ) << "get_name_in_Lua() should"
        "return the correct device name.";
    }
