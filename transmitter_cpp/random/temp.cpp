#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "hardware/regs/adc.h"
#include "hardware/clocks.h"
#include "pico/time.h"

#define LED_PIN 0
#define BUTTON_PIN 14
#define ADC_INPUT 26
#define ADC_NUM 0

#define SAMPLE_RATE 16000
#define ACQUISITION_TIME_SEC 5
#define TOTAL_SAMPLES (SAMPLE_RATE * ACQUISITION_TIME_SEC)

// Conversion parameters
#define SIGNAL_MEAN 2050.0f
#define SIGNAL_RANGE 350.0f
#define SIGNAL_MAX (int)(SIGNAL_MEAN + SIGNAL_RANGE)
#define SIGNAL_MIN (int)(SIGNAL_MEAN - SIGNAL_RANGE)

static inline float convert(uint16_t raw);
void initialize();

int main(){

    
    while (gpio_get(BUTTON_PIN) == 1) {
        tight_loop_contents();
    }

    // Button pressed: turn LED on
    gpio_put(G_LED_PIN, 1);

    uint32_t sample_interval_us = 62; 

    // To reduce UART overhead, buffer samples before sending
    const int BATCH_SIZE = 512;
    float sample_buffer[BATCH_SIZE];
    int buffer_index = 0;

    // Acquire samples
    for (int i = 0; i < TOTAL_SAMPLES; i++) {
        // Read from ADC
        uint16_t raw = adc_read(); 
        float val = convert(raw);

        sample_buffer[buffer_index++] = val;
        if (buffer_index >= BATCH_SIZE) {
            // Send batch over UART (stdout)
            for (int j = 0; j < BATCH_SIZE; j++) {
                // Print as float. You might choose a more compact format if needed.
                // For example: printf("%f\n", sample_buffer[j]);
                // Or send binary. Here we use text for simplicity.
                printf("%f\n", sample_buffer[j]);
            }
            buffer_index = 0;
        }
    }
        // Send remaining samples if any
    for (int j = 0; j < buffer_index; j++) {
        printf("%f\n", sample_buffer[j]);

        sleep_us(sample_interval_us);
    }


    // After all samples transmitted, turn LED off
    gpio_put(G_LED_PIN, 0);

    // End
    while (1) {
        tight_loop_contents();
    }

    return 0;
}


void initialize(){

    stdio_init_all();
    printf("watchout 1\n");
    sleep_ms(10000);
    printf("watchout 2\n");

    // Initialize LED
    gpio_init(G_LED_PIN);
    gpio_set_dir(G_LED_PIN, GPIO_OUT);
    gpio_put(G_LED_PIN, 0);

    // Initialize Button
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // Initialize ADC
    adc_init();
    adc_gpio_init(ADC_INPUT);
    adc_select_input(ADC_INPUT - 26);
}

static inline float convert(uint16_t raw) {
    if (raw >= SIGNAL_MAX) {
        return 1.0f;
    } else if (raw <= SIGNAL_MIN) {
        return -1.0f;
    } else {
        return (raw - SIGNAL_MEAN) / SIGNAL_RANGE;
    }
}