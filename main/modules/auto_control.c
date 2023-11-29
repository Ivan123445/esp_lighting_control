#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/gpio.h"

#include "esp_log.h"

#include "../common_struct.h"

#define THRESHOLD_VAL_OF_ILLUMINATION 1800
#define HYSTERESIS                    300
#define AUTO_MODE_UPDATE_FREQUENCY    1000

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

static esp_adc_cal_characteristics_t *adc_chars = NULL;

static const adc_channel_t channel = ADC_CHANNEL_0;
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;

static const adc_atten_t atten = ADC_ATTEN_DB_11;

#define COL_SAMPLES 10

static const char *TAG_AUTO_CONTROL = "wifi station";

static void check_efuse(void)
{
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }
    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
}

static int get_degree_of_illumination() {
    uint32_t adc_reading = 0;
    for (int i = 0; i < COL_SAMPLES; ++i) {
        adc_reading += adc1_get_raw((adc1_channel_t)channel);
    }
    adc_reading /= COL_SAMPLES;
    return adc_reading;
}

void auto_control_exec(void *parametrs) {
    struct app_state_t *app_state = parametrs;
    check_efuse();

    adc1_config_width(width);
    adc1_config_channel_atten(channel, atten);

    int adc_reading = 0;
    while (1) {
//        ESP_LOGI(TAG_AUTO_CONTROL, "AUTO_CONTROL_IS_WORKING");

        if (app_state->common_auto_mode) {
            adc_reading = get_degree_of_illumination();

            if (adc_reading >= THRESHOLD_VAL_OF_ILLUMINATION + HYSTERESIS) {
                app_state->line_state &= !app_state->individual_auto_mode;
            } else if (adc_reading < THRESHOLD_VAL_OF_ILLUMINATION - HYSTERESIS) {
                app_state->line_state |= app_state->individual_auto_mode;
            }
        }

//        ESP_LOGI(TAG_AUTO_CONTROL, "adc_reading: %d", adc_reading);


//        ESP_LOGI(TAG_AUTO_CONTROL, "");
        vTaskDelay(pdMS_TO_TICKS(AUTO_MODE_UPDATE_FREQUENCY));
    }

}