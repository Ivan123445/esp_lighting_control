#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "../common_struct.h"
#include "led_control.c"

#define LINE_1 2
#define LINE_2 22
#define LINE_3 23
#define UPDATE_LINE_FREQUENCY 100


void control_line_exec(void *param) {
    struct app_state_t *app_state = param;
    led_init();

    gpio_pad_select_gpio(22);
    gpio_set_direction (22,GPIO_MODE_OUTPUT);

    while (1) {
        gpio_set_level(LINE_1, app_state->line_state&1);
        gpio_set_level(LINE_2, app_state->line_state&0b10);
        gpio_set_level(LINE_3, app_state->line_state&0b10);

        vTaskDelay(pdMS_TO_TICKS(UPDATE_LINE_FREQUENCY));
    }
}
