#include "Thermal_Controller.h"
#include "Charge_Controller.h"
#include "Constant_Current_Controller.h"
#include "BT_Board_Presets.h"

//
// Program Setup 
//

#define ID 0

// Constant Set 
int     digipot_resistance, ntc_R2;
byte    slave_address;
double  dischage_current_slope; 

// Pin Set
/*
const int PIN_VBAT_READ      = A0;
const int PIN_NTC            = A1; 
const int PIN_UD             = A2; 
const int PIN_CS             = A3; 
    
const int PIN_SDA            = 2;
const int PIN_SCL            = 3; 

const int PIN_CHRG_CTL       = 4;
const int PIN_HEAT_CTL       = 5;
const int PIN_DISCHARGE_CTL  = 6;
const int PIN_INT            = 7;
const int PIN_CLR            = 8;
const int PIN_POL            = 9;
const int PIN_SA0            = 10;
const int PIN_SA1            = 16;
const int PIN_SA2            = 14;
const int PIN_SA3            = 15;
*/

const int PIN_UD             = 4; //A2
const int PIN_CS             = 5; //A3
const int PIN_CHRG_CTL       = 6; //4 
const int PIN_DISCHARGE_CTL  = 7; //6

// if there is an error stop running the program and flash an led
boolean error_stat = 0; 
boolean serial_debug = 0;

BT_Board_Presets presets_library(ID, &error_stat);

//
// End Setup
//



int counter = 0;

//Thermal_Controller(int pNTC, int pPWM, int r2) 
//Thermal_Controller THC(pin_ntc, heater_ctl, ntc_R2);

Constant_Current_Controller CCC; 

Charge_Controller CC; 

int cnt = 100;


void setup() {
  pinMode(PIN_DISCHARGE_CTL, OUTPUT);
  digitalWrite(PIN_DISCHARGE_CTL, LOW);
  
  // Full Setup

  CC.controller_setup(PIN_CS, PIN_UD, PIN_CHRG_CTL);
  //CC.enable();
  
  // load the correct values for this board
  presets_library.loadCalibrationValues(&digipot_resistance, &ntc_R2, &slave_address, &dischage_current_slope);

  //
  
  
  
  delay(1000);
  Serial.begin(115200);
  delay(100);
  //THC.set_temp(52);
  Serial.println("Setting Temp to 40C");
  delay(100);
  //THC.enable(1);
  Serial.println("Setting THC Enable Bit");
  Serial.println();
}

void loop() {

  
  
  CC.setCurrent(cnt);

  cnt = cnt + 50; 
  
  if (cnt == 850)
    cnt = 100;

  delay(1000);
  
  /*
  // put your main code here, to run repeatedly:
  THC.compute();
  loop_print();
  counter = counter + 1; 
  delay(4000);
  //THC.enable(0);
  */
}

void loop_print(){
  /*
  if ( counter == 1){
    
    THC.debug_print();
    counter = 0;
    
  }
  */
  /*
  Serial.print("Current Temperature: ");
  Serial.println(THC.get_temp());
  Serial.print("Heater PWM: ");
  Serial.println(THC.PWM_value);
  Serial.println();
  */
}

