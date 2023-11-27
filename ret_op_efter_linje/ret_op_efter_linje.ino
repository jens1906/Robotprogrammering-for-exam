#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
Zumo32U4LineSensors lineSensors;
Zumo32U4OLED display;

#define NUM_SENSORS 5
uint16_t lineSensorValues[NUM_SENSORS];

int stage = 0;
int speed = 100;
int threshold = 1000;
int countsLeft;
int countsRight;
int encoderAverage;
int counter;

void setup() {
  lineSensors.initFiveSensors();
  Serial.begin(9600);
}

void loop() {
  lineUp();
}


void readLineSensors() {
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  //  printReadingsToSerial();
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
    delay(1000);

  }
}

void stop() {
  motors.setSpeeds(0, 0);
}

/*
void printReadingsToSerial() {
  char buffer[80];
  sprintf(buffer, "%4d %4d %4d %4d %4d\n",
          lineSensorValues[0],
          lineSensorValues[1],
          lineSensorValues[2],
          lineSensorValues[3],
          lineSensorValues[4]);
  Serial.print(buffer);
}
*/
