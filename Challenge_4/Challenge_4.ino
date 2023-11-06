#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Encoders encoders;
Zumo32U4IMU imu;
Zumo32U4OLED display;
Zumo32U4Motors motors;

double value = 0;
float wheelCirc = 11.938;

double ForwardVal[] = { 200, 200 };
double MotorChange = 0.1;
float EncoderMultipliers[] = { 1, 0.996 };
double EncoderArray[2];


void setup() {
  Serial.begin(9600);
}
void loop() {
  CH4_Forward_Distance(10);
  delay(1000);
}

double EncoderL() {
  double countsLeft = abs(encoders.getCountsLeft());
  countsLeft,EncoderArray[0] = (countsLeft * EncoderMultipliers[0]);
  return countsLeft;
}
double EncoderR() {
  double countsRight = abs(encoders.getCountsRight());
  countsRight,EncoderArray[1] = (countsRight * EncoderMultipliers[1]);
  return countsRight;
}
void Encoders(){
  EncoderL();
  EncoderR();
}

int CalcDistance(int Left, int Right) {
  int Distance = (int)((((Left + Right) / 2) / (12 * 75)) * wheelCirc);
  Serial.println(Distance);
  return Distance;
}

void CH4_Forward_Distance(int Distance) {
  Encoders();
  while (Distance < CalcDistance(EncoderL, EncoderR)) {
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