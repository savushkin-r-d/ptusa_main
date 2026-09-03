#include "led_test.h"

namespace
{
class led_test_stub : public led
    {
    public:
        void on( LEDS_NAMES, COLOUR ) override
            {
            }

        void off( LEDS_NAMES ) override
            {
            }
    };
}

TEST( sys_led, get_led_returns_singleton_instance )
    {
    auto* first = get_led();
    ASSERT_NE( first, nullptr );

    auto* second = get_led();
    EXPECT_EQ( first, second );
    }

TEST( sys_led, base_interface_can_be_used_polymorphically )
    {
    led* base = new led_test_stub();
    ASSERT_NE( base, nullptr );

    EXPECT_NO_THROW( base->on( led::L_STATUS, led::C_GREEN ) );
    EXPECT_NO_THROW( base->off( led::L_STATUS ) );

    delete base;
    }

#if defined( LINUX_OS ) && defined( PAC_PC )
TEST( sys_led, led_pc_noop_methods_are_callable )
    {
    auto* led_instance = get_led();
    ASSERT_NE( led_instance, nullptr );

    EXPECT_NO_THROW( led_instance->on( led::L_USER, led::C_YELLOW ) );
    EXPECT_NO_THROW( led_instance->off( led::L_USER ) );

    auto* led_pc = dynamic_cast<led_PC*>( led_instance );
    ASSERT_NE( led_pc, nullptr );
    EXPECT_NO_THROW( led_pc->blink( led::L_STATUS, led::C_RED ) );
    }
#endif
