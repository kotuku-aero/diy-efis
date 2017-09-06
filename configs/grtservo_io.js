#include "ap_io.h"
#include <app_cfg.h>
#include <uart_device.h>
#include <microkernel.h>

static void process_line(uint8_t uart_number, char *buffer);

static void ap_roll_cmd(CanasInstance *inst, CanasParamCallbackArgs *args);
static void ap_pitch_cmd(CanasInstance *inst, CanasParamCallbackArgs *args);
static void ap_roll_engage_cmd(CanasInstance *inst, CanasParamCallbackArgs *args);
static void ap_roll_release_cmd(CanasInstance *inst, CanasParamCallbackArgs *args);
static void ap_pitch_engage_cmd(CanasInstance *inst, CanasParamCallbackArgs *args);
static void ap_pitch_release_cmd(CanasInstance *inst, CanasParamCallbackArgs *args);

static CanasParamSubscription ap_roll_param = { 0, id_ap_roll, ap_roll_cmd };
static CanasParamSubscription ap_pitch_param = { 0, id_ap_pitch, ap_pitch_cmd };
static CanasParamSubscription ap_roll_engage_param = { 0, id_ap_roll_engage, ap_roll_engage_cmd };
static CanasParamSubscription ap_roll_release_param = { 0, id_ap_roll_release, ap_roll_release_cmd };
static CanasParamSubscription ap_pitch_engage_param = { 0, id_ap_pitch_engage, ap_pitch_engage_cmd };
static CanasParamSubscription ap_pitch_release_param = { 0, id_ap_pitch_release, ap_pitch_release_cmd };

void ap_init()
  {
  void *uart;
  
  // create the left mag worker
  uart = init_uart(SELUART1, BAUD4800, process_line);
  // create a worker
  create_task(UART0_RX_READY, uart_worker, uart);
  
  // now we monitor the can bus for incoming messages
  canasParamSubscribe(&canas_instance, &ap_roll_param);
  canasParamSubscribe(&canas_instance, &ap_pitch_param);
  canasParamSubscribe(&canas_instance, &ap_roll_engage_param);
  canasParamSubscribe(&canas_instance, &ap_roll_release_param);
  canasParamSubscribe(&canas_instance, &ap_pitch_engage_param);
  canasParamSubscribe(&canas_instance, &ap_pitch_release_param);
  }

void process_line(uint8_t uart_number, char *buffer)
  {
  }
  
 /*
 This software controls the stepper motor driver. It performs the following functions:

Serial Command Structure:

A servo will not transmit any serial data unless it receives a command. Since all servos listen and transmit on the same bus
it is safe to transmit commands to a servo, and then read the response. The timing of the response shows it is possible for the
servo transmissions to collide (one servo to be transmitting before the previous servo is finished with its transmission).  To prevent this,
transmit a dummy byte as needed before between commands to servos if the servo commands could be transmitted in successive bytes 
without delay. (If the software that transmits commands to the servo has a delay or a mS or so between transmissions to each servo, 
this would not be necessary.)

Since the data transmitted back from the servo will may not be processed in such a way that the response can be matched to
a command, no effort was made to echo back servo move or torque setting commands. To assure the integrity of the communication
path, a bit is used to indicate if the move command checksum is invalid (it will stay invalid for a period of time after
each bad checksum). The engage/disengage status is also provided.

The servo will stay engaged (on) after it receives a "turn servo on" or a "move servo" command as long as it receives this
command, or a move servo command (including a move zero steps command), every 3000 mS max.

Servo Commands

Servo Header Command Byte - Must be sent for all commands

bits
  7 = Always 1. - Designates this as the header command. No other byte will have this bit set.
  65 -> Servo ID. 11 = Roll, 10 = Pitch, 01 = reserved, 00 = Trim Module
  4  -> 0 = servo move command. Always followed by a 1-byte "Servo Move Command" and checksum to be valid
  4 -> 1 = servo mode command (as follows with no other bytes transmitted...same as move zero steps)

               3210
             ------
             0 0000 - turn servo off (disengage)
             0 0001 - send software version
             0 0010 - send servo torque sensing - triggers servo to respond with "Servo Torque Sensed" byte
             0 0011 - reserved
             0 0111 - enter bootloader (only valid if the servo has never been engaged since power-up)
             0 1000 -
             0 1101 - set servo torque (followed by the Torque Set Byte) - response includes echo of torque setting
             0 1110 - turn servo on (ok to transmit this periodically). Same as move zero steps
             0 1111 - not used

 Torque Set Byte
   - must follow servo command header byte

          0000 xxxx - Set servo torque - xxxx = 0-15 levels of torque command. 15 = highest torque setting


 Servo Move Command - First byte transmitted after header byte
  bit 7 = 0
  bit 6-0 = msb-lsbits of number of commanded steps - 2's compliment

 Servo Move Command Checksum
  bit 7 = 0
  bit 6-0 = sum of "Servo Header Command Byte" + "Servo Move Command" with bit 7 overwritten to 0.


Servo Response (Serial Out Messages)

  "Servo Status" response. - Transmitted after every "Servo Header Command Byte"

  bits:
  7 Always 1 to identify this as the response header byte
  65 -> servo ID
  4 1 = unreliable serial communication (checksum failed in last message recieved)
  3 1 = sensed torque, sw version, or commanded torque is transmitted in next byte. Only sensed torque is followed by checksum.
        Upper bit is always 0 or next byte is always 0.
  2 1 = over-current sensed - hardware fault. Over 10% current with servo off 150% of max current.
  1 1 = hardware fault warning on controller board - Servo Not able be be disenganged due to hardware fault!!
  0 1 = discrete input open (engage/disengage discrete)

 if bits 4-0 are all set, this indicates the the bootloader responded and we are in the bootloader.

 When bit 3 is set:

 "Servo Torque Sensed"

 This byte is sent following the "Servo Status" byte and should be considered valid
 only if the previous byte has bit 3 set, and the checksum byte that follows is correct. If this
 data is to be used to control a pitch trim servo, an algorthm might be incorporated to detect
 unreasonable data, such as data that changes too rapidly, or limiting the amount of time the pitch
 trim servo can move.

 This byte indicates the amount of torque sensed by the pitch
 servo. For the roll servo, this indicates the status of the control stick disengage button.

 servo_torque_hi_byte:

  7 -> always 0
  6 -> 1 = servo engaged
  5 -> 1 = overheat sensed by H-Bridge

  4,3  ->  00 = sensed torque
           01 = SW version followed by motor version
           10 = commanded torque
           11 = reserved

 if bits 4,3 are 0,0 (Sensed Torque) then this folowing is sent:

  2-0 - torque sensed - highest bits

  It is followed by

  servo_torque_low_byte:

  7 -> always 0
  6-0  lowest bits of sensed torque.

  The response is always followed with a checksum with msbit set to zero.


if bits 4,3 are 0,1 (SW and Motor Version) then this folowing is sent:
  2-0 - motor type

  000 = standard torque
  001 = high torque

  It is followed by:

  SW Version Low Byte

  bit 7 -> always 0
  bits 6-0 -> software version

  The response is always followed with a checksum with msbit set to zero.

if bits 4,3 are 1,0 (Commanded Torque) then this folowing is sent:
  2-0 - unused

  It is followed by:


  bit 7 -> always 0
  bits 6-0 -> commanded torque

  The response is NOT followed with a checksum.

if bits 4,3 are 1,1 (Filtered PWM Command to Stepper Motor) then this folowing is sent:

  2-0 - unused

  It is followed by:

  bit 7 -> always 0
  bits 6-0 -> filtered pulse-width modulation command (valid only when servo is engaged).

  The response is always followed with a checksum with msbit set to zero.

  Motor Current is proportional to:
  const unsigned int8 torque[16] = {22,26,32,39,46,53,60,66,72,78,84,90,94,99,103,106}; // 0.2-1.4 amp/phase

  such that Motor Current = 3.0 Amps * torque[x]/255

The maximum torque setting must be controlled to limit the motor case temperature to an absolute maximum of 200 deg F,
with 180 deg F is recommended at a practical upper limit.

This controller is responsible for adjusting the PWM output duty cycle to maintain the desired
current to the stepper motor. This is accomplished by sampling the motor current at 5kHz, and adjusting
the PWM output using the control law described within.
*/
  
void ap_roll_cmd(CanasInstance *inst, CanasParamCallbackArgs *args)
  {
  uint8_t cmd[3];
  
  cmd[0] = 0b11000000;
  cmd[1] = args->message.data.container.UCHAR & 0x7f;
  cmd[2] = (cmd[1] + cmd[0]) & 0x7f;
  
  send_bytes(SELUART1, cmd, 3);    
  }

void ap_pitch_cmd(CanasInstance *inst, CanasParamCallbackArgs *args)
  {
  uint8_t cmd[3];
  
  cmd[0] = 0b10100000;
  cmd[1] = args->message.data.container.UCHAR & 0x7f;
  cmd[2] = (cmd[1] + cmd[0]) & 0x7f;
  
  send_bytes(SELUART1, cmd, 3);    
  }

void ap_roll_engage_cmd(CanasInstance *inst, CanasParamCallbackArgs *args)
  {
  uint8_t cmd = 0b11010000;
  
  send_bytes(SELUART1, &cmd, 1);    
  }
  
void ap_roll_release_cmd(CanasInstance *inst, CanasParamCallbackArgs *args)
  {
  uint8_t cmd = 0b11011110;
  
  send_bytes(SELUART1, &cmd, 1);    
  }
  
void ap_pitch_engage_cmd(CanasInstance *inst, CanasParamCallbackArgs *args)
  {
  uint8_t cmd = 0b10110000;
  
  send_bytes(SELUART1, &cmd, 1);    
  }
  
void ap_pitch_release_cmd(CanasInstance *inst, CanasParamCallbackArgs *args)
  {
  uint8_t cmd = 0b10111110;
  
  send_bytes(SELUART1, &cmd, 1);    
  }
