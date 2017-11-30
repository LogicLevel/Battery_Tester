#ifndef Charge_Controller_h
#define Charge_Controller_h
#include <Arduino.h>


/*
 * Charge Controller is designed for management of lithium ion battery controllers
 *  using a digipot it has the ability to set the charge current
 *  charge termination current is done through software if desired value is above the preset value on the IC
 * 
 */

/* charge termination must be done in software
 *    needs a preset value or percentage 
 *    
 * chage current set with feadback to adjust to desired value
 *    this can either use a table or trial and error
 *    needs a minimum and max value field
 *    make note of I set resistor used
 *      int const min_I_set
 *      int const max_T_set 
 *      
 * simulate lower charge terminaton voltages by redusing I Set ??? 
 *      
 *  
 */

class Charge_Controller {

  public:
    // Constructor - takes pointer inputs for control variales, so they are updated automatically
    Charge_Controller();
  
    // Methods
    void setup(int CS, int UD, int CHRG_CTL,int *I_chrg, double *current, int *I_cut, int *stat); 
    void setCurrent(int c);     // input is in mA
    void enable();
    void disable();
    void compute();

  //private:
    // Methods
    void down();
    void up();
    void wiperSet(int stp); 

    // Pins
    int PIN_CS;
    int PIN_UD;
    int PIN_CHRG_CTL;

    // Fields
    int step_val;
    double *measured_current;
    int *charge_current;
    int *cutoff_current;
    int *charge_status; // goes low when charging is complete
    
    const int I_MAX = 800; 
    const int I_MIN = 10;
    const int POT_RESISTANCE = 10000; 
    const int STEP_LOADVAL = 31; 
    const int MIN_STEPVAL = 0;
    const int MAX_STEPVAL = 63; 
    const int STEP_CONST = 62;

}; //class Charge_Controller

#endif
