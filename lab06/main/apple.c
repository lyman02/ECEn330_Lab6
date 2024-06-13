#include <stdlib.h>
#include <time.h>
#include "apple.h"

// Constants

static int32_t apple_x;
static int32_t apple_y;

// Initialize the apple by seeding the random number generator
void initApple(TFT_t *dev) {
    // Initialize random seed
    srand(time(NULL));
}

// Draw the apple at a random location
void drawApple(TFT_t *dev) {
    // Generate random x and y coordinates within the screen bounds
    int32_t x = rand() % (LCD_W - APPLE_SIZE);
    int32_t y = rand() % (LCD_H - APPLE_SIZE);

    // Draw the red square (apple) at the random coordinates
    lcdFillRect(dev, x, y, x + APPLE_SIZE - 1, y + APPLE_SIZE - 1, APPLE_COLOR);
}

void getAppleCoordinates(int32_t *x, int32_t *y)
{
    *x = apple_x;
    *y = apple_y;
}