#include "Constant_Current_Controller.h"
#include "Arduino.h"

#define testPWM 51

int pin_heater_PWM;
int PWM_val = 0; 
int setpoint;                 // this is the set current value
double *current;              // this is the measured current from the sensor value
double slope = 16.33987;      // current per pwm setep (default slope is 16.33

// Constructor for constant current driver
Constant_Current_Controller::Constant_Current_Controller(){

}

void Constant_Current_Controller::setup(int _pin_PWM, double *_current) {
  pin_heater_PWM = _pin_PWM;
  current = _current; 

  // Zero output at start
  pinMode(pin_heater_PWM, OUTPUT); 
  analogWrite(pin_heater_PWM, PWM_val); 

  // Calibrade at start 
  calibrate(); 
}

void Constant_Current_Controller::calibrate() {
  // accelerate transistion time by going full on for 40ms
  output(255); 
  delay(40);
  // Set output to 1/5 of max PWM
  output(testPWM); 
  // wait for output to reach final voltage
  delay(1300);
  double temp = *current; 
  // Calculate the current per PWM step; 
  slope = temp / testPWM;
  output(0); 
}

void Constant_Current_Controller::setCurrent(int c){
  // store setpoint current
  setpoint = c; 
  
  // this will give us the pwm value needed to output the disired current
  PWM_val = c / slope; 
  
  //output pwm value
  output(PWM_val);
}


void Constant_Current_Controller::output(int val){
  analogWrite(pin_heater_PWM, val); 
}

