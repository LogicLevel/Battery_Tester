# Slave_Tester 

# I2C Protocol DataPacket Overview
| (___byte1___) | (___byte2___) | (___byte3___) | (___byte4___) |
| :-----------: | :-----------: | :-----------: | :-----------: |
| (  Address  ) | ( Operation ) | (  Payload  ) | (  Payload  ) |

Address: slave address of this device
Operations: op_code table 
  {0,1,2,3,4,5,6,7} 
      Bit 0: dictates a read (0) or write (1) operation will be performed
        - Write operations can only be performed on master settable fields:
          discharge_current, cutoff_voltage, rest_time
        - Read operations can be performed on all master accessible fields:
          discharge_current, cutoff_voltage, rest_time, battery_voltage,
          charge_current, EC_flag, ED_flag, charge_mAh, discharge_mAh
          
      Bits {4,5,6,7}: dictate the field accessed by the operation
| OP_CODE           | Name              | Description                                       |
| :---------------: | :---------------: | :-----------------------------------------------: |
| {x,0,0,0,0,0,0,0} | discharge_current | ( if write operation following 2 bits are value ) |
| {x,0,0,0,0,0,0,1} | cutoff_voltage    | ( if write operation following 2 bits are value ) |
| {x,0,0,0,0,0,1,0} | rest_time         | ( if write operation following 2 bits are value ) |
| {0,0,0,0,0,0,1,1} | battery_voltage   |                                                   |
| {0,0,0,0,0,1,0,0} | charge_current    |                                                   |
| {0,0,0,0,0,1,0,1} | EC_flag           |                                                   |
| {0,0,0,0,0,1,1,0} | ED_flag           |                                                   |
| {0,0,0,0,0,1,1,1} | charge_mAh        |                                                   |
| {0,0,0,0,1,0,0,0} | discharge_mAh     |                                                   |
| {0,0,0,0,1,0,0,1} | reset()           | ( calls reset() method )                          |
| {0,0,0,0,1,0,1,0} | run_cycle()       | ( calls run_cycle() method )                      |
