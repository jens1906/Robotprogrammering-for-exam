#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Encoders encoders;
Zumo32U4IMU imu;
Zumo32U4OLED display;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;


double MotorChange = 0.1;
double ForwardVal[] = { 200, 200 };
float wheelCirc = 12.6;
int threshold = 1000;
int encoderAverage;


//VARIABLER FOR GYRO:
uint32_t turnAngle = 0;
int16_t turnRate;
int16_t gyroOffset;
uint16_t gyroLastUpdate = 0;
uint32_t CurrentAngle;
int WantedAngle = 0;
int turn = 0;

const int32_t turnAngle45 = 0x20000000;
const int32_t turnAngle90 = turnAngle45 * 2;
const int32_t turnAngle1 = (turnAngle45 + 22) / 45;
unsigned int lineSensorValues[3];


void setup() {
  Serial.begin(9600);
  /*
  while (1) {
    readLineSensors();
    for (int i = 0; i < sizeof(lineSensorValues); i++) {
      Serial.println(lineSensorValues[i]);
    }
    Serial.println();
    delay(100);
  }
  */
  CH6_Drive_Angle(Choose_Distance_MAXED(90, 1));
}

void loop() {
  // put your main code here, to run repeatedly:
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

void CH6_Drive_Angle(int Degrees) {
  lineSensors.initFiveSensors();
  turnSensorSetup();
  delay(10);
  turnSensorReset();
  lineSensors.initThreeSensors();
  calibrateLineSensors();
  delay(500);
  CH4_Forward_Distance(5);
  turnByDegree(Degrees);
  /*while(1){
    readLineSensors();
    delay(100);
  }*/
  while (readLineSensors() < 300) {
    DriveStraight();
    Serial.println("Tur: " + (String)readLineSensors());
  }
    motors.setSpeeds(0, 0);
}

//Nulstil gyro sensor
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

int Choose_Distance_MAXED(int MAX, int Mode) {
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

void CH4_Forward_Distance(double Distance) {
  ResetEncoders();
  int DrivenDistance = CalcDistance(EncoderL(), EncoderR());
  while (Distance >= DrivenDistance) {
    DrivenDistance = CalcDistance(EncoderL(), EncoderR());
    DriveStraight();
    DrivenDistance = CalcDistance(EncoderL(), EncoderR());
    delay(5);
  }
  motors.setSpeeds(0, 0);
  ResetDrive(200);
}


float EncoderMultipliers[] = { 1, 0.996 };  //The values is found by testing, and the usage can be seen later.
//Graph: https://drive.google.com/file/d/1DbhvCuoILpcMSU08_YMRrD7T6l368Fy1/view?usp=sharing
//Data: https://docs.google.com/spreadsheets/d/13F6HxoBgwMkGPW054eQ8_4Q4qsMWnN_1/edit?usp=drive_link&ouid=101880835896362727209&rtpof=true&sd=true

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

int CalcDistance(double Left, double Right) {
  int DrivenDistance = (int)(Left / (12 * 75) * wheelCirc + Right / (12 * 75) * wheelCirc) / 2;
  return DrivenDistance;
}

void ResetDrive(int speed) {
  ForwardVal[0] = speed;
  ForwardVal[1] = speed;
}
