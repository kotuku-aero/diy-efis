//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Text.RegularExpressions
{
  /// <summary>
  /// Group class.
  /// </summary>
  [Serializable]
  public class Group : Capture
  {
    internal CaptureCollection _capcoll;
    internal int _capcount;
    internal int[] _caps;
    internal static Group _emptygroup = new Group(string.Empty, new int[0], 0);

    public CaptureCollection Captures
    {
      get
      {
        if (_capcoll == null)
        {
          _capcoll = new CaptureCollection(this);
        }

        return _capcoll;
      }
    }

    /// <summary>
    /// Gets a value indicating whether the match was successful. 
    /// </summary>
    public bool Success
    {
      get { return _capcount != 0; }
    }

    internal Group(string text, int[] caps, int capcount)
            : base(text, (capcount == 0) ? 0 : caps[(capcount - 1) * 2], (capcount == 0) ? 0 : caps[(capcount * 2) - 1])
    {
      _caps = caps;
      _capcount = capcount;
    }

    /// <summary>
    /// Synchronized
    /// </summary>
    /// <param name="inner">The inner group</param>
    /// <returns>Group</returns>
    public static Group Synchronized(Group inner)
    {
      if (inner == null)
      {
        throw new ArgumentNullException("inner");
      }

      CaptureCollection captures = inner.Captures;
      if (inner._capcount > 0)
      {
        Capture capture1 = captures[0];
      }

      return inner;
    }

  }
}
