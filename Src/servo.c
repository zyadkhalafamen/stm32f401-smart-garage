#include "servo.h"
#include "app_config.h"

#include <stdint.h>

/* ================= RCC Registers ================= */

#define RCC_APB2ENR (*(volatile uint32_t *)0x40023844U)

/* ================= GPIOA Registers =============== */

#define GPIOA_MODER (*(volatile uint32_t *)0x40020000U)
#define GPIOA_AFRH  (*(volatile uint32_t *)0x40020024U)

/* ================= TIM1 Registers ================ */

#define TIM1_CR1    (*(volatile uint32_t *)0x40010000U)
#define TIM1_EGR    (*(volatile uint32_t *)0x40010014U)
#define TIM1_CCMR1  (*(volatile uint32_t *)0x40010018U)
#define TIM1_CCER   (*(volatile uint32_t *)0x40010020U)
#define TIM1_PSC    (*(volatile uint32_t *)0x40010028U)
#define TIM1_ARR    (*(volatile uint32_t *)0x4001002CU)
#define TIM1_CCR1   (*(volatile uint32_t *)0x40010034U)
#define TIM1_BDTR   (*(volatile uint32_t *)0x40010044U)

void servo_init(void)
{
    /* Enable TIM1 peripheral clock */
    RCC_APB2ENR |= (1U << 0U);

    /* Configure PA8 as alternate-function mode */
    GPIOA_MODER &= ~(3U << 16U);
    GPIOA_MODER |=  (2U << 16U);

    /* Select AF1: TIM1 channel 1 */
    GPIOA_AFRH &= ~(0xFU << 0U);
    GPIOA_AFRH |=  (1U << 0U);

    /*
     * Timer clock = 16 MHz
     * Prescaler = 16
     * Timer resolution = 1 microsecond
     */
    TIM1_PSC = 15U;

    /* 20 ms PWM period = 50 Hz */
    TIM1_ARR = 19999U;

    /* Channel 1: PWM mode 1 with preload */
    TIM1_CCMR1 &= ~(7U << 4U);
    TIM1_CCMR1 |=  (6U << 4U);
    TIM1_CCMR1 |=  (1U << 3U);

    /* Enable channel 1 output */
    TIM1_CCER |= (1U << 0U);

    /* Enable TIM1 main output */
    TIM1_BDTR |= (1U << 15U);

    /* Begin with the gate closed */
    TIM1_CCR1 = SERVO_CLOSED;

    /* Load timer configuration */
    TIM1_EGR = 1U;

    /* Enable auto-reload preload and start timer */
    TIM1_CR1 |= (1U << 7U);
    TIM1_CR1 |= (1U << 0U);
}

void servo_open(void)
{
    TIM1_CCR1 = SERVO_OPEN;
}

void servo_close(void)
{
    TIM1_CCR1 = SERVO_CLOSED;
}
