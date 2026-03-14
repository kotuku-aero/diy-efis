# CanFly Avionics SDK

Open-source SDK for developing custom avionics applications for experimental aircraft. Build once, run on Windows for development and deploy to embedded MIPS32 hardware.

## Table of Contents

- [System Overview](#system-overview)
- [Dual Development Architecture](#dual-development-architecture)
- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Project Structure](#project-structure)
- [Building Applications](#building-applications)
  - [Windows Development Build](#windows-development-build)
  - [MIPS32 Embedded Build](#mips32-embedded-build)
- [Application Packaging](#application-packaging)
- [CLion IDE Setup](#clion-ide-setup)
- [Creating Your Own Application](#creating-your-own-application)
- [Troubleshooting](#troubleshooting)

## System Overview

The CanFly SDK provides a complete framework for building glass cockpit avionics applications that run on both Windows (for development/testing) and embedded MIPS32 hardware (for deployment).

**Key Features:**
- **Dual-target compilation:** Same codebase runs on Windows and MIPS32 embedded systems
- **User-mode architecture:** Applications run in protected user space with kernel syscalls
- **Proton UI framework:** XML-based layout system with automatic code generation
- **CanFly syscalls:** Hardware abstraction for CAN bus, sensors, displays, and I/O
- **Automatic packaging:** Post-build packaging creates deployable binary images with SHA256 integrity

## Dual Development Architecture

CanFly applications use a unique dual-target architecture:

### Windows Development Build (MSVC/MinGW)

- **Purpose:** Rapid UI development, testing, and debugging on PC
- **Platform:** Windows 10/11 with Visual Studio or MinGW
- **Libraries:** `w32_canfly.dll` - Windows emulation of CanFly kernel syscalls
- **Output:** Windows executable (`.exe`)
- **Use case:**
  - Develop and test UI layouts
  - Debug application logic
  - Iterate quickly without hardware

### MIPS32 Embedded Build (mips32r2-elf-gcc)

- **Purpose:** Deployment to embedded avionics hardware
- **Platform:** MIPS32 processor running CanFly kernel
- **Libraries:** `canfly` - User-mode syscall stubs
- **Output:**
  - ELF executable (`.elf`) for debugging
  - Packaged binary (`.bin`) with header and SHA256 for deployment
- **Architecture:** User-mode applications running at 0x00000020 in 384KB RAM
- **Use case:**
  - Final deployment to aircraft hardware
  - Hardware integration testing
  - Performance validation

### How It Works

1. **Write once:** Your application code is identical for both targets
2. **Conditional compilation:** `#ifdef MIPS32_BUILD` for platform-specific code
3. **Common syscalls:** CanFly API is the same on both platforms
4. **Build twice:**
   - Windows build for development
   - MIPS32 build for deployment

## Prerequisites

### For Windows Development (Required)

**Recommended IDE:**
- **CLion** (free for non-commercial use): https://www.jetbrains.com/clion/
  - Excellent CMake integration
  - Cross-platform debugging
  - Built-in code analysis

**Build Tools (choose one):**

**Option 1: Visual Studio 2022 Build Tools** (recommended)
```powershell
# Run as Administrator
winget install Microsoft.VisualStudio.2022.BuildTools --force --override "--passive --wait --add Microsoft.VisualStudio.Workload.VCTools;includeRecommended"
```

**Option 2: MinGW-w32** (alternative)
```powershell
winget install mingw-w32
```

**Additional Tools:**
- CMake 3.15+ (bundled with CLion)
- Git with submodules support
- Python 3.8+ (for build tools and packaging)

### For MIPS32 Embedded Development (Required)

**MIPS32 Compiler Toolchain:**
- Download from: https://github.com/kotuku-aero/mips32
- Extract to: `C:\pic32\` (or update toolchain file path)
- Adds: `mips32r2-elf-gcc`, `mips32r2-elf-ld`, `mips32r2-elf-size`, `mips32r2-elf-gdb`

**Python Libraries:**
```bash
pip install pyelftools
```

### Not Supported

**XC32 Compiler:** While the codebase can build with XC32, we do not officially support it. Use the free mips32r2-elf-gcc toolchain instead.

## Quick Start

### 1. Clone the Repository

```bash
git clone --recurse-submodules https://github.com/kotuku-aero/canfly.git
cd canfly/diy-efis
```

### 2. Build Example Application (Windows)

```bash
cd apps/att_au_nz
mkdir build-win
cd build-win
cmake ..
cmake --build . --config Release

# Run the application
./bin/Release/att_au_nz.exe -x 1024 -y 768
```

### 3. Build for MIPS32 Hardware

```bash
cd apps/att_au_nz
mkdir build-mips32
cd build-mips32
cmake -DCMAKE_TOOLCHAIN_FILE=../../cmake/pic32-mips32r2-toolchain.cmake ..
cmake --build .

# Outputs:
# - firmware/att_au_nz.elf (for debugging)
# - images/att_au_nz.bin (for deployment)
```

## Project Structure

```
diy-efis/
├── apps/                      # User applications
│   ├── att_au_nz/            # Example: Attitude indicator (AU/NZ regs)
│   │   ├── main.c            # Application entry point
│   │   ├── att.c             # Attitude display logic
│   │   ├── layout/           # XML UI layouts
│   │   └── CMakeLists.txt
│   └── [your_app]/           # Your custom application
|
├── bin/                      # binary files
|   ├── CanFly.Commander.exe  # CanFly application manager
|   └── msh.exe               # gdb server for canfly hardware.
│
├── src/                      # SDK source code
│   ├── canfly/               # User-mode kernel interface
│   │   ├── canfly_stubs.c    # Syscall stubs (generated)
│   │   ├── newlib_stubs.c    # Minimal C library support
│   │   ├── crt0.S            # User-mode startup code
│   │   └── canfly_app.ld     # Linker script (384KB user RAM)
│   ├── mfdlib/               # Display/UI library
│   └── proton/               # UI framework & XML processing
|
├── tools/                    # Development tools
|   └── yehudi                # XML layout code generator
│
├── lib/                      # Platform abstraction
│   ├── libw32_canfly.so      # linux version (for development)
|   └── w32_canfly.dll/lib    # Windows DLL export definitions
│
├── cmake/                    # Build system
│   ├── MFDCommon.cmake       # Shared build configuration
│   └── mips32-toolchain.cmake
│
├── scripts/                  # Build tools
│   └── package_usermode.py   # Binary packaging tool
│
└── include/                  # SDK headers
    ├── canfly_api.md         # API documentation
    ├── canfly_types.h        # Common types
    ├── CanFlyID.def          # Definitions of the published CAN IDs
    ├── canfly.h              # Main CanFly API
    └── sys_canfly.h          # Syscall definitions
```

## Building Applications

### Windows Development Build

Used for rapid development and testing without hardware.

**From CLion:**
1. Open `diy-efis` as project
2. Select **Windows-Release** profile
3. Build → Build Project (Ctrl+F9)
4. Run → Run (Shift+F10)

**From Command Line:**
```bash
cd apps/att_au_nz
cmake -B build-win -DCMAKE_BUILD_TYPE=Release
cmake --build build-win --config Release

# Run with custom window size
./build-win/bin/Release/att_au_nz.exe -x 1024 -y 768
```

**Debug Build:**
```bash
cmake -B build-win-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-win-debug --config Debug
```

### MIPS32 Embedded Build

Used for deployment to avionics hardware.

**From CLion:**
1. Select **MIPS32-Release** profile
2. Build → Build Project (Ctrl+F9)
3. Output:
   - `cmake-build-mips32-release/firmware/att_au_nz.elf`
   - `cmake-build-mips32-release/images/att_au_nz.bin`

**From Command Line:**
```bash
cd apps/att_au_nz
cmake -B build-mips32 \
  -DCMAKE_TOOLCHAIN_FILE=../../cmake/pic32-mips32r2-toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build-mips32

# View program size
mips32r2-elf-size build-mips32/firmware/att_au_nz.elf
```

**Build Output:**
```
firmware/att_au_nz.elf    # ELF file with debug symbols
images/att_au_nz.bin      # Packaged binary for deployment
```

## Application Packaging

MIPS32 builds automatically package the ELF into a deployable binary format.

### Package Format

```
[32-byte header]
  - 0x10000008: Branch instruction (skip header)
  - 0x00000c00: EHB (execution hazard barrier)
  - Image length (big-endian)
  - Reserved
  - Image name (16 bytes)

[Application binary]
  - Code and data sections from ELF
  - 32-byte aligned

[SHA256 signature]
  - "sig-sha256" (28 bytes)
  - Hash length: 32 (big-endian)
  - SHA256 hash of header + binary
  - Padding to 32-byte alignment
```

### Manual Packaging

```bash
python3 scripts/package_usermode.py \
  -e firmware/att_au_nz.elf \
  -o images \
  -n att_au_nz
```

### Deployment

Upload the `.bin` file to the CanFly device using the Commander tool:
1. Open Commander (.NET tool)
2. File Explorer → Upload
3. Select `att_au_nz.bin`
4. Device loads and verifies SHA256
5. Application runs in user-mode at 0x00000020

Keep the `.elf` file for debugging with GDB/JTAG.

## CLion IDE Setup

CLion is the recommended IDE for CanFly development.

### Install CLion

Download from: https://www.jetbrains.com/clion/

**Free License Options:**
- Educational license (students/teachers)
- Open source project license
- 30-day trial

### CMake Profiles

Create these profiles in: **Settings → Build, Execution, Deployment → CMake**

#### Profile: Windows-Debug
```
Name: Windows-Debug
Build type: Debug
Toolchain: Visual Studio (or MinGW)
CMake options: (leave empty)
Build directory: cmake-build-debug
```

#### Profile: Windows-Release
```
Name: Windows-Release
Build type: Release
Toolchain: Visual Studio (or MinGW)
CMake options: (leave empty)
Build directory: cmake-build-release
```

#### Profile: MIPS32-Debug
```
Name: MIPS32-Debug
Build type: Debug
Toolchain: Visual Studio (placeholder - actual toolchain from file)
CMake options: -DCMAKE_TOOLCHAIN_FILE=cmake/pic32-mips32r2-toolchain.cmake
Build directory: cmake-build-mips32-debug
```

#### Profile: MIPS32-Release
```
Name: MIPS32-Release
Build type: Release
Toolchain: Visual Studio (placeholder - actual toolchain from file)
CMake options: -DCMAKE_TOOLCHAIN_FILE=cmake/pic32-mips32r2-toolchain.cmake
Build directory: cmake-build-mips32-release
```

### Building in CLion

1. **Select Profile:** Bottom toolbar → Profile dropdown
2. **Build All:** Hammer icon or Ctrl+F9
3. **Build Target:** Right-click target → Build
4. **Clean:** Build → Clean
5. **Reload CMake:** Tools → CMake → Reload CMake Project

### Debugging

**Windows Builds:**
- Set breakpoints in source
- Select Windows-Debug profile
- Run → Debug (Shift+F9)
- Full debugging support

**MIPS32 Builds:**
- Use `.elf` file with GDB
- Create an Embedded debug target in CLion
- CLion will download the application into the device and start debugging

## Creating Your Own Application

### 1. Create Application Directory

```bash
cd diy-efis/apps
mkdir my_avionics
cd my_avionics
```

### 2. Create CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.15)
project(my_avionics C)

# Include common configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/MFDCommon.cmake)

# Source files
set(SOURCES
    main.c
    my_logic.c
)

# Platform-specific build
if(MIPS32_BUILD)
    # MIPS32 embedded build
    add_executable(my_avionics ${SOURCES})

    mfd_configure_pic32_target(
        TARGET my_avionics
    )
else()
    # Windows development build
    add_executable(my_avionics ${SOURCES})

    mfd_configure_win32_target(
        TARGET my_avionics
    )
endif()
```

### 3. Create main.c

```c
#include "canfly.h"

#ifndef MIPS32_BUILD
// Windows debug arguments
int main(int argc, char** argv)
  {
  if (failed(canfly_init(argc, argv)))
    return -1;
#else
// MIPS32 user-mode (no arguments)
int main()
  {
#endif

  // Your application logic here

  return 0;
  }
```

### 4. Build and Test

```bash
# Windows build
cmake -B build-win
cmake --build build-win

# MIPS32 build
cmake -B build-mips32 \
  -DCMAKE_TOOLCHAIN_FILE=../../cmake/pic32-mips32r2-toolchain.cmake
cmake --build build-mips32
```

## Troubleshooting

### MIPS32 Compiler Not Found

**Error:** `CMAKE_C_COMPILER not found`

**Solution:**
```bash
# Verify installation
C:\pic32\bin\mips32r2-elf-gcc.exe --version

# Update toolchain file if installed elsewhere
# Edit: cmake/pic32-mips32r2-toolchain.cmake
set(MIPS32_TOOLCHAIN_PATH "C:/your/path/here")
```

### Missing Python Dependencies

**Error:** `ModuleNotFoundError: No module named 'elftools'`

**Solution:**
```bash
pip install pyelftools
```

### Linker Errors: Undefined Reference to Syscalls

**Error:** `undefined reference to 'cfg_open_key'`

**Solution:**
- Ensure `canfly` library is linked (should be automatic via `MFDCommon.cmake`)
- Check link order in `mfd_configure_pic32_target()`
- Verify syscall stubs are generated: `diy-efis/src/canfly/canfly_stubs.c`

### Entry Point Not Found

**Error:** `warning: cannot find entry symbol _reset`

**Solution:**
- Ensure `crt0.S` is in `canfly` library sources
- Verify CMake project includes ASM language: `project(canfly LANGUAGES C ASM)`
- Check linker script: `canfly_app.ld` defines `ENTRY(_reset)`

### Program Size Too Small (24 bytes)

**Issue:** Binary is suspiciously small, likely missing code

**Solution:**
- Check that `canfly` library links before `gcc` in link order
- Verify all source files are in `add_executable()` or linked libraries
- Look for undefined symbols in build output

### CLion Shows Red Underlines

**Issue:** Code appears to have errors but builds successfully

**Solution:**
1. Tools → CMake → Reset Cache and Reload Project
2. Ensure correct CMake profile is selected
3. File → Invalidate Caches → Invalidate and Restart

### Windows Build Can't Find w32_canfly.lib

**Error:** `Cannot find w32_canfly.lib`

**Solution:**
- Ensure submodules are initialized: `git submodule update --init --recursive`
- Check that `libs/w32_canfly/` exists and contains the library
- Verify path in `mfd_configure_win32_target()` function

## Additional Resources

- **CanFly Kernel Documentation:** [Coming soon]
- **Proton UI Framework Guide:** See `src/proton/README.md`
- **MIPS32 Toolchain:** https://github.com/kotuku-aero/mips32r2-elf-gcc
- **CMake Documentation:** https://cmake.org/documentation/
- **CLion CMake Support:** https://www.jetbrains.com/help/clion/cmake-support.html

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Test on both Windows and MIPS32
4. Submit a pull request

## License

[Your license here]

## Support

For questions or issues:
- GitHub Issues: https://github.com/kotuku-aero/canfly/issues
- Email: support@kotuku.aero
- Forum: [Coming soon]

---

**Happy flying with CanFly!** ✈️
