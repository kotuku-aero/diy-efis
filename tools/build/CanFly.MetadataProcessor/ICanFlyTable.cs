//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

namespace CanFly.Tools.MetadataProcessor
{
  /// <summary>
  /// Common interface for all metadata tables in .NET nanoFramework assembly.
  /// </summary>
  public interface IAssemblyTable
  {
    /// <summary>
    /// Writes metadata table from memory representation into output stream.
    /// </summary>
    /// <param name="writer">Binary writer with correct endianness.</param>
    void Write(CLRBinaryWriter writer);
  }
}