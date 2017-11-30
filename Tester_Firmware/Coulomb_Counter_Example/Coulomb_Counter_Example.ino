
// CLR and SHDN.)
#define INT 7 // On 328 Arduinos, only pins 2 and 3 support interrupts
#define POL 9 // Polarity signal
#define CLR 8 // Unneeded in this sketch, set to input (hi-Z)
#define PIN_DISCHARGE_CTL 6 

// Change the following two lines to match your battery
// and its initial state-of-charge:

// Global variables ("volatile" means the interrupt can
// change them behind the scenes):

volatile boolean isrflag;
volatile long int time, lasttime;
volatile double mA;
double ah_quanta = 0.17067759; // mAh for each INT
double percent_quanta; // calculate below

void setup()
{
  pinMode(INT,INPUT); // Interrupt input pin (must be D2 or D3)

  pinMode(POL,INPUT); // Polarity input pin

  pinMode(CLR,INPUT); // Unneeded, disabled by setting to input

  pinMode(PIN_DISCHARGE_CTL, OUTPUT);

  analogWrite(PIN_DISCHARGE_CTL, 20); 

  // Enable serial output:

  Serial.begin(115200);
  Serial.println("LTC4150 Coulomb Counter BOB interrupt example");

  // One INT is this many percent of battery capacity:
  
  //percent_quanta = 1.0/(battery_mAh/1000.0*5859.0/100.0);

  // Enable active-low interrupts on D3 (INT1) to function myISR().
  // On 328 Arduinos, you may also use D2 (INT0), change '1' to '0'. 

  isrflag = false;
  attachInterrupt(4,myISR,FALLING);
}

void loop()
{ 
  analogWrite(6, 10);
  static int n = 0;

  // When we detect an INT signal, the myISR() function
  // will automatically run. myISR() sets isrflag to TRUE
  // so that we know that something happened.

  if (isrflag)
  {
    // Reset the flag to false so we only do this once per INT
    
    isrflag = false;

    // Print out current status (variables set by myISR())

    Serial.print("mAh: ");
    Serial.print(mA);
    //Serial.print(" soc: ");
    //Serial.print(battery_percent);
    Serial.print("% time: ");
    Serial.print((time-lasttime)/1000000.0);
    Serial.print("s mA: ");
    Serial.println(mA);
  }

  // You can run your own code in the main loop()
  // myISR() will automatically update information
  // as it needs to, and set isrflag to let you know
  // that something has changed.
}

void myISR() // Run automatically for falling edge on D3 (INT1)
{
  lasttime = time;
  time = micros();

  if (digitalRead(POL)) // high = charging
  {
    mA += ah_quanta;
  }
  else // low = discharging
  {
    mA -= ah_quanta;
  }

  // Calculate mA from time delay (optional)
  mA = 614.4/((time-lasttime)/1000000.0);
  
  isrflag = true;
}
