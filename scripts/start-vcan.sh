#!/bin/sh
sudo modprobe can
sudo modprobe can-raw
sudo modprobe vcan
sudo ip link add dev can0 type vcan
sudo ifconfig can0 up

