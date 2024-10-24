#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

int main()
{
    stdio_init_all();
    cyw43_arch_init();
    bool led_state = 0;

    while (true)
    {
        // Check if the button is pressed (LOW means pressed since it's connected to GND)
        if (led_state == 0)
        {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            led_state = 1;
        }
        else
        {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            led_state = 0;
        }

        printf("Hello, world! C++\n");
        sleep_ms(1000);
    }
}
