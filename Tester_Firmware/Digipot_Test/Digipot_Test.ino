#include "Charge_Controller.h"

// Pin Set
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


int charge_current = 330;           // mA
double current_non_vol;             // this is the non volitile int form of curren
int charge_term_current = 50;       // mA
int EC_flag = 0;                    // end of charge flag (set high at end of charge)


Charge_Controller CC; 

int counter = 31;
int direct = 1;

void setup() {
  // put your setup code here, to run once:
  CC.setup(PIN_CS, PIN_UD, PIN_CHRG_CTL, &charge_current, &current_non_vol, &charge_term_current, &EC_flag);
  Serial.begin(115200);

  
  
}

void loop() {
  if (counter < 60) {
    if (counter > 2) 
      counter = counter + direct; 
  } else {
    if (counter == 60)
      direct = -1; 
    else 
      direct = 1; 
  }
  if (direct == 1) {
    CC.up();
    Serial.println("UP");
  } else {
    CC.down();
    Serial.println("Down");
  }

  delay(2000);
}



