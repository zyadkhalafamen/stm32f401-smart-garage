#include "timebase.h"
#include "gpio_driver.h"
#include "lcd.h"
#include "keypad.h"
#include "servo.h"
#include "access_control.h"

int main(void)
{
    char pressed_key;

    timebase_init();
    gpio_driver_init();
    keypad_init();
    servo_init();
    lcd_init();
    access_control_init();

    while (1)
    {
        pressed_key = keypad_get_key();

        if (pressed_key != 0)
        {
            access_control_handle_key(pressed_key);
        }

        access_control_task();
    }
}
