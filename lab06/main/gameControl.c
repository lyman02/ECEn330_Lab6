
#include <stdio.h>
#include <stdlib.h> // rand

#include "config.h"
#include "lcd.h"
#include "cursor.h"
#include "pin.h"
#include "btn.h"
#include "gameControl.h"



//Define stats constants
#define Y_MARGIN 10
#define X_MARGIN_SHOTS 20
#define X_MARGIN_IMPACTED 200



// Initialize the game control logic.
// This function initializes all missiles, planes, stats, etc.
void gameControl_init(void)
{
	
}

// Update the game control logic.
// This function calls the missile & plane tick functions, reinitializes
// idle enemy missiles, handles button presses, fires player missiles,
// detects collisions, and updates statistics.
void gameControl_tick(void)
{
	
}
