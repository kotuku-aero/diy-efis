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
using System.Runtime.CompilerServices;

namespace CanFly
{
  public class Stream : IDisposable
  {
    private uint _handle;
    private Stream() { }

    protected internal Stream(uint handle)
    {
      _handle = handle;
    }
    
    public void Dispose()
    {
      Syscall.StreamClose(Handle);
    }

    /// <summary>
    /// Underlying OS handle
    /// </summary>
    internal uint Handle
    {
      get { return _handle; }
    }

    /// <summary>
    /// Delete the underlying stream
    /// </summary>
    public void Delete()
    {
      Syscall.StreamDelete(Handle);
      _handle = 0;
    }
    /// <summary>
    /// true if the stream is at end of file
    /// </summary>
    /// <returns></returns>
    public bool Eof()
    {
      bool isEof;
      Syscall.StreamEof(Handle, out isEof);

      return isEof;
    }
    /// <summary>
    /// Read bytes from the stream
    /// </summary>
    /// <param name="size">Number of bytes to read</param>
    /// <returns>Data read</returns>
    public byte[] Read(ushort size)
    {
      byte[] value;
      Syscall.StreamRead(Handle, size, out value);

      return value;
    }
    /// <summary>
    /// Write bytes to the stream
    /// </summary>
    /// <param name="buffer">Bytes to write</param>
    public void Write(byte[] buffer)
    {
      Syscall.StreamWrite(Handle, buffer);
    }
    /// <summary>
    /// Position of the read/write pointer for the stream
    /// </summary>
    public uint Pos 
    {
      get
      {
        uint pos;
        Syscall.StreamGetPos(Handle, out pos);

        return pos;
      }
      set { Syscall.StreamSetPos(Handle, value); }
    }
    /// <summary>
    /// Length of the stream
    /// </summary>
    public uint Length
    {
      get 
      {
        uint length;
        Syscall.StreamLength(Handle, out length);
        return length;
      }
      set { Syscall.StreamTruncate(Handle, value); }
    }
    /// <summary>
    /// Copy the contents of a stream
    /// </summary>
    /// <param name="to">Stream to copy to.</param>
    public void Copy(Stream to)
    {
      Syscall.StreamCopy(Handle, to.Handle);
    }
    /// <summary>
    /// Path of the stream
    /// </summary>
    /// <param name="full_path">If set then the full path is returned, otherwise only the filename</param>
    /// <returns>Filename of full path</returns>
    public string Path(bool full_path)
    {
      string path;
      Syscall.StreamPath(Handle, full_path, out path);

      return path;
    }

  }

  public sealed class FileStream : Stream
  {
    private FileStream(uint handle) : base(handle) { }

    public static FileStream Open(string path)
    {
      uint handle;
      Syscall.FileStreamOpen(path, out handle);

      return new FileStream(handle);
    }

    public static FileStream Create(string path)
    {
      uint handle;
      Syscall.FileStreamCreate(path, out handle);

      return new FileStream(handle);
    }

    public DirectoryEnumerator EnumerateDirectory(string path)
    {
      uint handle;
      Syscall.GetDirectoryEnumerator(path, out handle);

      return new DirectoryEnumerator(handle);
    }
  }

  public sealed class RegistryStream : Stream
  {

    private RegistryStream(uint handle) : base(handle) {}

    public static RegistryStream Open(uint parent, string path)
    {
      uint handle;
      Syscall.RegStreamOpen(parent, path, out handle);

      return new RegistryStream(handle);
    }

    public static RegistryStream Create(uint parent, string path)
    {
      uint handle;
      Syscall.RegStreamCreate(parent, path, out handle);

      return new RegistryStream(handle);
    }
  }
}