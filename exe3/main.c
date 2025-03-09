#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;
volatile int start_ms = 0;
volatile int finish_ms = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        if (gpio == BTN_PIN_R) {
            start_ms = to_ms_since_boot(get_absolute_time());
            flag_f_r = 1;
        }

    } else if (events == 0x8) { // rise edge
        if (gpio == BTN_PIN_R) {
            finish_ms = to_ms_since_boot(get_absolute_time());
            flag_f_r = 0;
        }
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    int LED_STATE = 0;

    while (true) {
        if (!flag_f_r) {
            if (finish_ms - start_ms >= 500) {
                gpio_put(LED_PIN_R, !LED_STATE);
                LED_STATE = !LED_STATE;
            } else {
                start_ms = 0;
                finish_ms = 0;
            }
        }
    }
}
