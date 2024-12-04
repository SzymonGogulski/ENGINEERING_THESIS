#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/adc.h"
#include "hardware/timer.h"

// Program definitions
#define GREEN_LED 0
#define BUTTON_1 14
#define BUTTON_2 15
#define ADC_PIN 26

// Constants
const float FREQUENCY = 16000.0;
const uint16_t NUM_SAMPLES = 16000;
const float SAMPLE_PERIOD = 1.0 / FREQUENCY;
const uint16_t SAMPLE_PERIOD_US = SAMPLE_PERIOD * 1000 * 1000;

// Variables
int16_t buffer[NUM_SAMPLES];

void initialize(){
    
    // STDIO
    stdio_init_all();

    // GPIO
    gpio_init(GREEN_LED);
    gpio_init(BUTTON_1);
    gpio_init(BUTTON_2);
    gpio_set_dir(GREEN_LED, GPIO_OUT);
    gpio_put(GREEN_LED, false);
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
    // Ensure the buffer is cleared (optional)
    memset(buffer, 0, sizeof(buffer));
    
    // The ADC clock divider for 16kHz sample rate
    float adc_clkdiv = 3000;
    adc_set_clkdiv(adc_clkdiv);
    
    // Configure the ADC FIFO
    adc_fifo_setup(
        true,    // Write each completed conversion to the FIFO
        true,    // Enable DMA data request (DREQ)
        1,       // DREQ (DMA request) when at least 1 sample is present
        false,   // Disable error bit in FIFO
        false    // Don't shift bits; keep 12-bit data right-aligned
    );
    
    // Select ADC input (ADC0)
    adc_select_input(0);
    
    // Clear the ADC FIFO
    adc_fifo_drain();
    
    // Claim a free DMA channel
    int dma_chan = dma_claim_unused_channel(true);
    
    // Configure the DMA channel
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16); // 16-bit transfers
    channel_config_set_read_increment(&c, false);           // Read from fixed address (ADC FIFO)
    channel_config_set_write_increment(&c, true);           // Write to incrementing buffer address
    channel_config_set_dreq(&c, DREQ_ADC);                  // Set DREQ to ADC
    
    // Configure the DMA channel with the ADC FIFO as source and buffer as destination
    dma_channel_configure(
        dma_chan,
        &c,
        buffer,               // Destination pointer (buffer)
        &adc_hw->fifo,        // Source pointer (ADC FIFO)
        NUM_SAMPLES,          // Number of transfers
        false                 // Don't start yet
    );
    
    // Start the ADC
    adc_run(true);
    
    // Start the DMA transfer
    dma_channel_start(dma_chan);
    
    // Wait for the DMA transfer to complete
    dma_channel_wait_for_finish_blocking(dma_chan);
    
    // Stop the ADC
    adc_run(false);
    
    // Drain the ADC FIFO (optional)
    adc_fifo_drain();
    
    // Release the DMA channel
    dma_channel_unclaim(dma_chan);
}


void analize(){
    printf("damn");
}

void transmit(){
    printf("damn");
}

int main() {

    // Inicjalizacja peryferiow
    initialize();

    
    while (true) {

        
    }

    return 0;
}
