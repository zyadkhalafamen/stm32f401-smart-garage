#include "gpio_driver.h"
#include "app_config.h"

/* ================= RCC Registers ================= */

#define RCC_AHB1ENR (*(volatile uint32_t *)0x40023830U)

/* ================= GPIOA Registers =============== */

#define GPIOA_IDR   (*(volatile uint32_t *)0x40020010U)
#define GPIOA_BSRR  (*(volatile uint32_t *)0x40020018U)

/* ================= GPIOB Registers =============== */

#define GPIOB_MODER (*(volatile uint32_t *)0x40020400U)
#define GPIOB_BSRR  (*(volatile uint32_t *)0x40020418U)

void gpioa_write(uint32_t pin, uint32_t state)
{
    if (state != 0U)
    {
        GPIOA_BSRR = (1U << pin);
    }
    else
    {
        GPIOA_BSRR = (1U << (pin + 16U));
    }
}

void gpiob_write(uint32_t pin, uint32_t state)
{
    if (state != 0U)
    {
        GPIOB_BSRR = (1U << pin);
    }
    else
    {
        GPIOB_BSRR = (1U << (pin + 16U));
    }
}

uint32_t gpioa_read(uint32_t pin)
{
    return ((GPIOA_IDR >> pin) & 1U);
}

void green_led_set(uint32_t state)
{
    gpiob_write(GREEN_LED, state);
}

void red_led_set(uint32_t state)
{
    gpiob_write(RED_LED, state);
}

void gpio_driver_init(void)
{
    static const uint32_t output_pins[] =
    {
        LCD_RS,
        LCD_EN,
        LCD_D4,
        LCD_D5,
        LCD_D6,
        LCD_D7,
        GREEN_LED,
        RED_LED
    };

    uint32_t index;

    /* Enable GPIOA and GPIOB peripheral clocks */
    RCC_AHB1ENR |= (1U << 0U);
    RCC_AHB1ENR |= (1U << 1U);

    /* Configure LCD and LED pins as GPIO outputs */
    for (index = 0U; index < 8U; index++)
    {
        GPIOB_MODER &=
            ~(3U << (output_pins[index] * 2U));

        GPIOB_MODER |=
            (1U << (output_pins[index] * 2U));
    }

    green_led_set(0U);
    red_led_set(1U);
}
