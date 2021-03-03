
#include "tensorflow/lite/micro/examples/hello_world/sine_model_data.h"
#include "tensorflow/lite/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/testing/micro_test.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

TF_LITE_MICRO_TESTS_BEGIN

TF_LITE_MICRO_TEST(LoadModelAndPerformanceInference) {

  // set up logging
  tflite::MicroErrorReporter micro_error_reporter;
  tflite::ErrorReporter* error_reporter = &micro_error_reporter;

  const tflite::Model* model = ::tflite::GetModel(g_sine_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter, 
      "Model provided is schema version %d not equal to supported version %d.\n",
      model->version(), TFLITE_SCHEMA_VERSION);
  }

  // resolver
  tflite::ops::micro::AllOpsResolver resolver;

  // create arena area
  const int tensor_arena_size = 2 * 1024;
  uint8_t tensor_arena[tensor_arena_size];

  // build interpreter
  tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, 
    tensor_arena_size, error_reporter);

  TF_LITE_MICRO_EXPECT_EQ(interpreter.AllocateTensors(), kTfLiteOk);

  // check input
  TfLiteTensor* input = interpreter.input(0);
  TF_LITE_MICRO_EXPECT_NE(nullptr, input);
  TF_LITE_MICRO_EXPECT_EQ(2, input->dims->size);
  TF_LITE_MICRO_EXPECT_EQ(1, input->dims->data[0]);
  TF_LITE_MICRO_EXPECT_EQ(1, input->dims->data[1]);
  TF_LITE_MICRO_EXPECT_EQ(kTfLiteFloat32, input->type);

  // provide input value 
  input->data.f[0] = 0.;

  // Run model and check output
  TfLiteStatus invoke_status = interpreter.Invoke();
  TF_LITE_MICRO_EXPECT_EQ(kTfLiteOk, invoke_status);
  TfLiteTensor* output = interpreter.output(0);
  TF_LITE_MICRO_EXPECT_EQ(2, output->dims->size);
  TF_LITE_MICRO_EXPECT_EQ(1, input->dims->data[0]);
  TF_LITE_MICRO_EXPECT_EQ(1, input->dims->data[1]);
  TF_LITE_MICRO_EXPECT_EQ(kTfLiteFloat32, output->type);

  // Run inference on several more values
  input->data.f[0] = 5.;
  invoke_status = interpreter.Invoke();
  TF_LITE_MICRO_EXPECT_EQ(kTfLiteOk, invoke_status);
  float value = output->data.f[0];
  TF_LITE_MICRO_EXPECT_NEAR(-0.959, value, 0.05);

}


TF_LITE_MICRO_TESTS_END
