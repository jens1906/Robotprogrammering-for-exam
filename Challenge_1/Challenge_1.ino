// Include the Zumo 32U4 library
#include <Zumo32U4.h>

// Create an object for the proximity sensors
Zumo32U4ProximitySensors proxSensors;

// Create an array to store the sensor readings
uint16_t sensorValues[3];

void setup() {
  // Initialize the serial monitor
  Serial.begin(9600);

  // Initialize the proximity sensors
  proxSensors.initThreeSensors();

}

void loop() {
  // Read the sensor values and store them in the array
  proxSensors.read(sensorValues);

  // Print the sensor values to the serial monitor
  Serial.print("Left: ");
  Serial.print(sensorValues[0]);
  Serial.print("\tFront: ");
  Serial.print(sensorValues[1]);
  Serial.print("\tRight: ");
  Serial.println(sensorValues[2]);

  // Wait for 100 milliseconds before reading again
  delay(100);
}
