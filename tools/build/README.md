This folder holds the build tools for diy-efis

The whole structure of the tools were changed in 2021 to deliver the diy-efis as a C# application targeting what was then the [nanoFramework](https:nanoFramework.net).  Kotuku has written their own C# runtime system that is built into their products, and provides an interpreter/debugger for [VSCode](https://code.visualstudio.com/) that allows debugging and testing of CanFly® on Windows and Linux.

The runtime system does not have debugging capabilities and is delivered as a downloadable image to flash for a Raspberry PI.

This repository holds the C# compiler for CanFly® which uses the Microsoft [Roslyn](https://github.com/dotnet/roslyn) compiler to compile C# code into a .pe format.

The .pe format was part of the nanoFramework and this code comes from there.  The build tool has been changed to include calling Roslyn to compile the source files and generate the assembly.  No intermediate files are generated.

The .pe files used by CanFly are basically the same as the nanoFramework except for:

* The Roslyn compiler is asked to syntax check against .NET v1 which is what the orginal SPOT code came from.
* The support which was experimental in the nanoFramework for generics is removed.
* An extra table is defined in the .pe format to include debugging symbols in a debug build.

The last point is important as the VSCode debugger uses the symbols loaded in an assembly for source level debugging. There is no external .pdb or .pdbx file that as is used by the nanoFramework integration with VS2019
