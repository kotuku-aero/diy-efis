//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

namespace CanFly.Tools.MetadataProcessor
{
    /// <summary>
    /// Default implementation of <see cref="InanoTable"/> interface. Do nothing and
    /// used for emulating temporary not supported metadata tables and for last fake table.
    /// </summary>
    public sealed class nanoEmptyTable : InanoTable
    {
        /// <summary>
        /// Singleton pattern - single unique instance of object.
        /// </summary>
        private static readonly InanoTable _instance = new nanoEmptyTable();

        /// <summary>
        /// Singleton pattern - private constructor prevents direct instantiation.
        /// </summary>
        private nanoEmptyTable() { }

        /// <inheritdoc/>
        public void Write(
            nanoBinaryWriter writer)
        {
        }

        /// <summary>
        /// Singleton pattern - gets single unique instance of object.
        /// </summary>
        public static InanoTable Instance { get { return _instance; } }
    }
}
