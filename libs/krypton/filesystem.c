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
#include "../neutron/neutron.h"
#include "../neutron/bsp.h"
#include <Windows.h>

typedef struct _krypton_fs_t {
  filesystem_t fs;
  char file_base[MAX_PATH+1];
  } krypton_fs_t;

static result_t krypton_mount(filesystem_p fs, nand_device_t *device) 
  {
  return s_ok; 
  }

static result_t krypton_unmount(filesystem_p fs, nand_device_t *device) 
  {
  return s_ok;
  }

static result_t krypton_fssync(filesystem_p fs, nand_device_t *device) 
  {
  return s_ok; 
  }

static result_t krypton_fsync(filesystem_p fs, nand_device_t *device, uint32_t fd) 
  {
  return s_ok; 
  }

static result_t krypton_format(filesystem_p fs, nand_device_t *device) 
  {
  return e_not_implemented; 
  }

static result_t krypton_sync(filesystem_p fs, nand_device_t *device, uint32_t fshndl)
  {
  return s_ok; 
  }

static result_t krypton_freespace(filesystem_p fs, nand_device_t *device, const char *path, uint32_t *space)
  {
  return e_not_implemented; 
  }
static result_t krypton_totalspace(filesystem_p fs, nand_device_t *device, const char *path, uint32_t *space) 
  {
  return e_not_implemented; 
  }

static result_t as_kyrpton(filesystem_p fs, krypton_fs_t **handle)
  {
  if (fs == 0 || handle == 0)
    return e_bad_parameter;

  *handle = (krypton_fs_t *)fs;
  if ((*handle)->fs.version != sizeof(krypton_fs_t))
    return e_bad_handle;

  return s_ok;
  }

// merge 2 paths, but don't support . or .. in path yet (this is for testing only)
// to make this consistent we convert all paths to lower case.  Windows is case insensitive
// linux and canfly on embedded devices are case sensitive.  so we assume lower case
// on the test system
static result_t path_combine(const char *path_in, const char *path_more, char *path_out, size_t len)
  {
  if (len < 2 || path_in == 0 || path_more == 0 || path_out == 0)
    return e_bad_parameter;
  
  // always support a trailing null so take it now.
  len--;

  // first copy the path_in which is considered to be a valid root path.
  // don't use autoincrement in assign as we want path_out to point to the
  // last char
  for (; len > 0 && *path_in != 0; len--, path_out++, path_in++)
    {
    if (*path_in == '\\')
      *path_out = '/';
    else
      *path_out = tolower(*path_in);
    }

  if (*path_out != '/')
    {
    *path_out++ = '/';
    len--;
    }
  else
    path_out++;     // skip the trailing '/'

  if (len == 0)
    goto exit_copy;

  // skip a possible leading '.'
  if (*path_more == '/')
    path_more++;

  for (; len > 0 && *path_more != 0; len--)
    *path_out++ = tolower(*path_more++);

exit_copy:
  // this was always available from above
  *path_out = 0;
  return s_ok;
  }

static result_t krypton_open(filesystem_p fs, nand_device_t *device, const char *path, uint32_t *fd) 
  {
  if (fs == 0 || path == 0 || fd == 0)
    return e_bad_parameter;

  krypton_fs_t *handle;
  result_t result;
  if(failed(result = as_kyrpton(fs, &handle)))
    return result;

  char file_path[MAX_PATH];
  if (failed(result = path_combine(handle->file_base, path, file_path, MAX_PATH)))
    return result;

  HANDLE hndl = CreateFile(file_path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hndl == INVALID_HANDLE_VALUE)
    result = GetLastError();
  else
    {
    *fd = (uint32_t)hndl;
    result = s_ok;
    }

  return result;
  }

static result_t krypton_create(filesystem_p fs, nand_device_t *device, const char *path, uint32_t *fd)
  {
  if (fs == 0 || path == 0 || fd == 0)
    return e_bad_parameter;

  krypton_fs_t *handle;
  result_t result;
  if (failed(result = as_kyrpton(fs, &handle)))
    return result;

  char file_path[MAX_PATH];
  if (failed(result = path_combine(handle->file_base, path, file_path, MAX_PATH)))
    return result;

  HANDLE hndl = CreateFile(file_path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hndl == INVALID_HANDLE_VALUE)
    result = GetLastError();
  else
    {
    *fd = (uint32_t)hndl;
    result = s_ok;
    }

  return result;
  }

static result_t krypton_close(filesystem_p fs, nand_device_t *device, uint32_t fd)
  {
  if (fs == 0 || fd == 0)
    return e_bad_parameter;

  CloseHandle((HANDLE)fd);

  return s_ok; 
  }
static result_t krypton_read(filesystem_p fs, nand_device_t *device, uint32_t fd, uint32_t pos, void *buf, uint32_t nbyte, uint32_t *read) 
  {
  if (fs == 0 || fd == 0 || buf == 0)
    return e_bad_parameter;

  result_t result;
  LARGE_INTEGER file_pos;
  file_pos.QuadPart = pos;

  if (!SetFilePointerEx((HANDLE)fd, file_pos, NULL, FILE_BEGIN))
    {
    result = GetLastError();
    return result;
    }

  DWORD bytes_read;
  if (ReadFile((HANDLE)fd, buf, nbyte, &bytes_read, NULL) == 0)
    result = GetLastError();
  else
    result = s_ok;

  if (read != 0)
    *read = bytes_read;

  return result; 
  }

static result_t krypton_write(filesystem_p fs, nand_device_t *device, uint32_t fd, uint32_t pos, const void *buf, uint32_t nbyte, uint32_t *written) 
  {
  if (fs == 0 || fd == 0 || buf == 0)
    return e_bad_parameter;

  result_t result;
  LARGE_INTEGER file_pos;
  file_pos.QuadPart = pos;

  if (!SetFilePointerEx((HANDLE)fd, file_pos, NULL, FILE_BEGIN))
    {
    result = GetLastError();
    return result;
    }

  DWORD bytes_written;
  if (WriteFile((HANDLE)fd, buf, nbyte, &bytes_written, NULL) == 0)
    result = GetLastError();
  else
    result = s_ok;

  return result;
  }
static result_t krypton_truncate(filesystem_p fs, nand_device_t *device, uint32_t fd, uint32_t new_size) 
  {
  if (fs == 0 || fd == 0)
    return e_bad_parameter;

  result_t result;
  LARGE_INTEGER file_pos;
  file_pos.QuadPart = new_size;

  if (!SetFilePointerEx((HANDLE)fd, file_pos, NULL, FILE_BEGIN) ||
    !SetEndOfFile((HANDLE)fd))
    {
    result = GetLastError();
    return result;
    }

  return s_ok;
  }

static result_t krypton_unlink(filesystem_p fs, nand_device_t *device, uint32_t fd)
  {
  if (fs == 0 || fd == 0)
    return e_bad_parameter;

  char full_path[MAX_PATH];

  // get the filename
  if (GetFinalPathNameByHandle((HANDLE)fd, full_path, MAX_PATH, FILE_NAME_NORMALIZED) == 0)
    return GetLastError();

  // close the file
  CloseHandle((HANDLE)fd);

  if (!DeleteFile(full_path))
    return GetLastError();

  return s_ok; 
  }
static result_t krypton_rename(filesystem_p fs, nand_device_t *device, uint32_t fd, const char *name) 
  {
  return e_not_implemented; 
  }

static result_t krypton_stat(filesystem_p fs, nand_device_t *device, uint32_t fd, stat_t *buf) 
  {
  if (fs == 0 || fd == 0 || buf == 0)
    return e_bad_parameter;

  memset(buf, 0, sizeof(stat_t));

  // not fully implemented
  LARGE_INTEGER sz;
  GetFileSizeEx((HANDLE)fd, &sz);
  buf->st_size = sz.LowPart;

  return s_ok;
  }

static result_t krypton_getpath(filesystem_p fs, nand_device_t *device, uint32_t fd, bool full_path, char *buffer, size_t size) 
  {
  return e_not_implemented; 
  }

static result_t krypton_mkdir(filesystem_p fs, nand_device_t *device, const char *path)
  {
  return e_not_implemented; 
  }

static result_t krypton_rmdir(filesystem_p fs, nand_device_t *device, const char *path)
  {
  return e_not_implemented;
  }

static result_t krypton_opendir(filesystem_p fs, nand_device_t *device, const char *dirname, uint32_t *dirp)
  {
  return e_not_implemented; 
  }

static result_t krypton_readdir(filesystem_p fs, nand_device_t *device, uint32_t dirp, dir_entry_type *et, char *buffer, size_t len)
  {
  return e_not_implemented; 
  }

static result_t krypton_rewinddir(filesystem_p fs, nand_device_t *device, uint32_t dirp)
  {
  return e_not_implemented; 
  }

static result_t krypton_closedir(filesystem_p fs, nand_device_t *device, uint32_t dirp)
  {
  return e_not_implemented; 
  }

result_t krypton_fs_init(const char *root_path, filesystem_p *handle)
  {
  // open the file system, and get the handle
  krypton_fs_t *filesystem = (krypton_fs_t *)neutron_malloc(sizeof(krypton_fs_t));

  filesystem->fs.version = sizeof(krypton_fs_t);
  filesystem->fs.close = krypton_close;
  filesystem->fs.closedir = krypton_closedir;
  filesystem->fs.create = krypton_create;
  filesystem->fs.format = krypton_format;
  filesystem->fs.freespace = krypton_freespace;
  filesystem->fs.fssync = krypton_fssync;
  filesystem->fs.fsync = krypton_fsync;
  filesystem->fs.getpath = krypton_getpath;
  filesystem->fs.mkdir = krypton_mkdir;
  filesystem->fs.mount = krypton_mount;
  filesystem->fs.open = krypton_open;
  filesystem->fs.opendir = krypton_opendir;
  filesystem->fs.read = krypton_read;
  filesystem->fs.readdir = krypton_readdir;
  filesystem->fs.rename = krypton_rename;
  filesystem->fs.rewinddir = krypton_rewinddir;
  filesystem->fs.rmdir = krypton_rmdir;
  filesystem->fs.stat = krypton_stat;
  filesystem->fs.sync = krypton_sync;
  filesystem->fs.totalspace = krypton_totalspace;
  filesystem->fs.truncate = krypton_truncate;
  filesystem->fs.unlink = krypton_unlink;
  filesystem->fs.unmount = krypton_unmount;
  filesystem->fs.write = krypton_write;

  lstrcpyn(filesystem->file_base, root_path, MAX_PATH);
  
  // ensure files are ansi
  SetFileApisToANSI();

  *handle = &filesystem->fs;

  return s_ok;
  }