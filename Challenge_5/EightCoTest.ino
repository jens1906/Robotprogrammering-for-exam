#include <Zumo32U4.h>

Zumo32U4OLED display;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;

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
    motors.setSpeeds(maxSpeed, maxSpeed);
}

void poleDanceLeft() {
    motors.setSpeeds(maxSpeed, maxSpeed);
}

void GetProximityValues() {
  proxSensors.read();
  ProximityValues[0] = proxSensors.countsLeftWithLeftLeds();
  ProximityValues[1] = proxSensors.countsFrontWithLeftLeds();
  ProximityValues[2] = proxSensors.countsFrontWithRightLeds();
  ProximityValues[3] = proxSensors.countsRightWithRightLeds();
}

void loop() {
  switch (currentState) {
    case MOVE_STRAIGHT:
      // Code to move robot straight
      motors.setSpeeds(200, 200); // Assuming motors object controls the speed of the robot
      // Check if it's time to turn, if so update currentState
      if (ProximityValues[1] > THRESHOLD || ProximityValues[2] > THRESHOLD) { // Assuming THRESHOLD is the distance to start turning
        currentState = TURN_LEFT;
      }
      break;
    case TURN_LEFT:
      // Code to turn robot left
      motors.setSpeeds(-200, 200); // Negative speed for left motor to turn left
      // Check if it's time to move straight, if so update currentState
      if (ProximityValues[0] < THRESHOLD) {
        currentState = MOVE_STRAIGHT;
      }
      break;
    case TURN_RIGHT:
      // Code to turn robot right
      motors.setSpeeds(200, -200); // Negative speed for right motor to turn right
      // Check if it's time to move straight, if so update currentState
      if (ProximityValues[3] < THRESHOLD) {
        currentState = MOVE_STRAIGHT;
      }
      break;
  }
}