#include "Constant_Current_Controller.h"
#include "Arduino.h"

#define testPWM 51


// Constructor for constant current driver
Constant_Current_Controller::Constant_Current_Controller(){

}

void Constant_Current_Controller::setup(int _pin_PWM, int *I_discharge, int *v_cut, double *_current, int *voltage, int *stat) {
  pin_discharge_PWM = _pin_PWM;
  sensor_current = _current; 
  discharge_current = I_discharge;
  cutoff_voltage = v_cut; 
  battery_voltage = voltage; 
  discharge_status = stat;

  // Zero output at start
  pinMode(pin_discharge_PWM, OUTPUT); 
  analogWrite(pin_discharge_PWM, PWM_val); 

  // Calibrade at start 
  //calibrate(); 
  slope = 12.63513;      // current per pwm setep (default slope is 16.33
  max_current = 2000; 
  PWM_val = 0; 
}

void Constant_Current_Controller::calibrate() {
  // accelerate transistion time by going full on for 40ms
  output(255); 
  delay(40);
  // Set output to 1/5 of max PWM
  output(testPWM); 
  // wait for output to reach final voltage
  delay(1300);
  // Calculate the current per PWM step; 
  slope = *sensor_current / testPWM;
  output(0); 
}

void Constant_Current_Controller::setCurrent(int c){
  // Check that max_current is not exceeded
  if (c > max_current) 
    setpoint = max_current;
  else 
    setpoint = c; 
  
  // this will give us the pwm value needed to output the disired current
  PWM_val = setpoint / slope;
  //output pwm value
  output(PWM_val);
}

void Constant_Current_Controller::compute(){
  if (*discharge_status == 1) { 
    
    // Recalculate charge current parameters
    setCurrent(*discharge_current);

    // Check if cutoff current has been reached
    if (*battery_voltage < *cutoff_voltage)
      disable(); 
      
  }
}

void Constant_Current_Controller::enable(){
  *discharge_status = 1; 
}

void Constant_Current_Controller::disable(){
  *discharge_status = 0; 
  output(0); 
}

void Constant_Current_Controller::setMaxCurrent(int c){
  max_current = c; 
}


void Constant_Current_Controller::output(int val){
  analogWrite(pin_discharge_PWM, val); 
}

