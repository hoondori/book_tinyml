

#include "tensorflow/lite/micro/examples/hello_world/main_functions.h"

int main(int argc, char* argv[]) {
  setup();
  while(true) {
    loop();
  }

}
