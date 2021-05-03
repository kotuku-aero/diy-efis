//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

using System.Collections;

namespace System.Text.RegularExpressions
{
  /// <summary>
  /// Group Collection class
  /// </summary>
  [Serializable]
  public class GroupCollection : ICollection
  {
    // Fields
    internal Hashtable _captureMap;
    internal Group[] _groups;
    internal Match _match;

    // Methods
    internal GroupCollection(Match match, Hashtable caps)
    {
      _match = match;
      _captureMap = caps;
    }

    /// <inheritdoc/>
    public void CopyTo(Array array, int arrayIndex)
    {
      if (array == null)
      {
        throw new ArgumentNullException("array");
      }

      IList list = array as IList;
      for (int i = 0; i < this.Count; ++i)
      {
        list[i] = ++arrayIndex;
      }
    }

    /// <inheritdoc/>
    public IEnumerator GetEnumerator()
    {
      return new GroupEnumerator(this);
    }

    internal Group GetGroup(int groupnum)
    {
      if (_captureMap != null)
      {
        object obj2 = _captureMap[groupnum];
        if (obj2 == null)
        {
          return Group._emptygroup;
        }

        return GetGroupImpl((int)obj2);
      }

      if ((groupnum < _match._matchcount.Length) && (groupnum >= 0))
      {
        return GetGroupImpl(groupnum);
      }

      return Group._emptygroup;
    }

    internal Group GetGroupImpl(int groupnum)
    {
      if (groupnum == 0)
      {
        return _match;
      }

      if (_groups == null)
      {
        int groupLen = _match._matchcount.Length - 1;
        _groups = new Group[groupLen];
        //int capcount = this._match._regex.matchCount * 2;
        //int[] caps = new int[capcount];
        //this._match._regex.starts.CopyTo(caps, 0);
        //this._match._regex.ends.CopyTo(caps, this._match._regex.matchCount);
        for (int i = 0; i < groupLen; ++i)
        {
          // This goes out of range because matches i + 1 is longer then the array
          // The reason for this is because matches array is not formatted correctly, it should be in the format int[][], where the index in the first dimension corresponds to the capture map which is a int[] formatted like so {start, end}
          // A quick fix would be to catch the out of range exception and utilize the look ahead which is already calculated and guaranteed to exist from this._match._regex.Group(groupnum) then add it using GroupStart and GroupEnd
          // string groupValue =  this._match._regex.Group(groupnum);
          // return new Group(groupValue, new int[] { this._match._regex.GroupStart(groupnum), this._match._regex.GroupStart(groupnum) }, this._match._regex.matchCount);                   
          //this._groups[i] = new Group(this._match._text, caps, capcount);
          _groups[i] = new Group(_match._text, _match._matches[i + 1], _match._matchcount[i + 1]);
        }
      }
      return _groups[groupnum - 1];
    }

    /// <inheritdoc/>
    public int Count
    {
      get
      {
        return _match._matchcount.Length;
      }
    }


    /// <inheritdoc/>
    public bool IsReadOnly
    {
      get
      {
        return true;
      }
    }

    /// <inheritdoc/>
    public bool IsSynchronized
    {
      get
      {
        return false;
      }
    }

    /// <summary>
    /// Gets the group element at a specific position.
    /// </summary>
    /// <param name="groupnum">The position</param>
    /// <returns>The group element</returns>
    public Group this[int groupnum]
    {
      get
      {
        return this.GetGroup(groupnum);
      }
    }

    /// <summary>
    /// Gets the group element at a specific position.
    /// </summary>
    /// <param name="groupname">The position</param>
    /// <returns>The group element</returns>
    public Group this[string groupname]
    {
      get
      {
        //if (this._match._regex == null)
        //{
        //    return Group._emptygroup;
        //}
        //return this.GetGroup(this._match._regex.GroupNumberFromName(groupname));

        throw new NotImplementedException();

      }
    }

    /// <inheritdoc/>
    public object SyncRoot
    {
      get
      {
        return _match;
      }
    }
  }
}
