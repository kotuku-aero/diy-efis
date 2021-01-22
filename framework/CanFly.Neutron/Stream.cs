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
      return Syscall.StreamEof(Handle) == 0;
    }
    /// <summary>
    /// Read bytes from the stream
    /// </summary>
    /// <param name="size">Number of bytes to read</param>
    /// <returns>Data read</returns>
    public byte[] Read(ushort size)
    {
      byte[] value;
      ExceptionHelper.ThrowIfFailed(Syscall.StreamRead(Handle, size, out value));

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
        ExceptionHelper.ThrowIfFailed(Syscall.StreamGetPos(Handle, out pos));

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
        ExceptionHelper.ThrowIfFailed(Syscall.StreamLength(Handle, out length));
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
      ExceptionHelper.ThrowIfFailed(Syscall.StreamPath(Handle, full_path, out path));

      return path;
    }

  }

  public sealed class FileStream : Stream
  {
    private FileStream(uint handle) : base(handle) { }

    public static FileStream Open(string path)
    {
      uint handle;
      ExceptionHelper.ThrowIfFailed(Syscall.FileStreamOpen(path, out handle));

      return new FileStream(handle);
    }

    public static FileStream Create(string path)
    {
      uint handle;
      ExceptionHelper.ThrowIfFailed(Syscall.FileStreamCreate(path, out handle));

      return new FileStream(handle);
    }

    public DirectoryEnumerator EnumerateDirectory(string path)
    {
      uint handle;
      ExceptionHelper.ThrowIfFailed(Syscall.GetDirectoryEnumerator(path, out handle));

      return new DirectoryEnumerator(handle);
    }
  }

  public sealed class RegistryStream : Stream
  {

    private RegistryStream(uint handle) : base(handle) {}

    public static RegistryStream Open(uint parent, string path)
    {
      uint handle;
      ExceptionHelper.ThrowIfFailed(Syscall.RegStreamOpen(parent, path, out handle));

      return new RegistryStream(handle);
    }

    public static RegistryStream Create(uint parent, string path)
    {
      uint handle;
      ExceptionHelper.ThrowIfFailed(Syscall.RegStreamCreate(parent, path, out handle));

      return new RegistryStream(handle);
    }
  }
}