#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/timer.h"

// Zmienne programowe
#define GREEEN_LED 0
#define BUTTON_1 14
#define BUTTON_2 15
#define ADC_PIN 26

// Stale
const float FREQUENCY = 16000.0;
const uint16_t NUM_SAMPLES = 16000;
const float SAMPLE_PERIOD = 1.0 / FREQUENCY;
const uint16_t SAMPLE_PERIOD_US = SAMPLE_PERIOD * 1000 * 1000;

// Zmienne
int16_t buffer[NUM_SAMPLES];


void initialize(){
    
    // STDIO
    stdio_init_all();

    // GPIO
    gpio_init(GREEEN_LED);
    gpio_init(BUTTON_1);
    gpio_init(BUTTON_2);
    gpio_set_dir(GREEEN_LED, GPIO_OUT);
    gpio_set_dir(BUTTON_1, GPIO_IN);
    gpio_set_dir(BUTTON_2, GPIO_IN);
    gpio_pull_up(BUTTON_1);
    gpio_pull_up(BUTTON_2);

    // ADC
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(0);
}

void record(){
    printf("damn");
}

int main() {

    // Inicjalizacja peryferiow
    initialize();

    
    while (true) {

        uint16_t result = adc_read();
        // printf("%d\n", result);
        sleep_ms(5000);
        printf("sample period = %d\n", SAMPLE_PERIOD_US);

    }

    return 0;
}





// NOTATKI

// INICJALIZACJA CYW43
// #include "pico/cyw43_arch.h"
// CYW43
// if (cyw43_arch_init()) {
//     printf("WiFi init failed");
//     return -1;
// }


// LED ON OFF Z PRZYCISKAMI I PRZEZ UART
// bool state = false;

// while (true) {

//     if (stdio_usb_connected() && getchar_timeout_us(0) >= 0) {
//         char user_input = getchar();

//         if(user_input == '1'){
//             state = true;
//             printf("recived '1' \n");
//         }
//         else if (user_input == '0'){
//             state = false;
//             printf("recived '0' \n");
//         }
//         else{
//             state = false;
//             printf("recived unrecognized char '%c'\n", user_input);
//         }
//     }
//     if (gpio_get(BUTTON_1) == 0){
//         state=true;
//     }
//     if (gpio_get(BUTTON_2) == 0){
//         state=false;
//     }
//     if (state == true){
//         gpio_put(GREEEN_LED, true);
//         sleep_ms(100);
//         gpio_put(GREEEN_LED, false);
//         sleep_ms(100);
//     }
        
// }

