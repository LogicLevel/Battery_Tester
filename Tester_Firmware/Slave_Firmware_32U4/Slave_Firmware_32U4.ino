
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
#define SLAVE_ADDRESS 0x16

// master setable fields ( all setable payloads are 2 bytes max )
int discharge_current = 0;      // mA
int cutoff_voltage = 0;         // mV
int rest_time = 0;              // seconds
byte op_code = 0;               // first byte of any I2C frame sent by master
                                //   dictates data contense and operation

// read only fields of methods ( max readable payload is 4 bytes )
int battery_voltage = 0;        // mV
int charge_current = 0;         // mA
boolean EC_flag = 0;            // end of charge flag (set high at end of charge)
boolean ED_flag = 0;            // end of discharge flag (set high at end of discharge)
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


void setup() {
  // Start coulombic counter setup 
  pinMode(INT,INPUT);               // Interrupt input pin (must be D2 or D3)
  pinMode(POL,INPUT);               // Polarity input pin
  pinMode(CLR,INPUT);               // Unneeded, disabled by setting to input
  attachInterrupt(2,myISR,FALLING); // INT net is attached to interupt 2 
  // Emd coulombic counter setup

  // Setup for I2C comunications
  Wire.begin(SLAVE_ADDRESS); // fill in address 
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

}



void loop() {
  // put your main code here, to run repeatedly:

}


void requestEvent(){
  case (request_select) {
    
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
 *          - {0,0,0,0,1,0,0,1} reset() 
 *      
 */

void reset(){
  // resets charge_mAh, discharge_mAh, EC_flag, ED_flag, sets state to Idle
  
}

void receiveEvent(int bytesReceived){
  
}


void myISR() // Run automatically for falling edge on D3 (INT1)
{
  static boolean polarity;
  
  // Determine delay since last interrupt (for mA calculation)
  // Note that first interrupt will be incorrect (no previous time!)

  //lasttime = time;
  //time = micros();

  case (tester_state) 
  {
    case 0: // Charging 
      charge_mAh += ah_quanta;
    break;
    
    case 1: // Discharging
      discharge_mAh -= ah_quanta;
    break;
    
    default // Hold or Idle
    // do nothing and log no data
    break;
  }

  // Calculate mA from time delay (optional)
  //mA = 614.4/((time-lasttime)/1000000.0);
  
  isrflag = true; // set interupt flag for main loop
}

