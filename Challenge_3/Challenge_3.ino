#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4OLED display;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4Motors motors;

int line_value;

uint16_t L_proxSensors;
uint16_t LC_proxSensors;
uint16_t R_proxSensors;
uint16_t RC_proxSensors;
int error;
int last_error = 0;

float kp = 100;
float kd = 0.1;

int regulation_speed;

uint16_t base_speed = 400;

uint16_t Levels[] = { 2, 12, 22, 32, 42, 52, 62, 72, 82, 92, 136 };

#define NUM_SENSORS 3
unsigned int lineSensorValues[NUM_SENSORS];

unsigned int lineSensorValues1;


int ProximityValues[4];

void setup() {
  Serial.begin(9600);
  proxSensors.initThreeSensors();
  lineSensors.initThreeSensors();
  calibrateLineSensors();
  proxSensors.setPeriod(500);
  proxSensors.setBrightnessLevels(Levels, sizeof(Levels));
  
}

void GetProximityValues() {
  proxSensors.read();
  ProximityValues[0] = proxSensors.countsLeftWithLeftLeds();
  ProximityValues[1] = proxSensors.countsFrontWithLeftLeds();
  ProximityValues[2] = proxSensors.countsFrontWithRightLeds();
  ProximityValues[3] = proxSensors.countsRightWithRightLeds();
}

void drive_in_middle() {
  while (lineSensorValues[1] < 150) {
    display.clear();
    //display.print(turnSensorUpdate());
    lineSensors.read(lineSensorValues);
    //lineSensors.readCalibrated(d);

    //PrintProximityValues();
    GetProximityValues();
    error = ProximityValues[0] + ProximityValues[1] * 0.1 - ProximityValues[2] * 0.1 - ProximityValues[3];

    regulation_speed = kp * error + kd * (error - last_error);

    last_error = error;

    Serial.print(regulation_speed);
    Serial.print("\n");

    //no error
    if (error == 0) {
      motors.setSpeeds(base_speed, base_speed);
    }
    //left close
    else if (error > 0) {
      motors.setSpeeds(base_speed, base_speed - regulation_speed);
    }
    //right close
    else if (error < 0) {
      motors.setSpeeds(base_speed + regulation_speed, base_speed);
    }
  }
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

  Serial.println(ProximityValues[0]);
  Serial.println(ProximityValues[1]);
  Serial.println(ProximityValues[2]);
  Serial.println(ProximityValues[3]);
  Serial.println();
}

void calibrateLineSensors() {
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

void printReadingsToSerial() {
  static char buffer[80];
  sprintf(buffer, "%4d\n",
          lineSensorValues[1]);
  Serial.print(buffer);
}

void loop() {

  while (lineSensorValues[1] < 150) {
    drive_in_middle();
    //lineSensors.readCalibrated(lineSensorValues);
  }
  while (lineSensorValues[1] > 150) {
    motors.setSpeeds(0, 0);
    display.print("test");
  }
}
