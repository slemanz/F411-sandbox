#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTestExt/MockSupport_c.h"

extern "C"
{
#include "bsp/led.h"
#include "shared/pool.h"
}

/* ------------------------------------------------------------------ */
/*  Pin IDs used across all tests                                     */
/* ------------------------------------------------------------------ */

static const uint8_t PIN_A = 0;   /* single LED          */
static const uint8_t PIN_R = 1;   /* RGB — red channel   */
static const uint8_t PIN_G = 2;   /* RGB — green channel */
static const uint8_t PIN_B = 3;   /* RGB — blue channel  */

/* ------------------------------------------------------------------ */
/*  Helper: set one IO_write expectation                               */
/* ------------------------------------------------------------------ */

static void expect_write(uint8_t pin_id, uint8_t value)
{
    mock_c()->expectOneCall("IO_write")
             ->withUnsignedIntParameters("pin_id", pin_id)
             ->withUnsignedIntParameters("value",  value);
}

/* ================================================================== */
/*  TEST GROUP: Led (single-colour)                                    */
/* ================================================================== */

TEST_GROUP(Led)
{
    ledPtr_t led;

    void setup()
    {
        pool_Init();
        led = NULL;
    }

    void teardown()
    {
        mock_c()->checkExpectations();
        mock_c()->clear();

        if(led)
        {
            led_destroy(led);
            led = NULL;
        }
    }
};

/* ---- creation / lookup ------------------------------------------- */

TEST(Led, CreateReturnsNonNull)
{
    led = led_create("L", PIN_A);
    CHECK(led != NULL);
}

TEST(Led, CreateWithUuidReturnsNonNull)
{
    led = led_createWithUuid("L", PIN_A, 42);
    CHECK(led != NULL);
}

TEST(Led, CreateWithDuplicateUuidReturnsNull)
{
    led = led_createWithUuid("L1", PIN_A, 10);
    CHECK(led != NULL);

    ledPtr_t dup = led_createWithUuid("L2", PIN_A, 10);
    CHECK(dup == NULL);
}

TEST(Led, GetByUuidFindsLed)
{
    led = led_createWithUuid("L", PIN_A, 77);
    POINTERS_EQUAL(led, led_getByUuid(77));
}

TEST(Led, GetByUuidReturnsNullForUnknown)
{
    CHECK(led_getByUuid(0xFFFF) == NULL);
}

/* ---- control ----------------------------------------------------- */

TEST(Led, TurnOn_WritesHigh)
{
    led = led_create("L", PIN_A);
    expect_write(PIN_A, IO_PIN_HIGH);

    LONGS_EQUAL(IO_OK, led_turn_on(led));
}

TEST(Led, TurnOff_WritesLow)
{
    led = led_create("L", PIN_A);
    expect_write(PIN_A, IO_PIN_LOW);

    LONGS_EQUAL(IO_OK, led_turn_off(led));
}

TEST(Led, Toggle_CallsIOToggle)
{
    led = led_create("L", PIN_A);
    mock_c()->expectOneCall("IO_toggle")
             ->withUnsignedIntParameters("pin_id", PIN_A);

    LONGS_EQUAL(IO_OK, led_toggle(led));
}

TEST(Led, Toggle_CalledTwice_ExpectsTwoCalls)
{
    led = led_create("L", PIN_A);
    mock_c()->expectOneCall("IO_toggle")->withUnsignedIntParameters("pin_id", PIN_A);
    mock_c()->expectOneCall("IO_toggle")->withUnsignedIntParameters("pin_id", PIN_A);

    led_toggle(led);
    led_toggle(led);
}