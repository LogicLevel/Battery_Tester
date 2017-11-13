#include "Arduino.h"
#include "Pin_Interrupt.h"

// Constructor
Pin_Interrupt::Pin_Interrupt(int pin){


  
}

void LTC4150::myISR() 
{
  // Determine delay since last interrupt (for mA calculation)
  // Note that first interrupt will be incorrect (no previous time!)

  lasttime = time_;
  time_ = micros();

  // only logs data while charging or discharging
  if (digitalRead(pin_pol)) 
    charge_mAh += ah_quanta; 
  else 
    discharge_mAh += ah_quanta; 

  // Calculate mA from time delay (optional)
  current = 614.4/((time_-lasttime)/1000000.0);
  
  isrflag = true; // set interupt flag for main loop
}
