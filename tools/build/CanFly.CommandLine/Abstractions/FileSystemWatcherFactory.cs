﻿using System;

namespace CanFly.CommandLine
{
    [Serializable]
    public class FileSystemWatcherFactory : IFileSystemWatcherFactory
    {
        public IFileSystemWatcher CreateNew()
        {
            return new FileSystemWatcherWrapper();
        }

        public IFileSystemWatcher CreateNew(string path) =>
            new FileSystemWatcherWrapper(path);

        public IFileSystemWatcher CreateNew(string path, string filter)
            => new FileSystemWatcherWrapper(path, filter);
    }
}
