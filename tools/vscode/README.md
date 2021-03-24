# canfly README

This is the VSCode extension for CanFly.   It registers helpers and installs binaries needed to develop CanFly applications

## Features

The CanFly VSCode extension adds the debugging adapapter for CanFly applications.  A CanFly device includes an embedded ECMA-334
interpreter that allows the development of Avionics applications using C#, VB or any CIL compatible language.

The CanFly interpreter is based on the nanoFramework but is implemented to run on smaller and dedicated hardware.  This package
includes the CanFly metadata processor that will create an application that can be uploaded to a CanFly device.


## Requirements

While not mandatory, the CanFly build system assumes VSCode can build an application as a series of DLL files.  We recommend installing
teh CMake extension and the C# extension.  All of the CanFly runtime source code is provided in C#


## Extension Settings

This extension contributes the following settings:

* `canfly.enable`: enable/disable this extension
* `canfly.screen-x`: set to the width of the display to simulate
* `canfly.screen-y`: set to the height of the display to simulate
* `canfly.num-spin`: number of spin-buttons to simulate 1, or 2
* `canfly.num-action`: number of action buttons to simulate, is num-spin + value max is 8
* `canfly.touch-enabled`: a touch screen is simulated, if the device is not a touch screen the mouse is used
* `canfly.com-adapter`: com port for a can bus interface


## Known Issues

None

## Release Notes

None

### 1.0.0

Initial release of the CanFly emulator

