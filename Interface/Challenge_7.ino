#include <Wire.h>
#include <Zumo32U4.h>

double ForwardVal[] = { 200, 200 };
double MotorChange = 0.1;

void CH7_ForwardToLine() {
  for (int i = 0; i < 500; i++) {
    Encoders();
    DriveStraight();
    delay(5);
  }
  motors.setSpeeds(0, 0);
  ResetEncoders();
  ResetDrive(200);
}

void DriveStraight() {
  Encoders();
  if (EncoderL() > EncoderR()) {
    ForwardVal[0] = ForwardVal[0] - MotorChange;
    ForwardVal[1] = ForwardVal[1] + MotorChange;
  } else if (EncoderL() < EncoderR()) {
    ForwardVal[0] = ForwardVal[0] + MotorChange;
    ForwardVal[1] = ForwardVal[1] - MotorChange;
  }
  motors.setSpeeds(ForwardVal[0], ForwardVal[1]);
}

void ResetDrive(int speed) {
  ForwardVal[0] = speed;
  ForwardVal[1] = speed;
}
