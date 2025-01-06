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
    const tflite::Model* model = ::tflite::GetModel(my_model);
    TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);
    printf("checkpoint 1\n");
    // Register operations
    ModelOpResolver op_resolver;
    TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));
    printf("checkpoint 2\n");
    // Define tensor arena size and allocate memory
    constexpr int kTensorArenaSize = 64 * 1024; // Adjust size based on model requirements
    uint8_t tensor_arena[kTensorArenaSize];

    printf("checkpoint 3\n");
    tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena, kTensorArenaSize);
    TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());
    printf("ceckpoint 4\n");
    // Input and output tensors
    TfLiteTensor* input_tensor = interpreter.input(0);
    TfLiteTensor* output_tensor = interpreter.output(0);
    printf("checkpoint 5\n");
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
        // TODO

        //CALCULATE MFCCS
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

        // Retrieve and print the output values
        printf("Model output:\n");
        for (int i = 0; i < 11; ++i) {
            printf("Output[%d]: %f\n", i, output_tensor->data.f[i]);
        }

        // TRANSMIT RESULTS
        // TODO

    }

    return 0;
}
