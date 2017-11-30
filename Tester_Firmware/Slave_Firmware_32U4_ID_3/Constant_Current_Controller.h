#ifndef Constant_Current_Controller_h
#define Constant_Current_Controller_h
#include <Arduino.h>

class Constant_Current_Controller {

  public:
    // Constructor - takes pointer inputs for control variales, so they are updated automatically
    Constant_Current_Controller();
  
    // Methods
    void setup(int _pin_PWM, int *I_discharge, int *v_cut, double *_current, int *voltage, int *stat); 
    void setCurrent(int c);
    void compute(); 
    void enable();
    void disable();
    void setMaxCurrent(int c); // adjust the default max discharge current of 2A 

    // Fields
    int setpoint;     // this is the set current value

    

  private:
    // Methods
    void calibrate(); 
    void output(int val); 

    // Fields
    int *discharge_current;
    int *cutoff_voltage;  
    int *discharge_status; 
    int *battery_voltage; 
    int max_current; 
    int pin_discharge_PWM;
    int PWM_val; 
    double *sensor_current;  // this is the measured current from the sensor value
    double slope;     // current per pwm setep

}; //class Constant_Current_Controller

#endif
