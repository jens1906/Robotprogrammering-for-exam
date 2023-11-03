#include <List.hpp>
#include <Wire.h>
#include <Zumo32U4.h>

List<double> list;
double value = 0;

Zumo32U4Encoders encoders;
Zumo32U4IMU imu;
Zumo32U4Motors motors;
Zumo32U4OLED display;

uint32_t turnAngle = 0;
int16_t turnRate;
int16_t gyroOffset;
uint16_t gyroLastUpdate = 0;
uint32_t CurrentAngle;
int way;


double EncoderArray[2];
double ForwardVal[] = { 200, 200 };
double MotorChange = 0.1;
float EncoderMultipliers[] = { 1, 1.2 };

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  turnSensorSetup();
  delay(500);
  turnSensorReset();
  delay(2000);
}

void loop() {
  CH7_ForwardToLine();
  delay(2000);
}

void Encoders() {
  double countsLeft = abs(encoders.getCountsLeft());
  double countsRight = abs(encoders.getCountsRight());

  value = value + (countsLeft / countsRight);

  display.clear();
  display.print((countsLeft + countsRight) / 2);
  display.gotoXY(0, 1);
  way = turnSensorUpdate();
  display.print(way);

  EncoderArray[0] = countsLeft;
  EncoderArray[1] = (countsRight * 0.996);
}

void CH7_ForwardToLine() {
  for (int i = 0; i < 500; i++) {
    motors.setSpeeds(ForwardVal[0], ForwardVal[1]);
    Encoders();
    if (EncoderArray[0] > EncoderArray[1]) {
      ForwardVal[0] = ForwardVal[0] - MotorChange;
      ForwardVal[1] = ForwardVal[1] + MotorChange;
    } else if (EncoderArray[0] < EncoderArray[1]) {
      ForwardVal[0] = ForwardVal[0] + MotorChange;
      ForwardVal[1] = ForwardVal[1] - MotorChange;
    }
    delay(5);
  }
  motors.setSpeeds(0, 0);
  ForwardVal[0], ForwardVal[1] = 200;
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}

void turnSensorReset() {
  gyroLastUpdate = micros();
  turnAngle = 0;
}

//Update gyro sensor kun med værdier fra 0-360
uint32_t turnSensorUpdate() {
  imu.readGyro();
  turnRate = imu.g.z - gyroOffset;
  uint16_t m = micros();
  uint16_t dt = m - gyroLastUpdate;
  gyroLastUpdate = m;
  int32_t d = (int32_t)turnRate * dt;
  turnAngle += (int64_t)d * 14680064 / 17578125;
  return ((((uint32_t)turnAngle >> 16) * 360) >> 16);
}

void turnSensorSetup() {
  Wire.begin();
  imu.init();
  imu.enableDefault();
  imu.configureForTurnSensing();
  delay(500);
  int32_t total = 0;
  for (uint16_t i = 0; i < 1024; i++) {
    while (!imu.gyroDataReady()) {}
    imu.readGyro();
    total += imu.g.z;
  }
  gyroOffset = total / 1024;
  turnSensorReset();
}
