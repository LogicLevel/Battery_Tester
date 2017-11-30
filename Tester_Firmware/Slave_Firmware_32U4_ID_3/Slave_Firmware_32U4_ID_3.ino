#include <Wire.h>
#include "Thermal_Controller.h"
#include "Charge_Controller.h"
#include "Constant_Current_Controller.h"
#include "BT_Board_Presets.h"


// ----- Configuration Fields ----- //

// Error and Debug Field
boolean error_stat = 0; 
boolean serial_debug = 0;

#define ID 0
#define SLAVE_ADDRESS 8
#define VCC 4.88758553
#define VOLTAGE_OFFSET -40

// ----- End Configuration Field ----- //

//
// Program Setup 
//

// Constant Set 
/*
int     digipot_resistance, ntc_R2;
byte    slave_address;
double  dischage_current_slope; 
*/

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

//
// Coulomb Conter Fields
//
const double CURRENT_CALC_CONSTANT = 1228.8;
double ah_quanta = 0.34135518; // mAh for each INT 25mOhm sense
volatile boolean isrflag;
volatile long int time, lasttime;

//BT_Board_Presets presets_library(ID, &error_stat);

//
// ---------- TESTER FIELDS ---------- //
//

// Bus master setable Teser fields ( all setable payloads are 2 bytes max )
int charge_current = 500;           // mA
int charge_term_current = 50;       // mA
int charge_term_voltage = 4200;     // mv 
int cutoff_voltage = 3200;          // mV
long charge_rest_time = 10000;       // ms
long discharge_rest_time = 10000;    // ms
int discharge_current = 2000;       // mA
int cell_temp_set = 40;             // temperature in C 
byte op_code = 0;                   // first byte of any I2C frame sent by master

// Static Teser fields (for now) 
int NTC_R2 = 10000;                 // Second resistor in divider
int Temp_Check_Tollerance = 3;      // 

byte tx[2] = {0,0};                 // used to store data bytes to be transmited

// Status Field
int battery_voltage = 0;            // mV
int measured_cell_temp;
volatile double current; 
double current_non_vol;             // this is the non volitile int form of current (updated in compute

volatile double charge_mAh = 0;
volatile double discharge_mAh = 0;
int EC_flag = 0;                    // end of charge flag (set high at end of charge)
int ED_flag = 0;                    // end of discharge flag (set high at end of discharge)
int data_ready = 0;                  // goes high when a full cycle has been complete

// private fields
long finish_time = 0;               // ms
//int charge_term_current = 50;     // mA

//
// ---------- END TESTER FIELDS ---------- //
//

// State Variable
int state = 0; 

//
//
// End Setup
//
//

int print_counter = 0;
const int print_counter_interval = 2;

// Syntax: Thermal_Controller(int pNTC, int pPWM, int *r2, int *tSet, int *tMeas) 
Thermal_Controller THC(PIN_NTC, PIN_HEAT_CTL, &NTC_R2, &cell_temp_set, &measured_cell_temp);

Constant_Current_Controller CCC; 

Charge_Controller CC; 




void setup() {
  // Setup Coulomb Counter
  pinMode(PIN_INT, INPUT);
  pinMode(PIN_CLR, INPUT);
  pinMode(PIN_POL, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_INT), CoulombCount_ISR, FALLING);

  // Setup for I2C comunications
  Wire.begin(SLAVE_ADDRESS); // fill in address 
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  
  // Begin Serial for debugging
  Serial.begin(115200);

  // Syntax: setup(int _pin_PWM, int *I_discharge, int *v_cut, double *_current, int *voltage, int *stat)
  CCC.setup(PIN_DISCHARGE_CTL, &discharge_current, &cutoff_voltage, &current_non_vol, &battery_voltage, &ED_flag);
  
  // Syntax: setup(int CS, int UD, int CHRG_CTL,int *I_chrg, int *current, int *I_cut, boolean *stat)
  CC.setup(PIN_CS, PIN_UD, PIN_CHRG_CTL, &charge_current, &current_non_vol, &charge_term_current, &EC_flag);
  
  // load the correct values for this board
  //presets_library.loadCalibrationValues(&digipot_resistance, &ntc_R2, &slave_address, &dischage_current_slope);
  
  delay(1000);
  Serial.begin(115200);
  delay(100);
  //THC.set_temp(52);
  Serial.println("Setting Temp to 40C");
  delay(100);
  //THC.enable(1);
  Serial.println("Setting THC Enable Bit");
  Serial.println();

  Serial.println("Beginning Thermal Regulation"); 
  THC.enable(1);
  
  Serial.println("Beginning Cycle"); 
  Serial.println();

  run_cycle();
  
}

void loop() {
  //Serial.println("Entering Loop");
  stateMachine();
  //Serial.println("Running Statemachine");
  compute();
  //Serial.println("Running Compute");
  loop_print();
  //print_status();
  delay(500);
}

// 
// State Machine
// 

// State Definitions
#define S0 0
#define S1 1
#define S2 2
#define S3 3
#define S4 4
#define S5 5

void stateMachine() 
{ 
  //Serial.print("   Starting statemachine at state: ");
  //Serial.println(state);
  long temp = 0;
  int t_upper = cell_temp_set + Temp_Check_Tollerance;
  int t_lower = cell_temp_set - Temp_Check_Tollerance;
  
  switch(state) { 
    case S0 :
      // wait for run method to be called (do nothing)
    break; 

    case S1 :
      // setup discharge
      // Reset data ready 
      data_ready = 0; 

      // If charge hold time has not been completed
      temp = finish_time + charge_rest_time;
      if ( temp > millis() ) 
        temp = temp - millis(); 

      // only tranisition if the cell is up to temp 
      if ( (measured_cell_temp > t_lower) && (measured_cell_temp < t_upper) ){
        // Wait charge hold time
        delay(temp);
        CCC.enable();
        state = S2;
      }
    break; 

    case S2 :
      // Discharge
      if (ED_flag == 0){
        state = S3;
      }
    break; 
    
    case S3 : // Discharge Time Hold
      delay(discharge_rest_time);
      CC.enable();
      state = S4;
    break; 

    case S4 : // Charge
      if (EC_flag == 0){
        state = S5;
      }
    break; 

    case S5 :
      // System Teardown
      data_ready = 1; 
      finish_time = millis(); 
      state = S0;
    break; 
  }
  //Serial.println("   Ending Statemachine");
  
}


//
// I2C Coms 
//

void requestEvent(){
  
  int temp;
  byte tx_1, tx_2;
  /*
  switch (op_code) {
      case B00000000: // transmit discharge_current
        int_to_byte(discharge_current);
        Wire.write(tx, 2);
      break;

      case B00000001: // transmit cutoff_voltage
        int_to_byte(cutoff_voltage);
        Wire.write(tx, 2);
      break;

      case B00000010: // transmit rest_time
        int_to_byte((int)(rest_time / 1000));
        Wire.write(tx, 2);
      break;

      case B00000011: // transmit battery_voltage
        int_to_byte(battery_voltage);
        Wire.write(tx, 2);
      break;

      case B00000100: // transmit charge_current
        int_to_byte(charge_current);
        Wire.write(tx, 2);
      break;

      case B00000101: // transmit EC_flag
        int_to_byte(EC_flag);
        Wire.write(tx, 2);
      break;

      case B00000110: // transmit ED_flag
        int_to_byte(ED_flag);
        Wire.write(tx, 2);
      break;

      case B00000111: // transmit charge_mAh
        int_to_byte( (int)charge_mAh );
        Wire.write(tx, 2);
      break;

      case B00001000: // transmit discharge_mAh
        int_to_byte( (int)discharge_mAh );
        Wire.write(tx, 2);
      break;

      default: 
        // do nothing (Throw ERROR????)
      break; 
  }
  */
}

/* _________________ DataPacket Overview _________________
 * (___byte1___),(___byte2___),(___byte3___),(___byte4___)
 * (  Address  ),( Operation ),(  Payload  ),(  Payload  )
 * 
 * Address: slave address of this device
 * Operations: op_code table 
 *    {0,1,2,3,4,5,6,7}
 *      Bit 0: dictates a read (0) or write (1) operation will be performed
 *        - Write operations can only be performed on master setable fields:
 *          discharge_current, cutoff_voltage, rest_time
 *        - Read operations can be performed on all master aCoulombCountessible fields:
 *          discharge_current, cutoff_voltage, rest_time, battery_voltage, 
 *          charge_current, EC_flag, ED_flag, charge_mAh, discharge_mAh
 *          
 *      Bits {4,5,6,7}: dictate the field aCoulombCountessed by the operation
 *          - {x,0,0,0,0,0,0,0} discharge_current ( if write operation following 2 bits are value )
 *          - {x,0,0,0,0,0,0,1} cutoff_voltage    ( if write operation following 2 bits are value )
 *          - {x,0,0,0,0,0,1,0} rest_time         ( if write operation following 2 bits are value )
 *          - {0,0,0,0,0,0,1,1} battery_voltage
 *          - {0,0,0,0,0,1,0,0} charge_current
 *          - {0,0,0,0,0,1,0,1} EC_flag
 *          - {0,0,0,0,0,1,1,0} ED_flag
 *          - {0,0,0,0,0,1,1,1} charge_mAh
 *          - {0,0,0,0,1,0,0,0} discharge_mAh 
 *          - {0,0,0,0,1,0,0,1} reset()           ( calls reset() method )
 *          - {0,0,0,0,1,0,1,0} run_cycle()       ( calls run_cycle() method )
 *      
 */

void receiveEvent(int bytesReceived){
  op_code = Wire.read(); // op_code is stored to determine what data is to be sent 
  // Register the 2 bytes recieved and connect the two 8 bit values into a 16 bit int

  int int_recieved;
  if (Wire.available() == 2) {
    int_recieved = Wire.read();
    int_recieved = int_recieved << 8 | Wire.read();
  }
  /*
  // Only do anything for op_codes that require a value to be set or a method called
  switch (op_code){
    case B10000000: 
      //discharge_current = int_recieved; // write value to discharge_current
    break; 
       
    case B10000001:
      cutoff_voltage = int_recieved; // write value to cutoff_voltage
    break;
    
    case B10000010:
      rest_time = ( (long)int_recieved ) * 1000; // convet value to ms and write to rest_time 
    break;

    case B00001001:
      reset(); // call reset()
    break;

    case B00001010:
      run_cycle(); // call run_cycle()
    break;
      
    default:
      // do nothing
    break;     
    }
    */
}

void loop_print(){
  if (print_counter == print_counter_interval) {
    print_status();
    print_counter = 0; 
  } else {
    print_counter = print_counter + 1;
  }
}

void print_status(){
  Serial.print("State: ");
  Serial.print(state);
  switch (state) {
    case S0: Serial.println("  Waiting for run command"); 
    case S1: Serial.println("  Charge rest time");
    case S2: Serial.println("  Discharging");
    case S3: Serial.println("  Discharge rest time");
    case S4: Serial.println("  Charging");
    case S5: Serial.println("  Teardown");
  }

  Serial.print("Current: ");
  if (digitalRead(PIN_POL))
    Serial.print("-");
  Serial.println(current_non_vol);

  Serial.print("Battery Voltage: ");
  Serial.println(battery_voltage); 

  Serial.print("Charge mAh: ");
  Serial.println(charge_mAh); 

  Serial.print("Discharge mAh: ");
  Serial.println(discharge_mAh); 

  Serial.print("Set Temperature: ");
  Serial.println(cell_temp_set);
  
  Serial.print("Current Temperature: ");
  Serial.println(measured_cell_temp);
  
  Serial.print("Heater PWM: ");
  Serial.println(THC.PWM_value);
  
  Serial.println();
}

// resets charge_mAh, discharge_mAh, EC_flag, ED_flag, sets state to Idle
void reset(){
  Serial.println("reseting program"); 
  charge_mAh = 0;
  discharge_mAh = 0;
  EC_flag = 0;
  ED_flag = 0;
  state = 3;
}

// starts test by changing state to charge;
void run_cycle(){
  state = 1; 
}

// Converts input into its single byte components which are
//  stored in the tx[] for transmition to the bus master
void int_to_byte(int input) {
  tx[0] = input >> 8;
  tx[1] = input;
}

//
// Coulomb Counter Interupt Service Routime
//
void CoulombCount_ISR() { // Run automatically for falling edge on D3 (INT1)

  lasttime = time;
  time = micros();

  if (digitalRead(PIN_POL)) // high = charging
  {
    charge_mAh += ah_quanta;
  }
  else // low = discharging
  {
    discharge_mAh += ah_quanta;
  }

  // Calculate mA from time delay
  current = CURRENT_CALC_CONSTANT/((time-lasttime)/1000000.0);
  
  isrflag = true;
}

void read_battery_voltage(){
  // Take several measurement and average them
  int temp = 0;
  int i;
  for (i=0; i < 5; i++)
    temp = temp + analogRead(PIN_VBAT_READ); 
  temp = temp / 5.0; 
  // Calcualte battery voltage (takes into account VCC and offset voltage)
  battery_voltage = (int) (temp * VCC) + VOLTAGE_OFFSET;
}

void compute() {
  THC.compute(); 
  current_non_vol = current;
  read_battery_voltage(); 
  CC.compute();
  CCC.compute();
}

