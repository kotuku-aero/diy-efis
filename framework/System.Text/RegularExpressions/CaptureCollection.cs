//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

using System.Collections;

namespace System.Text.RegularExpressions
{
    /// <summary>
    /// Capture Collection
    /// </summary>
    [Serializable]
    public class CaptureCollection : ICollection, IEnumerable
    {
        // Fields
        internal int _capcount;
        internal Capture[] _captures;
        internal Group _group;

        // Methods
        internal CaptureCollection(Group group)
        {
            _group = group;
            _capcount = _group._capcount;
        }

        /// <inheritdoc/>
        public void CopyTo(Array array, int arrayIndex)
        {
            if (array == null)
            {
                throw new ArgumentNullException("array");
            }

            IList list = array as IList;
            for (int i = 0; i < _capcount; ++i)
            {
                list[i] = this[++arrayIndex];
            }
        }

        internal Capture GetCapture(int i)
        {
            if ((i == (_capcount - 1)) && (i >= 0))
            {
                return _group;
            }

            if ((i >= _capcount) || (i < 0))
            {
                throw new ArgumentOutOfRangeException("i");
            }

            if (_captures == null)
            {
                _captures = new Capture[_capcount];
                for (int j = 0; j < (_capcount - 1); j++)
                {
                    _captures[j] = new Capture(_group._text, _group._caps[j * 2], _group._caps[(j * 2) + 1]);
                }
            }

            return _captures[i];
        }

        /// <inheritdoc/>
        public IEnumerator GetEnumerator()
        {
            return new CaptureEnumerator(this);
        }

    public int Count
    {
      get { return _capcount; }
    }

    /// <inheritdoc/>
    public bool IsReadOnly
    {
      get{ return true; }
    }

    public bool IsSynchronized
    {
      get { return false; }
    }

    /// <summary>
    /// Gets a capture element at a specific position.
    /// </summary>
    /// <param name="i">The position.</param>
    /// <returns>The capture element.</returns>
    public Capture this[int i]
    {
      get { return GetCapture(i); }
    }

    /// <inheritdoc/>
    public object SyncRoot
    {
      get { return _group; }
    }
  }
}
