//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Collections.Generic;

namespace CanFly.Tools.MetadataProcessor.Core
{
  public class AssemblyClassStubs
  {
    public string HeaderFileName;

    public string ClassHeaderFileName;

    public string ClassName;

    public string ShortNameUpper;

    public string RootNamespace;

    public string ProjectName;

    public string AssemblyName;

    public List<MethodStub> Functions = new List<MethodStub>();
  }
}
