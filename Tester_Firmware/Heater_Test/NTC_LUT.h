#ifndef NTC_LUT_h
#define NTC_LUT_h

#include "Arduino.h"

class NTC_LUT
{
  public:

    // Constructor
    //NTC_LUT() {  };
    
    // Methods
    int get_temp();
    
    int temperature_to_ADC(int t);
    int readADC(); 

    void NTC_LUT_setup(int pin, int r2);

  private:
    // Fields
    // Stores lookup table values 
    //const int temperature[17];      
    const int temperature[17] = 
    {
      0,  5,  10, 15, 20,
      25, 30, 35, 40, 45,
      50, 55, 60, 65, 70,
      75, 80
    };
    //int resistance[17];
    const int resistance[17] = 
    { 
      27217, 22021, 17926, 14674, 12081, 
      10000, 8315,  6948,  5834,  4917, 
      4161,  3535,  3014,  2586,  2228, 
      1925,  1669 
    };
    
    int resistor2; 
    int pin_ADC;        
    
    // Methods
    int interpolate(); 
    
    int find_resistance_index(int res);
    int find_temp_index(int temp);
    
    int resistance_interpolate(int index, int res);
    int temp_interpolate(int index, int temp); 
    
    
};

#endif

