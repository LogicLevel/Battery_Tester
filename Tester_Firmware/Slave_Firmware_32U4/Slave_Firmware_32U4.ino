#include <Wire.h>
/***** coulombic counter *****/ 
#define INT 0 // On 328 Arduinos, only pins 2 and 3 support interrupts
#define POL 4 // Polarity signal
#define CLR 1 // Unneeded in this sketch, set to input (hi-Z)

// Global variables ("volatile" means the interrupt can
// change them behind the scenes):

volatile boolean isrflag;
volatile long int time, lasttime;
double ah_quanta = 0.17067759; // mAh for each INT

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
volatile double charge_mAh;     // mAh of charge
volatile double discharge_mAh;  // mAh of discharge

// internal state variables 
int tester_state = 0;
/* State:
 *  0: Charge
 *  1: Discharge
 *  2: Hold
 *  3: Idle
 */

// used for debuging 
byte op_code_last = 0;             

void setup() {
  // Start coulombic counter setup 
  pinMode(INT,INPUT);               // Interrupt input pin (must be D2 or D3)
  pinMode(POL,INPUT);               // Polarity input pin
  pinMode(CLR,INPUT);               // Unneeded, disabled by setting to input
  //attachInterrupt(2,myISR,FALLING); // INT net is attached to interupt 2 
  // Emd coulombic counter setup

  // Setup for I2C comunications
  Wire.begin(SLAVE_ADDRESS); // fill in address 
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);

  // Setting Test Values for test case:  test_reading_values()
  discharge_current = 1100;   // mA
  cutoff_voltage = 3200;      // mV
  rest_time = 60000;          // seconds
  op_code = 0;                // first byte of any I2C frame sent by master
                              //   dictates data contense and operation
  battery_voltage = 3808;     // mV
  charge_current = 900;       // mA
  EC_flag = 1;                // end of charge flag (set high at end of charge)
  ED_flag = 0;                // end of discharge flag (set high at end of discharge)
  charge_mAh = 1234;          // mAh of charge
  discharge_mAh = 5678;       // mAh of discharge
}

void loop() {
  if ( op_code != op_code_last )
  {
    Serial.print("Opcode: ");
    Serial.println(op_code);
    op_code_last = op_code;
  }
  delay(1);
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
 *      Bit 0: dictates a read (0) or write (1) opperation will be perfomed
 *        - Write operations can only be performed on master setable fields:
 *          discharge_current, cutoff_voltage, rest_time
 *        - Read opperations can be performed on all master accessible fields:
 *          discharge_current, cutoff_voltage, rest_time, battery_voltage, 
 *          charge_current, EC_flag, ED_flag, charge_mAh, discharge_mAh
 *          
 *      Bits {4,5,6,7}: dictate the field accessed by the opperation
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

// resets charge_mAh, discharge_mAh, EC_flag, ED_flag, sets state to Idle
void reset(){
  Serial.println("reseting program"); 
  charge_mAh = 0;
  discharge_mAh = 0;
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

/*
void myISR() // Run automatically for falling edge on D3 (INT1)
{
  // Determine delay since last interrupt (for mA calculation)
  // Note that first interrupt will be incorrect (no previous time!)

  //lasttime = time;
  //time = micros();

  // only logs data while charging or discharging
  switch (tester_state) 
  {
    case 0: // Charging 
      charge_mAh += ah_quanta;
    break;
    
    case 1: // Discharging
      discharge_mAh -= ah_quanta;
    break;
    
    default: // Hold or Idle
    // do nothing and log no data
    break;
  }

  // Calculate mA from time delay (optional)
  //mA = 614.4/((time-lasttime)/1000000.0);
  
  isrflag = true; // set interupt flag for main loop
}
*/

