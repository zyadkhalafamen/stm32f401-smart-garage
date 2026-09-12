#include "keypad.h"
#include "gpio_driver.h"
#include "timebase.h"

#include <stdint.h>

/* ================= GPIOA Registers ================= */

#define GPIOA_MODER (*(volatile uint32_t *)0x40020000U)
#define GPIOA_PUPDR (*(volatile uint32_t *)0x4002000CU)

/*
 * Hardware keypad layout:
 *
 * 1  2  3  A
 * 4  5  6  B
 * 7  8  9  C
 * *  0  #  D
 */
static const char keypad_map[4][4] =
{
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void keypad_init(void)
{
    uint32_t pin;

    /*
     * PA0 to PA3:
     * Keypad rows configured as outputs.
     */
    for (pin = 0U; pin < 4U; pin++)
    {
        GPIOA_MODER &= ~(3U << (pin * 2U));
        GPIOA_MODER |=  (1U << (pin * 2U));

        gpioa_write(pin, 1U);
    }

    /*
     * PA4 to PA7:
     * Keypad columns configured as inputs
     * with internal pull-up resistors.
     */
    for (pin = 4U; pin < 8U; pin++)
    {
        GPIOA_MODER &= ~(3U << (pin * 2U));

        GPIOA_PUPDR &= ~(3U << (pin * 2U));
        GPIOA_PUPDR |=  (1U << (pin * 2U));
    }
}

static char keypad_raw_scan(void)
{
    uint32_t row;
    uint32_t column;
    uint32_t current_row;

    for (row = 0U; row < 4U; row++)
    {
        /* Deactivate all rows */
        for (current_row = 0U;
             current_row < 4U;
             current_row++)
        {
            gpioa_write(current_row, 1U);
        }

        /* Activate one row */
        gpioa_write(row, 0U);

        /*
         * Very short electrical settling time.
         * This is not a user-visible blocking delay.
         */
        for (volatile uint32_t settle = 0U;
             settle < 40U;
             settle++)
        {
        }

        for (column = 0U; column < 4U; column++)
        {
            if (gpioa_read(column + 4U) == 0U)
            {
                return keypad_map[row][column];
            }
        }
    }

    return 0;
}

char keypad_get_key(void)
{
    static char candidate_key = 0;
    static char accepted_key = 0;
    static uint32_t changed_time = 0U;

    char raw_key = keypad_raw_scan();

    if (raw_key != candidate_key)
    {
        candidate_key = raw_key;
        changed_time = millis();
    }

    /*
     * Non-blocking 30 ms debounce.
     * No waiting loop is used for key release.
     */
    if (time_elapsed(changed_time, 30U))
    {
        if ((candidate_key != 0) &&
            (accepted_key == 0))
        {
            accepted_key = candidate_key;
            return accepted_key;
        }

        if (candidate_key == 0)
        {
            accepted_key = 0;
        }
    }

    return 0;
}
