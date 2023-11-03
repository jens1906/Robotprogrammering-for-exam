#include <Wire.h>
#include <Zumo32U4.h>

double value = 0;

double EncoderArray[2];
double ForwardVal[] = { 200, 200 };
double MotorChange = 0.1;
float EncoderMultipliers[] = { 1, 0.996 };

void Encoders() {
  double countsLeft = abs(encoders.getCountsLeft());
  double countsRight = abs(encoders.getCountsRight());

  value = value + (countsLeft / countsRight);

  display.clear();
  display.print(countsLeft);
  display.gotoXY(0, 1);
  display.print(countsRight);

  EncoderArray[0] = (countsLeft * EncoderMultipliers[0]);
  EncoderArray[1] = (countsRight * EncoderMultipliers[1]);
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