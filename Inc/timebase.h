#ifndef TIMEBASE_H_
#define TIMEBASE_H_

#include <stdint.h>

void timebase_init(void);
uint32_t millis(void);
uint32_t time_elapsed(uint32_t start_time, uint32_t period);
void short_delay_ms(uint32_t milliseconds);

#endif /* TIMEBASE_H_ */
