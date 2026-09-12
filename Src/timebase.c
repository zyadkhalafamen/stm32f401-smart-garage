#include "timebase.h"

/* ================= SysTick Registers ================= */

#define SYST_CSR (*(volatile uint32_t *)0xE000E010U)
#define SYST_RVR (*(volatile uint32_t *)0xE000E014U)
#define SYST_CVR (*(volatile uint32_t *)0xE000E018U)

static volatile uint32_t system_ms = 0U;

/* يتم استدعاؤها تلقائياً كل 1 millisecond */
void SysTick_Handler(void)
{
    system_ms++;
}

void timebase_init(void)
{
    /*
     * Processor clock = 16 MHz
     * 16000 clock cycles = 1 millisecond
     */
    SYST_RVR = 16000U - 1U;
    SYST_CVR = 0U;

    /*
     * Bit 0: Enable SysTick
     * Bit 1: Enable interrupt
     * Bit 2: Use processor clock
     */
    SYST_CSR = 7U;
}

uint32_t millis(void)
{
    return system_ms;
}

uint32_t time_elapsed(uint32_t start_time, uint32_t period)
{
    return ((uint32_t)(millis() - start_time) >= period);
}

/*
 * تستخدم فقط في توقيت شاشة LCD والـstartup.
 * لا تستخدم داخل منطق الباب الرئيسي.
 */
void short_delay_ms(uint32_t milliseconds)
{
    uint32_t start_time = millis();

    while (!time_elapsed(start_time, milliseconds))
    {
    }
}
