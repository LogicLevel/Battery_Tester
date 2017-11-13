#include <Wire.h>
#include "LTC4150.h"

LTC4150 CoulombCounter(1, 1, 1, 1);
// LTC4150::LTC4150(int int_pin, int clr, int pol, int interupt);

const int vbat_read = 0; // battery voltage pin

//slave setup
#define SLAVE_ADDRESS 8

// master setable fields ( all setable payloads are 2 bytes max )
int discharge_current = 0;      // mA
int cutoff_voltage = 0;         // mV
long rest_time = 0;             // ms
byte op_code = 0;               // first byte of any I2C frame sent by master
                                //   dictates data contense and operation

byte tx[2] = {0,0};

// read only fields of methods ( max readable payload is 2 bytes )
int battery_voltage = 0;        // mV
int charge_current = 0;         // mA
int EC_flag = 0;                // end of charge flag (set high at end of charge)
int ED_flag = 0;                // end of discharge flag (set high at end of discharge)


// internal state variables 
byte tester_state = 0;
/* State:
 *  0: Charge
 *  1: Discharge
 *  2: Hold
 *  3: Idle
 */

#define S0 B00000000
#define S1 B00000001
#define S2 B00000010
#define S3 B00000100
#define S4 B00001000
#define S5 B00010000


void setup() {
  attachInterrupt(CoulombCounter.interupt_number, CC_ISR, FALLING);

  // Setup for I2C comunications
  Wire.begin(SLAVE_ADDRESS); // fill in address 
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);

  // *********** Setting Test Values for test case:  test_reading_values() ********** /
  discharge_current = 1100;   // mA
  cutoff_voltage = 3200;      // mV
  rest_time = 60000;          // seconds
  op_code = 0;                // first byte of any I2C frame sent by master
                              //   dictates data contense and operation
  battery_voltage = 3808;     // mV
  charge_current = 900;       // mA
}

void loop() {
  switch(tester_state) { 
    case S0 :

    break; 

    case S1 :

    break; 

    case S2 :

    break; 

    case S3 :

    break; 
  }
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
        int_to_byte( (int)CoulombCounter.charge_mAh );
        Wire.write(tx, 2);
      break;

      case B00001000: // transmit discharge_mAh
        int_to_byte( (int)CoulombCounter.discharge_mAh );
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
 *        - Read operations can be performed on all master accessible fields:
 *          discharge_current, cutoff_voltage, rest_time, battery_voltage, 
 *          charge_current, EC_flag, ED_flag, charge_mAh, discharge_mAh
 *          
 *      Bits {4,5,6,7}: dictate the field accessed by the operation
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
      discharge_current = int_recieved; // write value to discharge_current
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
  int temp = analogRead(vbat_read); 
  battery_voltage = temp * (5000.0 / 1023);
  
}

// resets charge_mAh, discharge_mAh, EC_flag, ED_flag, sets state to Idle
void reset(){
  Serial.println("reseting program"); 
  CoulombCounter.charge_mAh = 0;
  CoulombCounter.discharge_mAh = 0;
  EC_flag = 0;
  ED_flag = 0;
  tester_state = 3;
}

// starts test by changing state to charge;
void run_cycle(){
  tester_state = 1; 
}

void int_to_byte(int input) {
  tx[0] = input >> 8;
  tx[1] = input;
}



void CC_ISR() 
{
  CoulombCounter.myISR();
}


