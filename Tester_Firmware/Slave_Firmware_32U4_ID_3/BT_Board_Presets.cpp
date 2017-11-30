#include "BT_Board_Presets.h"
#include "Arduino.h"


// Create a presets library with the given device ID
BT_Board_Presets::BT_Board_Presets(int id, boolean *_error)
{
  device_ID = id;
  error = _error; 

  /*
  // check if the board is not supported
  if (SUPPORTED_BOARD_ID[device_ID] == 0) {
    // Board not supported (trigger error) 
    *error = 1; 
    // Print error
    if (Serial1) 
      Serial.println("[ WARN ] Board Not Supported \n");
  } else {
    // Print Pass
    if (Serial1) 
      Serial.println("[  OK  ] Board Supported \n");
  }
  */
}


void BT_Board_Presets::loadCalibrationValues(int *digipot_resistance, int *ntc_R2, byte *slave_address, double *dischage_current_slope)
{ 
  // Don't load presets if error has occured
  if (*error){
    if (Serial1) 
      Serial.println("[ WARN ] Loading Presets Failed \n");
  } else {
    // Check that presets exist board
    if (device_ID < MAX_ID) {
      
      // load presets
      *digipot_resistance = DIGIPOT_RESISTANCE[device_ID];
      *ntc_R2 = NTC_R2[device_ID];
      *slave_address = device_ID + 1;
      *dischage_current_slope = DISCHARGE_CURRENT_SLOPE[device_ID];

      if (Serial1) 
        Serial.println("[  OK  ] Presets Loaded \n");
        
    } else {
      if (Serial1) 
        Serial.println("[ WARN ] No presets exist \n");
    }
  }
   
}


