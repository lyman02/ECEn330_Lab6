#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "lcd.h"
#include "lcd_test.h"

static const char *TAG = "lcd_test";

void app_main(void)
{
	ESP_LOGI(TAG, "Start up");

	xTaskCreate(LCD, "LCD", 1024*6, NULL, 2, NULL);
}
