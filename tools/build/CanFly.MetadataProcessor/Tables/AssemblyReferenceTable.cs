//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace CanFly.Tools.MetadataProcessor
{
  /// <summary>
  /// Encapsulates logic for storing external assembly references list and writing
  /// this collected list into target assembly in .NET nanoFramework format.
  /// </summary>
  public sealed class AssemblyReferenceTable :
      ReferenceTableBase<AssemblyNameReference>
  {
    /// <summary>
    /// Helper class for comparing two instances of <see cref="AssemblyNameReference"/> objects
    /// using <see cref="AssemblyNameReference.FullName"/> property as unique key for comparison.
    /// </summary>
    private sealed class AssemblyNameReferenceComparer : IEqualityComparer<AssemblyNameReference>
    {
      /// <inheritdoc/>
      public bool Equals(AssemblyNameReference lhs, AssemblyNameReference rhs)
      {
        return string.Equals(lhs.FullName, rhs.FullName, StringComparison.Ordinal);
      }

      /// <inheritdoc/>
      public int GetHashCode(AssemblyNameReference item)
      {
        return item.FullName.GetHashCode();
      }
    }

    /// <summary>
    /// Creates new instance of <see cref="AssemblyReferenceTable"/> object.
    /// </summary>
    /// <param name="items">List of assembly references in Mono.Cecil format.</param>
    /// <param name="context">
    /// Assembly tables context - contains all tables used for building target assembly.
    /// </param>
    public AssemblyReferenceTable(
        IEnumerable<AssemblyNameReference> items,
        TablesContext context)
        : base(items, new AssemblyNameReferenceComparer(), context)
    {
    }

    /// <inheritdoc/>
    protected override void WriteSingleItem(
        CLRBinaryWriter writer,
        AssemblyNameReference item)
    {
      if (!_context.MinimizeComplete)
      {
        return;
      }

      WriteStringReference(writer, item.Name);
      writer.WriteUInt16(0); // padding

      writer.WriteVersion(item.Version);
    }

    /// <inheritdoc/>
    protected override void AllocateSingleItemStrings(
        AssemblyNameReference item)
    {
      GetOrCreateStringId(item.Name);
    }

    /// <summary>
    /// Gets assembly reference ID by assembly name reference in Mono.Cecil format.
    /// </summary>
    /// <param name="assemblyNameReference">Assembly name reference in Mono.Cecil format.</param>
    /// <returns>Refernce ID for passed <paramref name="assemblyNameReference"/> item.</returns>
    public ushort GetReferenceId(
        AssemblyNameReference assemblyNameReference)
    {
      ushort referenceId;
      TryGetIdByValue(assemblyNameReference, out referenceId);
      return referenceId;
    }
  }
}
