#include <Wire.h>
#include "NTC_LUT.h"
//#include "LTC4150.h"

//LTC4150 CoulombCount;
//LTC4150(int_pin, clr, pol, interupt);
//BatteryManagement(int vbat_pin, int vref_pin, int ref_voltage, int pin, int clr, int pol, int interupt


const int pin_int = 3; // change back to 7 for pro micro 
const int pin_clr = 8;
const int pin_pol = 9;
const int heater_ctl = 5; 
const int pin_battery = 0;
const int pin_ntc = 1;

//private constatnts
int discharge_current = 0;      // mA
const int ntc_resistance = 10000; 
const int ntc_R2 = 10000;           // Second resistor in divider

volatile double current; 

double ah_quanta = 0.34135518; // mAh for each INT 50mOhm sense
volatile boolean isrflag;
volatile long int time, lasttime;

volatile double charge_mAh = 0;
volatile double discharge_mAh = 0;

//slave setup
#define SLAVE_ADDRESS 8

// master setable fields ( all setable payloads are 2 bytes max )
int charge_term_voltage = 4100; // mv 
int cutoff_voltage = 3200;      // mV
long rest_time = 0;             // ms
int cell_temp = 20;             // temperature in C 
byte op_code = 0;               // first byte of any I2C frame sent by master
                                //   dictates data contense and operation

byte tx[2] = {0,0};             // used to store data bytes to be transmited

// read only fields of methods ( max readable payload is 2 bytes )
int battery_voltage = 0;        // mV
int charge_current = 0;         // mA
int EC_flag = 0;                // end of charge flag (set high at end of charge)
int ED_flag = 0;                // end of discharge flag (set high at end of discharge)

// private fields
long finish_time = 0;           // ms
int charge_term_current = 50;   // mA


// internal state variables 
byte state = 0;
/* State:
 *  0: Charge
 *  1: Discharge
 *  2: Hold
 *  3: Idle
 */

#define S0 0
#define S1 1
#define S2 2
#define S3 3
#define S4 4
#define S5 5
#define S6 6
#define S7 7


void setup() {
  pinMode(pin_int, INPUT);
  pinMode(pin_clr, INPUT);
  pinMode(pin_pol, INPUT);
  pinMode(heater_ctl, OUTPUT);
  
  attachInterrupt(1, CoulombCount_ISR, FALLING);

  // Setup for I2C comunications
  Wire.begin(SLAVE_ADDRESS); // fill in address 
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  Serial.begin(115200);

  pinMode(6, OUTPUT);

  // *********** Setting Test Values for test case:  test_reading_values() ********** /
  discharge_current = 1100;   // mA
  cutoff_voltage = 3200;      // mV
  rest_time = 60000;          // seconds
  op_code = 0;                // first byte of any I2C frame sent by master
                              //   dictates data contense and operation
  battery_voltage = 3808;     // mV
  charge_current = 900;       // mA
  delay(1000);
  Serial.println("Setup Complete");
  Serial.println("Waiting 10 seconds");
  delay(10000);
  Serial.println("Starting Now");
  Serial.println();
  read_battery_voltage();
  //int heater_val = 250;
  while (battery_voltage > cutoff_voltage)
  {
     
     analogWrite(heater_ctl, 255);
     printOut(); 
     read_battery_voltage();
     analogWrite(6, 80);
     delay(1000); 
     /*heater_val = heater_val - 10;
     if (heater_val < 0)
        heater_val = 250;
        */
     
  }
  analogWrite(6, 0);
  Serial.println();
  Serial.println("Finished!!!");
}

void loop() {
  switch(state) { 
    case S0 :
      // wait for run method to be called
    break; 

    case S1 : // Charge Time Hold
      if ( (finish_time + rest_time) < millis() ) {
        state = S2; 
      }
    break; 

    case S2 :
      // setup discharge
      state = S3;
    break; 

    case S3 :
      // Discharge
      if (battery_voltage < cutoff_voltage) {
        state = S4;
        EC_flag = 1;
      }
    break; 
    
    case S4 : // Discharge Time Hold
    
      delay(rest_time);
      state = S5; 
      
    break; 
    
    case S5 :
      // setup Charge
      state = S6;
    break; 

    case S6 : // Charge
    
      if ( (current < charge_term_current) && (battery_voltage < charge_term_voltage) ){ // if fully charged 
        state = S7; 
        EC_flag = 1;
      }
      
      
    break; 

    case S7 :
      // System Teardown

      finish_time = millis(); 
      state = S0;
    break; 
  }

  //update variables
  read_battery_voltage();
  // update current and voltage
}

void printOut(){
  Serial.println();
  Serial.print("Battery Voltage: "); 
  Serial.println(battery_voltage);

  Serial.print("Charge mAh: "); 
  Serial.println(charge_mAh);

  Serial.print("Discharge mAh: "); 
  Serial.println(discharge_mAh);

  Serial.print("Current: "); 
  Serial.println(current);
  
}

/* ---------- Helper Methods ---------- */ 

void requestEvent(){
  
  int temp;
  byte tx_1, tx_2;
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
}

void read_battery_voltage(){
  int temp = analogRead(pin_battery); 
  battery_voltage = (int) (temp * (4.88758553));
}

int get_temperature(){
  int temp = analogRead(pin_ntc); 
  
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

void int_to_byte(int input) {
  tx[0] = input >> 8;
  tx[1] = input;
}

void CoulombCount_ISR() { // Run automatically for falling edge on D3 (INT1)

  lasttime = time;
  time = micros();

  if (digitalRead(pin_pol)) // high = charging
  {
    charge_mAh += ah_quanta;
  }
  else // low = discharging
  {
    discharge_mAh += ah_quanta;
  }

  // Calculate mA from time delay (optional)
  // current = 614.4/((time-lasttime)/1000000.0);
  current = 1228.8/((time-lasttime)/1000000.0);
  
  isrflag = true;
}

void setup_fast_PWM(){


  
}





