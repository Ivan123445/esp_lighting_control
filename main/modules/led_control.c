#include "driver/gpio.h"

#define LED_PIN 2

void led_init() {
    gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction (LED_PIN,GPIO_MODE_OUTPUT);
}

void led_on() { gpio_set_level(LED_PIN,1); }

void led_off() { gpio_set_level(LED_PIN,0); }
