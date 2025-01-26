#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "tusb.h"

// GPIO PINS
#define G_LED_PIN 0
#define Y_LED_PIN 1
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

    // Initialize Button
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    sleep_ms(1000);
}

// NORMALIZATION
#define SIGNAL_MEAN 2050.0f
#define SIGNAL_RANGE 350.0f
#define SIGNAL_MAX (int)(SIGNAL_MEAN + SIGNAL_RANGE)
#define SIGNAL_MIN (int)(SIGNAL_MEAN - SIGNAL_RANGE)
#define SIGNAL_RANGE_INV (1.0f / SIGNAL_RANGE)

static inline float normalize(uint16_t raw) {
    float normalized = (raw - SIGNAL_MEAN) * SIGNAL_RANGE_INV;
    normalized = (normalized > 1.0f) ? 1.0f : normalized;
    normalized = (normalized < -1.0f) ? -1.0f : normalized;
    return normalized;
}

// ADC SETUP
#define ADC_PIN 26
#define ADC_NUM 0
#define ADC_FREQUENCY 16000

// CIRCULAR BUFFER
#define BUFFER_SIZE 1024
uint16_t buffer[BUFFER_SIZE];
volatile uint16_t write_index = 0;
volatile uint16_t read_index = 0;
int dma_channel;

// TINY USB BULK TRANSMIT
void usb_task() {

    if (tud_cdc_connected()) {
        printf("USB connected, checking buffer...\n");
        while (read_index != write_index) {
            printf("Writing index %d\n", write_index);
            printf("Reading index %d\n", read_index);

            float normalized_value = normalize(buffer[read_index]);
            tud_cdc_write(&normalized_value, sizeof(normalized_value));
            read_index = (read_index + 1) % BUFFER_SIZE;
        }
        tud_cdc_write_flush();
    }
}

// DMA INTERRUP HANDLER, ADVANCE THE WRITE INDEX
void dma_irq_handler() {
    dma_hw->ints0 = (1u << dma_channel);
    write_index = (write_index + 1) % BUFFER_SIZE;

    // OVERFLOW CHECK
    if (write_index == read_index) {
        gpio_put(Y_LED_PIN, 1);}
}

// DMA AND ADC SETUP
void setup_adc_dma() {
    // INITIALIZE ADC
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_NUM);
    float clkdiv = 48e6 / ADC_FREQUENCY;
    adc_set_clkdiv(clkdiv);
    adc_fifo_setup(
        true, 
        true, 
        1, 
        false, 
        true);

    // INITIALIZE DMA
    dma_channel = dma_claim_unused_channel(true);
    dma_channel_config dma_config = dma_channel_get_default_config(dma_channel);
    channel_config_set_transfer_data_size(&dma_config, DMA_SIZE_16);
    channel_config_set_read_increment(&dma_config, false);
    channel_config_set_write_increment(&dma_config, true);
    channel_config_set_dreq(&dma_config, DREQ_ADC);
    dma_channel_configure(
        dma_channel,
        &dma_config,
        buffer, 
        &adc_hw->fifo, 
        BUFFER_SIZE, 
        true);

    // DMA INTERRUPT TO ADVANCE WRITE INDEX
    dma_channel_set_irq0_enabled(dma_channel, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    // START ADC
    adc_run(true);
}

// MAIN LOOP, TRANSMIT BULK USB
int main() {

    setup();
    
    tusb_init();

    // WAIT FOR HOST CONNECTION
    while (!tud_cdc_connected()){ 
        printf("Waiting for host connection. \n");
        sleep_ms(1000);
    }

    // START READING DATA
    setup_adc_dma();

    // START TRANSMITING DATA
    while (true) {
        usb_task();
    }
}





    // // BLINK YELLOW DIODE ONCE FOR 3 SECONDS
    // gpio_put(Y_LED_PIN, 1);
    // sleep_ms(3000);
    // gpio_put(Y_LED_PIN, 0);

    // // WAIT FOR USER BUTTON TO START RECORDING
    // while (gpio_get(BUTTON_PIN)) {
    //     sleep_ms(50);
    // }

    // // BLINK YELLOW DIODE 3 TIMES
    // for (int i=0; i<=2; i++){
    //     sleep_ms(400);
    //     gpio_put(Y_LED_PIN, 1);
    //     sleep_ms(100);
    //     gpio_put(Y_LED_PIN, 0);
    // }