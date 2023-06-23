#!/bin/bash
# run setserial -a on the device to get the baud_base and change here
BAUDBASE=24000000
BAUDCUST=`expr $BAUDBASE / 250000`
sudo modprobe can
sudo modprobe can-raw
sudo modprobe slcan
sudo setserial $1 divisor $BAUDCUST
sudo setserial $1 spd_cust
sudo slcan_attach -o -s4 $1
sudo slcand -o -c -f -S38400 -s4 $1 $2
sudo ifconfig $2 up



