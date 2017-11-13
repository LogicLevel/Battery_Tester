#ifndef Pin_Interrupt_h
#define Pin_Interrupt_h

#include "Arduino.h"

class Pin_Interrupt
{
  public:
    // Constructor
    Pin_Interrupt (int pin); 

    // Mehods 
    void myISR();
    
    // Fields


  private: 
    
};

#endif

