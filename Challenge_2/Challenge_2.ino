#include <Wire.h>
#include <Zumo32U4.h>  //includes headerfile content, for us its the pololu syntax

const uint16_t maxSpeed = 370;  //Set a Max speed for the pololu, which we will later implement


//The different obeject we refer to later on, like the Zumo32U4Motors class, that we just rename motors
Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;

Zumo32U4LCD display;
//Zumo32U4OLED display;

//Set a list of global variabels that we use later assigent to some variabels we can change
//type name
int16_t lastError = 0;
float Kmax = 0.4;
float kp = 0.6 * Kmax;
float kd = 0.3;  //if overshoot decrease
float Ki = 0.1;

int16_t integral = 0;

//"int16_t" and "int" both is for whole numbers, but "int" can have a size of 16, 32 or 64 bits where "int16_t" is set to only fill 16 bits
//"floats" is floating-point numbers

//"unsigned int" is just ints that is non negative, so positive integers or zero

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];  //setup an array with a size of 5 to later hold our sensor values


//return-type function-name
void calibrateSensors()  //defines a function with no return variable and no input variabel
{
  display.clear();

  // Wait 1 second and then begin automatic sensor calibration
  // by rotating in place to sweep the sensors over the line
  delay(1000);

  //for(initialization; condition; increment)
  for (uint16_t i = 0; i < 120; i++)  //starts a for-loop that runs for 120 times
  {
    //if(condition==true){
    //then
    //}
    if (i > 30 && i <= 90)  //checks if i is bigger than 30 AND if i is less than or equal to 90
    {
      motors.setSpeeds(-150, 150);  //sets motors to a speed between 0 and 400, here it turns left
    } else {
      motors.setSpeeds(150, -150);  //Here it turns right
    }

    lineSensors.calibrate();  //Uses the "lineSensors" class and the function calibrate to, calibrate the sensors
  }
  motors.setSpeeds(0, 0);  //Stops the motors
}

void lineFollow() {                                           //setup and array with a size of 5 to later hold our sensor values
  int16_t position = lineSensors.readLine(lineSensorValues);  //Uses the "lineSensors" class and the function readline, this function return an integer between 0 and 4000, depending on the lines position

  // Our "error" is how far we are away from the center of the
  // line, which corresponds to position 2000.
  int16_t error = position - 2000;  //therefore we correct the position with our center.

  integral += error;  //This is a sum of all previous errors

  //int16_t speedDifference = (kp) * error;
  //int16_t speedDifference = (kp) * error + (kd) * (error - lastError);
  int16_t speedDifference = (kp)*error + (Ki)*integral + (kd) * (error - lastError);  //PID controller given three global set variabels and the error computed in the lines above

  lastError = error;

  // Get individual motor speeds.  The sign of speedDifference
  // determines if the robot turns left or right.
  int16_t leftSpeed = (int16_t)maxSpeed + speedDifference;   //adds maxSpeed and speedDifference and places the result in leftSpeed
  int16_t rightSpeed = (int16_t)maxSpeed - speedDifference;  //substracts maxSpeed and speedDifference and places the result in rightSpeed

  leftSpeed = constrain(leftSpeed, -200, (int16_t)maxSpeed);    //the number leftSpeed is contrained i.e. it is limited down or up, to a range from -200 to maxSpeed
  rightSpeed = constrain(rightSpeed, -200, (int16_t)maxSpeed);  //the number rightSpeed is contrained i.e. it is limited down or up, to a range from -200 to maxSpeed

  motors.setSpeeds(leftSpeed, rightSpeed);  //The motors is set at leftSpeed and rightSpeed
}

void setup() {  //void setup defines a function that only runs once and dosent return anything
  // Uncomment if necessary to correct motor directions:
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);

  lineSensors.initFiveSensors();  //initialize the five linesensors

  Serial.begin(9600);  //initializes serial communication from the pololu to the computer

  //makes a home screen when the program is started
  // Wait for button A to be pressed and released.
  display.clear();
  display.print(F("Press A"));
  display.gotoXY(0, 1);
  display.print(F("to calib"));
  buttonA.waitForButton();

  //calls the function "calibrateSensors"
  calibrateSensors();
}

void loop() {    //void loop defines a function that loops for eternity or until otherwise stated in the following code
  lineFollow();  //calls the funtion lineFollow forever
}
