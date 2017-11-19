const int pin_battery = 0; 
const int heater_ctl = 5; 
const int pin_ntc = 1;
const int ntc_R2 = 10000;           // Second resistor in divider
const int discharge_ctl = 6; 
const int charge_ctl = 4; 

int battery_voltage = 0;        // mV

void setup() {
  pinMode(discharge_ctl, OUTPUT);
  pinMode(charge_ctl, OUTPUT);
  pinMode(heater_ctl, OUTPUT);
  digitalWrite(heater_ctl, LOW); 
  digitalWrite(discharge_ctl, LOW); 
  digitalWrite(charge_ctl, LOW); 
  Serial.begin(115200);
  digitalWrite(charge_ctl, HIGH);
}

void loop() {
  read_battery_voltage();
  Serial.print("Battery Voltage: ");
  Serial.println(battery_voltage);
  delay(1000);
}

void read_battery_voltage(){
  int temp = analogRead(pin_battery); 
  battery_voltage = (int) (temp * (4.88758553));
}

