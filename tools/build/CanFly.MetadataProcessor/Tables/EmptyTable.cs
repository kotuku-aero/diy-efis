//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

namespace CanFly.Tools.MetadataProcessor
{
  /// <summary>
  /// Default implementation of <see cref="IAssemblyTable"/> interface. Do nothing and
  /// used for emulating temporary not supported metadata tables and for last fake table.
  /// </summary>
  public sealed class EmptyTable : IAssemblyTable
  {
    /// <summary>
    /// Singleton pattern - single unique instance of object.
    /// </summary>
    private static readonly IAssemblyTable _instance = new EmptyTable();

    /// <summary>
    /// Singleton pattern - private constructor prevents direct instantiation.
    /// </summary>
    private EmptyTable() { }

    /// <inheritdoc/>
    public void Write(
        CLRBinaryWriter writer)
    {
    }

    /// <summary>
    /// Singleton pattern - gets single unique instance of object.
    /// </summary>
    public static IAssemblyTable Instance { get { return _instance; } }
  }
}
