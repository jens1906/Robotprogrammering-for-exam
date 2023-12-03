#include <Wire.h>
#include <Zumo32U4.h>

// Change the next line depending on if the Zumo has an LCD or OLED display
Zumo32U4OLED display;
//Zumo32U4LCD display;

Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4Motors motors;

int Tape_value = 150;

uint16_t L_proxSensors;
uint16_t LC_proxSensors;
uint16_t R_proxSensors;
uint16_t RC_proxSensors;

//Parameters for the PD regulation - Used method Ziegler & Nichols 
float ku = 125 //This value is found by testing, when the robot is stable and consistent in it oscillations
float tu = 0.8 //The oscillation period 
float td = 0.125*tu
float kp = 0.8*ku //proportional gain //kp = 100 
float kd = 0.1*ku*tu //derivative (Compassion between the current- and previous error) //kd=0.1

int error; //The differens between the sensor values
int previous_error = 0;

//Default speed
uint16_t base_speed = 400;
//Regulation on motor to regulate the robot.
int regulation_speed;

//Brightness levels for proxy sensors.
uint16_t Levels[] = { 2, 12, 22, 32, 42, 52, 62, 72, 82, 92, 136 };

#define NUM_LINE_SENSORS 3 //Constant value
unsigned int LineSensor_values[NUM_LINE_SENSORS]; //Creating an array for the number of line sensors.

#define NUM_PROXY_SENSORS 4
int Proximity_values[NUM_PROXY_SENSORS];

//Start up
void setup() {
  Serial.begin(9600);
  proxSensors.initThreeSensors(); //Configures the proximity sensors - 3 sensors (left, front, right).
  lineSensors.initThreeSensors(); //Configures the line sensors - 3 sensors (left, middle, right).
  Calibrate_line_sensors(); //Runs the function Calibrate_line_sensors().
  proxSensors.setPeriod(500); //Sets the period on the proximity sensors.
  proxSensors.setBrightnessLevels(Levels, sizeof(Levels)); //Sets the brightnesslevels on the proximity sensors.
}

void Calibrate_line_sensors() {
  ledYellow(1);
  display.clear();
  display.print(F("Line cal"));

  for (uint16_t i = 0; i < 200; i++) { 
    display.gotoXY(0, 1);
    display.print(i);
    lineSensors.calibrate(); //runs the calibration command "calibrate" from the zumo library.
  }

  ledYellow(0);
  display.clear();
}

void Get_proximity_values() { //Updates the proxymity sensors values.
  proxSensors.read(); 
  Proximity_values[0] = proxSensors.countsLeftWithLeftLeds();
  Proximity_values[1] = proxSensors.countsFrontWithLeftLeds();
  Proximity_values[2] = proxSensors.countsFrontWithRightLeds();
  Proximity_values[3] = proxSensors.countsRightWithRightLeds();
}

void drive_in_middle() {
  while (LineSensor_values[1] < Tape_value) {
    display.clear();
    lineSensors.read(LineSensor_values); //Updates the linesensors value.

    Get_proximity_values();
    //Sets the error depending on the difference between the sensors values.
    error = Proximity_values[0] + Proximity_values[1] * 0.1 - Proximity_values[2] * 0.1 - Proximity_values[3]; //The front left and front right sensors are not prioritized as much as the sensor on the sides of the zumo.

    //regulation speed is determent on PD configurations, the current error and previous error.
    regulation_speed = kp * error + kd * (error - previous_error);

    //Sets the current error as the previous.
    previous_error = error;

    //no error
    if (error == 0) { //if the error is equal to 0 it continues drive with the based speed.
      motors.setSpeeds(base_speed, base_speed);
    }
    
    else if (error > 0) { //if the error is greater than 0 slow down the right motor.
      motors.setSpeeds(base_speed, base_speed - regulation_speed); // regulation_speed is a positive value. 
    }
    
    else if (error < 0) { //if the error is less than 0 slow down the left motor.
      motors.setSpeeds(base_speed + regulation_speed, base_speed); // regulation_speed is a negative value. 
    }
  }
}

//Infinite loop
void loop() {
  //Runs the funtion drive_in_middle if the tape is not detected.
  if (LineSensor_values[1] < Tape_value) {
    drive_in_middle();
  }

  //Displays "Done" and stops the motors if the tape is detected.
  else if (LineSensor_values[1] > Tape_value) {
    motors.setSpeeds(0, 0);
    display.print("Done");
  }
}

void Print_proximity_values_testing(){
  display.clear();
  display.print(Proximity_values[0]);
  display.print(" ");
  display.print(Proximity_values[1]);
  display.gotoXY(0, 1);
  display.print(Proximity_values[2]);
  display.print(" ");
  display.print(Proximity_values[3]);

  Serial.println(Proximity_values[0]);
  Serial.println(Proximity_values[1]);
  Serial.println(Proximity_values[2]);
  Serial.println(Proximity_values[3]);
  Serial.println();
}