# diy-efis
![DIY-EFIS](/resources/LogoSq.png)

Experimental Aircraft Avionics

This repository is the public code and design store for the Kotuku Aerospace http://www.kotuku.aero contributed avionics project.

The goal of DIY-EFIS is to provide a reference implementation of an open-source avionics stack.

The display technology is based on a [Raspberry-PI](https://www.raspberrypi.org/) and a 3D printed case and a power supply with a PIC processor implementing the CanFly protocol

The simplest way to try the code is to build on Microsoft Windows.  This is not a supported platform for the actual device but will allow you to easily test the devices.

* If you don't have Microsoft Visual Studio 2017 download a free copy from [www.visualstudio.com](https://www.visualstudio.com/vs/cplusplus/)
* I use [GitKraken](https://www.gitkraken.com/download) to do the checkout of the DIY-EFIS repository.
* Open the project file /win32_tools/diy-efis.sln
* Set the default project to pfd.

There are a few other repositories that may be useful if you want to process the map data:

### Landmass features

https://github.com/kotuku-aero/natural_earth

This is a copy of the necessary data from [Natural Earth Website](https://www.naturalearthdata.com/) The data is not always useful as it has too much detail for aircraft use.  See comments below about data plans.

### Terrain data

https://github.com/kotuku-aero/terrain

This is a copy of the GTOPO30 DEM data.  The gtopo data can be difficult to source and does not change rapidly.  See comments below about data plans.

### Contour data

https://github.com/kotuku-aero/contours

This is my processed contours.  These are created from the GTOPO30 DEM dataset.  This is done using QGIS

- Load the GTOPO DEM file
- use a gaussian filter to remove noise.  This smooths the lidar data
- generate contours at 1000ft intervals and save them

The shp2db tool is then used to convert the shape files into a CanFly atom database.  This has a 1 degree spatial index.  The clipper2 library the clips the contours to those (approx 14000)  1 degree squares.  The AtomDB then loads those clipped (closed) contours and renders them.

Makes for a very fast and light-weight rendering engine.  The 1000ft intervals are hypsometrically tinted

## Facebook page 

https://www.facebook.com/diyefis/

We are working actively on the yehudi branch.

## Raspberry PI

Portable C code that implements a full graphical display for avionics that can run on very low power devices.  The development environment is Eclipse on Linux or Visual Studio on Windows

## Data Plans

We will be working in this project on automating data conversion from the terrain, natural earth and map-data repositories.

While the general layers (terrain and natural earth) don't change very much, the airspace data does.  The later is every 3 months.  As a minimum we plan to process the airspace data for Australia/New Zealand and North America.  Others may want to add to that.

All of the applications in the dbtools part of the project are intended in the long term to be automated using github actions so when a new data set is checked-in the map overlays will be updated.

You can probably see that it will take some time, but one day you will be able to pull free data for the navigator every 3 months and use it.

## Disclaimer

diy-efis is managed by Kotuku Aerospace and forms the basis of all hardware designs provided by them.  The Kotuku designs are all 32bit embedded solutions and embed the latest code.  All of the
code provided here is provided as-is in the hope it may be useful.  Kotuku Aerospace takes no responsibility for the use or suitability of the code for any purpose.
