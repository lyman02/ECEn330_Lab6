#include "esp_adc/adc_oneshot.h"
#include "joy.h"

#define SAMPLE_COUNT 4

static int16_t joyOriginX;
static int16_t joyOriginY;

adc_oneshot_unit_handle_t adc1_handle;

//Initialize the adc
int32_t joy_init(void){
    
    int x_val;
    int y_val;

    //Configure adc initialization
    adc_oneshot_unit_init_cfg_t init_config1 = {
    .unit_id = ADC_UNIT_1,
    .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //Configure channel settings in adc
    adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_DEFAULT,
    .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_7, &config));

    // Take 4 ADC readings and average them
    for(int i = 0; i < SAMPLE_COUNT; i++){
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &x_val));
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_7, &y_val));

        joyOriginX += x_val;
        joyOriginY += y_val;
    }
    joyOriginX /= SAMPLE_COUNT;
    joyOriginY /= SAMPLE_COUNT;

    return 0;
}

//De-initialize the adc
int32_t joy_deinit(void){
    // If the adc1_handle is non-NULL then delete it.
    if(adc1_handle != NULL){
        ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
    }
    return 0;
}

// Get the joystick displacement from center position.
// Displacement values range from 0 to +/- JOY_MAX_DISP.
// This function is not safe to call from an ISR context.
// Therefore, it must be called from a software task context.
// *dcx: pointer to displacement in x.
// *dcy: pointer to displacement in y.
void joy_get_displacement(int32_t *dcx, int32_t *dcy){
    int x_pos = 0;
    int y_pos = 0;

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &x_pos));
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_7, &y_pos));
    *dcx = (x_pos - joyOriginX);
    *dcy = (y_pos - joyOriginY);
}