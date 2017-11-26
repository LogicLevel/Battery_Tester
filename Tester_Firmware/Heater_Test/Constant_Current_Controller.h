#ifndef Constant_Current_Controller_h
#define Constant_Current_Controller_h
#include <Arduino.h>

class Constant_Current_Controller {

  public:
    // Constructor - takes pointer inputs for control variales, so they are updated automatically
    Constant_Current_Controller();
  
    // Methods
    void setup(int _pin_PWM, double *_current); 
    void setCurrent(int c);

    // Fields
    int setpoint;     // this is the set current value

    

  private:
    // Methods
    void calibrate(); 
    void output(int val); 

    // Fields
    int pin_heater_PWM;
    int PWM_val; 
    double *current;  // this is the measured current from the sensor value
    double slope;     // current per pwm setep

}; //class Constant_Current_Controller

#endif
