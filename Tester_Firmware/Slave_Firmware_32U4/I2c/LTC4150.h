#ifndef LTC4150_h
#define LTC4150_h

#include "Arduino.h"

class LTC4150
{
  public:
    // Constructor
    LTC4150 (); 

    
    // Mehods 
    void myISR();
    void configure(int pin, int clr, int pol, int interupt);
    
    // Fields
    int interupt_number;
    double current; 
    volatile double charge_mAh;
    volatile double discharge_mAh;

    double ah_quanta;
    volatile boolean isrflag;
    volatile long int time_, lasttime;

    int pin_int;
    int pin_clr;
    int pin_pol;

  private: 
    // Methods 
    void reset();
    
    
    
};

#endif

