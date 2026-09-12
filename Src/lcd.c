#include "lcd.h"
#include "gpio_driver.h"
#include "timebase.h"
#include "app_config.h"

static void lcd_enable_pulse(void)
{
    gpiob_write(LCD_EN, 1U);
    short_delay_ms(1U);

    gpiob_write(LCD_EN, 0U);
    short_delay_ms(1U);
}

static void lcd_write_nibble(uint8_t data)
{
    gpiob_write(LCD_D4, (data >> 0U) & 1U);
    gpiob_write(LCD_D5, (data >> 1U) & 1U);
    gpiob_write(LCD_D6, (data >> 2U) & 1U);
    gpiob_write(LCD_D7, (data >> 3U) & 1U);

    lcd_enable_pulse();
}

static void lcd_send_command(uint8_t command)
{
    gpiob_write(LCD_RS, 0U);

    lcd_write_nibble(command >> 4U);
    lcd_write_nibble(command & 0x0FU);

    short_delay_ms(2U);
}

void lcd_send_character(uint8_t character)
{
    gpiob_write(LCD_RS, 1U);

    lcd_write_nibble(character >> 4U);
    lcd_write_nibble(character & 0x0FU);

    short_delay_ms(1U);
}

void lcd_print(const char *text)
{
    while (*text != '\0')
    {
        lcd_send_character((uint8_t)*text);
        text++;
    }
}

void lcd_clear(void)
{
    lcd_send_command(0x01U);
    short_delay_ms(3U);
}

void lcd_set_cursor(uint8_t row, uint8_t column)
{
    uint8_t address;

    if (row == 1U)
    {
        address = (uint8_t)(0x80U + column - 1U);
    }
    else
    {
        address = (uint8_t)(0xC0U + column - 1U);
    }

    lcd_send_command(address);
}

void lcd_print_number(uint32_t value)
{
    if (value >= 10U)
    {
        lcd_send_character(
            (uint8_t)('0' + ((value / 10U) % 10U)));
    }

    lcd_send_character(
        (uint8_t)('0' + (value % 10U)));
}

void lcd_init(void)
{
    short_delay_ms(40U);

    gpiob_write(LCD_RS, 0U);
    gpiob_write(LCD_EN, 0U);

    lcd_write_nibble(0x03U);
    short_delay_ms(5U);

    lcd_write_nibble(0x03U);
    short_delay_ms(2U);

    lcd_write_nibble(0x03U);
    short_delay_ms(2U);

    lcd_write_nibble(0x02U);

    /* 4-bit mode, two lines, 5x8 font */
    lcd_send_command(0x28U);

    /* Display ON, cursor OFF */
    lcd_send_command(0x0CU);

    /* Move cursor automatically to the right */
    lcd_send_command(0x06U);

    lcd_clear();
}
