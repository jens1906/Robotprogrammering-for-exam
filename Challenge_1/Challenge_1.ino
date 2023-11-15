#include <Zumo32U4.h>

Zumo32U4OLED display;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;

int HighValue = 0;
bool Done = 0;

uint16_t Levels[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120 };

int ProximityValues[4];
bool Position;

void InitiateStuff() {
  proxSensors.initThreeSensors();
  double time = 375;
  proxSensors.setPulseOnTimeUs(time);
  proxSensors.setPulseOffTimeUs(time);
  proxSensors.setBrightnessLevels(Levels, sizeof(Levels));
}

void CH1_DriveProximity() {
  GetProximityValues();
  while (Done == 0) {
    GetProximityValues();
    PrintProximityValues();
    DriveStraight();
    Serial.println(HighValue);
    if (ProximityValues[1] == 19) {
      HighValue = 1;
      CH4_Forward_Distance((19.5 - 10));
      Done = 1;
    }
  }
}

void setup(){
  
}

void loop() {
  GetProximityValues();
}

void GetProximityValues() {
  proxSensors.read();
  ProximityValues[0] = proxSensors.countsLeftWithLeftLeds();
  ProximityValues[1] = proxSensors.countsFrontWithLeftLeds();
  ProximityValues[2] = proxSensors.countsFrontWithRightLeds();
  ProximityValues[3] = proxSensors.countsRightWithRightLeds();
}

void PrintProximityValues() {
  display.clear();
  display.print(ProximityValues[0]);
  display.print(" ");
  display.print(ProximityValues[1]);
  display.gotoXY(0, 1);
  display.print(ProximityValues[2]);
  display.print(" ");
  display.print(ProximityValues[3]);
}











float EncoderMultipliers[] = { 1, 0.996 };  //The values is found by testing, and the usage can be seen later.

//This function takes and gets the encoder value from the left encoder and processes it so the value works with the right encoder.
double EncoderL() {
  double countsLeft = abs(encoders.getCountsLeft());
  countsLeft = (countsLeft * EncoderMultipliers[0]);  //Multiplies the encoder value with a found value, so the 2 encoders count the 'same' way. This value is found by testing.
  return countsLeft;
}

//This function takes and gets the encoder value from the right encoder and processes is so the value works with the left encoder.
double EncoderR() {
  double countsRight = abs(encoders.getCountsRight());
  countsRight = (countsRight * EncoderMultipliers[1]);  //Multiplies the encoder value with a found value, so the 2 encoders count the 'same' way. This value is found by testing.
  return countsRight;
}

//This function updates both encoder values, if wanted.
void Encoders() {
  EncoderL();
  EncoderR();
}

//This function is for resetting both encoders.
void ResetEncoders() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}

double ForwardVal[] = { 100, 100 };
double MotorChange = 0.1;

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

void ResetDrive() {
  ForwardVal[0] = 100;
  ForwardVal[1] = 100;
}

double value = 0;
float wheelCirc = 12.6;

int CalcDistance(double Left, double Right) {
  GetProximityValues();
  PrintProximityValues();
  int DrivenDistance = (int)(Left / (12 * 75) * wheelCirc + Right / (12 * 75) * wheelCirc) / 2;
  return DrivenDistance;
}

void CH4_Forward_Distance(double Distance) {
  ResetEncoders();

  int DrivenDistance = CalcDistance(EncoderL(), EncoderR());
  while (Distance >= DrivenDistance) {
    DrivenDistance = CalcDistance(EncoderL(), EncoderR());
    DriveStraight();
    DrivenDistance = CalcDistance(EncoderL(), EncoderR());
    Serial.println(DrivenDistance);
    delay(5);
  }
  motors.setSpeeds(0, 0);
  HighValue = 0;
  ResetDrive();
}