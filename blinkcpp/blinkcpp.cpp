#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

int main()
{
    stdio_init_all();
    cyw43_arch_init();
    const uint BUTTON_PIN = 13; // GPIO pin 13 for the button

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN); // Set the button pin as input
    gpio_pull_up(BUTTON_PIN);          // Enable internal pull-up resistor

    while (true)
    {
        // Check if the button is pressed (LOW means pressed since it's connected to GND)
        if (gpio_get(BUTTON_PIN) == 0)
        {
            printf("Button pressed\n");
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        }
        else
        {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        }

        printf("Hello, world! C++ 2\n");
        sleep_ms(1000);
    }
}
