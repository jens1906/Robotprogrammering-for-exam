#include <Zumo32U4.h>
#include <Wire.h>

Zumo32U4Encoders encoders;
Zumo32U4Motors motors;
Zumo32U4Buzzer buzzer;
Zumo32U4IMU imu;
Zumo32U4LCD display;
Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA buttonA;

int countsLeft;
int countsRight;
int stage;
int speed = 150;
int maxSpeed = 150;
//bool buttonPressed = false;  // Boolean for om knappen et trykket ned eller ikke.

int sample = 0;
unsigned long startMillis;
unsigned long currentMillis;

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

//VARIABLER FOR LINEFOLLOWER
int16_t lastError = 0;
float Kmax = 0.4;
float kp = 0.6 * Kmax;
float kd = 0.3;  //if overshoot decrease
float Ki = 0.1;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

void setup() {
  Serial.begin(9600);
  turnSensorSetup();
  delay(10);
  turnSensorReset();
  startMillis = millis();
  // Uncomment if necessary to correct motor directions:
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);
  delay(100);
  lineSensors.initFiveSensors();
  calibrateSensors();
  buzzer.playNote(NOTE_E(4), 350, 15);
}

void loop() {
  if (buttonA.isPressed()) {
    stage = 2;
  }
  switch (stage) {
    case 1:
      turnByDegree(90);  //vinkelret venstre
      encoderReset();
      forwardByEncoder(2000);
      encoderReset();
      sampleCollect();
      turnByDegree(180);
      forwardByEncoder(1980);
      encoderReset();
      turnByDegree(90);
      stage = 2;
    case 2:
      if (sample != 2) {
        encoderReset();
        lineFollow();
        delay(10);
        stage = 1;
      } else {
        returnToBase();
      }
    case 3:
      buzzer.playNote(NOTE_E(4), 350, 15);
      delay(1000);
      buzzer.playNote(NOTE_E(4), 350, 15);
      delay(1000);
      buzzer.playNote(NOTE_E(4), 350, 15);
      delay(1000);
  }
}

void buzzerAndDelay() {
  currentMillis = millis();
  if (currentMillis - startMillis >= 1000) {
    buzzer.playNote(NOTE_E(4), 350, 15);
    startMillis = currentMillis;
  }
}

void encoderReset() {
  countsLeft = encoders.getCountsAndResetLeft();
  countsRight = encoders.getCountsAndResetRight();
}

void sampleCollect() {
  buzzer.playNote(NOTE_E(4), 350, 15);
  delay(400);
  sample += 1;
}

void returnToBase() {
  turnByDegree(180);
  encoderReset();
  lineFollow();
  encoderReset();
  lineFollow();
  delay(100);
  turnByDegree(180);
  delay(100);
  sample = 0;
  stage = 3;
}

void forwardByEncoder(int EncoderWanted) {  //kører fremad indtil både højre og venstre bælte bliver == 2000 "hjulklik"
  delay(100);
  motors.setSpeeds(speed, speed);
  do {
    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
  } while (((countsLeft + countsRight) / 2) < EncoderWanted);  //(countsLeft < 2000 && countsRight < 2000);
  motors.setSpeeds(0, 0);
  delay(100);
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

void lineFollow() {
  while (((countsLeft + countsRight) / 2) < 2000) {
    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    int16_t position = lineSensors.readLine(lineSensorValues);
    int16_t integral = 0;
    // Our "error" is how far we are away from the center of the line, which corresponds to position 2000.
    int16_t error = position - 2000;
    integral += error;

    //int16_t speedDifference = (kp) * error + (kd) * (error - lastError);
    int16_t speedDifference = (kp)*error + (Ki)*integral + (kd) * (error - lastError);

    lastError = error;

    // Get individual motor speeds.  The sign of speedDifference
    // determines if the robot turns left or right.
    int16_t leftSpeed = (int16_t)maxSpeed + speedDifference;
    int16_t rightSpeed = (int16_t)maxSpeed - speedDifference;

    leftSpeed = constrain(leftSpeed, -200, (int16_t)maxSpeed);
    rightSpeed = constrain(rightSpeed, -200, (int16_t)maxSpeed);

    motors.setSpeeds(leftSpeed, rightSpeed);
  }
  motors.setSpeeds(0, 0);
  delay(100);
}

//Calibrate linesensors
void calibrateSensors() {
  delay(1000);
  for (uint16_t i = 0; i < 120; i++) {
    if (i > 30 && i <= 90) {
      motors.setSpeeds(-speed, speed);
    } else {
      motors.setSpeeds(speed, -speed);
    }
    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}

//ALT FOR GYRO HERUNDER:
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