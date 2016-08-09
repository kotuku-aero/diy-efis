#!/bin/bash
sudo modprobe can
sudo modprobe can-raw
sudo modprobe slcan
sudo slcan_attach -o -s4 $1
sudo slcand -o -c -f -S57600 -s4 $1 $2
sudo ifconfig $2 up



