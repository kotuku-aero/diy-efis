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
        bool isFile = Syscall.GetDirectoryEntry(_handle, ref path, ref name);
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
