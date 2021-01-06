using System;
using System.Runtime.CompilerServices;

namespace CanFly
{
  public abstract class Stream : IDisposable
  {
    private uint _handle;

    protected Stream(uint handle)
    {
      _handle = handle;
    }
    
    public void Dispose()
    {
      OnDispose();
    }

    /// <summary>
    /// Underlying OS handle
    /// </summary>
    internal uint Handle
    {
      get { return _handle; }
    }


    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void CreateDirectory(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RemoveDirectory(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint GetirectoryEnumertor(string path);

    public DirectoryEnumerator EnumerateDirectory(string path)
    {
      return new DirectoryEnumerator(GetirectoryEnumertor(path));
    }

    protected abstract void OnDispose();
    /// <summary>
    /// Delete the underlying stream
    /// </summary>
    public abstract void Delete();
    /// <summary>
    /// true if the stream is at end of file
    /// </summary>
    /// <returns></returns>
    public abstract bool Eof();
    /// <summary>
    /// Read bytes from the stream
    /// </summary>
    /// <param name="size">Number of bytes to read</param>
    /// <returns>Data read</returns>
    public abstract byte[] Read(ushort size);
    /// <summary>
    /// Write bytes to the stream
    /// </summary>
    /// <param name="buffer">Bytes to write</param>
    public abstract void Write(byte[] buffer);
    /// <summary>
    /// Position of the read/write pointer for the stream
    /// </summary>
    public abstract uint Pos { get; set; }
    /// <summary>
    /// Length of the stream
    /// </summary>
    public abstract uint Length { get; }
    /// <summary>
    /// Set the length of the streaam
    /// </summary>
    /// <param name="length">New Length, if greather than length then length remains unchanged</param>
    public abstract void Truncate(uint length);
    /// <summary>
    /// Copy the contents of a stream
    /// </summary>
    /// <param name="to">Stream to copy to.</param>
    public abstract void Copy(Stream to);
    /// <summary>
    /// Path of the stream
    /// </summary>
    /// <param name="full_path">If set then the full path is returned, otherwise only the filename</param>
    /// <returns>Filename of full path</returns>
    public abstract string Path(bool full_path);
  }

  public class FileStream : Stream
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

    protected override void OnDispose()
    {
      FileStreamClose(Handle);
    }

    public override uint Pos { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

    public override uint Length => throw new NotImplementedException();


    public override void Delete()
    {
      throw new NotImplementedException();
    }

    public override bool Eof()
    {
      throw new NotImplementedException();
    }

    public override byte[] Read(ushort size)
    {
      throw new NotImplementedException();
    }

    public override void Write(byte[] buffer)
    {
      throw new NotImplementedException();
    }

    public override void Truncate(uint length)
    {
      throw new NotImplementedException();
    }

    public override void Copy(Stream to)
    {
      throw new NotImplementedException();
    }

    public override string Path(bool full_path)
    {
      throw new NotImplementedException();
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint FileStreamOpen(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint FileStreamCreate(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void FileStreamClose(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void FileStreamDelete(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool FileStreamEof(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern byte[] FileStreamRead(uint stream, ushort size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void FileStreamWrite(uint stream, byte[] buffer);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint FileStreamGetPos(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void FileStreamSetPos(uint stream, uint pos);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint FileStreamLength(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void FileStreamTruncate(uint stream, uint length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void FileStreamCopy(uint from, uint to);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern string FileStreamPath(uint stream, bool full_path);
  }

  public class RegistryStream : Stream
  {

    private RegistryStream(uint handle) : base(handle) {}

    public static RegistryStream Open(uint parent, string path)
    {
      return new RegistryStream(RegStreamOpen(parent, path));
    }

    protected override void OnDispose()
    {
      RegStreamClose(Handle);
    }
    public override uint Pos { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

    public override uint Length => throw new NotImplementedException();

    public override void Delete()
    {
      throw new NotImplementedException();
    }

    public override bool Eof()
    {
      throw new NotImplementedException();
    }

    public override byte[] Read(ushort size)
    {
      throw new NotImplementedException();
    }

    public override void Write(byte[] buffer)
    {
      throw new NotImplementedException();
    }

    public override void Truncate(uint length)
    {
      throw new NotImplementedException();
    }

    public override void Copy(Stream to)
    {
      throw new NotImplementedException();
    }

    public override string Path(bool full_path)
    {
      throw new NotImplementedException();
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint RegStreamOpen(uint parent, string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint RegStreamCreate(uint parent, string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void RegStreamClose(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void RegStreamDelete(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern bool RegStreamEof(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern byte[] RegStreamRead(uint stream, ushort size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void RegStreamWrite(uint stream, byte[] buffer);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint RegStreamGetPos(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void RegStreamSetPos(uint stream, uint pos);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint RegStreamLength(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void RegStreamTruncate(uint stream, uint length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void RegStreamCopy(uint from, uint to);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern string RegStreamPath(uint stream, bool full_path);
  }
}