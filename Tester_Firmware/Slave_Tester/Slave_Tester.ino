#include <Wire.h>

#define slave_address 8
#define op_code_list_length 11
#define op_code_read_length 9
// used to test all op_codes 
byte op_code_list[op_code_list_length] = {B00000000, B00000001, B00000010, B00000011, 
                                          B00000100, B00000101, B00000110, B00000111, 
                                          B00001000, B00001001, B00001010};

String op_code_names[ op_code_list_length ] = {"discharge_current", "cutoff_voltage", "rest_time", 
                                             "battery_voltage", "charge_current", "EC_flag", "ED_flag",
                                             "charge_mAh", "discharge_mAh", "reset()", "run_cycle()"}; 

int read_value_expected[ op_code_read_length ] = { 1100, 3200, 60, 3808, 900, 1, 0, 1234, 5678 }; 

byte tx[2] = {0,0};

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600); 
  
}

void loop() { 
  test_reading_values();

  Serial.print("\n\n\n\n");
  Serial.println("*------- VALUES SET ------- *");
  int new_discharge_current = 2500;
  int new_cutoff_voltage = 3000;
  int new_rest_time = 100;
  set_value(B10000000, new_discharge_current);
  set_value(B10000001, new_cutoff_voltage);
  set_value(B10000010, new_rest_time);
  read_value_expected[0] = new_discharge_current;
  read_value_expected[1] = new_cutoff_voltage;
  read_value_expected[2] = new_rest_time;

  test_reading_values();

  delay(100000);
}

/* ---------- Testing Methods ---------- */

void test_op_code_list(){
  for (int i=0; i < op_code_list_length; i++) {
    Wire.beginTransmission(slave_address); // transmit to device #8
    Wire.write(op_code_list[i]);        // sends five bytes
    Wire.endTransmission();    // stop transmitting
    delay(3000);
  }
}

void set_value(byte op_code, int value){
  Wire.beginTransmission(slave_address);
  Wire.write(op_code);
  int_to_byte(value);
  Wire.write(tx, 2);
  Wire.endTransmission();
}

void test_reading_values() {

  for (int i=0; i<op_code_read_length; i++)
  {
    Wire.beginTransmission(slave_address);  // transmit to slave device 
    Wire.write(op_code_list[i]);            // sends five bytes max
    Wire.endTransmission();                 // stop transmitting
  
    Wire.beginTransmission(slave_address);
    Wire.requestFrom(slave_address, 2); // requests data from slave
  
    if (Wire.available() == 2) {
      int receivedValue = Wire.read() << 8 | Wire.read();
      Serial.print("Reading ");
      Serial.print(op_code_names[i]); // print opcode name 
      Serial.print(":    ");
      Serial.println(receivedValue);
      Serial.print("Expected:      ");
      Serial.println(read_value_expected[i]);
      Serial.println();
    }
  }
}


void int_to_byte(int input) {
  tx[0] = input >> 8;
  tx[1] = input;
}

