#include "Arduino.h"
#include "NTC_LUT.h"

#define ADC_Res 1023.0

int resistor2 = 0; 
int pin_ADC;

/*
NTC_LUT::NTC_LUT(){
}
*/

void NTC_LUT::NTC_LUT_setup(int pin, int r2){
    resistor2 = r2; 
    pin_ADC = pin; 
}

int NTC_LUT::get_temp() {
  int temp = analogRead(pin_ADC); 
  int res = (int) ( (resistor2 / (ADC_Res - temp)) * temp );     // (Careful not to overflow the int value)
  
  int i = 0;
  while(res < resistance[i])      // while the NTC resistance is less than the table of resistance[index]
    i = i + 1;
    
  //temp = find_resistance_index(resistance);                             // temp is now the resistance index
  return resistance_interpolate(i, res); 
}

int NTC_LUT::temperature_to_ADC(int t){
  int index = find_temp_index(t);
  int res = temp_interpolate(index, t); // stores resistance coresponding to temperature t 
  // back calculates the ADC value (Careful not to overflow int value)
  // index is now used to store return value
  index = (int) ( (ADC_Res / (res + resistor2)) * res );
  return index; 
}

// Returns the first index for which the resistance was less than 
int NTC_LUT::find_resistance_index(int res){ 
  int i = 0;
  while(res < resistance[i])      // while the NTC resistance is less than the table of resistance[index]
    i = i + 1;
  return i;                       // the index returned is the lower bound (ie: 8315 < res < 6948    the index of 6948 is returned)
}

int NTC_LUT::find_temp_index(int temp){
  int index = 0;
  int temp_compare = temperature[index];
  while (temp > temp_compare) {
    index = index + 1;
    temp_compare = temperature[index];
  }
  
  return index;                       // index returned is the upper bound 
}

// interpolates between the resistance value at the given index and at index+1 and returns corresponding temperature
int NTC_LUT::resistance_interpolate(int index, int res){
  return map (res, resistance[index], resistance[index - 1], temperature[index], temperature[index - 1]);
}

// interpolates between the temperature value at the given index and at index+1 and returns corresponding resistance
int NTC_LUT::temp_interpolate(int index, int temp){
  // calculate lower index 
  int index_lower = index - 1;
  return map(temp, temperature[index_lower], temperature[index], resistance[index_lower], resistance[index]); 
}

int NTC_LUT::readADC(){
  // we invert the DAC value as the PID module needs pos feedback
  return (int)ADC_Res - analogRead(pin_ADC); 
}

