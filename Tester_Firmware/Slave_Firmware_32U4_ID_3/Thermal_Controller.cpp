#include "Thermal_Controller.h"
#include "Arduino.h"
#include "NTC_LUT.h"
#include <math.h>
#include "AutoPID.h"

#define PID_UPDATE_INTERVAL 1000 // ms 
#define BANG_BANG_TRESHOLD 30    // ADC counts

#define PWM_LOW  0.0
#define PWM_HIGH 255.0

double aggKp=4, aggKi=0.2, aggKd=1;
double consKp=1, consKi=0.05, consKd=0.25;

double target_ADC_value;         // ADC value of setpoint temperature     (PID)
double ADC_value;                // Input ADC value from NTC              (PID)
double PWM_value;                // Ouput PWM signal to heater            (PID)
int _enable = 0;                  // Run Stop Status
int pin_PWM;            // Stores input and output pins for the system

NTC_LUT temperature_LUT;

// Constructor
//  Initializes PID but does not enable or set temperature to be maintained
Thermal_Controller::Thermal_Controller(int pNTC, int pPWM, int *r2, int *tSet, int *tMeas) 
{ 
  // Store pointers 
  resistor2 = r2; 
  target_temperature = tSet;
  current_temperature = tMeas;
  
  pin_PWM = pPWM; 
  
  pinMode(pin_NTC, INPUT);
  pinMode(pin_PWM, OUTPUT);

  // setup table
  temperature_LUT.NTC_LUT_setup(pNTC, resistor2);

  //myPID.AutoPIDSetup(&ADC_value, &target_ADC_value, &PWM_value, PWM_LOW, PWM_HIGH, aggKp, aggKi, aggKd); 
  myPID.AutoPIDSetup(&ADC_value, &target_ADC_value, &PWM_value, PWM_LOW, PWM_HIGH, 2, 1, 1);

  // if temperature is more or thes than 30 ADC counts from target_ADC_value, OUTPUT will be set to min or max respectively
  myPID.setBangBang(BANG_BANG_TRESHOLD);
  
  // set PID update interval to 4000ms
  myPID.setTimeStep(PID_UPDATE_INTERVAL);

}

void Thermal_Controller::compute() // Computes PID loop and measures temperature
{ 
  
  // Compute the target ADC value for PID (disabled to fix errer and debug)
  target_ADC_value = 1023 - temperature_LUT.temperature_to_ADC(*target_temperature);

  // Update current temperature
  temperature_LUT.get_temp();
  
  // Calculate current temperature
  *current_temperature = temperature_LUT.get_temp();
   
  // Only opperate heater if enable is hight
 
  if (_enable) {
    ADC_value = temperature_LUT.readADC(); // updates ADC value

    int difference = *target_temperature - *current_temperature;
    double gap = abs(difference); //distance away from setpoint
    if(gap<20)
    {  //we're close to setpoint, use conservative tuning parameters
      myPID.setGains(consKp, consKi, consKd);
    }
    else
    {
     //we're far from setpoint, use aggressive tuning parameters
     myPID.setGains(aggKp, aggKi, aggKd);
    }
    
    myPID.run();

    analogWrite(pin_PWM, PWM_value);
  } else {
    analogWrite(pin_PWM, 0);
  }
  
}

void Thermal_Controller::enable(int stat) { // enable bit 
  if (stat <= 0){
    _enable = 0;
    myPID.stop();
  } else {
    _enable = 1; 
    myPID.reset();
  }
}

void Thermal_Controller::debug_print() {
  Serial.print("Target Temperature: ");
  Serial.println(*target_temperature);
  Serial.print("Current Temperature: ");
  Serial.println(*current_temperature);
  Serial.print("Target ADC Value: ");
  Serial.println(target_ADC_value);
  Serial.print("Enable: ");
  Serial.println(_enable);
  Serial.print("PWM Value: ");
  Serial.println(PWM_value);
  Serial.print("ADC Value: ");
  Serial.println(ADC_value);
  Serial.print("THC is stopped?: ");
  Serial.println(myPID.isStopped());
  
  Serial.println();
}

