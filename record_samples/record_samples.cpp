#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

// GPIO PINS
#define G_LED_PIN 0
#define Y_LED_PIN 1
#define R_LED_PIN 2
#define BUTTON_PIN 14

// SETUP STDIO, GPIO
void setup(){

    stdio_init_all();

    // Initialize LED
    gpio_init(G_LED_PIN);
    gpio_set_dir(G_LED_PIN, GPIO_OUT);
    gpio_put(G_LED_PIN, 0);

    gpio_init(Y_LED_PIN);
    gpio_set_dir(Y_LED_PIN, GPIO_OUT);
    gpio_put(Y_LED_PIN, 0);

    gpio_init(R_LED_PIN);
    gpio_set_dir(R_LED_PIN, GPIO_OUT);
    gpio_put(R_LED_PIN, 0);

    // Initialize Button
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    sleep_ms(1000);
}

// ADC SETUP
#define ADC_PIN 26
#define ADC_NUM 0

void setup_adc(){
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_NUM);
}

// BUFFER SETUP AND OPERATIONS
#define FREQUENCY 16000
#define SAMPLE_PERIOD_US 62
#define SECONDS 1
#define BUFFER_SIZE (FREQUENCY * SECONDS) 
uint16_t buffer[BUFFER_SIZE];

void record(uint16_t* ptr, int size){
    
    for (int i=0; i<size; i++){
        *(ptr+i) = adc_read();
        sleep_us(SAMPLE_PERIOD_US);
    }
}

void transmit(uint16_t* ptr, int size){
    
    for (int i=0; i<size; i++){
        printf("%d \n", *(ptr+i));
    }
}   

int main() {

    setup();
    setup_adc();

    while (true) {

        // WAIT FOR THE BUTTON TO BE PRESSED
        gpio_put(R_LED_PIN, 1);
        while (gpio_get(BUTTON_PIN)) {
            tight_loop_contents();}
        sleep_ms(100);
        gpio_put(R_LED_PIN, 0);
        
        // RECORD SAMPLES
        gpio_put(Y_LED_PIN, 1);
        record(buffer, BUFFER_SIZE);
        gpio_put(Y_LED_PIN, 0);

        // send recorded sample to USB
        gpio_put(G_LED_PIN, 1);
        transmit(buffer, BUFFER_SIZE);
        gpio_put(G_LED_PIN, 0);        
    }

    return 0;
}
