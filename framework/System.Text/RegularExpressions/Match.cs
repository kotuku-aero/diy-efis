//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Text.RegularExpressions
{
  /// <summary>
  /// Represents the results from a single regular expression match.
  /// </summary>
  [Serializable]
  public class Match : Group
  {
    internal bool _balancing;
    internal static Match _empty = new Match(null, 1, string.Empty, 0, 0, 0);
    internal GroupCollection _groupcoll;
    internal int[] _matchcount;
    internal int[][] _matches;
    internal Regex _regex;
    internal int _textbeg;
    internal int _textend;
    internal int _textpos;
    internal int _textstart;

    /// <summary>
    /// Gets and empty match.
    /// </summary>
    public static Match Empty
    {
      get { return _empty; }
    }

    /// <summary>
    /// Gets a group collection.
    /// </summary>
    public virtual GroupCollection Groups
    {
      get
      {
        if (_groupcoll == null)
        {
          _groupcoll = new GroupCollection(this, null);
        }

        return _groupcoll;
      }
    }

    internal Match(Regex regex, int capcount, string text, int begpos, int len, int startpos)
        : base(text, new int[2], 0)
    {
      _regex = regex;
      _matchcount = new int[capcount];
      _matches = new int[capcount][];
      _matches[0] = base._caps;
      _textbeg = begpos;
      _textend = begpos + len;
      _textstart = startpos;
      _balancing = false;
    }

    internal virtual void AddMatch(int cap, int start, int len)
    {
      if (_matches[cap] == null)
      {
        _matches[cap] = new int[2];
      }
      int num = _matchcount[cap];
      if (((num * 2) + 2) > _matches[cap].Length)
      {
        int[] numArray = _matches[cap];
        int[] numArray2 = new int[num * 8];
        for (int i = 0; i < (num * 2); i++)
        {
          numArray2[i] = numArray[i];
        }
        _matches[cap] = numArray2;
      }
      _matches[cap][num * 2] = start;
      _matches[cap][(num * 2) + 1] = len;
      _matchcount[cap] = ++num;
    }

    internal virtual void BalanceMatch(int cap)
    {
      _balancing = true;
      int num = _matchcount[cap];
      int index = (num * 2) - 2;
      //if (cap > 0) index -= 2;
      if (_matches[cap][index] < 0)
      {
        index = -3 - _matches[cap][index];
      }
      index -= 2;
      if ((index >= 0) && (_matches[cap][index] < 0))
      {
        AddMatch(cap, _matches[cap][index], _matches[cap][index + 1]);
      }
      else
      {
        AddMatch(cap, -3 - index, -4 - index);
      }
    }

    internal virtual string GroupToStringImpl(int groupnum)
    {
      int num = _matchcount[groupnum];
      if (num == 0)
      {
        return string.Empty;
      }
      int[] numArray = _matches[groupnum];
      return base._text.Substring(numArray[(num - 1) * 2], numArray[(num * 2) - 1]);
    }

    internal virtual bool IsMatched(int cap)
    {
      return (((cap < _matchcount.Length) && (_matchcount[cap] > 0)) && (_matches[cap][(_matchcount[cap] * 2) - 1] != -2));
    }

    internal string LastGroupToStringImpl()
    {
      return this.GroupToStringImpl(_matchcount.Length - 1);
    }

    internal virtual int MatchIndex(int cap)
    {
      int num = _matches[cap][(_matchcount[cap] * 2) - 2];
      if (num >= 0)
      {
        return num;
      }
      return _matches[cap][-3 - num];
    }

    internal virtual int MatchLength(int cap)
    {
      int num = _matches[cap][(_matchcount[cap] * 2) - 1];
      if (num >= 0)
      {
        return num;
      }
      return _matches[cap][-3 - num];
    }

    /// <summary>
    /// Gets the next match
    /// </summary>
    /// <returns>the next match or this one if not any</returns>
    public Match NextMatch()
    {
      return _regex == null ? this : _regex.Match(base._text, _textpos, _textend);
    }

    internal virtual void RemoveMatch(int cap)
    {
      _matchcount[cap]--;
    }

    internal virtual void Reset(Regex regex, string text, int textbeg, int textend, int textstart)
    {
      _regex = regex;
      base._text = text;
      _textbeg = textbeg;
      _textend = textend;
      _textstart = textstart;
      for (int i = 0, e = _matchcount.Length; i < e; ++i) _matchcount[i] = 0;
      _balancing = false;
    }

    /// <summary>
    /// Gets the result of the replacement
    /// </summary>
    /// <param name="replacement">The replacement string</param>
    /// <returns>The string replaced</returns>
    public virtual string Result(string replacement)
    {
      if (replacement == null)
      {
        throw new ArgumentNullException("replacement");
      }

      if (_regex == null)
      {
        throw new NotSupportedException("null regex");
      }

      return _regex.Replace(this.Value, replacement);
    }

    /// <summary>
    /// Synchronized the match.
    /// </summary>
    /// <param name="inner">The inner match.</param>
    /// <returns>The syncrhonized match</returns>
    public static Match Synchronized(Match inner)
    {
      if (inner == null)
      {
        throw new ArgumentNullException("inner");
      }

      int length = inner._matchcount.Length;
      for (int i = 0; i < length; ++i)
      {
        Group group = inner.Groups[i];
        Synchronized(group);
      }

      return inner;
    }

    internal virtual void Tidy(int textpos)
    {
      int[] numArray = _matches[0];
      base._index = numArray[0];
      base._length = numArray[1];
      _textpos = textpos;
      base._capcount = _matchcount[0];
      if (_balancing)
      {
        for (int i = 0; i < _matchcount.Length; ++i)
        {
          int num2 = _matchcount[i] * 2;
          int[] numArray2 = _matches[i];
          int index = 0;
          index = 0;
          while (index < num2)
          {
            if (numArray2[index] < 0)
            {
              break;
            }

            ++index;
          }

          int num4 = index;
          while (index < num2)
          {
            if (numArray2[index] < 0)
            {
              num4--;
            }
            else
            {
              if (index != num4)
              {
                numArray2[num4] = numArray2[index];
              }
              ++num4;
            }

            ++index;
          }

          _matchcount[i] = num4 / 2;
        }

        _balancing = false;
      }
    }
  }
}
