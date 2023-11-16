#include <Wire.h>
#include <Zumo32U4.h>

int HighValue = 0;
bool Done = 0;

uint16_t CH1_Levels[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120 };
int ProximityValues[4];

void CH1_InitiateStuff() {
  CH1_Reset_Variables();
  proxSensors.initThreeSensors();
  proxSensors.setPulseOnTimeUs(375);
  proxSensors.setPulseOffTimeUs(375);
  proxSensors.setBrightnessLevels(CH1_Levels, sizeof(CH1_Levels));
}

void CH1_Reset_Variables() {
  HighValue = 0;
  Done = 0;
}

void CH1_DriveProximity(int Distance) {
  CH1_InitiateStuff();
  GetProximityValues();
  while (Done == 0) {
    GetProximityValues();
    PrintProximityValues();
    DriveStraight();
    Serial.println(HighValue);
    if (ProximityValues[1] == 15) {
      CH4_Forward_Distance((abs(17.5 - Distance)));
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