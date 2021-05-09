//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

using System.Collections;

namespace System.Text.RegularExpressions
{
  [Serializable]
  public class MatchCollection : ICollection, IEnumerable
  {
    #region Fields

    internal const int infinite = 0x7fffffff;
    internal string _input;
    /// <summary>
    /// the last index to match against
    /// </summary>
    internal int _lastIndex;
    internal ArrayList _matches;
    internal int _previousIndex;
    internal Regex _regex;
    internal int _startIndex;
    internal bool _done;

    #endregion

    #region Properties

    /// <summary>
    /// Gets the number of matches. 
    /// </summary>
    public int Count
    {
      get
      {
        if (!_done)
        {
          GetMatch(infinite);
        }

        return _matches.Count;
      }
    }

    /// <summary>
    /// Gets a value that indicates whether the collection is read only. 
    /// </summary>
    public bool IsReadOnly
    {
      get
      {
        return true;
      }
    }

    /// <summary>
    /// Gets a value indicating whether access to the collection is synchronized (thread-safe). 
    /// </summary>

    /// <summary>
    /// Gets an individual member of the collection. 
    /// </summary>
    /// <param name="i"></param>
    /// <returns></returns>
    public virtual Match this[int i]
    {
      get
      {
        if (i > Count)
        {
          throw new ArgumentOutOfRangeException("i");
        }

        return this.GetMatch(i);
      }
    }

    /// <summary>
    /// Gets an object that can be used to synchronize access to the collection. 
    /// </summary>
    public object SyncRoot { get { return this; } }

    public bool IsSynchronized { get { return false; } }

    internal MatchCollection(Regex regex, string input, int length, int startat)
    {
      if ((startat < 0) || (startat > input.Length))
       throw new ArgumentOutOfRangeException("startat");

      if (length < 0)
       length = input.Length;
       
      _regex = regex;
      _input = input;
      _lastIndex = length;
      _startIndex = startat;
      _previousIndex = 0;
      _matches = new ArrayList();
    }

    #endregion

    #region Methods

    /// <inheritdoc/>
    public void CopyTo(Array array, int arrayIndex)
    {
      int count = Count;
      try
      {
        _matches.CopyTo(array, arrayIndex);
      }
      catch
      {
        throw new ArgumentException("Arg_InvalidArrayType");
      }
    }

    /// <inheritdoc/>
    public IEnumerator GetEnumerator()
    {
      return new MatchEnumerator(this);
    }

    internal Match GetMatch(int i)
    {
      Match match;
      if (_matches.Count > i)
      {
        return (Match)_matches[i];
      }

      if (i < 0 || _done)
      {
        return null;
      }

      do
      {
        match = _regex.Match(_input, _previousIndex, _lastIndex);
        if (!match.Success)
        {
          _done = true;
          return null;
        }
        _matches.Add(match);
        _previousIndex = match._textend;
      }
      while (_matches.Count <= i);
      return match;
    }

    #endregion

  }
}
