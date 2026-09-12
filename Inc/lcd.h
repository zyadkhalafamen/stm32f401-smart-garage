#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>

void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t column);
void lcd_print(const char *text);
void lcd_print_number(uint32_t value);
void lcd_send_character(uint8_t character);

#endif /* LCD_H_ */
