#ifndef BT_Board_Presets_h
#define BT_Board_Presets_h
#include <Arduino.h>

class BT_Board_Presets {
  
  public:
    // Constructor - takes pointer inputs for control variales, so they are updated automatically
    BT_Board_Presets(int id, boolean *_error);
  
    // Methods
    void loadCalibrationValues(int *digipot_resistance, int *ntc_R2, byte *slave_address, double *dischage_current_slope); 
    
  private:
    // Methods

    // Device Specific Fields 
    int device_ID = 0;
    boolean serial_enable = 0; 
    boolean *error;

    //
    // BOARD CALIBRATION FIELDS 
    //
    #define MAX_ID 5
    //const boolean SUPPORTED_BOARD_ID[4]         = {0,0,0,1}; // a 1 indicates that the board ID at that index is supported
    const int DEVICE_ID[MAX_ID]                   = {0, 1, 2, 3, 4};
    const int DIGIPOT_RESISTANCE[MAX_ID]          = {10000, 10000, 10000, 10000, 10000};
    const int NTC_R2[MAX_ID]                      = {10000, 10000, 10000, 10000, 10000};
    const double DISCHARGE_CURRENT_SLOPE[MAX_ID]  = {16.33987, 16.33987, 16.33987, 16.33987, 16.33987};

    // Fields
    

}; //class BT_Board_Presets

#endif
