The BLE Write protocol is as follows:

Starting with command of two characters. Following is payload with the size specific to the command.

All values below are in hexadecimal.

Motor Control Byte(MCB):
00 (max backward) to 7f (stop) to ff (max forward)

Command | Payload |
--- | --- | 
DD - Differential Drive  <br> -> control the motors |  B0: MCB of left motor  <br> B1: MCB of right motor <br> B2: Duration in ms| 
RO - ROtation <br> -> rotate the device |  B0: MCB of left motor  <br> B1: MCB of right motor <br> B2: Duration in ms <br> B3: Brake in ms| 
BK - BraKe  <br> -> brake the motors |  | 
OA - Obstacle Avoidance <br> -> turn it on or off | B0: 00 -> OFF ; 01 -> ON | 
VS - VerSion  <br> -> get FW version |  | 
ID - chip ID <br> -> get chip ID |  | 
