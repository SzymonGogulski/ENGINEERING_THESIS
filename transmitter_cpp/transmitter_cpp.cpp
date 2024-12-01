#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#define GREEEN_LED 0
#define BUTTON_1 14
#define BUTTON_2 15

bool state = false;

void gpio_initialize(){
    gpio_init(GREEEN_LED);
    gpio_init(BUTTON_1);
    gpio_init(BUTTON_2);
    gpio_set_dir(GREEEN_LED, GPIO_OUT);
    gpio_set_dir(BUTTON_1, GPIO_IN);
    gpio_set_dir(BUTTON_2, GPIO_IN);
    gpio_pull_up(BUTTON_1);
    gpio_pull_up(BUTTON_2);
}

int main() {

    // Inicjalizacja peryferiow
    stdio_init_all();
    gpio_initialize();

    if (cyw43_arch_init()) {
        printf("WiFi init failed");
        return -1;
    }

    bool state = false;

    while (true) {

        if (stdio_usb_connected() && getchar_timeout_us(0) >= 0) {
            char user_input = getchar();

            if(user_input == '1'){
                state = true;
                printf("recived '1' \n");
            }
            else if (user_input == '0'){
                state = false;
                printf("recived '0' \n");
            }
            else{
                state = false;
                printf("recived unrecognized char '%c'\n", user_input);
            }
        }

        
        if (gpio_get(BUTTON_1) == 0){
            state=true;
        }

        if (gpio_get(BUTTON_2) == 0){
            state=false;
        }

        if (state == true){
            gpio_put(GREEEN_LED, true);
            sleep_ms(100);
            gpio_put(GREEEN_LED, false);
            sleep_ms(100);
        }
            
    }

    return 0;
}
