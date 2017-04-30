// the setup function runs once when you press reset or power the board
// To use VescUartControl stand alone you need to define a config.h file, that should contain the Serial or you have to comment the line
// #include Config.h out in VescUart.h

//Include libraries copied from VESC
#include "VescUart.h"
#include "datatypes.h"
#include "VescUart.h"

#include <stdio.h>

// Define Values which are going to be measured
float current = 0.0;           //measured battery current
float motor_current = 0.0;     //measured motor current
float voltage = 0.0;           //measured battery voltage
float c_speed = 0.0;           //measured rpm * Pi * wheel diameter [km] * 60 [minutes]
float c_dist = 0.00;           //measured odometry tachometer [turns] * Pi * wheel diameter [km] 
double power = 0.0;            //calculated power

mc_values VescMeasuredValues;
unsigned long count;

void setup() {

	//Setup UART port
	Serial.begin(115200);

}



// the loop function runs over and over again until power down or reset
void loop() {
	
	// Measure and obtain the motor controller values
	if (vesc_get_values(VescMeasuredValues)) 
	{
		voltage = VescMeasuredValues.v_in;
		current = VescMeasuredValues.current_in;
		motor_current = VescMeasuredValues.current_motor;
		power = current*voltage;
		c_speed = (VescMeasuredValues.rpm / 38)*3.14159265359*0.000083 * 60;
		c_dist = (VescMeasuredValues.tachometer / 38)*3.14159265359*0.000083;

	}
	else {

	}
}
