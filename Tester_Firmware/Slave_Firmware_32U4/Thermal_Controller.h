#ifndef Thermal_Controller_h
#define Thermal_Controller_h

#include "Arduino.h"
#include "NTC_LUT.h"
#include "AutoPID.h"

class Thermal_Controller
{
  public:
    
    // Constructor
    Thermal_Controller(int pNTC, int pPWM, int r2);
    
    // Methods
    void debug_print();
    int get_temp();                 // This is a pass through from NTC_LUT
    int set_temp(int t);            // Sets temperature
    double PWM_value;               // Non-private for debugging purposes

    void compute();                 // Computes PID loop 
    void enable(int stat);      // enable and disables thermal contol

  private:
    // Fields
    int current_temperature;        // Current temperature in C
    int target_temperature;         // Target temperature in C
    double ADC_value;
    //double PWM_value;
    double target_ADC_value;        // ADC value of setpoint temperature 
    int _enable;                     // Run Stop Status
    int pin_NTC, pin_PWM;           // Stores input and output pins for the system

    AutoPID myPID;           // PID Instance 
    NTC_LUT temperature_LUT;        // NTC_LUT Instance
    
};

#endif

