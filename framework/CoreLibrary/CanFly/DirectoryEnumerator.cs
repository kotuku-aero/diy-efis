/*
diy-efis
Copyright (C) 2021 Kotuku Aerospace Limited

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice is
subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.

If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
using System;
using System.Collections;

namespace CanFly
{
  public sealed class DirectoryEntry
  {
    private string _path;
    private string _name;
    private bool _isFile;

    internal DirectoryEntry(string path, string name, bool isFile)
    {
      _path = path;
      _name = name;
      _isFile = isFile;
    }

    public string Path
    {
      get { return _path; }
    }

    public string Name
    {
      get { return _name; }
    }

    public bool IsFile
    {
      get { return _isFile; }
    }

    public Stream Open()
    {
      if (!_isFile)
        throw new InvalidOperationException();

      return FileStream.Open(Path + Name);
    }
  }

  public class DirectoryEnumerator : IEnumerator
  {
    private uint _handle;
    private uint _pos;
    private uint _enumPos;

    internal DirectoryEnumerator(uint hndl) 
    {
      _handle = hndl;
      _pos = 0;
    }

    public object Current
    {
      get
      {
        string path = null;
        string name = null;
        bool isFile;
        Syscall.GetDirectoryEntry(_handle, out path, out name, out isFile);

        _enumPos++;

        return new DirectoryEntry(path, name, isFile);
      }
    }

    public bool MoveNext()
    {
      object x;
      _pos++;
      while (_enumPos < _pos)
        x = Current;

      return true;
    }

    public void Reset()
    {
      while(_enumPos > 0)
      {
        Syscall.RewindDirectoryEntry(_handle);
        _enumPos--;
      }
      _pos = 0;
    }
  }
}
