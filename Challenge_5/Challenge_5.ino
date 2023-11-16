#include <Zumo32U4.h>

Zumo32U4LCD display;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;

//uint16_t Levels[] = { 20 };

uint16_t lineSensorValues[3] = { 0, 0, 0,};

int ProximityValues[4];

bool state = 0;

bool line = 0;

const uint16_t maxSpeed = 300;

void calibrateLineSensors() {
  // To indicate we are in calibration mode, turn on the yellow LED
  // and print "Line cal" on the display.
  ledYellow(1);
  display.clear();
  display.print(F("Line cal"));

  for (uint16_t i = 0; i < 200; i++) {
    display.gotoXY(0, 1);
    display.print(i);
    lineSensors.calibrate();
  }

  ledYellow(0);
  display.clear();
}

// --- Setup function
void setup() {
  Serial.begin(9600);
  lineSensors.initThreeSensors();
  proxSensors.initThreeSensors();
  uint16_t Levels[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120 };
  double time = 0.5;
  proxSensors.setPulseOnTimeUs(time);
  proxSensors.setPulseOffTimeUs(time);
  proxSensors.setBrightnessLevels(Levels, sizeof(Levels) / 2);

  calibrateLineSensors();
}

void poleDanceRight() {
  if (ProximityValues[3] >= 65) {
    motors.setSpeeds(maxSpeed * 1.3, maxSpeed * 0.5);
  } else {
    motors.setSpeeds(maxSpeed, maxSpeed);
  }
}

void poleDanceLeft() {
  if (ProximityValues[0] >= 65) {
    motors.setSpeeds(maxSpeed * 0.5, maxSpeed * 1.3);
  } else {
    motors.setSpeeds(maxSpeed, maxSpeed);
  }
}

void printReadingsToSerial() {
  static char buffer[80];
  sprintf(buffer, "%4d %4d %4d\n",
          lineSensorValues[0],
          lineSensorValues[1],
          lineSensorValues[2]);
  Serial.print(buffer);
}

void GetProximityValues() {
  proxSensors.read();
  ProximityValues[0] = proxSensors.countsLeftWithLeftLeds();
  ProximityValues[1] = proxSensors.countsFrontWithLeftLeds();
  ProximityValues[2] = proxSensors.countsFrontWithRightLeds();
  ProximityValues[3] = proxSensors.countsRightWithRightLeds();
}


// --- Main program loop
void loop() {
  switch (state) {
    case 0:
      poleDanceRight();
      lineSensors.readCalibrated(lineSensorValues);
      display.clear();
      display.gotoXY(0, 1);
      display.print(lineSensorValues[1]);
      if (lineSensorValues[1] > 800 && line == 0) {
        line = !line;
      }
      switch (line) {
        case 0:
          break;
        case 1:
          nextPole();
          break;
      }
      Serial.println(line);
      break;
    case 1:
      poleDanceLeft();
      lineSensors.readCalibrated(lineSensorValues);
      Serial.println(" Left");
      display.clear();
      display.gotoXY(0, 1);
      display.print(lineSensorValues[1]);
      break;
    default:
      break;
  }
  GetProximityValues();
  Serial.print(ProximityValues[0]);
  Serial.print(" : ");
  Serial.print(ProximityValues[3]);
}

void nextPole() {

  GetProximityValues();
  if (ProximityValues[0] > 80 && ProximityValues[3] > 80) {
    state = !state;
    line = !line;
  }
}
