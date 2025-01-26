#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LED_PIN 0
#define BUTTON_PIN 14

// We'll store the LED state in a variable we can update in the IRQ
static volatile bool led_state = false;

// Interrupt service routine for GPIO events
static void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_PIN) {
        // Toggle LED state
        led_state = !led_state;
        gpio_put(G_LED_PIN, led_state);
    }
}

int main() {
    stdio_init_all();

    // Initialize the LED GPIO
    gpio_init(G_LED_PIN);
    gpio_set_dir(G_LED_PIN, GPIO_OUT);
    gpio_put(G_LED_PIN, led_state);

    // Initialize the button GPIO
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // Configure interrupt on falling edge (button press)
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // In this example, main just sleeps while interrupts handle the logic
    while (true) {
        tight_loop_contents();
    }

    return 0;
}
