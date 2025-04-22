#pragma once
#include "../includes.h"

#include "device/life_device.h"

class mock_DI_device : public device
    {
    public:
        mock_DI_device() : device( "MockDevice1", DEVICE_TYPE::DT_DI,
            DEVICE_SUB_TYPE::DST_DI, 0 ) {}

        MOCK_METHOD( int, get_state, ( ), ( override ) );
    };

class lifebit_test : public ::testing::Test
    {
    protected:
        life_device* lb_dev;
        mock_DI_device* mock_dev;

        void SetUp() override 
            {
            mock_dev = new mock_DI_device();

            lb_dev = new life_device( "TestDevice", device::DEVICE_SUB_TYPE::DST_LIFEBIT );
            // Устанавливаем mock устройство.
            lb_dev->set_property( "DEV", mock_dev );
            }

        void TearDown() override
            {
            delete lb_dev;
            lb_dev = nullptr;

            delete mock_dev;
            mock_dev = nullptr;
            }
    };

class mock_AI_device : public device
    {
    public:
        mock_AI_device() : device( "MockDevice2", DEVICE_TYPE::DT_AI,
            DEVICE_SUB_TYPE::DST_AI, 0 ) {
            }

        MOCK_METHOD( float, get_value, ( ), ( override ) );
    };

class lifecounter_test : public ::testing::Test
    {
    protected:
        life_device* life_counter;
        mock_AI_device* mock_AI_dev;

        void SetUp() override
            {
            mock_AI_dev = new mock_AI_device();

            life_counter = new life_device( "TestDevice", device::DEVICE_SUB_TYPE::DST_LIFECOUNTER );
            // Устанавливаем mock устройство.
            life_counter->set_property( "DEV", mock_AI_dev );
            }

        void TearDown() override
            {
            delete life_counter;
            life_counter = nullptr;

            delete mock_AI_dev;
            mock_AI_dev = nullptr;
            }
    };

TEST_F( lifebit_test, EvaluateIO_NoDiDevice_NoAction ) 
    {
    EXPECT_EQ( lb_dev->get_state(), 0 );   // Состояние должно быть нулевое.
    
    lb_dev->evaluate_io();
    EXPECT_EQ( lb_dev->get_state(), 0 );   // Состояние должно быть нулевое.

    lb_dev->set_property( "DI", nullptr ); // Убираем устройство.
    lb_dev->evaluate_io();
    EXPECT_EQ( lb_dev->get_state(), 0 );   // Состояние должно быть нулевое.
    }

TEST_F( lifebit_test, EvaluateIO_StateChanged_DeviceActivated ) 
    {
    EXPECT_CALL( *mock_dev, get_state() )
        .WillOnce( ::testing::Return( 1 ) ); // Состояние сигнала изменилось.

    lb_dev->evaluate_io();
    EXPECT_EQ( lb_dev->get_state(), 1 );     // Устройство должно активироваться.
    }

TEST_F( lifebit_test, EvaluateIO_TimerExceeded_DeviceDeactivated )
    {
    EXPECT_CALL( *mock_dev, get_state() )
        .WillOnce( ::testing::Return( 1 ) ); // Состояние изменилось.

    lb_dev->evaluate_io();                
    EXPECT_EQ( lb_dev->get_state(), 1 );     // Устройство должно активироваться.


    // Эмулируем превышение времени.
    // Устанавливаем DT = 100 мс.
    lb_dev->set_par( static_cast<u_int>( life_device::PARAM::DT ), 0, 100 );
    EXPECT_CALL( *mock_dev, get_state() )
        .WillRepeatedly( ::testing::Return( 1 ) ); // Состояние не меняется.

    // Эмулируем время. Устанавливаем время так, чтобы таймер истек.
    lb_dev->start_time -= 200;
    lb_dev->evaluate_io();
    EXPECT_EQ( lb_dev->get_state(), 0 ); // Устройство должно деактивироваться


    EXPECT_CALL( *mock_dev, get_state() )
        .WillOnce( ::testing::Return( 0 ) ); // Состояние изменилось.

    lb_dev->evaluate_io();
    EXPECT_EQ( lb_dev->get_state(), 1 );     // Устройство должно активироваться.
    }

TEST_F( lifebit_test, save_device )
    {
    // Создаем std::array для записи
    std::array<char, 256> buffer = { 0 };

    // Вызываем метод save_device
    auto bytes_written = lb_dev->save_device( buffer.data() );

    // Проверяем, что метод вернул корректное количество записанных байт.
    EXPECT_GT( bytes_written, 0 );

    EXPECT_STREQ( "TestDevice={M=0, ST=0, V=0, DT=0},\n", buffer.data() );
    }

TEST_F( lifebit_test, SetStringProperty_SetsDIDevice )
    {
    lb_dev->set_property( "DI", nullptr );
    // Проверяем, что изначально dev равен nullptr
    EXPECT_EQ( lb_dev->dev, nullptr );

    // Устанавливаем свойство "DI" с именем устройства
    const char* device_name = "MockDevice1";
    G_DEVICE_MANAGER()->add_device( mock_dev,
        device::DEVICE_TYPE::DT_LIFE_DEVICE );
    lb_dev->set_string_property( "DI", device_name );

    // Проверяем, что dev был установлен
    EXPECT_EQ( lb_dev->dev, mock_dev );

    G_DEVICE_MANAGER()->remove_device( mock_dev->get_serial_n() );
    }

TEST_F( lifebit_test, SetStringProperty_InvalidField_NoAction )
    {
    // Устанавливаем несуществующее поле
    const char* invalid_field = "INVALID";
    lb_dev->set_string_property( invalid_field, "SomeValue" );

    // Проверяем, что dev не изменился
    EXPECT_EQ( lb_dev->dev, mock_dev );
    }

TEST_F( lifebit_test, GetNameInLua_ReturnsCorrectName )
    {
    const char* expected_name = "TestDevice";
    const char* lua_name = lb_dev->get_name_in_Lua();
    ASSERT_STREQ( lua_name, expected_name ) << "get_name_in_Lua() should"
        "return the correct device name.";
    }


TEST_F( lifecounter_test, EvaluateIO_TimerExceeded_DeviceDeactivated )
    {
    EXPECT_CALL( *mock_AI_dev, get_value() )
        .WillOnce( ::testing::Return( 10 ) ); // Состояние изменилось.
    life_counter->evaluate_io();
    EXPECT_EQ( life_counter->get_state(), 1 );// Устройство должно активироваться.

    EXPECT_CALL( *mock_AI_dev, get_value() )
        .WillRepeatedly( ::testing::Return( 10 ) );// Состояние не меняется.
    // Эмулируем время. Устанавливаем время так, чтобы таймер истек.
    life_counter->start_time -= 200;
    life_counter->evaluate_io();
    EXPECT_EQ( life_counter->get_state(), 0 );     // Устройство должно деактивироваться

    EXPECT_CALL( *mock_AI_dev, get_value() )
        .WillOnce( ::testing::Return( 11 ) ); // Состояние изменилось.
    life_counter->evaluate_io();
    EXPECT_EQ( life_counter->get_state(), 1 );// Устройство должно активироваться.
    }
