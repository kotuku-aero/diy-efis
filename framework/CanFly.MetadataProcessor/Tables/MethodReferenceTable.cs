//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

using Mono.Cecil;
using System;
using System.Collections.Generic;

namespace CanFly.Tools.MetadataProcessor
{
  /// <summary>
  /// Encapsulates logic for storing methods references list and writing
  /// this collected list into target assembly in .NET nanoFramework format.
  /// </summary>
  public sealed class MethodReferenceTable :
      ReferenceTableBase<MethodReference>
  {
    /// <summary>
    /// Helper class for comparing two instances of <see cref="MethodReference"/> objects
    /// using <see cref="MethodReference.FullName"/> property as unique key for comparison.
    /// </summary>
    private sealed class MemberReferenceComparer : IEqualityComparer<MethodReference>
    {
      /// <inheritdoc/>
      public bool Equals(MethodReference lhs, MethodReference rhs)
      {
        return string.Equals(lhs.FullName, rhs.FullName, StringComparison.Ordinal);
      }

      /// <inheritdoc/>
      public int GetHashCode(MethodReference that)
      {
        return that.FullName.GetHashCode();
      }
    }

    /// <summary>
    /// Creates new instance of <see cref="MethodReferenceTable"/> object.
    /// </summary>
    /// <param name="items">List of member references in Mono.Cecil format.</param>
    /// <param name="context">
    /// Assembly tables context - contains all tables used for building target assembly.
    /// </param>
    public MethodReferenceTable(
        IEnumerable<MethodReference> items,
        TablesContext context)
        : base(items, new MemberReferenceComparer(), context)
    {
    }

    /// <summary>
    /// Gets method reference ID if possible (if method is external and stored in this table).
    /// </summary>
    /// <param name="methodReference">Method reference metadata in Mono.Cecil format.</param>
    /// <param name="referenceId">Method reference ID in .NET nanoFramework format.</param>
    /// <returns>Returns <c>true</c> if reference found, otherwise returns <c>false</c>.</returns>
    public bool TryGetMethodReferenceId(
        MethodReference methodReference,
        out ushort referenceId)
    {
      return TryGetIdByValue(methodReference, out referenceId);
    }

    /// <inheritdoc/>
    protected override void WriteSingleItem(
        CLRBinaryWriter writer,
        MethodReference item)
    {
      if (!_context.MinimizeComplete)
      {
        return;
      }

      bool experimentalCode = true;

      ushort referenceId;


      if (experimentalCode)
      {
        ////////////////////////////////////
        // EXPERIMENTAL CODE FOR GENERICS //
        ////////////////////////////////////

        if (_context.TypeReferencesTable.TryGetTypeReferenceId(item.DeclaringType, out referenceId))
        {
          WriteStringReference(writer, item.Name);
          writer.WriteUInt16(referenceId);

          writer.WriteUInt16(_context.SignaturesTable.GetOrCreateSignatureId(item));
          writer.WriteUInt16(0); // padding
        }
        else if (_context.TypeReferencesTable.TryGetTypeReferenceId(item.DeclaringType.Resolve(), out referenceId))
        {
          WriteStringReference(writer, item.Name);
          writer.WriteUInt16((ushort)(referenceId | 0x8000));

          writer.WriteUInt16(_context.SignaturesTable.GetOrCreateSignatureId(item));
          writer.WriteUInt16(0); // padding
        }
        else if (_context.TypeDefinitionTable.TryGetTypeReferenceId(item.DeclaringType.Resolve(), out referenceId))
        {
          WriteStringReference(writer, item.Name);
          writer.WriteUInt16((ushort)(referenceId | 0x8000));

          writer.WriteUInt16(_context.SignaturesTable.GetOrCreateSignatureId(item));
          writer.WriteUInt16(0); // padding
        }
        else
        {
          throw new ArgumentException($"Can't find entry in type reference table for {item.DeclaringType.FullName} for Method {item.FullName}.");
        }
      }
      else
      {
        _context.TypeReferencesTable.TryGetTypeReferenceId(item.DeclaringType, out referenceId);

        WriteStringReference(writer, item.Name);
        writer.WriteUInt16(referenceId);

        writer.WriteUInt16(_context.SignaturesTable.GetOrCreateSignatureId(item));
        writer.WriteUInt16(0); // padding
      }
    }
  }
}
