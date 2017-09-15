# diy-efis
Experimental Aircraft Avionics based on CAN Aerospace

This repository is the public code and design store for the Kotuku Aerospace http://www.kotuku.aero contributed avionics project.

New: Facebook page created:

https://www.facebook.com/diyefis/

We will be posting to this regularly as the 2017 build gets closer.  The code has been subsntially re-written and will be release when flight testing is completed.

The repository branch RELEASE_2016 holds the complete designs as of 2016.

** Raspberry PI **
Portable C code that implements a full graphical display for avionics that can run on very low power devices.  The development environment is Eclipse on Linux or Visual Studio on Windows

** Hardware Designs **
All diy-efis hardware devices are based on a common hardware family, the dsPIC33EP series from Microchip.  These devices are programmed in C and have the core I/O functions needed by the CANFly systems.  There is a common operating system and I/O drivers for most peripherals.  These reference designs are located under the library called reference and can be used to base your own hardware designs on.
All diy-efis hardware designs are open source and are provided in Eagle PCB format.  They are designed to be very low cost 2 sided PCB's that can be hand assembled by a enthusiest.  Kotuku Aerospace will sell boards, partial kits, full kits or assembled hardware based on these designs.

diy-efis is managed by Kotuku Aerospace and forms the basis of all hardware designs provided by them.  The Kotuku designs are all 32bit embedded solutions and embed the latest code.


