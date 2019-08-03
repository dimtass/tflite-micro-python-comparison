#include "digit.h"
#include "model_data.h"
#include <stdio.h>
#include <sys/time.h>

#include "all_ops_resolver.h"
#include "micro_error_reporter.h"
#include "micro_interpreter.h"
#include "schema_generated.h"
#include "version.h"

#define CODE_VERSION 100

#define TENSOR_ARENA_SIZE 262144

uint32_t trace_levels;

struct tflite_model {
    const tflite::Model *model;
    tflite::SimpleTensorAllocator *tensor_allocator;
    tflite::ErrorReporter *error_reporter;
    tflite::ops::micro::AllOpsResolver resolver; // This pulls in all the operation implementations we need
    tflite::MicroInterpreter *interpreter;
    uint8_t tensor_arena[TENSOR_ARENA_SIZE];
    int inference_count;
};
struct tflite_model tf;


void CmdSendStats();
void ViewModel(struct tflite_model *tf);
void BenchmarkInference(struct tflite_model *tf, float *data, size_t data_size, uint16_t n_runs);


double get_time()
{
    struct timeval t;
    struct timezone tzp;
    gettimeofday(&t, &tzp);
    return t.tv_sec + t.tv_usec*1e-6;
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    printf("Program started...\n");

    // Set up logging
    tflite::MicroErrorReporter micro_error_reporter;
    tf.error_reporter = &micro_error_reporter;

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    tf.model = ::tflite::GetModel(jupyter_notebook_mnist_tflite);
    if (tf.model->version() != TFLITE_SCHEMA_VERSION) {
        tf.error_reporter->Report("Model provided is schema version %d not equal "
                                  "to supported version %d.\n",
                                  tf.model->version(), TFLITE_SCHEMA_VERSION);
    }

    // Create an area of memory to use for input, output, and intermediate arrays.
    // Finding the minimum value for your model may require some trial and error.
    tf.tensor_allocator = new tflite::SimpleTensorAllocator(tf.tensor_arena, TENSOR_ARENA_SIZE);

    // Build an interpreter to run the model with
    tf.interpreter = new tflite::MicroInterpreter(tf.model, tf.resolver, tf.tensor_allocator, tf.error_reporter);
    tf.inference_count = 0;

    // ViewModel(&tf);
    BenchmarkInference(&tf, (float *)digit, 784, 1000);

}

void ViewModel(struct tflite_model *tf)
{
    TfLiteTensor *input = tf->interpreter->input(0);
    TfLiteTensor *output = tf->interpreter->output(0);

    printf("Model input:\n");
    printf("dims->size: %d\n", input->dims->size);
    printf("dims->data[0]: %d\n", input->dims->data[0]);
    printf("dims->data[1]: %d\n", input->dims->data[1]);
    printf("dims->data[2]: %d\n", input->dims->data[2]);
    printf("dims->data[3]: %d\n", input->dims->data[3]);
    printf("input->type: %d\n\n", input->type);

    printf("Model output:\n");
    printf("dims->size: %d\n", output->dims->size);
    printf("dims->data[0]: %d\n", output->dims->data[0]);
    printf("dims->data[1]: %d\n\n", output->dims->data[1]);
}

void BenchmarkInference(struct tflite_model *tf, float *data, size_t data_size, uint16_t n_runs)
{
    // // Obtain pointers to the model's input and output tensors
    TfLiteTensor *input = tf->interpreter->input(0);
    TfLiteTensor *output = tf->interpreter->output(0);
    TfLiteStatus invoke_status = kTfLiteError;

    /* Copy data to the input buffer. So much wasted RAM! */
    for (size_t i = 0; i < data_size; i++) {
        input->data.f[i] = data[i];
    }

    printf("Running inference...\n");

    double startTime = get_time();
    for (int i=0; i<n_runs; i++) {
        invoke_status= tf->interpreter->Invoke();
    }
    double elapsedTime = get_time() - startTime;

    if (invoke_status != kTfLiteOk) {
        tf->error_reporter->Report("Invoke failed\n");
    }

    for (int i = 0; i < 10; i++) {
        printf("Out[%d]: %f\n", i, output->data.f[i]);
    }

    printf("\n----------------------\n");
    printf("Total elapsed time: %f ms\n", elapsedTime*1000/n_runs);


}