#include <gtest/gtest.h>

#include "fake-inputs.h"
#include "fake-outputs.h"
#include "runner.h"

bool warning_ = false;

void Warning() { warning_ = true; }

class HvacTest : public ::testing::Test {
 protected:
  RunnerParams runner_params;
  FakeInputs inputs;
  FakeOutputs outputs = FakeOutputs(FanType::PWM);
  Runner runner = Runner(runner_params, inputs, outputs);

  void SetUp() override {
    ASSERT_TRUE(inputs.Init());
    ASSERT_TRUE(outputs.Init());
  }
};

TEST_F(HvacTest, Initializes) { runner.Tick(); }

TEST_F(HvacTest, ManualMode) {
  runner_params.mode_auto = false;
  runner_params.fan_speed = 200;
  runner_params.pump_speed = 150;
  runner_params.use_water_switch = false;
  inputs.water_switch = true;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(150, outputs.GetPump());

  inputs.water_switch = true;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(150, outputs.GetPump());

  runner_params.use_water_switch = true;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(150, outputs.GetPump());

  inputs.water_switch = false;
  runner.Tick();
  EXPECT_EQ(0, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());

  runner_params.pump_speed = 0;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());
}

TEST_F(HvacTest, AutoModeOutsideAboveSwampThreshold) {
  runner_params.mode_auto = true;
  runner_params.fan_speed = 200;
  runner_params.pump_speed = 150;
  runner_params.set_temp = 60;
  runner_params.swamp_threshold = 80;
  runner_params.use_water_switch = false;
  inputs.water_switch = true;
  inputs.outside = 100;
  inputs.inside = 70;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(150, outputs.GetPump());

  inputs.inside = 110;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(150, outputs.GetPump());
  
  inputs.inside = 61;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(150, outputs.GetPump());

  inputs.inside = 59;
  runner.Tick();
  EXPECT_EQ(0, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());

  inputs.inside = 70;
  inputs.water_switch = false;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(150, outputs.GetPump());

  runner_params.use_water_switch = true;
  runner.Tick();
  EXPECT_EQ(0, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());
  EXPECT_TRUE(outputs.GetLed());

  inputs.water_switch = true;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(150, outputs.GetPump());
  EXPECT_FALSE(outputs.GetLed());

  inputs.inside = 100;
  inputs.outside = 90;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(150, outputs.GetPump());

  inputs.water_switch = false;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());
  EXPECT_TRUE(outputs.GetLed());
}

TEST_F(HvacTest, AutoModeOutsideBelowSwampThreshold) {
  runner_params.mode_auto = true;
  runner_params.fan_speed = 200;
  runner_params.pump_speed = 150;
  runner_params.set_temp = 60;
  runner_params.swamp_threshold = 100;
  runner_params.use_water_switch = false;
  inputs.water_switch = true;
  inputs.outside = 80;
  inputs.inside = 70;
  runner.Tick();
  EXPECT_EQ(0, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());

  inputs.inside = 90;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());
  
  runner_params.use_water_switch = true;
  inputs.water_switch = true;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());

  inputs.water_switch = false;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());

  inputs.inside = 59;
  runner.Tick();
  EXPECT_EQ(0, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());

  inputs.outside = 50;
  runner.Tick();
  EXPECT_EQ(0, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());

  inputs.inside = 61;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());
}

TEST_F(HvacTest, AutoModeInvalidTemps) {
  runner_params.mode_auto = true;
  runner_params.fan_speed = 200;
  runner_params.pump_speed = 150;
  runner_params.set_temp = 60;
  runner_params.swamp_threshold = 80;
  runner_params.use_water_switch = false;
  inputs.water_switch = true;
  inputs.outside = 100;
  inputs.inside = Inputs::kNoTemp - 1;
  runner.Tick();
  EXPECT_EQ(0, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());

  inputs.outside = Inputs::kNoTemp - 1;
  runner.Tick();
  EXPECT_EQ(0, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());

  inputs.inside = 100;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());

  inputs.outside = 80;
  runner.Tick();
  inputs.outside = Inputs::kNoTemp - 1;
  runner.Tick();
  EXPECT_EQ(200, outputs.GetFan());
  EXPECT_EQ(0, outputs.GetPump());
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