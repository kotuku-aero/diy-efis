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
      StreamClose(Handle);
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
      StreamDelete(Handle);
      _handle = 0;
    }
    /// <summary>
    /// true if the stream is at end of file
    /// </summary>
    /// <returns></returns>
    public bool Eof()
    {
      return StreamEof(Handle);
    }
    /// <summary>
    /// Read bytes from the stream
    /// </summary>
    /// <param name="size">Number of bytes to read</param>
    /// <returns>Data read</returns>
    public byte[] Read(ushort size)
    {
      return StreamRead(Handle, size);
    }
    /// <summary>
    /// Write bytes to the stream
    /// </summary>
    /// <param name="buffer">Bytes to write</param>
    public void Write(byte[] buffer)
    {
      StreamWrite(Handle, buffer);
    }
    /// <summary>
    /// Position of the read/write pointer for the stream
    /// </summary>
    public uint Pos 
    {
      get { return StreamGetPos(Handle); }
      set { StreamSetPos(Handle, value); }
    }
    /// <summary>
    /// Length of the stream
    /// </summary>
    public uint Length
    {
      get { return StreamLength(Handle); }
      set { StreamTruncate(Handle, value); }
    }
    /// <summary>
    /// Copy the contents of a stream
    /// </summary>
    /// <param name="to">Stream to copy to.</param>
    public void Copy(Stream to)
    {
      StreamCopy(Handle, to.Handle);
    }
    /// <summary>
    /// Path of the stream
    /// </summary>
    /// <param name="full_path">If set then the full path is returned, otherwise only the filename</param>
    /// <returns>Filename of full path</returns>
    public string Path(bool full_path)
    {
      return StreamPath(Handle, full_path);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool StreamEof(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern byte[] StreamRead(uint stream, ushort size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void StreamWrite(uint stream, byte[] buffer);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint StreamGetPos(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void StreamSetPos(uint stream, uint pos);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint StreamLength(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void StreamTruncate(uint stream, uint length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void StreamCopy(uint from, uint to);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern string StreamPath(uint stream, bool full_path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void StreamClose(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void StreamDelete(uint stream);
  }

  public sealed class FileStream : Stream
  {
    private FileStream(uint handle) : base(handle) { }

    public static FileStream Open(string path)
    {
      return new FileStream(FileStreamOpen(path));
    }

    public static FileStream Create(string path)
    {
      return new FileStream(FileStreamCreate(path));
    }


    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void CreateDirectory(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RemoveDirectory(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint GetDirectoryEnumertor(string path);

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint FileStreamOpen(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint FileStreamCreate(string path);

    public DirectoryEnumerator EnumerateDirectory(string path)
    {
      return new DirectoryEnumerator(GetDirectoryEnumertor(path));
    }
  }

  public sealed class RegistryStream : Stream
  {

    private RegistryStream(uint handle) : base(handle) {}

    public static RegistryStream Open(uint parent, string path)
    {
      return new RegistryStream(RegStreamOpen(parent, path));
    }

    public static RegistryStream Create(uint parent, string path)
    {
      return new RegistryStream(RegStreamCreate(parent, path));
    }


    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint RegStreamOpen(uint parent, string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint RegStreamCreate(uint parent, string path);
  }
}