#include <Wire.h>
#include <Zumo32U4.h>

//VARIABLER FOR GYRO:
uint32_t turnAngle = 0;
int16_t turnRate;
int16_t gyroOffset;
uint16_t gyroLastUpdate = 0;
uint32_t CurrentAngle;
int WantedAngle = 0;
int turn = 0;

void CH6_Reset_Variabels() {
  turnAngle = 0;
  turnRate = 0;
  gyroOffset = 0;
  gyroLastUpdate = 0;
  CurrentAngle = 0;
  WantedAngle = 0;
  turn = 0;
}

int readLineSensors() {
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  Serial.println(((lineSensorValues[0] + lineSensorValues[1] + lineSensorValues[2]) / 3));
  return (((lineSensorValues[0] + lineSensorValues[1] + lineSensorValues[2]) / 3));
}

void calibrateLineSensors() {
  display.clear();
  display.print(F("Line cal"));

  for (uint16_t i = 0; i < 200; i++) {
    display.gotoXY(0, 1);
    display.print(i);
    lineSensors.calibrate();
  }
  display.clear();
}

void CH6_Initialize() {
  turnSensorSetup();
  delay(10);
  turnSensorReset();
  lineSensors.initThreeSensors();
  calibrateLineSensors();
  delay(500);
}

void CH6_Drive_Angle(int Degrees) {
  CH4_Forward_Distance(5);
  turnByDegree(Degrees);
  while (readLineSensors() < 300) {
    DriveStraight();
  }
  motors.setSpeeds(0, 0);
}

void turnSensorReset() {
  gyroLastUpdate = micros();
  turnAngle = 0;
}

//Update gyro sensor kun med værdier fra 0-360
uint32_t turnSensorUpdate() {
  imu.readGyro();
  turnRate = imu.g.z - gyroOffset;
  uint16_t m = micros();
  uint16_t dt = m - gyroLastUpdate;
  gyroLastUpdate = m;
  int32_t d = (int32_t)turnRate * dt;
  turnAngle += (int64_t)d * 14680064 / 17578125;
  return ((((uint32_t)turnAngle >> 16) * 360) >> 16);
}

//setup gyro sensor
void turnSensorSetup() {
  Wire.begin();
  imu.init();
  imu.enableDefault();
  imu.configureForTurnSensing();
  delay(500);
  int32_t total = 0;
  for (uint16_t i = 0; i < 1024; i++) {
    while (!imu.gyroDataReady()) {}
    imu.readGyro();
    total += imu.g.z;
  }
  gyroOffset = total / 1024;
  turnSensorReset();
}

void turnByDegree(int DegreesWanted) {
  WantedAngle = DegreesWanted;

  //Kør så længe gyro siger man ikke er ved den rette vinkel
  while (turnSensorUpdate() != DegreesWanted) {
    display.clear();
    display.print(turnSensorUpdate());

    //For at finde nuværende position
    int Way = turnSensorUpdate();
    ((DegreesWanted - Way) > 0) ? motors.setSpeeds(-100, 100) : motors.setSpeeds(-100, 100);  //Hvis ? Så gør : Ellers;  For at udregne korteste mulige sving (ternary operator)
  }
  motors.setSpeeds(0, 0);  //gør hold
  delay(10);
  turnSensorReset();
}

int Choose_Mode_MAXED(int MAX, int Mode) {
  ResetEncoders();
  int FormerDisplayed = -1;
  int Value = -1;
  ResetEncoders();
  do {
    (EncoderR() / 50) < MAX ? Value = EncoderR() / 50 : Value = Value;
    if (FormerDisplayed != Value) {
      display.clear();
      Mode == 0 ? display.print("Distance: ") : display.print("Degrees: ");
      display.gotoXY(0, 1);
      display.print(Value);
      FormerDisplayed = Value;
    }
  } while (buttonA.isPressed() == false);
  ResetDrive(100);
  delay(1500);
  return Value;
}