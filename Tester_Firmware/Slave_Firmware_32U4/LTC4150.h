#ifndef LTC4150_h
#define LTC4150_h

#include "Arduino.h"

class LTC4150
{
  public:
    // Constructor
    LTC4150 (int pin, int clr, int pol, int interupt); 

    // Mehods 
    void myISR();
    
    // Fields
    int interupt_number;
    double current; 
    volatile double charge_mAh;
    volatile double discharge_mAh;

  private: 
    // Methods 
    void reset();
    
    // Fields
    int pin_int;
    int pin_clr;
    int pin_pol;
    double ah_quanta;
    volatile boolean isrflag;
    volatile long int time_, lasttime;
    
    
};

#endif

