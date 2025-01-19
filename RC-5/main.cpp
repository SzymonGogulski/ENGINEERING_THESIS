#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pwm.h"

#define IR_PIN 0
#define BUTTON_PIN 14
#define WRAP 3472
#define DUTY (uint32_t)(WRAP * 0.25)

void setup(){

    stdio_init_all();

    // Configure GPIO 0 as a PWM output
    gpio_set_function(IR_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(IR_PIN);
    pwm_set_enabled(slice_num, true);
    pwm_set_wrap(slice_num, WRAP);
    pwm_set_gpio_level(IR_PIN, 0);

    // Initialize Button
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    sleep_ms(1000);
}

// RC5 CONTSTANTS
#define HALF_BIT_US 889

static void send_manchester_bit(bool bit_val) {
    if (bit_val) {
        // bit=1 => HIGH → LOW
        pwm_set_gpio_level(IR_PIN, DUTY);
        sleep_us(HALF_BIT_US);
        pwm_set_gpio_level(IR_PIN, 0);
        sleep_us(HALF_BIT_US);
    } else {
        // bit=0 => LOW → HIGH
        pwm_set_gpio_level(IR_PIN, 0);
        sleep_us(HALF_BIT_US);
        pwm_set_gpio_level(IR_PIN, DUTY);
        sleep_us(HALF_BIT_US);
    }
}

void transmit_RC5(unsigned int toggle, unsigned int addr, unsigned int command){
    /*
    Accepts values:
        toggle: 0 or 1 (1 bit)
        addr: from 0 to 31 (5 bits)
        command: from 0 to 63 (6 bits)

    RC5 frame consists of 14 bits in total:
    START(2 bits) TOGGLE(1 bit) ADDRESS (5 bits) COMMAND (6 bits).

    start is constant 0b11.

    bits are encoded with the Manchester encoding.
    This encoding divides bits to half bits with HALF_BIT_PERIOD = 889us.
    Depending on the implementation:

    Manchester (as per G. E. Thomas):
    bit 1 -> halfbits: 10 (HIGH to LOW)
    bit 0 -> halfbits: 01 (LOW to HIGH)

    Manchester (as per IEEE 802.3):
    bit 1 -> halfbits: 01 (LOW to HIGH)
    bit 0 -> halfbits: 10 (HIGH to LOW)

    total message durration 14(bits) x 2(halfbits) x 889us = 24892us ~ 24.9ms
    */

    unsigned int data  = (3 << 12);     // Start bits = 11
    data |= ((toggle & 0x1) << 11);     // Toggle (1 bit)
    data |= ((addr   & 0x1F) << 6);     // Address (5 bits)
    data |=  (command & 0x3F);          // Command (6 bits)

    for(int bit_index = 13; bit_index >= 0; bit_index--) {
        bool bit_val = (data & (1 << bit_index)) != 0;
        send_manchester_bit(bit_val);
    }
    pwm_set_gpio_level(IR_PIN, 0);
}


int main() {

    setup();
    bool toggle = true;
    while (true) {
        while (gpio_get(BUTTON_PIN)) {tight_loop_contents();}

        // Change duty of PWM
        // printf("WHAT\n");
        // pwm_set_gpio_level(IR_PIN, DUTY);
        // sleep_ms(1000);
        // pwm_set_gpio_level(IR_PIN, 0);
    
        // Transmit RC5
        
        // uint64_t start=time_us_64();
        transmit_RC5(0,0,1);
        // uint64_t time_taken = time_us_64() - start;

        // printf("time taken: %llu \n", time_taken);
        // sleep_ms(400);
    }

    return 0;
}
