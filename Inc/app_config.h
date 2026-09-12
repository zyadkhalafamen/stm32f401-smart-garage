#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include <stdint.h>

/* LCD pins on GPIOB */
#define LCD_RS                  0U
#define LCD_EN                  1U
#define LCD_D4                 10U
#define LCD_D5                 12U
#define LCD_D6                 13U
#define LCD_D7                 14U

/* Status LEDs on GPIOB */
#define GREEN_LED               5U
#define RED_LED                 6U

/* Servo on PA8 using TIM1 channel 1 */
#define SERVO_CLOSED          1000U
#define SERVO_OPEN            2000U

/* Access-control settings */
#define PASSWORD_LENGTH         4U
#define GARAGE_CAPACITY         5U
#define MAX_FAILED_ATTEMPTS     3U

/* Timing values in milliseconds */
#define LOCKOUT_TIME_MS     30000U
#define GATE_TRAVEL_TIME_MS   800U
#define GATE_OPEN_TIME_MS     5000U
#define MESSAGE_TIME_MS       1800U

#endif /* APP_CONFIG_H_ */
