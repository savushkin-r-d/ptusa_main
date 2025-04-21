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

TEST_F( LifebitTest, EvaluateIo_NoDiDevice_NoAction ) 
    {
    EXPECT_EQ( lb_dev->get_state(), 0 );   // Состояние должно быть нулевое.
    
    lb_dev->evaluate_io();
    EXPECT_EQ( lb_dev->get_state(), 0 );   // Состояние должно быть нулевое.

    lb_dev->set_property( "DI", nullptr ); // Убираем устройство.
    lb_dev->evaluate_io();
    EXPECT_EQ( lb_dev->get_state(), 0 );   // Состояние должно быть нулевое.
    }

TEST_F( LifebitTest, EvaluateIo_StateChanged_DeviceActivated ) 
    {
    EXPECT_CALL( *mock_di_device, get_state() )
        .WillOnce( ::testing::Return( 1 ) ); // Состояние сигнала изменилось.

    lb_dev->evaluate_io();
    EXPECT_EQ( lb_dev->get_state(), 1 );     // Устройство должно активироваться.
    }

TEST_F( LifebitTest, EvaluateIo_TimerExceeded_DeviceDeactivated )
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
