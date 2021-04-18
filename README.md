# diy-efis
![DIY-EFIS](/resources/LogoSq.png)

Experimental Aircraft Avionics based on CAN Aerospace

This repository is the public code and design store for the Kotuku Aerospace http://www.kotuku.aero contributed avionics project.

The Oshkosh 2019 presentation is available [Here](/presentations/Oshkosh2019.pdf)

The goal of DIY-EFIS is to provide a reference implementation of an open-source avionics stack based on [Can-Aerospace](http://www.stockflightsystems.com/canaerospace.html)

The key display technology is based on a [Raspberry-PI](https://www.raspberrypi.org/) and a 3D printed case and a power supply with a PIC processor implementing the CanFly protocol

The simplest way to try the code is to build on Microsoft Windows.  This is not a supported platform for the actual device but will allow you to easily test the devices.

* If you don't have Visual Studio Code you will need to download it.
* I use [TortoiseGit](https://tortoisegit.org/) to do the checkout of the DIY-EFIS repository.  Remember to select the RELEASE_2021 branch after checking out.
* The shell used to test the applications is located at /framework/assemblies/msh.exe this is a win32 binary and is the same code that is embedded into Kotuku prducts
* Run VSCode and open the root folder of <path to the clone>/framework
* We use a Makefile for building, you need to have the makefile build extension installed.
* In the <path to the clone>/framework there is a debugger extension called diyefis.vsix that will add the debugger extension for diy-efis to vscode.
* Build dit-efis.  This creates an application called diy-efis.app in the <path to clone>/emulator/fs directory
* There is also a provided confi database called diy-efis.reg that has configs for a pfd and a basic edu.

We are writing a couple of howto pages.

* [Porting DiyEfis] describes how to port diy-efis to other .net applications.
* [Configuration of DiyEfis] describes the shell and how to make the layout engine work for you.
* [Creating a new .app] describes how to fork the repository and add your own widgets to the project.
* [Vision for DiyEfis] explains what this repository really is and how the open source project works.

Facebook page 

https://www.facebook.com/diyefis/

We are working actively on ALPHA_1 which is now the default branch of the repository.

The repository branch RELEASE_2016 holds the complete designs as of 2016.

** Hardware Designs **
All diy-efis hardware devices are based on a common hardware family, the dsPIC33EP series from Microchip.  These devices are programmed in C and have the core I/O functions needed by the CANFly systems.  There is a common operating system and I/O drivers for most peripherals.  These reference designs are located under the library called reference and can be used to base your own hardware designs on.
All diy-efis hardware designs are open source and are provided in Eagle PCB format.  They are designed to be very low cost 2 sided PCB's that can be hand assembled by a enthusiest.  Kotuku Aerospace will sell boards, partial kits, full kits or assembled hardware based on these designs.

diy-efis is managed by Kotuku Aerospace and forms the basis of all hardware designs provided by them.  The Kotuku designs are all 32bit embedded solutions and embed the latest code.


