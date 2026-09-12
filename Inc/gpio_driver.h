#ifndef GPIO_DRIVER_H_
#define GPIO_DRIVER_H_

#include <stdint.h>

void gpio_driver_init(void);

void gpioa_write(uint32_t pin, uint32_t state);
void gpiob_write(uint32_t pin, uint32_t state);

uint32_t gpioa_read(uint32_t pin);

void green_led_set(uint32_t state);
void red_led_set(uint32_t state);

#endif /* GPIO_DRIVER_H_ */
