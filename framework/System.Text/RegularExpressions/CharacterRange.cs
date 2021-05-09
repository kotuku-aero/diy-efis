/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Ported to C# for the .Net Micro Framework by <a href="mailto:juliusfriedman@gmail.com">Julius Friedman</a>
 * http://netmf.codeplex.com/
 *
 * Local, nested class for maintaining character ranges for character classes.
 * 
 * Adjusted by Laurent Ellerbach laurent@ellerbach.net
 * 2021/03/13
*/

namespace System.Text.RegularExpressions
{
  /// <summary>
  /// Class for maintaining char ranges for char classes.
  /// </summary>
  internal sealed class CharacterRange
  {
    private int _size = 16;                  // Capacity of current range arrays
    private int[] _minimums = new int[16];   // Range minima
    private int[] _maximums = new int[16];   // Range maxima
    private int _elements = 0;               // Number of range array elements in use

    /// <summary>
    /// Gets the elements.
    /// </summary>
    public int Elements
    {
      get { return _elements; }
    }

    /// <summary>
    /// Gets the minimums
    /// </summary>
    public int[] Minimums 
    {
      get { return _minimums; } 
    }

    /// <summary>
    /// Gets the maximums
    /// </summary>
    public int[] Maximums
    {
      get { return _maximums; }
    }

    /// <summary>
    /// Deletes the range at a given index from the range lists
    /// </summary>
    /// <param name="index">Index of range to delete from minRange and maxRange arrays.</param>
    internal void Delete(int index)
    {
      // Return if no elements left or index is out of range
      if (_elements == 0 || index >= _elements)
      {
        return;
      }

      // Move elements down
      while (++index < _elements)
      {
        if (index - 1 >= 0)
        {
          _minimums[index - 1] = _minimums[index];
          _maximums[index - 1] = _maximums[index];
        }
      }

      // One less element now
      --_elements;
    }

    /// <summary>
    /// Merges a range into the range list, coalescing ranges if possible.
    /// </summary>
    /// <param name="min">min Minimum end of range</param>
    /// <param name="max">max Maximum end of range</param>
    internal void Merge(int min, int max)
    {
      // Loop through ranges
      for (int i = 0; i < _elements; ++i)
      {
        // Min-max is subsumed by minRange[i]-maxRange[i]
        if (min >= _minimums[i] && max <= _maximums[i])
        {
          return;
        }

        // Min-max subsumes minRange[i]-maxRange[i]
        else if (min <= _minimums[i] && max >= _maximums[i])
        {
          Delete(i);
          Merge(min, max);
          return;
        }

        // Min is in the range, but max is outside
        else if (min >= _minimums[i] && min <= _maximums[i])
        {
          min = _minimums[i];
          Delete(i);
          Merge(min, max);
          return;
        }

        // Max is in the range, but min is outside
        else if (max >= _minimums[i] && max <= _maximums[i])
        {
          max = _maximums[i];
          Delete(i);
          Merge(min, max);
          return;
        }
      }

      // Must not overlap any other ranges
      if (_elements >= _size)
      {
        _size *= 2;
        int[] newMin = new int[_size];
        int[] newMax = new int[_size];
        Array.Copy(_minimums, 0, newMin, 0, _elements);
        Array.Copy(_maximums, 0, newMax, 0, _elements);
        _minimums = newMin;
        _maximums = newMax;
      }
      _minimums[_elements] = min;
      _maximums[_elements] = max;
      ++_elements;
    }

    /// <summary>
    /// Removes a range by deleting or shrinking all other ranges
    /// </summary>
    /// <param name="min">Minimum end of range</param>
    /// <param name="max">Maximum end of range</param>
    internal void Remove(int min, int max)
    {
      // Loop through ranges
      for (int i = 0; i < _elements; ++i)
      {
        // minRange[i]-maxRange[i] is subsumed by min-max
        if (_minimums[i] >= min && _maximums[i] <= max)
        {
          Delete(i);
          return;
        }

        // min-max is subsumed by minRange[i]-maxRange[i]
        else if (min >= _minimums[i] && max <= _maximums[i])
        {
          int minr = _minimums[i];
          int maxr = _maximums[i];
          Delete(i);
          if (minr < min)
          {
            Merge(minr, min - 1);
          }
          if (max < maxr)
          {
            Merge(max + 1, maxr);
          }
          return;
        }

        // minRange is in the range, but maxRange is outside
        else if (_minimums[i] >= min && _minimums[i] <= max)
        {
          _minimums[i] = max + 1;
          return;
        }

        // maxRange is in the range, but minRange is outside
        else if (_maximums[i] >= min && _maximums[i] <= max)
        {
          _maximums[i] = min - 1;
          return;
        }
      }
    }

    /// <summary>
    /// Includes (or excludes) the range from min to max, inclusive.
    /// </summary>
    /// <param name="min">Minimum end of range</param>
    /// <param name="max">Maximum end of range</param>
    /// <param name="include">True if range should be included.  False otherwise.</param>
    internal void Include(int min, int max, bool include)
    {
      if (include)
      {
        Merge(min, max);
      }
      else
      {
        Remove(min, max);
      }
    }

    /// <summary>
    /// Includes a range with the same min and max
    /// </summary>
    /// <param name="minmax">Minimum and maximum end of range (inclusive)</param>
    /// <param name="shouldInclude">True if range should be included.  False otherwise.</param>
    internal void Include(char minmax, bool shouldInclude)
    {
      Include(minmax, minmax, shouldInclude);
    }
  }
}
