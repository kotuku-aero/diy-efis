/*
diy-efis
Copyright (C) 2016-2022 Kotuku Aerospace Limited

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

If you wish to use any of this code in a commercial application then
you must obtain a licence from the copyright holder.  Contact
support@kotuku.aero for information on the commercial licences.
*/
#include "../../libs/neutron/neutron.h"
#include "../../libs/neutron/stream.h"

#undef STREAM_SEEK_SET
#undef STREAM_SEEK_END
#undef STREAM_SEEK_CUR

#define handle_t win_handle_t
#include <Windows.h>
#undef handle_t

typedef struct _krypton_fs_t {
  filesystem_t fs;
  char* file_base;
  } krypton_fs_t;

static const typeid_t krypton_fs_type;

static result_t krypton_fs_close(handle_t hndl)
  {
  result_t result;
  krypton_fs_t* fs;
  if (failed(result = is_typeof(hndl, &krypton_fs_type, (void**)&fs)))
    return result;

  neutron_free(fs->file_base);
  memset(fs, 0, sizeof(krypton_fs_t));

  neutron_free(fs);

  return s_ok;
  }

static const typeid_t krypton_fs_type =
  {
  .name = "krypton_fs_t",
  .base = &filesystem_type,
  .etherealize = krypton_fs_close
  };

// merge 2 paths, but don't support . or .. in path yet (this is for testing only)
// to make this consistent we convert all paths to lower case.  Windows is case insensitive
// linux and canfly on embedded devices are case sensitive.  so we assume lower case
// on the test system
static result_t path_combine(const char* path_in, const char* path_more, char* path_out, size_t len)
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
    if (*path_in == '/')
      *path_out = '\\';
    else
      *path_out = tolower(*path_in);
    }

  if (*path_out != '\\')
    {
    *path_out++ = '\\';
    len--;
    }
  else
    path_out++;     // skip the trailing '/'

  if (len == 0)
    goto exit_copy;

  // skip a possible leading '.'
  if (*path_more == '/' || *path_more == '\\')
    path_more++;

  for (; len > 0 && *path_more != 0; len--)
    {
    if (*path_more == '/')
      {
      *path_out++ = '\\';
      path_more++;
      }
    else
      *path_out++ = tolower(*path_more++);
    }

exit_copy:
  // this was always available from above
  *path_out = 0;
  return s_ok;
  }

static result_t krypton_mount(handle_t hndl)
  {
  return s_ok;
  }

static result_t krypton_unmount(handle_t hndl)
  {
  return s_ok;
  }

static result_t krypton_fssync(handle_t hndl)
  {
  return s_ok;
  }

static result_t krypton_sync(filesystem_t* fs, handle_t hndl)
  {
  return s_ok;
  }

static result_t krypton_freespace(handle_t hndl, const char* path, uint32_t* space)
  {
  if (space == 0 || path == 0)
    return e_bad_parameter;

  // return a fixed size;
  *space = UINT32_MAX;

  return s_ok;
  }
static result_t krypton_totalspace(handle_t hndl, const char* path, uint32_t* space)
  {
  if (space == 0 || path == 0)
    return e_bad_parameter;

  // return a fixed size;
  *space = UINT32_MAX;

  return s_ok;
  }

static result_t krypton_mkdir(handle_t hndl, const char* path)
  {
  return e_not_implemented;
  }

static result_t krypton_rmdir(handle_t hndl, const char* path)
  {
  return e_not_implemented;
  }

typedef struct _krypton_dirp_t {
  base_t base;
  WIN32_FIND_DATA ffd;
  HANDLE handle;
  uint32_t offs;
  bool is_eof;
  char search_path[MAX_PATH];
  } krypton_dirp_t;

static const typeid_t krypton_dirp_type;
static result_t krypton_closedir(handle_t hndl)
  {
  krypton_dirp_t* dirp;
  result_t result;
  if (failed(result = is_typeof(hndl, &krypton_dirp_type, (void**)&dirp)))
    return result;

  FindClose(dirp->handle);

  memset(dirp, 0, sizeof(krypton_dirp_t));
  neutron_free(dirp);

  return s_ok;
  }

static const typeid_t krypton_dirp_type =
  {
  .name = "dirp_t",
  .etherealize = krypton_closedir
  };

static result_t krypton_opendir(handle_t hndl, const char* dirname, handle_t* hdirp)
  {
  krypton_fs_t* handle;
  result_t result;
  if (failed(result = is_typeof(hndl, &krypton_fs_type, (void**)&handle)))
    return result;

  char file_path[MAX_PATH];
  if (failed(result = path_combine(handle->file_base, dirname, file_path, MAX_PATH)))
    return result;

  char search_path[MAX_PATH];
  strcpy(search_path, file_path);
  strcat(search_path, "\\*");

  WIN32_FIND_DATA ffd;
  HANDLE fddHandle = FindFirstFile(search_path, &ffd);

  if (hndl == INVALID_HANDLE_VALUE)
    return e_not_found;

  krypton_dirp_t* dirp;
  if (failed(result = neutron_malloc(sizeof(krypton_dirp_t), (void**)&dirp)))
    return result;

  dirp->base.type = &krypton_dirp_type;
  dirp->handle = fddHandle;
  dirp->is_eof = false;
  dirp->offs = 0;
  strcpy(dirp->search_path, search_path);
  memcpy(&dirp->ffd, &ffd, sizeof(WIN32_FIND_DATA));

  *hdirp = (handle_t)dirp;

  return s_ok;
  }

static result_t krypton_readdir(filesystem_t* fs, handle_t hndl, stat_t* st)
  {
  krypton_dirp_t* dirp;
  result_t result;
  if (failed(result = is_typeof(hndl, &krypton_dirp_type, (void**)&dirp)))
    return result;

  if (dirp->is_eof)
    return e_no_more_information;

  if (dirp->ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    st->type = FILE_TYPE_DIR;
  else
    st->type = FILE_TYPE_FILE;

  strncpy(st->name, dirp->ffd.cFileName, sizeof(st->name));
  st->size = dirp->ffd.nFileSizeLow;

  if (!FindNextFile(dirp->handle, &dirp->ffd))
    dirp->is_eof = true;
  else
    dirp->offs++;

  return s_ok;
  }

static result_t krypton_rewinddir(filesystem_t* fs, handle_t hndl)
  {
  krypton_dirp_t* dirp;
  result_t result;
  if (failed(result = is_typeof(hndl, &krypton_dirp_type, (void**)&dirp)))
    return result;

  FindClose(dirp->handle);
  dirp->handle = FindFirstFile(dirp->search_path, &dirp->ffd);
  dirp->is_eof = false;
  dirp->offs = 0;

  return s_ok;
  }

static result_t krypton_seekdir(filesystem_t* fs, handle_t hndl, uint32_t offs)
  {
  krypton_dirp_t* dirp;
  result_t result;
  if (failed(result = is_typeof(hndl, &krypton_dirp_type, (void**)&dirp)))
    return result;

  FindClose(dirp->handle);
  dirp->handle = FindFirstFile(dirp->search_path, &dirp->ffd);
  dirp->is_eof = false;
  dirp->offs = 0;

  while (offs-- > 0)
    if (!FindNextFile(dirp->handle, &dirp->ffd))
      {
      dirp->is_eof = true;
      break;
      }
    else
      dirp->offs++;

  return s_ok;
  }

static result_t krypton_telldir(filesystem_t* fs, handle_t hndl, uint32_t* offs)
  {
  krypton_dirp_t* dirp;
  result_t result;
  if (failed(result = is_typeof(hndl, &krypton_dirp_type, (void**)&dirp)))
    return result;

  *offs = dirp->offs;

  return s_ok;
  }

static  result_t krypton_stat(filesystem_t* hndl, const char* path, stat_t* st)
  {
  krypton_fs_t* fs;
  result_t result;
  if (failed(result = is_typeof(hndl, &krypton_fs_type, (void**)&fs)))
    return result;

  char file_path[MAX_PATH];
  if (failed(result = path_combine(fs->file_base, path, file_path, MAX_PATH)))
    return result;

  size_t len = strlen(file_path);

  if(file_path[len-1] == '\\' || file_path[len - 1] == '/')
    file_path[len-1] = 0;

  WIN32_FIND_DATA ffd;
  HANDLE fddHandle = FindFirstFile(file_path, &ffd);

  if (fddHandle == INVALID_HANDLE_VALUE)
    return e_not_found;

  strncpy(st->name, ffd.cFileName, FILE_NAME_MAX);
  st->size = ffd.nFileSizeLow;
  st->type = ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!= 0) ? FILE_TYPE_DIR : FILE_TYPE_FILE;  

  return s_ok;
  }

static result_t krypton_remove(filesystem_t* hndl, const char* path)
  {
  krypton_fs_t* handle;
  result_t result;
  if (failed(result = is_typeof(hndl, &krypton_fs_type, (void**)&handle)))
    return result;

  if (!DeleteFile(path))
    return GetLastError();

  return s_ok;
  }


static result_t krypton_open(filesystem_t* fs, const char* path, uint32_t flags, handle_t* stream);

result_t krypton_fs_init(const char* root_path, filesystem_t** fs)
  {
  result_t result;
  // open the file system, and get the handle
  krypton_fs_t* filesystem;
  if (failed(result = neutron_malloc(sizeof(krypton_fs_t), (void**)&filesystem)))
    return result;

  filesystem->fs.base.type = &krypton_fs_type;
  filesystem->fs.freespace = krypton_freespace;
  filesystem->fs.fssync = krypton_fssync;
  filesystem->fs.mkdir = krypton_mkdir;
  filesystem->fs.mount = krypton_mount;
  filesystem->fs.unmount = krypton_unmount;
  filesystem->fs.open = krypton_open;
  filesystem->fs.opendir = krypton_opendir;
  filesystem->fs.readdir = krypton_readdir;
  filesystem->fs.rewinddir = krypton_rewinddir;
  filesystem->fs.rmdir = krypton_rmdir;
  filesystem->fs.sync = krypton_sync;
  filesystem->fs.totalspace = krypton_totalspace;
  filesystem->fs.unmount = krypton_unmount;
  filesystem->fs.stat = krypton_stat;
  filesystem->fs.remove = krypton_remove;
  filesystem->fs.seekdir = krypton_seekdir;
  filesystem->fs.telldir = krypton_telldir;

  neutron_strdup(root_path, 0, &filesystem->file_base);

  // ensure files are ansi
  SetFileApisToANSI();

  *fs = (filesystem_t *) filesystem;

  return s_ok;
  }

/*************************************************************************/


typedef struct _krypton_file_t {
  stream_t base;
  HANDLE fd;
  } krypton_file_t;

static const typeid_t krypton_file_type;

static result_t krypton_file_close(handle_t hndl)
  {
  result_t result;
  krypton_file_t* file;
  if (failed(result = is_typeof(hndl, &krypton_file_type, (void**)&file)))
    return result;

  if (file->fd != 0)
    CloseHandle(file->fd);

  memset(file, 0, sizeof(krypton_file_t));

  return s_ok;
  }

static const typeid_t krypton_file_type =
  {
  .name = "krypton_file_t",
  .base = &stream_type,
  .etherealize = krypton_file_close
  };

static result_t krypton_read(handle_t hndl, void* buf, uint32_t nbyte, uint32_t* read)
  {
  if (buf == 0)
    return e_bad_parameter;

  result_t result;
  krypton_file_t* file;
  if (failed(result = is_typeof(hndl, &krypton_file_type, (void**)&file)))
    return result;

  DWORD bytes_read;
  if (ReadFile(file->fd, buf, nbyte, &bytes_read, NULL) == 0)
    result = GetLastError();
  else
    result = s_ok;

  if (read != 0)
    *read = (uint16_t)bytes_read;

  return result;
  }

static result_t krypton_write(handle_t hndl, const void* buf, uint32_t nbyte)
  {
  if (buf == 0)
    return e_bad_parameter;

  result_t result;
  krypton_file_t* file;
  if (failed(result = is_typeof(hndl, &krypton_file_type, (void**)&file)))
    return result;

  DWORD bytes_written = 0;
  if (WriteFile(file->fd, buf, nbyte, &bytes_written, NULL) == 0)
    result = GetLastError();
  else
    result = s_ok;

  return result;
  }

static result_t krypton_truncate(handle_t hndl, uint32_t new_size)
  {
  result_t result;
  krypton_file_t* file;
  if (failed(result = is_typeof(hndl, &krypton_file_type, (void**)&file)))
    return result;

  LARGE_INTEGER file_pos;
  file_pos.QuadPart = new_size;

  if (!SetFilePointerEx(file->fd, file_pos, NULL, FILE_BEGIN) ||
    !SetEndOfFile(file->fd))
    {
    result = GetLastError();
    return result;
    }

  return s_ok;
  }

static result_t krypton_length(handle_t hndl, uint32_t* length)
  {
  if (length == 0)
    return e_bad_parameter;

  result_t result;
  krypton_file_t* file;
  if (failed(result = is_typeof(hndl, &krypton_file_type, (void**)&file)))
    return result;

  // not fully implemented
  LARGE_INTEGER sz;
  GetFileSizeEx(file->fd, &sz);
  *length = sz.LowPart;

  return s_ok;
  }

static result_t krypton_eof(handle_t hndl)
  {
  result_t result;
  krypton_file_t* file;
  if (failed(result = is_typeof(hndl, &krypton_file_type, (void**)&file)))
    return result;

  DWORD pos = SetFilePointer(file->fd, 0, 0, FILE_CURRENT);

  if (pos == INVALID_SET_FILE_POINTER)
    return e_unexpected;

  LARGE_INTEGER sz;
  GetFileSizeEx(file->fd, &sz);

  return pos == sz.LowPart ? s_ok : s_false;
  }

static result_t krypton_getpos(handle_t hndl, uint32_t* value)
  {
  if (value == 0)
    return e_bad_pointer;

  result_t result;
  krypton_file_t* file;
  if (failed(result = is_typeof(hndl, &krypton_file_type, (void**)&file)))
    return result;

  DWORD pos = SetFilePointer(file->fd, 0, 0, FILE_CURRENT);

  if (pos == INVALID_SET_FILE_POINTER)
    return e_unexpected;

  *value = pos;

  return s_ok;
  }

static result_t krypton_setpos(handle_t hndl, int32_t value, uint32_t whence)
  {
  result_t result;
  krypton_file_t* file;
  if (failed(result = is_typeof(hndl, &krypton_file_type, (void**)&file)))
    return result;

  DWORD move;
  switch (whence)
    {
    case STREAM_SEEK_SET:
      move = FILE_BEGIN;
      break;
    case STREAM_SEEK_END:
      move = FILE_END;
      break;
    case STREAM_SEEK_CUR:
      move = FILE_CURRENT;
      break;
    }

  DWORD pos = SetFilePointer(file->fd, value, 0, FILE_BEGIN);

  if (pos == INVALID_SET_FILE_POINTER)
    return e_unexpected;

  return s_ok;
  }

static result_t create_krypton_file(HANDLE fd, const char* path, handle_t* hndl)
  {
  result_t result;

  krypton_file_t* file;
  if (failed(result = neutron_malloc(sizeof(krypton_file_t), (void**)&file)))
    return result;

  memset(file, 0, sizeof(krypton_file_t));

  file->base.base.type = &krypton_file_type;

  file->base.eof = krypton_eof;
  file->base.getpos = krypton_getpos;
  file->base.length = krypton_length;
  file->base.read = krypton_read;
  file->base.setpos = krypton_setpos;
  file->base.truncate = krypton_truncate;
  file->base.write = krypton_write;

  file->fd = fd;

  *hndl = (handle_t)file;
  return s_ok;
  }

static result_t krypton_open(filesystem_t* fs, const char* path, uint32_t flags, handle_t* fd)
  {
  if (path == 0 || fd == 0)
    return e_bad_parameter;

  result_t result;
  krypton_fs_t* handle = (krypton_fs_t*)fs;

  char file_path[MAX_PATH];
  if (failed(result = path_combine(handle->file_base, path, file_path, MAX_PATH)))
    return result;

  HANDLE whndl = CreateFile(file_path, GENERIC_READ | GENERIC_WRITE, 0, NULL,
    (flags & STREAM_O_CREAT) ? OPEN_ALWAYS : OPEN_EXISTING
    , FILE_ATTRIBUTE_NORMAL, NULL);

  if (whndl == INVALID_HANDLE_VALUE)
    {
    DWORD dwError = GetLastError();
    result = e_not_found;
    }
  else
    {
    handle_t out;
    if(failed(create_krypton_file(whndl, path, &out)))
      return result;

    *fd = out;
    result = s_ok;
    }

  return result;
  }
