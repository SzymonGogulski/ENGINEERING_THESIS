#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "pico/binary_info.h"

#define ADC_NUM 0
#define ADC_PIN 26
#define SIGNAL_MEAN 2050.0f
#define SIGNAL_RANGE 350.0f
#define SIGNAL_MAX (int)(SIGNAL_MEAN + SIGNAL_RANGE)
#define SIGNAL_MIN (int)(SIGNAL_MEAN - SIGNAL_RANGE)

float convert(uint16_t raw){
    if (raw >= SIGNAL_MAX){
        return 1.0f;
    } else if (raw <= SIGNAL_MIN){
        return -1.0f;
    } else {
        return (raw - SIGNAL_MEAN) / SIGNAL_RANGE;
    }
}

int main() {
    stdio_init_all();

    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_NUM);

    uint16_t adc_raw;
    while (1) {
        adc_raw = adc_read(); // raw voltage from ADC
        printf("%.2f\n", convert(adc_raw));
        sleep_ms(10);
    }
}
