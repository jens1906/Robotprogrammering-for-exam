/* This example uses the line sensors on the Zumo 32U4 to follow
a black line on a white background, using a PID-based algorithm.
It works decently on courses with smooth, 6" radius curves and
has been tested with Zumos using 75:1 HP motors.  Modifications
might be required for it to work well on different courses or
with different motors.

This demo requires a Zumo 32U4 Front Sensor Array to be
connected, and jumpers on the front sensor array must be
installed in order to connect pin 4 to DN4 and pin 20 to DN2. */

#include <Wire.h>
#include <Zumo32U4.h>

// This is the maximum speed the motors will be allowed to turn.
// A maxSpeed of 400 lets the motors go at top speed.  Decrease
// this value to impose a speed limit.
const uint16_t maxSpeed = 370;

Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;

// Change next line to this if you are using the older Zumo 32U4
// with a black and green LCD display:
Zumo32U4LCD display;
//Zumo32U4OLED display;

int16_t lastError = 0;
float Kmax = 0.4;
float kp = 0.6 * Kmax; 
float kd = 0.3; //if overshoot decrease
float Ki = 0.1; 

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

// Sets up special characters for the display so that we can show
// bar graphs.
void loadCustomCharacters()
{
  static const char levels[] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63
  };
  display.loadCustomCharacter(levels + 0, 0);  // 1 bar
  display.loadCustomCharacter(levels + 1, 1);  // 2 bars
  display.loadCustomCharacter(levels + 2, 2);  // 3 bars
  display.loadCustomCharacter(levels + 3, 3);  // 4 bars
  display.loadCustomCharacter(levels + 4, 4);  // 5 bars
  display.loadCustomCharacter(levels + 5, 5);  // 6 bars
  display.loadCustomCharacter(levels + 6, 6);  // 7 bars
}

void printBar(uint8_t height)
{
  if (height > 8) { height = 8; }
  const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, (char)255};
  display.print(barChars[height]);
}

void calibrateSensors()
{
  display.clear();

  // Wait 1 second and then begin automatic sensor calibration
  // by rotating in place to sweep the sensors over the line
  delay(1000);
  for(uint16_t i = 0; i < 120; i++)
  {
    if (i > 30 && i <= 90)
    {
      motors.setSpeeds(-150, 150);
    }
    else
    {
      motors.setSpeeds(150, -150);
    }

    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}

// Shows a bar graph of sensor readings on the display.
// Returns after the user presses A.
void showReadings()
{
  display.clear();

  while(!buttonA.getSingleDebouncedPress())
  {
    lineSensors.readCalibrated(lineSensorValues);

    display.gotoXY(0, 0);
    for (uint8_t i = 0; i < NUM_SENSORS; i++)
    {
      uint8_t barHeight = map(lineSensorValues[i], 0, 1000, 0, 8);
      printBar(barHeight);
    }
  }
  delay(1000);
}

void lineFollow(){
  int16_t position = lineSensors.readLine(lineSensorValues);
  int16_t integral = 0 ;
  // Our "error" is how far we are away from the center of the
  // line, which corresponds to position 2000.
  int16_t error = position - 2000;
  integral += error; 

  //int16_t speedDifference = (kp) * error + (kd) * (error - lastError);
  int16_t speedDifference = (kp) * error + (Ki) * integral + (kd) * (error - lastError);

  lastError = error;

  // Get individual motor speeds.  The sign of speedDifference
  // determines if the robot turns left or right.
  int16_t leftSpeed = (int16_t)maxSpeed + speedDifference;
  int16_t rightSpeed = (int16_t)maxSpeed - speedDifference;

  leftSpeed = constrain(leftSpeed, -200, (int16_t)maxSpeed);
  rightSpeed = constrain(rightSpeed, -200, (int16_t)maxSpeed);
  
  motors.setSpeeds(leftSpeed, rightSpeed);
}

void setup(){
  // Uncomment if necessary to correct motor directions:
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);

  lineSensors.initFiveSensors();

  Serial.begin(9600);

  loadCustomCharacters();

  // Wait for button A to be pressed and released.
  display.clear();
  display.print(F("Press A"));
  display.gotoXY(0, 1);
  display.print(F("to calib"));
  buttonA.waitForButton();

  calibrateSensors();

  showReadings();
}

void loop(){
  lineFollow();
}