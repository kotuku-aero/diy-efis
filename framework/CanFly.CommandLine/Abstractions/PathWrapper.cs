﻿using System;
using System.IO;

namespace CanFly.CommandLine
{
    [Serializable]
    public class PathWrapper : PathBase
    {
        public PathWrapper(IFileSystem fileSystem) : base(fileSystem)
        {
        }

        public override char AltDirectorySeparatorChar
        {
            get { return Path.AltDirectorySeparatorChar; }
        }

        public override char DirectorySeparatorChar
        {
            get { return Path.DirectorySeparatorChar; }
        }

        [Obsolete("Please use GetInvalidPathChars or GetInvalidFileNameChars instead.")]
        public override char[] InvalidPathChars
        {
            get { return Path.InvalidPathChars; }
        }

        public override char PathSeparator
        {
            get { return Path.PathSeparator; }
        }

        public override char VolumeSeparatorChar
        {
            get { return Path.VolumeSeparatorChar; }
        }

        public override string ChangeExtension(string path, string extension)
        {
            return Path.ChangeExtension(path, extension);
        }

        public override string Combine(params string[] paths)
        {
            return Path.Combine(paths);
        }

        public override string Combine(string path1, string path2)
        {
            return Path.Combine(path1, path2);
        }

        public override string Combine(string path1, string path2, string path3)
        {
            return Path.Combine(path1, path2, path3);
        }

        public override string Combine(string path1, string path2, string path3, string path4)
        {
            return Path.Combine(path1, path2, path3, path4);
        }

        public override string GetDirectoryName(string path)
        {
            return Path.GetDirectoryName(path);
        }

        public override string GetExtension(string path)
        {
            return Path.GetExtension(path);
        }

        public override string GetFileName(string path)
        {
            return Path.GetFileName(path);
        }

        public override string GetFileNameWithoutExtension(string path)
        {
            return Path.GetFileNameWithoutExtension(path);
        }

        public override string GetFullPath(string path)
        {
            return Path.GetFullPath(path);
        }

        public override char[] GetInvalidFileNameChars()
        {
            return Path.GetInvalidFileNameChars();
        }

        public override char[] GetInvalidPathChars()
        {
            return Path.GetInvalidPathChars();
        }

        public override string GetPathRoot(string path)
        {
            return Path.GetPathRoot(path);
        }

        public override string GetRandomFileName()
        {
            return Path.GetRandomFileName();
        }

        public override string GetTempFileName()
        {
            return Path.GetTempFileName();
        }

        public override string GetTempPath()
        {
            return Path.GetTempPath();
        }

        public override bool HasExtension(string path)
        {
            return Path.HasExtension(path);
        }

#if FEATURE_ADVANCED_PATH_OPERATIONS
        public override bool IsPathFullyQualified(string path)
        {
            return Path.IsPathFullyQualified(path);
        }

        public override string GetRelativePath(string relativeTo, string path)
        {
            return Path.GetRelativePath(relativeTo, path);
        }
#endif

#if FEATURE_PATH_JOIN_WITH_SPAN
        /// <inheritdoc />
        public override string Join(ReadOnlySpan<char> path1, ReadOnlySpan<char> path2) =>
            Path.Join(path2, path2);

        /// <inheritdoc />
        public override string Join(ReadOnlySpan<char> path1, ReadOnlySpan<char> path2, ReadOnlySpan<char> path3) =>
            Path.Join(path2, path2, path3);
        
        /// <inheritdoc />
        public override bool TryJoin(ReadOnlySpan<char> path1, ReadOnlySpan<char> path2, Span<char> destination, out int charsWritten) =>
            Path.TryJoin(path2, path2, destination, out charsWritten);
        
        /// <inheritdoc />
        public override bool TryJoin(ReadOnlySpan<char> path1, ReadOnlySpan<char> path2, ReadOnlySpan<char> path3, Span<char> destination, out int charsWritten) =>
            Path.TryJoin(path2, path2, path3, destination, out charsWritten);
#endif

        public override bool IsPathRooted(string path)
        {
            return Path.IsPathRooted(path);
        }
    }
}
