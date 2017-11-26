#include "Charge_Controller.h"
#include "Arduino.h"


// Pin Values
int PIN_CS;
int PIN_UD;
int PIN_CHRG_CTL;

// Fields
int step_val;

// Constructor for constant current driver
Charge_Controller::Charge_Controller(){

}

void Charge_Controller::setup(int CS, int UD, int CHRG_CTL) {
  // Store pin value 
  PIN_CS = CS;
  PIN_UD = UD; 
  PIN_CHRG_CTL = CHRG_CTL; 

  // Configure pins for output and set default values
  
  // CS setup
  pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_CS, HIGH); 
  // UD setup 
  pinMode(PIN_UD, OUTPUT);
  digitalWrite(PIN_UD, HIGH); 
  // MOSFET 
  pinMode(PIN_CHRG_CTL, OUTPUT);
  digitalWrite(PIN_CHRG_CTL, LOW); 

  // load the default value at startup
  step_val = STEP_LOADVAL; 
}

void enable(boolean e){
  // enable or disable charging
  digitalWrite(PIN_CHRG_CTL, e); 
}

void Charge_Controller::setCurrent(int c){
  // Equations from datasheet:
  //  Ibat = (Vprog/Rprog)*1000
  //  Rprog = 1000 * Vprog/Ibat
  
  // Calculate the Rprog resistor value
  int res_set = (1000.0 / c) * 1000; 
  // Map the calculated resistor value to nearest step of the digipot
  int stp = map(res_set, 0, POT_RESISTANCE, MIN_STEPVAL, MAX_STEPVAL);
  // Set the wiper
  wiperSet(stp); 
}

//
// Private methods used for up/down control of digipot
//


// Achives a target wiper position upsing the up/down comunications protocol
void Charge_Controller::wiperSet(int stp){
  // loop calling up or down until the desired wiper position is reached 
  while (stp != step_val) 
  { 
    if (step_val < stp)
      up();
    else 
      down();
  }
}

//
// Timing used for up control (2 us added to all timings)
//

// U/D Timings
#define tLUC 4      // U/D to CS Hold Time (us)
#define tLCUF 3     // CS to U/D Low Setup Time (us)
#define tLO 3       // U/D Low Time (us)
// CS Timings
#define tCSHI 3     // CS High Time (us)

//
// UP Control of Digipot
//
void Charge_Controller::up(){
  // Set U/D high
  digitalWrite(PIN_UD, HIGH);
  // U/D to CS Hold Time
  delayMicroseconds(tLUC); 
  // Set CS low
  digitalWrite(PIN_CS, LOW);
  // CS to U/D Low Setup Time (us)
  delayMicroseconds(tLCUF); 
  // Write U/D LOW
  digitalWrite(PIN_UD, LOW);
  // U/D Low Time
  delayMicroseconds(tLO);
  // Write U/D HIGH
  digitalWrite(PIN_UD, HIGH);
  // U/D to CS Hold Time
  delayMicroseconds(tLUC); 
  // Set CS High 
  digitalWrite(PIN_CS, HIGH);
  // CS High Time
  delayMicroseconds(tCSHI);
  // Increment the step count but don't go above the max step value
  if (step_val < MAX_STEPVAL)
    step_val = step_val + 1;
}

//
// Timing used for down control (2 us added to all timings)
//

#define tLCUR 5     // CS to U/D High Setup Time (us)
#define tHI 3       // U/D High Time (us)

//
// Down Control of Digipot
//
void Charge_Controller::down(){
  // Set U/D LOW
  digitalWrite(PIN_UD, LOW);
  // U/D to CS Hold Time
  delayMicroseconds(tLUC); 
  // Set CS low
  digitalWrite(PIN_CS, LOW);
  // CS to U/D Low Setup Time
  delayMicroseconds(tLCUR); 
  // Write U/D HIGH
  digitalWrite(PIN_UD, HIGH);
  // U/D to CS Hold Time
  delayMicroseconds(tLUC);
  // Write CS HIGH
  digitalWrite(PIN_CS, HIGH);
  // U/D to CS Hold Time
  delayMicroseconds(tCSHI); 
  // Decrement the step count but don't go below zero
  if (step_val > MIN_STEPVAL)
    step_val = step_val - 1;
}

