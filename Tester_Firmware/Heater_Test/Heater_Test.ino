#include "Thermal_Controller.h"

const int heater_ctl = 5; 
const int pin_ntc = 1;
const int ntc_R2 = 10000;           // Second resistor in divider
const int discharge_ctl = 6; 
const int charge_ctl = 4; 

int counter = 0;

Thermal_Controller THC(pin_ntc, heater_ctl, ntc_R2);
//Thermal_Controller(int pNTC, int pPWM, int r2) 

void setup() {
  pinMode(discharge_ctl, OUTPUT);
  pinMode(charge_ctl, OUTPUT);
  digitalWrite(discharge_ctl, LOW); 
  digitalWrite(charge_ctl, LOW); 
  
  delay(1000);
  Serial.begin(115200);
  delay(100);
  THC.set_temp(52);
  Serial.println("Setting Temp to 40C");
  delay(100);
  THC.enable(1);
  Serial.println("Setting THC Enable Bit");
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
  THC.compute();
  loop_print();
  counter = counter + 1; 
  delay(4000);
  //THC.enable(0);
}

void loop_print(){
  if ( counter == 1){
    
    THC.debug_print();
    counter = 0;
    
  }
  /*
  Serial.print("Current Temperature: ");
  Serial.println(THC.get_temp());
  Serial.print("Heater PWM: ");
  Serial.println(THC.PWM_value);
  Serial.println();
  */
}

