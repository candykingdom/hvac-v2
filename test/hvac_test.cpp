#include <gtest/gtest.h>

#include "fake-inputs.h"
#include "fake-outputs.h"
#include "runner.h"

bool warning_ = false;

void Warning() {
  warning_ = true;
}

class HvacTest : public ::testing::Test {
  protected:

  RunnerParams runner_params;
  FakeInputs inputs;
  FakeOutputs outputs;
  Runner runner = Runner(runner_params, inputs, outputs);

  void SetUp() override {
    ASSERT_TRUE(inputs.Init());
    ASSERT_TRUE(outputs.Init());
  }
};

TEST_F(HvacTest, Initializes) { 
  runner.Tick();
 }

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  // if you plan to use GMock, replace the line above with
  // ::testing::InitGoogleMock(&argc, argv);

  if (RUN_ALL_TESTS())
    ;

  // Always return zero-code and allow PlatformIO to parse results
  return 0;
}