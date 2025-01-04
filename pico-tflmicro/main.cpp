#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "model.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "librosa.h"


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

void record(uint16_t* ptr, int buffer_size){
    
    for (int i=0; i<buffer_size; i++){
        *(ptr+i) = adc_read();
        sleep_us(SAMPLE_PERIOD_US);
    }
}

int main(){

    setup();
    setup_adc();

    const int numSamples = 256;
    const int sr = 16000;
    const float frequency = 10.0f;

    std::vector<float> x(numSamples);

    // Fill the vector with sine wave samples
    for (int i = 0; i < numSamples; ++i) {
        float t = static_cast<float>(i) / sr;
        x[i] = std::sin(2.0f * M_PI * frequency * t);
    }

    int n_fft = 256;
    int n_hop = 256;
    std::string window = "hann";
    bool center = false;
    std::string pad_mode = "symmetric";
    float power = 1.0;
    int n_mel = 40;
    int fmin = 80;
    int fmax = 7600;
    int n_mfcc = 13;
    bool norm = true;
    int type = 2;

    sleep_ms(10000);
    printf("LETSGOO\n");

    //compute mfcc
    std::vector<std::vector<float>> mfcc = librosa::Feature::mfcc(x, sr, n_fft, n_hop, window, center, pad_mode, power, n_mel, fmin, fmax, n_mfcc, norm, type);

    printf("MFCC Values:\n");
    for (size_t i = 0; i < mfcc.size(); ++i) {
        printf("Frame %d: ", i+1);
        for (size_t j = 0; j < mfcc[i].size(); ++j) {
            printf("%.4f, ", mfcc[i][j]);
        }
        std::cout << std::endl;
        printf("\n");
    }

    printf("DONE\n");

    while(true){
    
        // WAIT FOR THE BUTTON TO BE PRESSED
        gpio_put(R_LED_PIN, 1);
        while (gpio_get(BUTTON_PIN)) {tight_loop_contents();}
        sleep_ms(100);
        gpio_put(R_LED_PIN, 0);
        
        // RECORD SAMPLES
        gpio_put(Y_LED_PIN, 1);
        sleep_ms(1000);
        gpio_put(Y_LED_PIN, 0);
  
    }

    return 0;
}
