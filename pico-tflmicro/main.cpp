#include <stdio.h>
#include <cmath>
#include <vector>
#include <cstdint>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

#include "model.h"
#include "tensorflow/lite/core/c/common.h"
#include "hello_world_float_model_data.h"
#include "hello_world_int8_model_data.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/recording_micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "arm_math.h"
#include "arm_const_structs.h"
#include "dsp/transform_functions.h"
#include "pre_def_mfcc.cpp"

// GPIO PINS
#define G_LED_PIN 0
#define Y_LED_PIN 1
#define R_LED_PIN 2
#define BUTTON_PIN 14
// ADC SETUP
#define ADC_PIN 26
#define ADC_NUM 0

// SETUP STDIO, GPIO, ADC
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

    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_NUM);

    sleep_ms(1000);
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

// MFCC buffers
static float32_t pSrc[fftLen];
static float32_t pTmp [fftLen + 2];
static float32_t pDst[nbDctOutputs];
static float32_t mfccOutput[window_num*nbDctOutputs];

// pico-tflmicro variables
namespace {
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
int inference_count = 0;

constexpr int kTensorArenaSize = 2000;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace
const float kXrange = 2.f * 3.14159265359f;
const int kInferencesPerCycle = 20;

int main(){

    setup();
    sleep_ms(10000);
    // SETUP CMSIS_DSP ---------------------------------------------------
    arm_mfcc_instance_f32 mfccInstance;
    mfccInstance.dctCoefs = dctCoefs;
    mfccInstance.filterCoefs = filterCoefs;
    mfccInstance.windowCoefs = windowCoefs;
    mfccInstance.filterPos = filterPos;
    mfccInstance.filterLengths = filterLengths;
    mfccInstance.fftLen = fftLen;
    mfccInstance.nbMelFilters = nbMelFilters;
    mfccInstance.nbDctOutputs = nbDctOutputs;
    
    // Initialize the RFFT
    arm_rfft_fast_instance_f32 rfft;
    arm_rfft_fast_init_f32(&rfft, fftLen);
    mfccInstance.rfft = rfft;

    // Check if initialization was successful
    arm_status status = arm_mfcc_init_f32(&mfccInstance, fftLen, nbMelFilters, nbDctOutputs, dctCoefs, filterPos, filterLengths, filterCoefs, windowCoefs);

    if (status != ARM_MATH_SUCCESS){
        printf("Failed to initialize MFCC instance. \n");
        return 0;
    }

    printf("mfcc setup complete. \n");

    // SETUP TENSORFLOW LITE MODEL ------------------------------------------------------------------------------------------------------------------
    tflite::InitializeTarget();

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(g_hello_world_float_model_data);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        printf(
            "Model provided is schema version %d not equal "
            "to supported version %d.",
            model->version(), TFLITE_SCHEMA_VERSION);
        return 0;
    }

    // This pulls in all the operation implementations we need.
    // NOLINTNEXTLINE(runtime-global-variables)
    static tflite::MicroMutableOpResolver<1> resolver;
    TfLiteStatus resolve_status = resolver.AddFullyConnected();
    if (resolve_status != kTfLiteOk) {
        printf("Op resolution failed");
        return 0;
    }

    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize);
    interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        printf("AllocateTensors() failed");
        return 0;
    }

    // Obtain pointers to the model's input and output tensors.
    input = interpreter->input(0);
    output = interpreter->output(0);

    // Keep track of how many inferences we have performed.
    inference_count = 0;

    printf("Tensorflow setup complete \n");

    while(true){
    
        // WAIT FOR THE BUTTON TO BE PRESSED ------------------------------------------------------------------------------------------------------
        gpio_put(R_LED_PIN, 1);
        while (gpio_get(BUTTON_PIN)) {tight_loop_contents();}
        sleep_ms(150);
        gpio_put(R_LED_PIN, 0);
        printf("Starting workflow. \n");

        // RECORD SAMPLES
        // TODO

        //CALCULATE MFCCS
        // for (int i=0; i<window_num; i++){

        //     // Extract signal frame
        //     for (int j=0; j<fftLen; j++) {
        //         pSrc[j] = audio[i*fftStep + j];
        //     }

        //     // Calculate mfcc coefitients of the signal frame
        //     arm_mfcc_f32(&mfccInstance, pSrc, pDst, pTmp);

        //     // Move frame MFCCs to mfccOutput;
        //     for (int j=0; j<nbDctOutputs; j++){
        //         mfccOutput[i*nbDctOutputs + j] = pDst[j];
        //     }
        // }
        
        // RUN INFERENCE ON THE MODEL -------------------------------------------------
        float position = static_cast<float>(inference_count) /
                        static_cast<float>(kInferencesPerCycle);
        float x = position * kXrange;

        // Quantize the input from floating-point to integer
        float scale = 0.02464;
        int zero_point = -128;

        int8_t x_quantized = static_cast<int8_t>(round(x / scale) + zero_point);
        // Place the quantized input in the model's input tensor
        input->data.int8[0] = x_quantized;

        // Run inference, and report any error
        TfLiteStatus invoke_status = interpreter->Invoke();
        if (invoke_status != kTfLiteOk) {
            printf("Invoke failed on x: %f\n", x);
            return 0;
        }

        // Obtain the quantized output from model's output tensor
        int8_t y_quantized = output->data.int8[0];
        // Dequantize the output from integer to floating-point
        const float output_scale = 2.0f / 255.0f;
        float y = y_quantized * output_scale;

        // Output the results. A custom HandleOutput function can be implemented
        // for each supported hardware target.
        printf("(%.4f, %d, %.4f)\n", x, x_quantized, y);

        int g_led_brightness = (int)(127.5f * (y + 1));

        // Log the current brightness value for display in the console.
        printf("%d\n", g_led_brightness);

        // Increment the inference_counter, and reset it if we have reached
        // the total number per cycle
        inference_count += 1;
        if (inference_count >= kInferencesPerCycle) inference_count = 0;

        // TRANSMIT RESULTS
        // TODO

    }

    return 0;
}
