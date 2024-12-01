#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"



int main() {

    stdio_init_all();

    const uint LED_PIN = 0;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    if (cyw43_arch_init()) {
        printf("WiFi init failed");
        return -1;
    }

    while (true) {
        printf("Hello, world!\n");
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        gpio_put(LED_PIN, true);
        sleep_ms(1000);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        gpio_put(LED_PIN, false);
        sleep_ms(1000);
    }

    return 0;
}