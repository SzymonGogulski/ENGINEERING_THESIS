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

// SIGNAL NORMALIZATION
#define SIGNAL_MEAN 2080
#define SIGNAL_RANGE 500
#define SIGNAL_RANGE_INV 1.0f/SIGNAL_RANGE

float32_t normalize(uint16_t raw){

    float32_t normalized = (raw - SIGNAL_MEAN) * SIGNAL_RANGE_INV;
    normalized = (normalized > 1.0) ? 1.0 : normalized;
    normalized = (normalized < -1.0) ? -1.0 : normalized;
    return normalized;
}

// BUFFER SETUP AND OPERATIONS
#define FREQUENCY 16000
#define SAMPLE_PERIOD_US 62
#define SECONDS 1
#define BUFFER_SIZE (FREQUENCY * SECONDS) 
float32_t audio[BUFFER_SIZE];

void record(float32_t* ptr, int buffer_size){
    
    for (int i=0; i<buffer_size; i++){
        *(ptr+i) = normalize(adc_read());
        sleep_us(SAMPLE_PERIOD_US);
    }
}

// MFCC buffers
static float32_t pSrc[fftLen];
static float32_t pTmp [fftLen + 2];
static float32_t pDst[nbDctOutputs];
static float32_t mfccOutput[window_num*nbDctOutputs];

namespace {
using ModelOpResolver = tflite::MicroMutableOpResolver<10>; // Adjust the number based on the operations your model uses

TfLiteStatus RegisterOps(ModelOpResolver& op_resolver) {
    TF_LITE_ENSURE_STATUS(op_resolver.AddConv2D());
    TF_LITE_ENSURE_STATUS(op_resolver.AddMaxPool2D());
    TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
    TF_LITE_ENSURE_STATUS(op_resolver.AddReshape());
    TF_LITE_ENSURE_STATUS(op_resolver.AddShape());
    TF_LITE_ENSURE_STATUS(op_resolver.AddStridedSlice());
    TF_LITE_ENSURE_STATUS(op_resolver.AddPack());
    TF_LITE_ENSURE_STATUS(op_resolver.AddSoftmax());
    TF_LITE_ENSURE_STATUS(op_resolver.AddQuantize());
    TF_LITE_ENSURE_STATUS(op_resolver.AddDequantize());
    return kTfLiteOk;
}
} // namespace

int main(){

    setup();

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
    const tflite::Model* model = ::tflite::GetModel(my_model);
    TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

    // Register operations
    ModelOpResolver op_resolver;
    TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

    // Define tensor arena size and allocate memory
    constexpr int kTensorArenaSize = 64 * 1024; // Adjust size based on model requirements
    uint8_t tensor_arena[kTensorArenaSize];

    tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena, kTensorArenaSize);
    TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());

    // Input and output tensors
    TfLiteTensor* input_tensor = interpreter.input(0);
    TfLiteTensor* output_tensor = interpreter.output(0);

    // Check input and output tensor sizes
    TFLITE_CHECK_EQ(input_tensor->dims->size, 2); // Ensure 2D input
    TFLITE_CHECK_EQ(input_tensor->dims->data[1], 793); // Ensure input size matches
    TFLITE_CHECK_EQ(output_tensor->dims->size, 2); // Ensure 2D output
    TFLITE_CHECK_EQ(output_tensor->dims->data[1], 11); // Ensure output size matches
    printf("Tensorflow setup complete \n");

    while(true){
    
        // WAIT FOR THE BUTTON TO BE PRESSED ------------------------------------------------------------------------------------------------------
        gpio_put(R_LED_PIN, 1);
        while (gpio_get(BUTTON_PIN)) {tight_loop_contents();}
        sleep_ms(150);
        gpio_put(R_LED_PIN, 0);
        printf("Starting workflow. \n");

        // RECORD SAMPLES
        gpio_put(Y_LED_PIN, 1);
        record(audio, BUFFER_SIZE);
        gpio_put(Y_LED_PIN, 0);

        //CALCULATE MFCCS
        gpio_put(G_LED_PIN, 1);
        for (int i=0; i<window_num; i++){

            // Extract signal frame
            for (int j=0; j<fftLen; j++) {
                pSrc[j] = audio[i*fftStep + j];
            }

            // Calculate mfcc coefitients of the signal frame
            arm_mfcc_f32(&mfccInstance, pSrc, pDst, pTmp);

            // Move frame MFCCs to mfccOutput;
            for (int j=0; j<nbDctOutputs; j++){
                mfccOutput[i*nbDctOutputs + j] = pDst[j];
            }
        }
        
        // RUN INFERENCE ON THE MODEL -------------------------------------------------
        // Copy input values into the model's input tensor
        for (int i = 0; i < 793; ++i) {
            input_tensor->data.f[i] = mfccOutput[i];
        }

            // Run inference
        TF_LITE_ENSURE_STATUS(interpreter.Invoke());

        // Print out the output with the highest amount.

        int maxIndex = 0;
        float maxValue = output_tensor->data.f[0];

        for (int i = 1; i < size; i++) {
            if (output_tensor->data.f[i] > maxValue) {
                maxValue = output_tensor->data.f[i];
                maxIndex = i;
            }
        }

        // // Retrieve and print the output values
        // printf("[Zero]:     %f\n", output_tensor->data.f[0]);
        // printf("[One]:      %f\n", output_tensor->data.f[1]);
        // printf("[Two]:      %f\n", output_tensor->data.f[3]);
        // printf("[Three]:    %f\n", output_tensor->data.f[4]);
        // printf("[Four]:     %f\n", output_tensor->data.f[5]);
        // printf("[Five]:     %f\n", output_tensor->data.f[6]);
        // printf("[Six]:      %f\n", output_tensor->data.f[7]);
        // printf("[Seven]:    %f\n", output_tensor->data.f[8]);
        // printf("[Eight]:    %f\n", output_tensor->data.f[9]);
        // printf("[Nine]:     %f\n", output_tensor->data.f[10]);
        // printf("[noise]:    %f\n", output_tensor->data.f[2]);

        gpio_put(G_LED_PIN, 0);
        // TRANSMIT RESULTS
        // TODO

    }

    return 0;
}
