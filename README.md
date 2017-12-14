# diy-efis
![DIY-EFIS](/resources/LogoSq.png)

Experimental Aircraft Avionics based on CAN Aerospace

This repository is the public code and design store for the Kotuku Aerospace http://www.kotuku.aero contributed avionics project.

The goal of DIY-EFIS is to provide a reference implementation of an open-source avionics stack based on [Can-Aerospace](http://www.stockflightsystems.com/canaerospace.html)

The key display technology is based on a [Raspberry-PI](https://www.raspberrypi.org/) and a 3D printed case and a power supply with a PIC processor implementing the CanFly protocol

[Case](/resources/case.jpg][Stack](/resources/stack.jpg)

The simplest way to try the code is to build on Microsoft Windows.  This is not a supported platform for the actual device but will allow you to easily test the devices.

* If you don't have Microsoft Visual Studio 2017 download a free copy from [www.visualstudio.com](https://www.visualstudio.com/vs/cplusplus/)
* I use [TortoiseGit](https://tortoisegit.org/) to do the checkout of the DIY-EFIS repository.
* The shell used to test the applications is located at /tools/msh/msh.sln
* compile and run the code.  You will get a prompt: **muon>**  This indicates that the muon application is running.  The muon library is the command line interface used to develop the code to build an instrument.
* at the command type the following:

    mkdir proton
    string layout "test"
    uint16 screen-x 320
    uint16 screen-y 240
		mkdir test
		exit
		exit
		exit

* This will exit the shell.  The code does not support run-time reconfiguration. Run it again and now 2 windows should open with one showing the startup logo.  From this point the script for the layout and event handlers can be developed.  Checkout the [Facebook page](https://www.facebook.com/diyefis/) for a screen cast of what happens next


Facebook page 

https://www.facebook.com/diyefis/

We are working actively on RELEASE_2017 which is now the default branch of the repository.

The repository branch RELEASE_2016 holds the complete designs as of 2016.

** Raspberry PI **
Portable C code that implements a full graphical display for avionics that can run on very low power devices.  The development environment is Eclipse on Linux or Visual Studio on Windows

** Hardware Designs **
All diy-efis hardware devices are based on a common hardware family, the dsPIC33EP series from Microchip.  These devices are programmed in C and have the core I/O functions needed by the CANFly systems.  There is a common operating system and I/O drivers for most peripherals.  These reference designs are located under the library called reference and can be used to base your own hardware designs on.
All diy-efis hardware designs are open source and are provided in Eagle PCB format.  They are designed to be very low cost 2 sided PCB's that can be hand assembled by a enthusiest.  Kotuku Aerospace will sell boards, partial kits, full kits or assembled hardware based on these designs.

diy-efis is managed by Kotuku Aerospace and forms the basis of all hardware designs provided by them.  The Kotuku designs are all 32bit embedded solutions and embed the latest code.


