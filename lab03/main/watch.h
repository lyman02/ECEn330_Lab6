#ifndef WATCH_H_
#define WATCH_H_

#include <stdint.h>

#include "lcd.h" // TFT_t

// Initialize the watch face. The device dev must be valid.
void watch_init(TFT_t *dev);

// Update the watch digits based on timer_ticks (1/100th of a second).
void watch_update(TFT_t *dev, uint32_t timer_ticks);

#endif // WATCH_H_
