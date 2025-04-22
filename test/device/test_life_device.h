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
        std::unique_ptr<life_device> life_bit = std::make_unique<life_device>(
            "TestDevice", device::DEVICE_SUB_TYPE::DST_LIFEBIT );
        std::unique_ptr<mock_DI_device> mock_DI_dev = 
            std::make_unique<mock_DI_device>();

        void SetUp() override 
            {
            // Устанавливаем mock устройство.
            life_bit->set_property( "DEV", mock_DI_dev.get() );
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
        std::unique_ptr<life_device> life_counter = std::make_unique<life_device>(
            "TestDevice", device::DEVICE_SUB_TYPE::DST_LIFECOUNTER );
        std::unique_ptr<mock_AI_device> mock_AI_dev = 
            std::make_unique<mock_AI_device>();

        void SetUp() override
            {
            // Устанавливаем mock устройство.
            life_counter->set_property( "DEV", mock_AI_dev.get() );
            }
    };

TEST_F( lifebit_test, EvaluateIO_NoDiDevice_NoAction ) 
    {
    EXPECT_EQ( life_bit->get_state(), 0 );   // Состояние должно быть нулевое.
    
    life_bit->evaluate_io();
    EXPECT_EQ( life_bit->get_state(), 0 );   // Состояние должно быть нулевое.

    life_bit->set_property( "DEV", nullptr ); // Убираем устройство.
    life_bit->evaluate_io();
    EXPECT_EQ( life_bit->get_state(), 0 );   // Состояние должно быть нулевое.
    }

TEST_F( lifebit_test, EvaluateIO_StateChanged_DeviceActivated ) 
    {
    EXPECT_CALL( *mock_DI_dev, get_state() )
        .WillOnce( ::testing::Return( 1 ) ); // Состояние сигнала изменилось.

    life_bit->evaluate_io();
    EXPECT_EQ( life_bit->get_state(), 1 );     // Устройство должно активироваться.
    }

TEST_F( lifebit_test, EvaluateIO_TimerExceeded_DeviceDeactivated )
    {
    EXPECT_CALL( *mock_DI_dev, get_state() )
        .WillOnce( ::testing::Return( 1 ) ); // Состояние изменилось.

    life_bit->evaluate_io();                
    EXPECT_EQ( life_bit->get_state(), 1 );     // Устройство должно активироваться.


    // Эмулируем превышение времени.
    // Устанавливаем DT = 100 мс.
    life_bit->set_par( static_cast<u_int>( life_device::PARAM::DT ), 0, 100 );
    EXPECT_CALL( *mock_DI_dev, get_state() )
        .WillRepeatedly( ::testing::Return( 1 ) ); // Состояние не меняется.

    // Эмулируем время. Устанавливаем время так, чтобы таймер истек.
    life_bit->start_time -= 200;
    life_bit->evaluate_io();
    EXPECT_EQ( life_bit->get_state(), 0 ); // Устройство должно деактивироваться


    EXPECT_CALL( *mock_DI_dev, get_state() )
        .WillOnce( ::testing::Return( 0 ) ); // Состояние изменилось.

    life_bit->evaluate_io();
    EXPECT_EQ( life_bit->get_state(), 1 );     // Устройство должно активироваться.
    }

TEST_F( lifebit_test, save_device )
    {
    // Создаем std::array для записи
    std::array<char, 256> buffer = { 0 };

    // Вызываем метод save_device
    auto bytes_written = life_bit->save_device( buffer.data() );

    // Проверяем, что метод вернул корректное количество записанных байт.
    EXPECT_GT( bytes_written, 0 );

    EXPECT_STREQ( "TestDevice={M=0, ST=0, V=0, DT=0},\n", buffer.data() );
    }

TEST_F( lifebit_test, SetStringProperty_SetsDIDevice )
    {
    life_bit->set_property( "DEV", nullptr );
    // Проверяем, что изначально dev равен nullptr
    EXPECT_EQ( life_bit->dev, nullptr );

    // Устанавливаем свойство "DEV" с именем устройства
    const char* device_name = "MockDevice1";
    G_DEVICE_MANAGER()->add_device(static_cast<device*>( mock_DI_dev.get() ),
        device::DEVICE_TYPE::DT_LIFE_DEVICE );
    life_bit->set_string_property( "DEV", device_name );

    // Проверяем, что dev был установлен
    EXPECT_EQ( life_bit->dev, mock_DI_dev.get() );

    G_DEVICE_MANAGER()->remove_device( mock_DI_dev->get_serial_n() );
    }

TEST_F( lifebit_test, SetStringProperty_InvalidField_NoAction )
    {
    // Устанавливаем несуществующее поле
    const char* invalid_field = "INVALID";
    life_bit->set_string_property( invalid_field, "SomeValue" );

    // Проверяем, что dev не изменился
    EXPECT_EQ( life_bit->dev, mock_DI_dev.get() );
    }

TEST_F( lifebit_test, GetNameInLua_ReturnsCorrectName )
    {
    const char* expected_name = "TestDevice";
    const char* lua_name = life_bit->get_name_in_Lua();
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
