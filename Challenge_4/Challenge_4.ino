#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Encoders encoders;
Zumo32U4IMU imu;
Zumo32U4OLED display;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;

float wheelCirc = 12.6;
float ForwardVal[] = { 200, 200 };
float MotorChange = 0.1;
float EncoderMultipliers[] = { 1, 0.996 };
float EncoderArray[2];



void setup() {
  Serial.begin(9600);
  delay(2000);
  CH4_Forward_Distance(CH4_Choose_Distance());
}

void loop() {
}

int CH4_Choose_Distance() {
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

double EncoderL() {
  double countsLeft = abs(encoders.getCountsLeft());
  countsLeft, EncoderArray[0] = (countsLeft * EncoderMultipliers[0]);
  return countsLeft;
}
double EncoderR() {
  double countsRight = abs(encoders.getCountsRight());
  countsRight, EncoderArray[1] = (countsRight * EncoderMultipliers[1]);
  return countsRight;
}
void Encoders() {
  EncoderL();
  EncoderR();
}

void ResetEncoders() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}

int CalcDistance(float Left, float Right) {
  int DrivenDistance = (int)(Left / (12 * 75) * wheelCirc + Right / (12 * 75) * wheelCirc) / 2;
  return DrivenDistance;
}

void CH4_Forward_Distance(int Distance) {
  ResetEncoders();
  int DrivenDistance = CalcDistance(EncoderL(), EncoderR());
  while (Distance >= DrivenDistance) {
    motors.setSpeeds(ForwardVal[0], ForwardVal[1]);
    DrivenDistance = CalcDistance(EncoderL(), EncoderR());
    Encoders();
    if (EncoderArray[0] > EncoderArray[1]) {
      ForwardVal[0] = ForwardVal[0] - MotorChange;
      ForwardVal[1] = ForwardVal[1] + MotorChange;
    } else if (EncoderArray[0] < EncoderArray[1]) {
      ForwardVal[0] = ForwardVal[0] + MotorChange;
      ForwardVal[1] = ForwardVal[1] - MotorChange;
    }
    DrivenDistance = CalcDistance(EncoderL(), EncoderR());
    delay(5);
  }
  motors.setSpeeds(0, 0);
  ForwardVal[0], ForwardVal[1] = 200;
}