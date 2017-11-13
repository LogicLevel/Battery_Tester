#include "Arduino.h"
#include "BatteryManagement.h"
//#include "LTC4150.h"

//int cutoff_voltage;
//int discharge_current;
//int charge_current;

int battery_pin;
int reference_pin;
int supply_adjustment;
int reference_voltage;

int ECFlag = 0;
int EDFlag = 0;


BatteryManagement::BatteryManagement(int vbat_pin, int vref_pin, int ref_voltage){
  battery_pin = vbat_pin;
  reference_pin = vref_pin; 
  reference_voltage = ref_voltage; 

    
}

// returns battery voltage in mv
int BatteryManagement::get_battery_voltage(){
  int temp = analogRead(battery_pin); 
  temp = (int) (temp/1023.0) * measure_vcc();
  return temp; 
}

int BatteryManagement::get_battery_current() {
  
  return 0;
}

// uses reference to back calculate VCC
int BatteryManagement::measure_vcc() {
  int temp = analogRead(reference_pin); 
  temp = (int)((reference_voltage * 1023.0)/temp);
  return temp;
}
