#include "Arduino.h"
#include "LTC4150.h"

int pin_int;
int pin_clr;
int pin_pol;
int interupt_number;
double current; 

double ah_quanta = 0.17067759; // mAh for each INT 50mOhm sense
volatile boolean isrflag;
volatile long int time_, lasttime;

volatile double charge_mAh = 0;
volatile double discharge_mAh = 0;

// Constructor
LTC4150::LTC4150(){
}

void LTC4150::configure(int pin, int clr, int pol, int interupt){
  pinMode(pin, INPUT);
  pinMode(clr, INPUT);
  pinMode(pol, INPUT);

  interupt_number = interupt;
  pin_int = pin;
  pin_clr = clr;
  pin_pol = pol; 
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

void LTC4150::reset()
{
  charge_mAh = 0;
  discharge_mAh = 0;
}

