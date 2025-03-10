#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

volatile int g_timer_0 = 0;
volatile int g_timer_1 = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_r = !flag_r;
        else if (gpio == BTN_PIN_G)
            flag_g = !flag_g;
    }
}

bool timer_0_callback(repeating_timer_t *rt) {
    g_timer_0 = 1;
    return true; // keep repeating
}

bool timer_1_callback(repeating_timer_t *rt) {
    g_timer_1 = 1;
    return true; // keep repeating
}

int main() {
    int LED_STATE_R = 0;
    int LED_STATE_G = 0;

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    repeating_timer_t timer_0;
    repeating_timer_t timer_1;
    int timer_r_exists = 0;
    int timer_g_exists = 0;

    while (true) {

        if (flag_r) {
            flag_r = 0;
            if (timer_r_exists) {
                cancel_repeating_timer(&timer_0);
                gpio_put(LED_PIN_R, 0);
                LED_STATE_R = 0;
                timer_r_exists = 0;
            } else {
                if (!add_repeating_timer_ms(-500, timer_0_callback, NULL, &timer_0)) {
                    printf("Failed to add timer\n");
                } else {
                    timer_r_exists = 1;
                }
            }
        }

        if (flag_g) {
            flag_g = 0;
            if (timer_g_exists) {
                cancel_repeating_timer(&timer_1);
                gpio_put(LED_PIN_G, 0);
                LED_STATE_G = 0;
                timer_g_exists = 0;
            } else {
                if (!add_repeating_timer_ms(-250, timer_1_callback, NULL, &timer_1)) {
                    printf("Failed to add timer\n");
                } else {
                    timer_g_exists = 1;
                }
            }
        }

        if (g_timer_0) {
            gpio_put(LED_PIN_R, !LED_STATE_R);
            LED_STATE_R = !LED_STATE_R;
            g_timer_0 = 0;
        }

        if (g_timer_1) {
            gpio_put(LED_PIN_G, !LED_STATE_G);
            LED_STATE_G = !LED_STATE_G;
            g_timer_1 = 0;
        }
    }
}
