#include <Wire.h>
#include <Zumo32U4.h>

int stage = 0;
int speed = 100;
int threshold = 1000;
int countsLeft;
int countsRight;
int encoderAverage;
int counter;
int LineUp = 0;

void LineUp_Reset_Variabels() {
  stage = 0;
  speed = 100;
  threshold = 1000;
  countsLeft;
  countsRight;
  encoderAverage;
  counter;
  LineUp = 0;
}

void StartLineUp() {
  LineUp_Reset_Variabels();
  LineUp = 0;
  lineSensors.initFiveSensors();
  while (LineUp == 0) {
    lineUp();
  }
}



void lineUp() {
  readLineSensors();
  if (lineSensorValues[0] < threshold && lineSensorValues[4] < threshold) {
    motors.setSpeeds(speed, speed);
  }
  //Hvis den går på skævt fra venstre
  if (lineSensorValues[0] > threshold) {
    stop();
    delay(1000);
    countsLeft = encoders.getCountsAndResetLeft();
    countsRight = encoders.getCountsAndResetRight();
    while (lineSensorValues[4] < threshold) {
      readLineSensors();
      motors.setSpeeds(speed, speed);
      countsLeft = encoders.getCountsLeft();
      countsRight = encoders.getCountsRight();
      encoderAverage = (countsLeft + countsRight) / 2;
    }
    encoders.getCountsAndResetLeft();
    while ((countsLeft + encoderAverage) != 0) {
      motors.setSpeeds(-speed, 0);
      countsLeft = encoders.getCountsLeft();
    }
    stop();
    delay(5000);
  }
  //Hvis den går på skævt fra højre
  if (lineSensorValues[4] > threshold) {
    stop();
    delay(1000);
    countsLeft = encoders.getCountsAndResetLeft();
    countsRight = encoders.getCountsAndResetRight();
    while (lineSensorValues[0] < threshold) {
      readLineSensors();
      motors.setSpeeds(speed, speed);
      countsLeft = encoders.getCountsLeft();
      countsRight = encoders.getCountsRight();
      encoderAverage = (countsLeft + countsRight) / 2;
    }
    encoders.getCountsAndResetRight();
    while ((countsRight + encoderAverage) != 0) {
      motors.setSpeeds(0, -speed);
      countsRight = encoders.getCountsRight();
    }
    stop();
    LineUp = 1;
    delay(1000);
  }
}

void stop() {
  motors.setSpeeds(0, 0);
}
