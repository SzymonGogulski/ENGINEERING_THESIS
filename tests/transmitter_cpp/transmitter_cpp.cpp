#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

#define BUFFER_SIZE 10
#define ADC_SYS_CLOCK 48000000 // Default ADC system clock in Hz

static uint16_t buffer[BUFFER_SIZE];  
static int dma_chan;

// DMA interrupt handler
void dma_handler() {
    // Clear the interrupt request
    dma_hw->ints0 = 1u << dma_chan; 

    // Reconfigure the DMA to start again from the beginning of the buffer
    // This causes the DMA to continuously overwrite the buffer in a circular fashion.
    dma_channel_set_read_addr(dma_chan, &adc_hw->fifo, true);
}

void fill_buffer() {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }
}

void setup_adc_dma() {
    // Initialize the ADC
    adc_init();
    // GPIO26 is ADC0
    adc_gpio_init(26);
    adc_select_input(0);

    // Set up the ADC FIFO:
    // Enable the FIFO, set sample width to 12 bits, and shift result into bottom bits of FIFO
    adc_fifo_setup(
        true,    // Write each completed conversion to the FIFO
        true,    // Enable DMA data request (DREQ) when FIFO is not empty
        1,       // DREQ (DMA) asserts when at least 1 sample is present
        false,   // No ERR bit
        true     // Shift to 12 bits
    );

    // Set ADC clock divider very high to approximate ~1 sample/sec
    // Each conversion takes 96 ADC clock cycles. With 48MHz and a divider of 500000:
    // Effective ADC clock ~ 96 Hz, so roughly 1 sample/sec.
    adc_set_clkdiv(500000.0f); 

    // Enable the ADC
    adc_run(true);

    // Set up DMA
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_chan);

    // Reading from a fixed address (ADC FIFO), writing incrementally to buffer
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16); // 16-bit samples
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_ADC); // Pace transfers by ADC FIFO

    // Configure the channel
    dma_channel_configure(
        dma_chan,
        &c,
        buffer,            // Destination pointer
        &adc_hw->fifo,     // Source pointer
        BUFFER_SIZE,       // Number of transfers (each is 16 bits)
        false              // Don't start yet
    );

    // Enable DMA IRQ on completion
    dma_channel_set_irq0_enabled(dma_chan, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    // Start the first DMA transfer
    dma_channel_start(dma_chan);
}

int main() {
    stdio_init_all();
    fill_buffer();

    // Main loop prints the buffer every second
    while (true) {
        // Print the buffer contents
        for (int i = 0; i < BUFFER_SIZE; i++) {
            printf("%d ", buffer[i]);
        }
        printf("\n");
        sleep_ms(1000);
    }

    return 0;
}
