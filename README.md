# diy-efis
![DIY-EFIS](/resources/LogoSq.png)

Experimental Aircraft Avionics based on CAN Aerospace

This repository is the public code and design store for the Kotuku Aerospace http://www.kotuku.aero contributed avionics project.

The Oshkosh 2019 presentation is available [Here](/presentations/Oshkosh2019.pdf)

The goal of DIY-EFIS is to provide a reference implementation of an open-source avionics stack based on [Can-Aerospace](http://www.stockflightsystems.com/canaerospace.html)

The key display technology is based on a [Raspberry-PI](https://www.raspberrypi.org/) and a 3D printed case and a power supply with a PIC processor implementing the CanFly protocol.

This is the 2021 version which has changed dramatically from the code presented in 2019.  Breaking changes are:

* The 'C' code base for the EFIS has all been converted to C#.  Still work to be done, but the basics are there.
* The designs and operating system code is removed as they were not really being used or maintained.
* Kotuku is having low cost displays and instruments made that will run a C# interpreter similar to the [nanoFramework](https://www.nanoframework.net/).
* The diy-efis uses different guiding prinicpals to the nanoFramework and so the tools are cloned and modified in this repository.
* There is documentation on porting diy-efis to other .net frameworks, although Kotuku won't be doing this.
* VSCode is the editor and development environment for diy-efis.
* Kotuku will be releasing 2 free images for the Raspberry Pi, one for the Adafruit 3.5" display, the other for the official 7" display.
* The code for building CanFly app's is here and is free.

The simplest way to try the code is to build on Microsoft Windows.  This is not a supported platform for the actual device but will allow you to easily test the devices.

* If you don't have Visual Studio Code you will need to download it.
* If you don't have Visual Studio 2019 installed you can install the [Roslyn Compiler](https://www.microsoft.com/en-US/download/details.aspx?id=48159) which is required.
* I use [TortoiseGit](https://tortoisegit.org/) to do the checkout of the DIY-EFIS repository.
* Run VSCode and open the root folder of {path to the diy-efis project}/framework
* We use a Makefile for building, you need to have the makefile build extension installed.
* In the <path to the clone>/rolls/vscode there is a debugger extension called canfly-0.0.1.vsix that will add the debugger extension for diy-efis to vscode.
* Build diy-efis.  This creates an application called diy-efis.app in the <path to the diy-efis project>/emulator/fs directory
* There is also a provided config database called diy-efis.cdb that has configs for a pfd and a basic edu.

At the present time, when debugging within vscode the configuration console is not displayed.  To enable this you need to launch the emulator launched from vscode from the home directory.  This is at {Your home directory}\.vscode\extensions\kotuku.canfly-0.0.1\out which is where the integrated debugger sets the home directory to.

Change to that directory within a command prompt and type msg -x 320 -y 240 -f <path to the diy-efis project>\framework\emulator\fs diy-efis.cdb
  
Replace the {path to the diy-efis project} and {Your home directory} with the correct paths for your system.
  
The next release of the emulator will use the embedded console in VSCode to expose the configuration tool and this won't be needed.

We are writing a couple of howto pages.

* [Porting DiyEfis] describes how to port diy-efis to other .net applications.
* [Configuration of DiyEfis] describes the shell and how to make the layout engine work for you.
* [Creating a new .app] describes how to fork the repository and add your own widgets to the project.
* [Vision for DiyEfis] explains what this repository really is and how the open source project works.

Facebook page 

https://www.facebook.com/diyefis/


diy-efis is managed by Kotuku Aerospace and forms the basis of all hardware designs provided by them.  The Kotuku designs are all 32bit embedded solutions and embed the latest code.


