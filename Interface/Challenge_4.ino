#include <Wire.h>
#include <Zumo32U4.h>

double value = 0;
float wheelCirc = 12.6;

int Choose_Distance() {
  int FormerDisplayed = -1;
  int Value = 0;
  do {
    Value = EncoderR() / 50;
    if (FormerDisplayed != Value) {
      display.clear();
      display.print("Distance: ");
      display.gotoXY(0, 1);
      display.print(Value);
      FormerDisplayed = Value;
    }
  } while (buttonA.isPressed() == false);
  delay(1500);
  return Value;
}

int CalcDistance(double Left, double Right) {
  int DrivenDistance = (int)(Left / (12 * 75) * wheelCirc + Right / (12 * 75) * wheelCirc) / 2;
  return DrivenDistance;
}

void CH4_Forward_Distance(int Distance) {
  ResetEncoders();
  int DrivenDistance = CalcDistance(EncoderL(), EncoderR());
  while (Distance >= DrivenDistance) {
    DrivenDistance = CalcDistance(EncoderL(), EncoderR());
    DriveStraight();
    DrivenDistance = CalcDistance(EncoderL(), EncoderR());
    delay(5);
  }
  motors.setSpeeds(0, 0);
  ResetDrive();
}