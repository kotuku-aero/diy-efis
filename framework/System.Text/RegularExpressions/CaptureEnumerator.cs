//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

using System.Collections;

namespace System.Text.RegularExpressions
{
  internal class CaptureEnumerator : IEnumerator
  {
    internal int _curindex = -1;
    internal CaptureCollection _rcc;

    /// <summary>
    /// Gets the capture element
    /// </summary>
    public Capture Capture
    {
      get
      {
        if ((_curindex < 0) || (_curindex >= _rcc.Count))
        {
          throw new InvalidOperationException("EnumNotStarted");
        }

        return _rcc[_curindex];
      }
    }

    /// <summary>
    /// Gets the current object.
    /// </summary>
    public object Current
    {
      get { return Capture; }
    }

    internal CaptureEnumerator(CaptureCollection rcc)
    {
      _rcc = rcc;
    }

    /// <summary>
    /// Move to the next capture element.
    /// </summary>
    /// <returns>True if success</returns>
    public bool MoveNext()
    {
      int count = _rcc.Count;
      if (_curindex >= count)
      {
        return false;
      }

      return (++_curindex < count);
    }

    /// <summary>
    /// Reset the position.
    /// </summary>
    public void Reset()
    {
      _curindex = -1;
    }
  }
}
