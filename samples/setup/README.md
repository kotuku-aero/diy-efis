# kMFD10 - 10" Multi-Function Display

The kMFD10 is the primary glass cockpit display for the Kotuku avionics ecosystem, providing a comprehensive flight instrument interface on a high-resolution 10-inch touchscreen display.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Building the Project](#building-the-project)
  - [Prerequisites](#prerequisites)
  - [Windows Development Build](#windows-development-build)
  - [PIC32 Embedded Build](#pic32-embedded-build)
- [CLion IDE Setup](#clion-ide-setup)
- [Project Structure](#project-structure)
- [Configuration](#configuration)
- [Programming the Device](#programming-the-device)
- [Debugging](#debugging)
- [Troubleshooting](#troubleshooting)

## Overview

The kMFD10 serves as the primary flight display, integrating:
- Primary Flight Display (PFD) functions
- Navigation display
- Engine monitoring
- System status
- Configuration interface
- Touch-based user interaction

The display communicates with other Kotuku modules via CAN bus and provides a complete glass cockpit solution for experimental aircraft.

## Hardware Specifications

- **Processor:** PIC32MZ2064DAR176 (DA Family)
  - 200 MHz MIPS M5150 core
  - 2048 KB Flash
  - 512 KB RAM
  - Integrated graphics controller
- **Display:** 10" TFT touchscreen (1024x600)
- **Communication:** Dual CAN bus interfaces
- **Storage:** External flash for maps and data
- **Power:** 12-28V DC aircraft power

## Features

### Flight Instruments
- Attitude indicator (artificial horizon)
- Airspeed indicator
- Altimeter
- Vertical speed indicator
- Heading indicator
- Turn coordinator

### Navigation
- Moving map display
- GPS navigation
- Flight plan management
- Waypoint database
- Approach procedures

### Engine Monitoring
- RPM, manifold pressure, fuel flow
- CHT/EGT monitoring (all cylinders)
- Oil pressure and temperature
- Fuel quantity and endurance
- Configurable limits and alarms

### System Integration
- CAN bus networking with other Kotuku modules
- Configuration via XML layouts (generated from `en_nz.xml` and `setup.xml`)
- Touch-based UI with customizable screens
- Data logging and playback

## Building the Project

### Prerequisites

**For Windows Development:**
```powershell
# Install Visual Studio 2022 Build Tools
winget install Microsoft.VisualStudio.2022.BuildTools --force --override `
  "--passive --wait --add Microsoft.VisualStudio.Workload.VCTools;includeRecommended"
```

**For PIC32 Embedded:**
- Microchip XC32 Compiler v5.00+
- DFP submodule (automatic via git)

**Clone the Repository:**
```bash
git clone --recurse-submodules https://github.com/yourorg/canfly.git
cd canfly
```

### Windows Development Build

The Windows build allows UI development and testing on PC before deploying to hardware.

**Configure and Build:**
```bash
cd apps/kMFD10
mkdir build-win
cd build-win

# Configure
cmake ..

# Build
cmake --build . --config Release

# Output: build-win/bin/Release/kMFD10.exe
```

**Run the Application:**
```bash
# From build-win directory
./bin/Release/kMFD10.exe
```

**Debug Build:**
```bash
cmake --build . --config Debug
./bin/Debug/kMFD10.exe
```

### PIC32 Embedded Build

**Step 1: Build Required Libraries**

The kMFD10 depends on several libraries that must be built first:

```bash
# Build atom library (hardware abstraction)
cd libs/atom
mkdir build-da
cd build-da
cmake -DCMAKE_TOOLCHAIN_FILE=/pic32/cmake/pic32-DA-toolchain.cmake \
      -DPIC32_PROCESSOR=PIC32MZ2064DAR176 \
      -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
cd ../../..

# Build other required libraries (neutron, photon, proton, etc.)
# Similar process for each library
```

**Step 2: Build kMFD10 Application**

```bash
cd apps/kMFD10
mkdir build-pic32
cd build-pic32

# Configure for PIC32
cmake -DCMAKE_TOOLCHAIN_FILE=/pic32/cmake/pic32-DA-toolchain.cmake \
      -DCMAKE_BUILD_TYPE=Release ..

# Build
cmake --build .

# Output files:
#   firmware/kMFD10.elf  - Executable with debug symbols
#   firmware/kMFD10.hex  - Programming file for production
#   firmware/kMFD10.bin  - Binary image
#   kMFD10.map           - Memory map
```

**Debug Build for Hardware Debugging:**
```bash
cmake -DCMAKE_TOOLCHAIN_FILE=/pic32/cmake/pic32-DA-toolchain.cmake \
      -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

**Clean Build:**
```bash
cmake --build . --target clean
# Or remove build directory entirely
cd ..
rm -rf build-pic32
```

## CLion IDE Setup

### Import Project

1. Open CLion
2. File → Open → Select `canfly/apps/kMFD10`
3. CLion will detect CMakeLists.txt

### CMake Profiles

Create these profiles in **Settings → Build, Execution, Deployment → CMake:**

#### Windows-Debug Profile
```
Name: Windows-Debug
Build type: Debug
Toolchain: Visual Studio
CMake options: (empty)
Build directory: cmake-build-debug
```

#### Windows-Release Profile
```
Name: Windows-Release
Build type: Release
Toolchain: Visual Studio
CMake options: (empty)
Build directory: cmake-build-release
```

#### PIC32-DA-Debug Profile
```
Name: PIC32-DA-Debug
Build type: Debug
Toolchain: Visual Studio
CMake options: -DCMAKE_TOOLCHAIN_FILE=/pic32/cmake/pic32-DA-toolchain.cmake
Build directory: cmake-build-pic32-debug
```

#### PIC32-DA-Release Profile
```
Name: PIC32-DA-Release
Build type: Release
Toolchain: Visual Studio
CMake options: -DCMAKE_TOOLCHAIN_FILE=/pic32/cmake/pic32-DA-toolchain.cmake
Build directory: cmake-build-pic32-release
```

### Building in CLion

1. Select desired profile from dropdown (bottom toolbar)
2. Build → Build Project (Ctrl+F9)
3. Outputs appear in the build directory specified in the profile

### Running/Debugging

**Windows Builds:**
- Click Run (Shift+F10) or Debug (Shift+F9)
- Debugger fully functional with breakpoints

**PIC32 Builds:**
- Use external programmer (MPLAB X + PICkit 4/ICD4)
- Or configure CLion to attach to GDB server

## Project Structure

```
kMFD10/
├── CMakeLists.txt          # Main build configuration
├── README.md               # This file
├── en_nz.xml               # Main UI layout definition
├── setup.xml               # Setup/configuration UI layout
├── pic32/
│   └── mfd10.c            # PIC32 main entry point
├── win32/
│   └── mfd10.c            # Windows main entry point
├── shared/
│   ├── layout_handlers.h  # Common layout logic
│   └── layout_handlers.c  # Layout switching
└── Generated files (built from XML):
    ├── en_nz.c
    ├── en_nz.h
    ├── en_nz_priv.h
    ├── setup.c
    ├── setup.h
    └── setup_priv.h
```

## Configuration

### UI Layout Generation

The kMFD10 UI is defined in XML and converted to C code at build time using the `yehudi` tool:

**XML Layout Files:**
- `en_nz.xml` - Main flight display layouts
- `setup.xml` - Configuration and settings screens

**Modification Process:**
1. Edit XML file (e.g., `en_nz.xml`)
2. Run CMake/build - `yehudi` automatically regenerates C code
3. Rebuild application

**XML Structure:**
```xml
<application>
  <windows>
    <window id="main_pfd">
      <widgets>
        <widget type="attitude_indicator" .../>
        <widget type="airspeed" .../>
        ...
      </widgets>
    </window>
  </windows>
</application>
```

### Platform-Specific Configuration

The build automatically detects the platform and uses appropriate code:

**PIC32 Build:**
- Uses `pic32/mfd10.c`
- Links with: atom, neutron, photon, proton, muon, atomdb, graviton, mfdlib

**Windows Build:**
- Uses `win32/mfd10.c`
- Links with: krypton, neutron, photon, proton, muon, atomdb, graviton, mfdlib
- Additional: DirectDraw, Windows USB libraries

## Programming the Device

### Using MPLAB X IPE (Recommended for Production)

1. Open MPLAB X IPE
2. Select Device: PIC32MZ2064DAR176
3. Select Tool: PICkit 4 (or ICD4)
4. Load HEX file: `build-pic32/firmware/kMFD10.hex`
5. Click "Program"

### Using PICkit 4 Command Line

```bash
# Windows
"C:\Program Files\Microchip\MPLABX\v6.20\mplab_platform\bin\ipecmd.exe" ^
  -P32MZ2064DAR176 ^
  -TPPK4 ^
  -F"build-pic32/firmware/kMFD10.hex" ^
  -M ^
  -OL

# Linux/Mac
/opt/microchip/mplabx/v6.20/mplab_platform/bin/ipecmd.sh \
  -P32MZ2064DAR176 \
  -TPPK4 \
  -F"build-pic32/firmware/kMFD10.hex" \
  -M \
  -OL
```

### Using Bootloader (Field Updates)

The kMFD10 uses the kBoot bootloader for field firmware updates:

1. Build kBoot for kMFD10 (see kBoot documentation)
2. Program kBoot to device (one time)
3. Use CAN bus update utility to flash new kMFD10 firmware
4. Device self-validates and boots new firmware

## Debugging

### Windows Debugging

CLion provides full debugging support for Windows builds:

1. Set breakpoints in source code
2. Select Windows-Debug profile
3. Run → Debug (Shift+F9)
4. Use debugger controls, inspect variables, etc.

### PIC32 Hardware Debugging

**Using MPLAB X:**

1. Open MPLAB X IDE
2. File → Open Project → Select kMFD10
3. Set kMFD10 as main project
4. Connect PICkit 4/ICD4
5. Debug → Debug Main Project

**Using Segger J-Link (Advanced):**

Configure CLion to attach to J-Link GDB server:
```bash
# Start GDB server
JLinkGDBServer -device PIC32MZ2064DAR176 -if SWD -speed 4000

# In CLion: Run → Edit Configurations → Add GDB Remote Debug
# Target: localhost:2331
```

### Debug Output

The kMFD10 can output debug messages via:
- **UART:** Debug console output (115200 baud)
- **CAN Bus:** Diagnostic messages
- **USB:** USB CDC virtual serial port

**Enable Debug Output:**
```c
// In pic32/mfd10.c
#define DEBUG_UART
#define DEBUG_LEVEL 3  // 0=errors only, 3=verbose
```

## Troubleshooting

### Build Issues

**Error: "yehudi tool not found"**
```bash
# Build the yehudi tool first
cd tools/yehudi
mkdir build
cd build
cmake ..
cmake --build .
```

**Error: "Library not found (neutron, photon, etc.)"**
```bash
# Build all required libraries first
cd libs
./build_all_libs.sh  # If available
# Or build each library individually
```

**Error: "DFP not found"**
```bash
# Ensure submodules are initialized
git submodule update --init --recursive

# Verify DFP exists
ls ../../../xc32/PIC32MZ-DA  # Should show DFP contents
```

### Runtime Issues

**Windows: Display not rendering**
- Check DirectDraw is installed (Windows SDK)
- Verify graphics drivers are up to date
- Try running as Administrator

**PIC32: Device not booting**
- Verify HEX file programmed correctly
- Check bootloader is present and valid
- Inspect UART output for boot messages
- Verify power supply is stable (12-28V)

**PIC32: CAN bus not working**
- Check CAN termination resistors (120Ω)
- Verify baud rate matches other devices (typically 250kbps)
- Use oscilloscope to check CAN_H/CAN_L signals

**Touch screen not responding**
- Calibrate touch screen via setup menu
- Check I2C connection to touch controller
- Verify touch controller firmware version

### Memory Issues

**Error: "Region overflow" during linking**

The kMFD10 uses significant memory for graphics. If you see overflow:

```bash
# Check current memory usage
pic32-size kMFD10.elf

# Optimize build
cmake -DCMAKE_BUILD_TYPE=Release ..  # Use Release, not Debug
```

**Reduce Memory Usage:**
- Decrease framebuffer size in configuration
- Reduce map data cache size
- Disable unused features
- Use `-Os` optimization (size) instead of `-O2`

## Performance Optimization

### Graphics Performance

**Frame Rate Optimization:**
```c
// In configuration
#define TARGET_FPS 30        // Reduce from 60 if needed
#define DOUBLE_BUFFER true   // Essential for smooth updates
```

**Reduce Overdraw:**
- Minimize overlapping widgets
- Use dirty rectangle updates
- Cache rendered elements

### CAN Bus Optimization

**Message Filtering:**
```c
// Accept only required CAN IDs
can_set_filter(CAN_FILTER_0, ENGINE_DATA_ID);
can_set_filter(CAN_FILTER_1, GPS_DATA_ID);
// etc.
```

**Message Prioritization:**
- Critical flight data: Highest priority
- Engine monitoring: Medium priority
- Configuration: Low priority

## Additional Resources

- **Main Project README:** See `../../README.md`
- **Hardware Schematics:** See `hardware/kMFD10/`
- **CAN Bus Protocol:** See `docs/CAN_Protocol.md`
- **UI Layout Guide:** See `docs/UI_Layout_Guide.md`

## Support

For issues or questions:
- Check troubleshooting section above
- Review main project documentation
- Contact: [your-email@domain.com]

## License

Proprietary - All rights reserved
