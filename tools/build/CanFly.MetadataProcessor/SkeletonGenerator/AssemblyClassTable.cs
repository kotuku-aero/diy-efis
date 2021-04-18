//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Collections.Generic;

namespace CanFly.Tools.MetadataProcessor.Core
{
  public class AssemblyClassTable
  {
    public string AssemblyName;
    public string HeaderFileName;
    public string ProjectName;
    public bool IsInterop;

    public List<Class> Classes = new List<Class>();
    public List<ClassWithStubs> ClassesWithStubs = new List<ClassWithStubs>();
  }
}
