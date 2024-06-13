#ifndef APPLE_H_
#define APPLE_H_
#include "lcd.h"

#define APPLE_SIZE 10
#define APPLE_COLOR RED

void initApple(TFT_t *dev);
void drawApple(TFT_t *dev);
void getAppleCoordinates(int32_t *x, int32_t *y);




#endif // APPLE_H_
