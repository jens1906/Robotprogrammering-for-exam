#include <Wire.h>
#include <Zumo32U4.h>

int HighValue = 0;
bool Done = 0;

uint16_t Levels[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120 };
int ProximityValues[4];

void InitiateStuff() {
  proxSensors.initThreeSensors();
  double time = 375;
  proxSensors.setPulseOnTimeUs(time);
  proxSensors.setPulseOffTimeUs(time);
  proxSensors.setBrightnessLevels(Levels, sizeof(Levels));
}

int Choose_Distance_MAXED(int MAX) {
  int FormerDisplayed = -1;
  int Value = 0;
  do {
    (EncoderR() / 50) < MAX ? Value = EncoderR() / 50 : Value = Value;
    if (FormerDisplayed != Value) {
      display.clear();
      display.print("Distance: ");
      display.gotoXY(0, 1);
      display.print(Value);
      FormerDisplayed = Value;
    }
  } while (buttonA.isPressed() == false);
  ResetDrive(100);
  delay(1500);
  return Value;
}

void CH1_DriveProximity(int Distance) {
  InitiateStuff();
  GetProximityValues();
  while (Done == 0) {
    GetProximityValues();
    PrintProximityValues();
    DriveStraight();
    Serial.println(HighValue);
    if (ProximityValues[1] == 15) {
      CH4_Forward_Distance((abs(17.5-Distance)));
      Done = 1;
    }
  }
  ResetDrive(200);
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