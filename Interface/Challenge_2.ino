#include <Wire.h>
#include <Zumo32U4.h>

const uint16_t maxSpeed = 370;

int16_t lastError = 0;
float Kmax = 0.8;
float kp = 0.8;
float kd = 2;  
float Ki = 0;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

void printBar(uint8_t height) {
  if (height > 8) { height = 8; }
  const char barChars[] = { ' ', 0, 1, 2, 3, 4, 5, 6, (char)255 };
  display.print(barChars[height]);
}

void calibrateSensors() {
  display.clear();
  delay(1000);
  for (uint16_t i = 0; i < 120; i++) {
    if (i > 30 && i <= 90) {
      motors.setSpeeds(-150, 150);
    } else {
      motors.setSpeeds(150, -150);
    }
    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}

void lineFollow() {
  int16_t position = lineSensors.readLine(lineSensorValues);
  int16_t integral = 0;
  int16_t error = position - 2000;
  integral += error;
  int16_t speedDifference = (kp)*error + (Ki)*integral + (kd) * (error - lastError);
  lastError = error;

  int16_t leftSpeed = (int16_t)maxSpeed + speedDifference;
  int16_t rightSpeed = (int16_t)maxSpeed - speedDifference;

  leftSpeed = constrain(leftSpeed, -(int16_t)maxSpeed, (int16_t)maxSpeed);
  rightSpeed = constrain(rightSpeed, -(int16_t)maxSpeed, (int16_t)maxSpeed);

  motors.setSpeeds(leftSpeed, rightSpeed);
}

void Linesetup() {
  lineSensors.initFiveSensors();

  // Wait for button A to be pressed and released.
  display.clear();
  display.print(F("Press A"));
  display.gotoXY(0, 1);
  display.print(F("to calib"));
  buttonA.waitForButton();

  calibrateSensors();
}

void CH2_LineFollower() {
  Linesetup();
  while (buttonA.isPressed() == false) {
    lineFollow();
  }
}