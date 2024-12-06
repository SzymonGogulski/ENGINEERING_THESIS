#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "hardware/regs/adc.h"
#include "hardware/clocks.h"
#include "pico/time.h"

#define G_LED_PIN 0
#define Y_LED_PIN 1
#define BUTTON_PIN 14
#define ADC_INPUT 26
#define ADC_NUM 0

#define SAMPLE_RATE 16000
#define ACQUISITION_TIME_SEC 5
#define TOTAL_SAMPLES (SAMPLE_RATE * ACQUISITION_TIME_SEC)

#define ADC_NUM 0
#define ADC_PIN 26
#define SIGNAL_MEAN 2050.0f
#define SIGNAL_RANGE 350.0f
#define SIGNAL_MAX (int)(SIGNAL_MEAN + SIGNAL_RANGE)
#define SIGNAL_MIN (int)(SIGNAL_MEAN - SIGNAL_RANGE)

#define BUFFER_SIZE 10
float buffer[BUFFER_SIZE];

void send_buffer_via_usb(float* ptr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%.2f\n", buffer[i]);
    }
}

static inline float convert(uint16_t raw);

void initialize();

int main() {

    initialize();

    while(true){
        while (gpio_get(BUTTON_PIN)) {
            // waiting for button press to start
            printf("press button to start recording. \n");
            sleep_ms(100);
        }

        for (int i=0; i<=2; i++){
            // 3 yellow blinks
            sleep_ms(400);
            gpio_put(Y_LED_PIN, 1);
            sleep_ms(100);
            gpio_put(Y_LED_PIN, 0);
        }

        uint16_t adc_raw;
        float adc_converted;
        uint64_t start_time = time_us_64();
        adc_raw = adc_read(); // raw voltage from ADC
        adc_converted = convert(adc_raw);
        uint64_t end_time = time_us_64();

        uint64_t elapsed_time = end_time - start_time;
        printf("Execution time: %llu microseconds\n", elapsed_time);
    }
    
    return 0;
}

static inline float convert(uint16_t raw){
    if (raw >= SIGNAL_MAX){
        return 1.0f;
    } else if (raw <= SIGNAL_MIN){
        return -1.0f;
    } else {
        return (raw - SIGNAL_MEAN) / SIGNAL_RANGE;
    }
}

void initialize(){

    stdio_init_all();

    // Initialize LED
    gpio_init(G_LED_PIN);
    gpio_set_dir(G_LED_PIN, GPIO_OUT);
    gpio_put(G_LED_PIN, 0);

    gpio_init(Y_LED_PIN);
    gpio_set_dir(Y_LED_PIN, GPIO_OUT);
    gpio_put(Y_LED_PIN, 0);

    // Initialize Button
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // Initialize ADC
    adc_init();
    adc_gpio_init(ADC_INPUT);
    adc_select_input(ADC_INPUT - 26);

    sleep_ms(1000);
}

