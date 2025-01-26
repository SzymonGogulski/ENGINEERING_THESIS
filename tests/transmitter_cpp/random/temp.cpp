#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "tusb.h"

// Pin and ADC Configuration
#define ADC_PIN 26
#define ADC_FREQUENCY 16000      // Sampling frequency
#define ACQUISITION_TIME_SEC 5   // Acquisition time
#define TOTAL_SAMPLES (ADC_FREQUENCY * ACQUISITION_TIME_SEC)
#define BUFFER_SIZE 1024         // Circular buffer size
#define SIGNAL_MEAN 2050.0f      // Signal mean for normalization
#define SIGNAL_RANGE 350.0f      // Signal range for normalization
#define SIGNAL_RANGE_INV (1.0f / SIGNAL_RANGE)

// Circular buffer for ADC samples
uint16_t buffer[BUFFER_SIZE];
volatile uint16_t write_index = 0;
volatile uint16_t read_index = 0;
int dma_channel;

// Normalize function: Convert uint16_t to float32
static inline float normalize(uint16_t raw) {
    float normalized = (raw - SIGNAL_MEAN) * SIGNAL_RANGE_INV;
    normalized = (normalized > 1.0f) ? 1.0f : normalized;
    normalized = (normalized < -1.0f) ? -1.0f : normalized;
    return normalized;
}

// USB Task: Transmit normalized samples via USB
void usb_task() {
    if (tud_cdc_connected()) {
        while (read_index != write_index) {
            // Normalize the sample
            float normalized_value = normalize(buffer[read_index]);
            
            // Transmit normalized sample
            tud_cdc_write(&normalized_value, sizeof(normalized_value));

            // Advance the read index
            read_index = (read_index + 1) % BUFFER_SIZE;
        }
        tud_cdc_write_flush(); // Ensure USB data is sent
    }
}

// DMA IRQ Handler: Advance write_index when DMA writes a new sample
void dma_irq_handler() {
    // Clear the DMA interrupt
    dma_hw->ints0 = (1u << dma_channel);

    // Advance the write index
    write_index = (write_index + 1) % BUFFER_SIZE;
}

// Setup ADC and DMA
void setup_adc_dma() {
    // Initialize ADC
    adc_init();
    adc_gpio_init(ADC_PIN);  // Initialize GPIO for ADC
    adc_select_input(0);     // Select ADC input 0 (ADC0)

    // Set ADC clock divider for 16 kHz sampling rate
    float clkdiv = 48e6 / ADC_FREQUENCY; // ADC clock: 48 MHz
    adc_set_clkdiv(clkdiv);

    // Configure ADC FIFO
    adc_fifo_setup(
        true,  // Enable FIFO
        true,  // Enable DMA request
        1,     // FIFO threshold (1 sample triggers DMA)
        false, // No error flag
        true   // Shift samples to 12 bits
    );

    // Initialize DMA
    dma_channel = dma_claim_unused_channel(true); // Get a free DMA channel
    dma_channel_config dma_config = dma_channel_get_default_config(dma_channel);

    // Configure DMA channel
    channel_config_set_transfer_data_size(&dma_config, DMA_SIZE_16);  // 16-bit transfers
    channel_config_set_read_increment(&dma_config, false);  // No increment for ADC FIFO
    channel_config_set_write_increment(&dma_config, true);  // Increment for buffer
    channel_config_set_dreq(&dma_config, DREQ_ADC);  // Trigger on ADC FIFO ready

    // Configure DMA to write to circular buffer
    dma_channel_configure(
        dma_channel,
        &dma_config,
        buffer,                // Destination (circular buffer)
        &adc_hw->fifo,         // Source (ADC FIFO)
        BUFFER_SIZE,           // Transfer count
        true                   // Start the transfer immediately
    );

    // Enable DMA interrupt
    dma_channel_set_irq0_enabled(dma_channel, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    // Start ADC
    adc_run(true);
}

int main() {
    // Initialize stdio and USB
    stdio_init_all();
    
}