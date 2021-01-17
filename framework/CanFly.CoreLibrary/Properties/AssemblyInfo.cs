// General Information about an assembly is controlled through the following 
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

[assembly: AssemblyTitle("CanFly")]
[assembly: AssemblyDescription("Core Canfly interface for the nanoFramework")]
[assembly: AssemblyConfiguration("")]
[assembly: AssemblyCompany("")]
[assembly: AssemblyProduct("CanFly")]
[assembly: AssemblyCopyright("Copyright © Kotuku Aerospace Limited")]
[assembly: AssemblyTrademark("CanFly")]
[assembly: AssemblyCulture("en-us")]

// Setting ComVisible to false makes the types in this assembly not visible 
// to COM components.  If you need to access a type in this assembly from 
// COM, set the ComVisible attribute to true on that type.
[assembly: ComVisible(false)]

// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version 
//      Build Number
//      Revision
//
// You can specify all the values or you can default the Build and Revision Numbers 
// by using the '*' as shown below:
// [assembly: AssemblyVersion("1.0.*")]
[assembly: AssemblyVersion("1.0.0.0")]
[assembly: AssemblyFileVersion("1.0.0.0")]

/////////////////////////////////////////////////////////////////
// This attribute is mandatory when building Interop libraries //
// update this whenever the native assembly signature changes  //
[assembly: AssemblyNativeVersion("1.0.0.0")]
/////////////////////////////////////////////////////////////////

[assembly: InternalsVisibleTo("CanFly.Proton")]
[assembly: InternalsVisibleTo("CanFly.Photon")]
[assembly: InternalsVisibleTo("CanFly.Neutron")]