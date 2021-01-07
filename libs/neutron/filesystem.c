/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include "neutron.h"
#include "stream.h"
#include "bsp.h"

typedef struct _mount_point_t {
  size_t version;
  const char *mount_point;
  filesystem_t *file_type;
  nand_device_t *device;
  } mount_point_t;

typedef struct _file_stream_t {
  stream_handle_t stream;
  // platform specific information
  mount_point_t *mount_point;
  uint32_t fd;
  // position in the stream
  uint32_t position;
  } file_stream_t;

// this is the number of mounts allows.  Generally 1 per physical device
// or flash device.
#ifndef NUM_MOUNTS
#define NUM_MOUNTS 4
#endif

static mount_point_t mounts[NUM_MOUNTS];

result_t filesystem_init()
  {
  memset(mounts, 0, sizeof(mount_point_t) * NUM_MOUNTS);

  return s_ok;
  }

// the root is an empty string
static const char root_alias[1] = { 0 };

// this assumes a canonical path.  That is /<dir>/<dir>
// cannot have and of . .. or // in the path
static result_t locate_mount_point(const char *path, mount_point_t **mount)
  {
  if (path == 0)
    return e_path_not_found;

  if (mount == 0)
    return e_bad_parameter;

  *mount = 0;

  size_t len_path = strlen(path);
  for (int i = 0; i < NUM_MOUNTS; i++)
    {
    if (mounts[i].mount_point == 0)
      continue;

    size_t mount_length = strlen(mounts[i].mount_point);

    // this is the case of the root file system.
    // it is registered as / but the rule is that the path must not end in a /
    // so that the mount can be enumerated
    if (mount_length == 0)
      {
      *mount = &mounts[i];        // always the last one
      return s_ok;
      }

    // if the path is not at least as long as the path then
    // it cannot be a mount
    if (mount_length > len_path)
      continue;

    if (strncmp(mounts[i].mount_point, path, mount_length) == 0)
      {
      *mount = &mounts[i];
      return s_ok;
      }

    return e_path_not_found;
    }

  // this also happens when no root filesystem is loaded.
  return e_path_not_found;
  }

result_t mount(const char *mount_point, filesystem_t *file_type, nand_device_t *device, handle_t *fshndl)
  {
  // check the params
  if (mount_point == 0 ||
    file_type == 0 ||
    device == 0 ||
    fshndl == 0)
    return e_bad_parameter;

  // check the mount point
  size_t len = strlen(mount_point);
  if (len < 1)
    return e_bad_parameter;

  // check for illegal strings
  for (size_t i = 0; i < len; i++)
    {
    if (mount_point[i] == '.' ||
      ((i < (len - 1)) && mount_point[i] == '/' && mount_point[i + 1] == '/'))
      return e_bad_parameter;
    }

  if (strcmp(mount_point, "/") == 0)
    {
    // special case.  The root mount point is always the last one.
    if (mounts[NUM_MOUNTS - 1].mount_point != 0)
      return e_invalid_operation;

    // this is the root file system
    mounts[NUM_MOUNTS - 1].mount_point = root_alias;
    mounts[NUM_MOUNTS - 1].device = device;
    mounts[NUM_MOUNTS - 1].file_type = file_type;
    mounts[NUM_MOUNTS - 1].version = sizeof(mount_point_t);

    *fshndl = &mounts[NUM_MOUNTS - 1];
    }
  else
    {
    // handle cases other than the root filesystem
    mount_point_t *existing_mount = 0;
    // first check that is not a mount similar
    if (succeeded(locate_mount_point(mount_point, &existing_mount)))
      {
      // there is a mount the same or a parent of this mount
      // which is not supported.
      return e_bad_parameter;
      }

    // not found so continue
    for (int i = 0; i < (NUM_MOUNTS - 1); i++)
      {
      if (mounts[i].mount_point == 0)
        {
        // a free slot so use it
        mounts[i].mount_point = mount_point;
        mounts[i].device = device;
        mounts[i].file_type = file_type;
        mounts[i].version = sizeof(mount_point_t);
        break;
        }
      }
    }

  // the fs is mounted so call the mount
  return file_type->mount(file_type, device);
  }

static mount_point_t *check_handle(handle_t fshndl)
  {
  mount_point_t *mount = (mount_point_t *)fshndl;

  // pointer needs to be in the static array
  if (mount < mounts || mount > &mounts[NUM_MOUNTS + 1])
    return 0;

  // can't do a shift/divide here as integer math
  for (int i = 0; i < NUM_MOUNTS; i++)
    {
    if ((mount == &mounts[i]) &&
      (mount->version == sizeof(mount_point_t)))
      return mount;
    }

  return 0;
  }

extern result_t umount(handle_t fshndl)
  {
  mount_point_t *mount = check_handle(fshndl);

  if (mount == 0)
    return e_bad_parameter;

  // cannot unmount the root file system (ever)
  if (mount->mount_point == 0 ||
    mount->mount_point[0] == 0)
    return e_bad_parameter;

  // call to unmount a fs
  result_t result;

  if (failed(result = mount->file_type->unmount(mount->file_type, mount->device)))
    return result;

  // remove the mount
  memset(mount, 0, sizeof(mount_point_t));

  return s_ok;
  }

extern result_t fs_sync(handle_t fshndl)
  {
  mount_point_t *mount = check_handle(fshndl);

  if (mount == 0)
    return e_bad_parameter;

  return mount->file_type->fssync(mount->file_type, mount->device);
  }

static result_t as_file_stream(stream_handle_t *stream, file_stream_t **hndl)
  {
  if (stream == 0 || hndl == 0)
    return e_bad_parameter;

  if (stream->version != sizeof(file_stream_t))
    return e_invalid_handle;

  *hndl = (file_stream_t *)stream;

  // check the driver
  if(check_handle((*hndl)->mount_point) == 0)
    return e_invalid_handle;

  return s_ok;
  }

static result_t file_stream_eof(stream_handle_t *stream) 
  {
  result_t result;
  file_stream_t *fshndl;
  if(failed(result = as_file_stream(stream, &fshndl)))
    return result; 

  stat_t fd_stat;
  if (failed(result = fshndl->mount_point->file_type->stat(fshndl->mount_point->file_type, fshndl->mount_point->device, fshndl->fd, &fd_stat)))
    return result;

  return fshndl->position >= fd_stat.st_size;
  }

static result_t file_stream_read(stream_handle_t *stream, void *buffer, uint16_t size, uint16_t *read)
  {
  result_t result;
  file_stream_t *fshndl;
  if (failed(result = as_file_stream(stream, &fshndl)))
    return result;

  uint32_t bytes_read;
  if(failed(result = fshndl->mount_point->file_type->read(fshndl->mount_point->file_type, 
    fshndl->mount_point->device, fshndl->fd, fshndl->position, buffer, size, &bytes_read)))
    return result;

  if (read != 0)
    *read = (uint16_t) bytes_read;

  fshndl->position += bytes_read;

  return s_ok;
  }

static result_t file_stream_write(stream_handle_t *stream, const void *buffer, uint16_t size)
  {
  result_t result;
  file_stream_t *fshndl;
  if (failed(result = as_file_stream(stream, &fshndl)))
    return result;

  uint32_t bytes_written;
  if (failed(result = fshndl->mount_point->file_type->write(fshndl->mount_point->file_type, 
    fshndl->mount_point->device, fshndl->fd, fshndl->position, buffer, size, &bytes_written)))
    return result;

  fshndl->position += bytes_written;

  return s_ok;
  }

static result_t file_stream_getpos(stream_handle_t *stream, uint32_t *pos)
  {
  if (stream == 0 || pos == 0)
    return e_bad_parameter;

  result_t result;
  file_stream_t *fshndl;
  if (failed(result = as_file_stream(stream, &fshndl)))
    return result;

  *pos = fshndl->position;
  return s_ok;
  }

static result_t file_stream_setpos(stream_handle_t *stream, uint32_t pos)
  {
  result_t result;
  file_stream_t *fshndl;
  if (failed(result = as_file_stream(stream, &fshndl)))
    return result;

  stat_t file_stat;
  // get the length
  if (failed(result = fshndl->mount_point->file_type->stat(fshndl->mount_point->file_type, 
    fshndl->mount_point->device, fshndl->fd, &file_stat)))
    return result;

  if (pos > file_stat.st_size)
    pos = (uint32_t) file_stat.st_size;

  // get the length
  fshndl->position = pos;
  return s_ok;
  }

static result_t file_stream_length(stream_handle_t *stream, uint32_t *length)
  {
  if (stream == 0 || length == 0)
    return e_bad_parameter;

  result_t result;
  file_stream_t *fshndl;
  if (failed(result = as_file_stream(stream, &fshndl)))
    return result;

  stat_t file_stat;
  // get the length
  if (failed(result = fshndl->mount_point->file_type->stat(fshndl->mount_point->file_type, 
    fshndl->mount_point->device, fshndl->fd, &file_stat)))
    return result;

  *length = file_stat.st_size;
  return s_ok;
  }

static result_t file_stream_truncate(stream_handle_t *stream, uint32_t length)
  {
  if (stream == 0)
    return e_bad_parameter;

  result_t result;
  file_stream_t *fshndl;
  if (failed(result = as_file_stream(stream, &fshndl)))
    return result;

  stat_t file_stat;
  // get the length
  if (failed(result = fshndl->mount_point->file_type->stat(fshndl->mount_point->file_type, 
    fshndl->mount_point->device, fshndl->fd, &file_stat)))
    return result;

  if (length > file_stat.st_size)
    return s_ok;

  if (failed(result = fshndl->mount_point->file_type->truncate(fshndl->mount_point->file_type, 
    fshndl->mount_point->device, fshndl->fd, length)))
    return result;

  if (fshndl->position > length)
    fshndl->position = length;

  return s_ok;
  }

static result_t file_stream_close(stream_handle_t *stream)
  {
  if (stream == 0)
    return e_bad_parameter;

  result_t result;
  file_stream_t *fshndl;
  if (failed(result = as_file_stream(stream, &fshndl)))
    return result;

  if (failed(result = fshndl->mount_point->file_type->close(fshndl->mount_point->file_type, 
    fshndl->mount_point->device, fshndl->fd)))
    return result;

  // remove the handle.
  // in case the caller re-uses it, invalidate it
  memset(fshndl, 0, sizeof(file_stream_t));
  neutron_free(fshndl);
  return s_ok;
  }

static result_t file_stream_delete(stream_handle_t *stream)
  {
  if (stream == 0)
    return e_bad_parameter;

  result_t result;
  file_stream_t *fshndl;
  if (failed(result = as_file_stream(stream, &fshndl)))
    return result;

  if (failed(result = fshndl->mount_point->file_type->unlink(fshndl->mount_point->file_type, 
    fshndl->mount_point->device, fshndl->fd)))
    return result;

  fshndl->fd = 0;
  return s_ok;
  }

static result_t file_stream_path(stream_handle_t *stream, bool full_path, uint16_t len, char *path)
  {
  if (stream == 0 || path == 0 || len < 2)
    return e_bad_parameter;

  result_t result;
  file_stream_t *fshndl;
  if (failed(result = as_file_stream(stream, &fshndl)))
    return result;

  // if full path, put the name of the mount first
  uint16_t bytes_remaining = len;
  if (full_path)
    {
    strncpy(path, fshndl->mount_point->mount_point, bytes_remaining);
    size_t copied = strlen(path);
    if (copied == 0)
      {
      strcpy(path, "/");
      copied = 1;
      }
    bytes_remaining -= (uint16_t) copied;
    // and skip the buffer
    path += copied;
    }

  return fshndl->mount_point->file_type->getpath(fshndl->mount_point->file_type, 
    fshndl->mount_point->device, fshndl->fd, full_path, path, bytes_remaining);
  }

result_t stream_create(const char *path, stream_p *stream)
  {
  if (path == 0 || stream == 0)
    return e_bad_parameter;

  // get the file system to use.
  result_t result;
  mount_point_t *mount;
  if (failed(result = locate_mount_point(path, &mount)))
    return result;

  // open the file handle by skipping to the start of the filename
  // relative to the mount point
  const char *fs_path = path + strlen(mount->mount_point);
  uint32_t fd;
  if (failed(result = mount->file_type->create(mount->file_type, mount->device, fs_path, &fd)))
    return result;

  file_stream_t *file_stream = (file_stream_t *)neutron_malloc(sizeof(file_stream_t));

  if (file_stream == 0)
    return e_not_enough_memory;

  file_stream->fd = fd;
  file_stream->mount_point = mount;
  file_stream->stream.version = sizeof(file_stream_t);
  file_stream->stream.stream_close = file_stream_close;
  file_stream->stream.stream_delete = file_stream_delete;
  file_stream->stream.stream_eof = file_stream_eof;
  file_stream->stream.stream_getpos = file_stream_getpos;
  file_stream->stream.stream_length = file_stream_length;
  file_stream->stream.stream_path = file_stream_path;
  file_stream->stream.stream_read = file_stream_read;
  file_stream->stream.stream_setpos = file_stream_setpos;
  file_stream->stream.stream_truncate = file_stream_truncate;
  file_stream->stream.stream_write = file_stream_write;

  return s_ok;
  }

result_t stream_open(const char *path, stream_p *stream)
  {
  if (path == 0 || stream == 0)
    return e_bad_parameter;

  // get the file system to use.
  result_t result;
  mount_point_t *mount;
  if (failed(result = locate_mount_point(path, &mount)))
    return result;

  // open the file handle by skipping to the start of the filename
  // relative to the mount point
  const char *fs_path = path + strlen(mount->mount_point);
  uint32_t fd;
  if (failed(result = mount->file_type->open(mount->file_type, mount->device, fs_path, &fd)))
    return result;

  file_stream_t *file_stream = (file_stream_t *)neutron_malloc(sizeof(file_stream_t));

  if (file_stream == 0)
    return e_not_enough_memory;

  file_stream->fd = fd;
  file_stream->mount_point = mount;
  file_stream->stream.version = sizeof(file_stream_t);
  file_stream->stream.stream_close = file_stream_close;
  file_stream->stream.stream_delete = file_stream_delete;
  file_stream->stream.stream_eof = file_stream_eof;
  file_stream->stream.stream_getpos = file_stream_getpos;
  file_stream->stream.stream_length = file_stream_length;
  file_stream->stream.stream_path = file_stream_path;
  file_stream->stream.stream_read = file_stream_read;
  file_stream->stream.stream_setpos = file_stream_setpos;
  file_stream->stream.stream_truncate = file_stream_truncate;
  file_stream->stream.stream_write = file_stream_write;

  return s_ok;
  }

result_t stream_rename(stream_p stream, const char *new_filename)
  {
  if (stream == 0 || new_filename == 0)
    return e_bad_parameter;

  result_t result;
  file_stream_t *fshndl;
  if (failed(result = as_file_stream((stream_handle_t *)stream, &fshndl)))
    return result;

  return fshndl->mount_point->file_type->rename(fshndl->mount_point->file_type, 
    fshndl->mount_point->device, fshndl->fd, new_filename);
  }

result_t create_directory(const char *path)
  {
  if (path == 0)
    return e_bad_pointer;

  // determine which mount_point

  // get the file system to use.
  result_t result;
  mount_point_t *mount;
  if (failed(result = locate_mount_point(path, &mount)))
    return result;

  // open the file handle by skipping to the start of the filename
  // relative to the mount point
  const char *fs_path = path + strlen(mount->mount_point);
  return mount->file_type->mkdir(mount->file_type, mount->device, fs_path);
  }

result_t remove_directory(const char *path)
  {
  if (path == 0)
    return e_bad_pointer;

  // determine which mount_point

  // get the file system to use.
  result_t result;
  mount_point_t *mount;
  if (failed(result = locate_mount_point(path, &mount)))
    return result;

  // open the file handle by skipping to the start of the filename
  // relative to the mount point
  const char *fs_path = path + strlen(mount->mount_point);
  return mount->file_type->rmdir(mount->file_type, mount->device, fs_path);
  }

typedef struct _dirp_t {
  size_t version;
  uint32_t dirp;
  mount_point_t *mount;
  } dirp_t;

result_t open_directory(const char *dirname, handle_t *dirp)
  {
  if (dirname == 0 || dirp == 0)
    return e_bad_pointer;

  // get the file system to use.
  result_t result;
  mount_point_t *mount;
  if (failed(result = locate_mount_point(dirname, &mount)))
    return result;

  // open the file handle by skipping to the start of the filename
  // relative to the mount point
  const char *fs_path = dirname + strlen(mount->mount_point);
  uint32_t fs_dirp;
  if (failed(result = mount->file_type->opendir(mount->file_type, mount->device, fs_path, &fs_dirp)))
    return result;

  dirp_t *dir = (dirp_t *)neutron_malloc(sizeof(dirp_t));
  dir->version = sizeof(dirp_t);
  dir->mount = mount;
  dir->dirp = fs_dirp;

  *dirp = dir;

  return s_ok;
  }

static result_t validate_dirp(handle_t hndl, dirp_t **dirp)
  {
  if (hndl == 0)
    return e_bad_parameter;

  *dirp = (dirp_t *)hndl;

  if (check_handle((*dirp)->mount) == 0)
    return e_bad_pointer;

  return s_ok;
  }

result_t read_directory(handle_t hndl, dir_entry_type *et, char *buffer, size_t len)
  {
  if (et == 0 || buffer == 0 || len < 2)
    return e_bad_parameter;

  result_t result;
  dirp_t *dirp;

  if (failed(result = validate_dirp(hndl, &dirp)))
    return result;

  return dirp->mount->file_type->readdir(dirp->mount->file_type, dirp->mount->device, dirp->dirp, et, buffer, len);
  }

result_t rewind_directory(handle_t hndl)
  {
  result_t result;
  dirp_t *dirp;

  if (failed(result = validate_dirp(hndl, &dirp)))
    return result;

  return dirp->mount->file_type->rewinddir(dirp->mount->file_type, dirp->mount->device, dirp->dirp);
  }

result_t close_directory(handle_t hndl)
  {
  result_t result;
  dirp_t *dirp;

  if (failed(result = validate_dirp(hndl, &dirp)))
    return result;

  memset(dirp, 0, sizeof(dirp_t));

  neutron_free(dirp);
  return s_ok;
  }

result_t stream_sync(stream_p stream)
  {
  if (stream == 0)
    return e_bad_parameter;

  result_t result;
  file_stream_t *fshndl;
  if (failed(result = as_file_stream((stream_handle_t *)stream, &fshndl)))
    return result;

  return fshndl->mount_point->file_type->sync(fshndl->mount_point->file_type, fshndl->mount_point->device, fshndl->fd);
  }

result_t freespace(const char *path, uint32_t *space)
  {
  if (path == 0 || space == 0)
    return e_bad_pointer;

  // get the file system to use.
  result_t result;
  mount_point_t *mount;
  if (failed(result = locate_mount_point(path, &mount)))
    return result;

  // open the file handle by skipping to the start of the filename
  // relative to the mount point
  const char *fs_path = path + strlen(mount->mount_point);

  return mount->file_type->freespace(mount->file_type, mount->device, fs_path, space);
  }

result_t totalspace(const char *path, uint32_t *space)
  {
  if (path == 0 || space == 0)
    return e_bad_pointer;

  // get the file system to use.
  result_t result;
  mount_point_t *mount;
  if (failed(result = locate_mount_point(path, &mount)))
    return result;

  // open the file handle by skipping to the start of the filename
  // relative to the mount point
  const char *fs_path = path + strlen(mount->mount_point);

  return mount->file_type->totalspace(mount->file_type, mount->device, fs_path, space);
  }