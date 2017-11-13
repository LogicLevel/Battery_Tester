#ifndef BatteryManagement_h
#define BatteryManagement_h

#include "Arduino.h"

class BatteryManagement
{
  public:
    BatteryManagement(int vbat_pin, int vref_pin, int ref_voltage);
    
    // Fields
    //int cutoff_voltage;
    //int discharge_current;
    //int charge_current;

    int ECFlag;
    int EDFlag;

    // Methods
    int get_battery_voltage();
    int get_battery_current(); 

  private: 
    int battery_pin; // arduino pin nubmer
    int reference_pin;
    int supply_adjustment;
    int reference_voltage;

    int measure_vcc();

    // Temp
    int temp;
    
};

#endif

