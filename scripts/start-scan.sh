#!/bin/sh
setserial /dev/$1 baud_base 961200 divisor 15 spd_cust
slcan_attach -o -s4 /dev/$1
slcand -o -c -f -S38400 -s4 /dev/$1 $2
ifconfig $2 up




